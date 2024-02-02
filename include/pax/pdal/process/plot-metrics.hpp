//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "plot-base.hpp"
#include "../metrics-infrastructure/function-filter.hpp"

// Read a csv file
#include <pax/tables/text-table.hpp>

// pdal stuff
#include <pdal/PointView.hpp>


namespace pax {
	
	/// A simple container for the spacial data of a plot.
	class Plot_metrics : public Plot_base, public metrics::Point_aggregator {
		static constexpr const char *	s_names[] = { "east", "north", "radius" };

		Plot_base::coord_type			m_max_distance{};
		
	public:
		using Plot_base::coord_type;
		using Tuply					  = std::tuple< coord_type, coord_type, coord_type >;
        [[nodiscard]] static constexpr auto names()				    	  noexcept	{
            return std::span( s_names );
        }

		template< std::size_t I >
        [[nodiscard]] friend auto get( const Plot_metrics & me_ )		  noexcept	{
			if constexpr( I < 2 )		return get< I >( me_ );
			else if constexpr( I == 2 )	return me_.m_max_distance;
		}

		constexpr Plot_metrics(
			const coord_type			east_, 
			const coord_type			north_, 
			const coord_type			radius_
		) noexcept : Plot_base{ east_, north_ }, m_max_distance{ radius_ } {}

		/// Should the plot recieve points from file_ (with the bbox_)?
		template< typename BBox >
        [[nodiscard]] constexpr bool in_box( const BBox & bbox_ )	const noexcept	{
            return Plot_base::in_box( bbox_, m_max_distance );
        }

		/// Was this plot processed during this run?
        [[nodiscard]] bool empty()							    	const noexcept	{
            return metrics::Point_aggregator::empty();
        }

		/// Is pt_ inside the plot?
		[[nodiscard]] bool contains( const pdal::PointRef pt_ ) 	const noexcept	{
            return Plot_base::contains( pt_, m_max_distance );
        }
	};



	/// Process the plots in a plots text file.
	/** 1. Construct (Process_plots_metrics) the base data from the plots source file (a csv type of file).
		2. Process directories of point cloud files file or individual point cloud files. 
		3. Save a new csv type of file, same as the original, but with new or updated metrics. 
	**/
	class Process_plots_metrics {
		using Table							  = Text_table< char >;
		using Metric						  = metrics::Function_filter;
		using value_type					  = metrics::metrics_value_type;
		using coord_type					  = Plot_metrics::coord_type;

		Table									m_plots_table;				// Text table of plots.
		std::vector< Plot_metrics >				m_plots;					// Binary "table" of plots.
		file_path								m_plots_source;				// From where did we read the plots.
		std::size_t								m_processed_plots{};		// Total number of "activated" plots. 
		const Metric							count_metric{ "count_all" };


		/// Return metadata of result. 
		template< typename Json >
		[[nodiscard]] Json json(
			const std::filesystem::path		  & plots_dest_, 
			const std::span< const Metric >		metrics_set_,
			const std::size_t 					plots_updated_
		) const {
			std::vector< std::string >			metric_names{};
			metric_names.reserve( metrics_set_.size() );
			for( const auto m : metrics_set_ )	metric_names.push_back( to_string( m ) );

			return Json{
				{	"plots",					{
					{	"source",				m_plots_source						},
					{	"destination",			plots_dest_							},
					{	"total",				m_plots.size()						},
					{	"updated",				plots_updated_						}
				}},
				{	"metrics",					{
					{	"count-metric",			to_string( count_metric )			}, 
					{	"metrics",				metric_names						}, 
					{	"plot-inclusion",		Plot_metrics::inclusion_id()		}
				}}
			};
		}


	public:
		/// Construct the base data from the plots source file (a csv type of file).
		explicit Process_plots_metrics( const file_path & plots_source_ ) :
			m_plots_table{ plots_source_ },
			m_plots{ m_plots_table.export_values< Plot_metrics, Plot_metrics::Tuply >( Plot_metrics::names() ) },
			m_plots_source{ plots_source_ }
		{}


		/// Process an individual point cloud file. 
		template< typename BBox >
		void process(
			const pdal::PointViewPtr				point_view_ptr_,
			const BBox							  & bbox_
		) {
			// Find the plots that are in bbox_.
			if( !empty( bbox_ ) ) {
				// Active plots: pointers to those plots that are inside bbox_.
				std::vector< Plot_metrics * >		active_plots;
				for( Plot_metrics & plot : m_plots )
					if( plot.in_box( bbox_ ) ) 
						active_plots.push_back( &plot );

				// Accumulate points at the activated plots (if there are any).
				if( !active_plots.empty() ) {
					using Id					  = pdal::Dimension::Id;
				
					m_processed_plots			 += active_plots.size();
					const bool						has_returns{ point_view_ptr_->hasDim( Id::ReturnNumber ) };

					// Normalized points maybe stored in Id::HeightAboveGround. 
					const auto						height_dimension{ point_view_ptr_->hasDim( Id::HeightAboveGround ) 
														? Id::HeightAboveGround : Id::Z };

					for( const pdal::PointRef & pt_ref : *point_view_ptr_ ) {
						for( auto plot_ptr : active_plots ) {
							if( plot_ptr->contains( pt_ref ) ) {
								plot_ptr->push_back(
									pt_ref.getFieldAs< coord_type >( height_dimension ),
									!has_returns ||
										pt_ref.getFieldAs<std::uint8_t>(pdal::Dimension::Id::ReturnNumber) == 1
								);
							}
						}
					}
				} 
			}
		}


		/// Save the plots table to a new csv type of file, similar as the original but with new or updated metrics. 
		/** Returns the number of plots processed.		**/
		std::size_t save(
			const dest_path						  & plots_dest_, 
			const std::span< const Metric >			metrics_set_
		) {
			static constexpr const char				merge_items[] = ";{}";	// Semicolon separator.
			std::vector< std::string >				metric_names{};
			try{
				metric_names.reserve( metrics_set_.size() );
				for( const auto m : metrics_set_ )	metric_names.push_back( to_string( m ) );

				if( m_processed_plots && !metric_names.empty() ) {
					// Usually very few plots in the m_plots_table total are processed, 
					// so we optimize by only working on those rows.
					const auto predicate		  = [ this ]( std::size_t i ){ return !this->m_plots[ i ].empty(); };

					// Reduce the original plots table to only the rows with processed plots.
					Table							table{ m_plots_table.as_str( predicate, *merge_items ) };

					{	// Create a Text_table with the metrics and insert it into table.
						std::stringstream			out;
						
						// Stream the header.
						if( metric_names.size() ) {
							auto								itr = metric_names.begin();
																out << std20::format( "{}",   *itr );
							while( ++itr < metric_names.end() )	out << std20::format( merge_items, *itr );
						}
						
						
						// Stream those metric rows that have actual metric values.
						const auto					begin = metrics_set_.begin();
						const auto					end   = metrics_set_.end();
						for( auto & row : m_plots )
							if( !row.empty() ) {	// Has been processed.
								auto				itr   = begin;
								out << std20::format( "\n{}", itr->calculate( row ) );
								while( ++itr < end ) {
									// Semicolon will be value separator:
									out << std20::format( merge_items, itr->calculate( row ) );
								}
							}
						
						// Create a table and insert it into the original.
						out << '\n';
						table.insert_cols( Table{ std::move( out ).str() } );
					}

					// Save the result. 
					table.save( plots_dest_ );
				}
				return m_processed_plots;

			} catch( const std::exception & error_ ) {
				throw error_message( std20::format( "{} (Metrics are {}, destination is '{}'.)", 
					error_.what(), std::span( metric_names ), to_string( plots_dest_ )
				) );
			}
			return 0;		// gcc requires a return value, even though the try statement do return.
		}

	};

}	// namespace pax
