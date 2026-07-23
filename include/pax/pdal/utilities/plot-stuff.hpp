//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "plot.hpp"
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>

// Read a csv file
#include <pax/tables/text-table.hpp>

// pdal write stuff
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>

#include <unordered_map>
#include <unordered_set>


namespace pax {

	/// A simple container for the spacial data of a plot.
	/// It has the plot stuff + an id and a vector of pdal points. 
	class Plot_stuff : public Plot_w_id {
		static_assert( sizeof( pdal::PointId ) == 8 );
		std::vector< pdal::PointId >		m_points_idx{};
		metrics::Point_aggregator			m_metric_agg{};
		pdal::Dimension::Id					m_height_dimension{ pdal::Dimension::Id::Z };
		bool								m_do_metrics{}, m_do_points{}, m_has_return_number{};

	public:
		using Plot_w_id::coord_type;

		constexpr Plot_stuff()									  = default;
		constexpr Plot_stuff( const Plot_stuff & )				  = default;
		constexpr Plot_stuff( Plot_stuff && )					  = default;
		constexpr Plot_stuff & operator=( const Plot_stuff & )	  = default;
		constexpr Plot_stuff & operator=( Plot_stuff && )		  = default;


		/// The actual constructor. 
		constexpr Plot_stuff(
			const Plot_w_id				  & plot_,
			const bool						do_metrics, 
			const bool						do_points, 
			const bool						has_return_number,
			const pdal::Dimension::Id		height_dimension
		) noexcept : 
			Plot_w_id					  { plot_ 				},
			m_height_dimension			  { height_dimension	}, 
			m_do_metrics				  { do_metrics			},
			m_do_points					  { do_points			},
			m_has_return_number			  { has_return_number	}
		{}


		/// Process a point. Return true if it was inside the plot.
		bool process( const pdal::PointRef & pt_ ) 			noexcept		{
			if( Plot::contains( pt_ ) ) {
				if( m_do_points )			m_points_idx.push_back( pt_.pointId() );
				if( m_do_metrics )			m_metric_agg.push_back(
					pt_.getFieldAs< coord_type >( m_height_dimension ),
					!m_has_return_number || pt_.getFieldAs<std::uint8_t>(pdal::Dimension::Id::ReturnNumber) == 1
				);
				return true;
			}
			return false;
		}
		
		
		/// Access the metrics aggregator.
		metrics::Point_aggregator & metric_aggregator()		noexcept	{	return m_metric_agg;	}


		/// Save the results, the point cloud[s] and the metric[s].


		void save_plot_points( 
			const pdal::PointViewPtr		  & view_ptr_,
			const dir_path					  & plot_points_dest_dir_,
			std::string_view					plot_points_file_format_
		) const {
			const std::filesystem::path	plot_points_dest   = plot_points_dest_dir_ / ( Plot_w_id::id() + plot_points_file_format_ );
			try{
				if( m_do_points && !m_points_idx.empty() && !plot_points_dest_dir_.empty() ) {
					// Create a new view and add the plot's points to it. 
					pdal::PointViewPtr				points{ view_ptr_->makeNew() };
					pdal::PointRef					pt( *view_ptr_, 0 );
					for( pdal::PointId idx : m_points_idx ) {
						pt.setPointId( idx );
						points->appendPoint( *view_ptr_, pt.pointId() );
					}

					// Save the view to a file.
					static constexpr const char *	las_suffix  = ".las";
					static constexpr const char *	laz_suffix  = ".laz";
					pdal::Options					options;
					options.add( "filename", plot_points_dest.native() );
					if( plot_points_file_format_ == laz_suffix ) {
						options.add( "compression",	"laszip" );
						plot_points_file_format_  = las_suffix;
					}

					pdal::BufferReader				reader;
					reader.addView( points );

					// StageFactory always "owns" the stages it creates. They'll be destroyed with the factory.
					pdal::StageFactory				factory;
					// Next line can not be "writers.laz", but must in that case be "writers.las".
					pdal::Stage					  * writer = factory.createStage( std::string( "writers" ) + plot_points_file_format_ );
					writer->setInput( reader );
					writer->setOptions( options );
					writer->prepare( points->table() );
					writer->execute( points->table() );
				}
			} catch( const std::exception & error_ ) {
				throw error_message( std20::format( "Plot_stuff: {}. (Saving point cloud of plot '{}' to file '{}'.)",
					error_.what(), Plot_w_id::id(), to_string( plot_points_dest )
				) );
			}
		}
	};

	
	/// Seve the metrics from a bunch of plots to a .csv file, not discarding previous columns.
	inline void save_metrics(
		const Text_table< char >						  & plots_table_,
		const std::span< Plot_stuff >						plots_,
		const dest_path									  & plot_metrics_dest_,
		const std::span< const metrics::Function_filter >	metrics_,
		const std::string_view								id_col_name_ = "id"
	) {
		static constexpr const std::string_view	merge_items  = ";{}";	// Semicolon separator.
		std::vector< std::string >				metric_names{};
		try{
			metric_names.reserve( metrics_.size() );
			for( const auto name : metrics_ )	metric_names.push_back( to_string( name ) );

			if( !plots_.empty() && !plot_metrics_dest_.empty() && !metrics_.empty() ) {
				const std::size_t 				id_col = plots_table_.header().index( id_col_name_ );
				if( id_col == -1u )
					throw error_message( std20::format( "In the plots table, there is no column \"{}\".", id_col_name_ ) );

				// Create an unordered map of all id -> index in plots_.
				std::unordered_map< std::string, std::size_t >		plot_id_idx{};
				for( std::size_t i{}; i<plots_.size(); ++i )		plot_id_idx.insert( { plots_[ i ].id(), i } );

				if( plot_id_idx.size() != plots_.size() ) {
					std::cerr << std20::format( "plot_id_idx: {}\n", plot_id_idx );
					throw error_message( std20::format( 
						"plot_id_idx should have {} elements, but only has {}. "
						"Are you sure that column \"{}\" has unique values on every row, no value ever repeated?", 
						plots_.size(), plot_id_idx.size(), id_col_name_
					) );
				}

				// Usually very few plots in the plots__table total are processed, 
				// so we optimize by only working on those rows.
				// Carculate a reduced Text_table, with only the rows corresponding to the plots in plots_.
				Text_table< char >				reduced_table{};
				{
					// Create an unordered set of all rows in plots_table_ with the same id as any item in plots_
					// and use it as a predicative. 
					std::unordered_set< std::size_t >	table_row_ok{};
					for( std::size_t i{}; i<plots_table_.rows(); ++i )
						if( plot_id_idx.contains( std::string( plots_table_[ i, id_col ] ) ) ) {
							table_row_ok.insert( i );
						}
					if( table_row_ok.size() != plots_.size() )
						throw error_message( std20::format( "table_row_ok should have {} elements, but has {}.", 
							plots_.size(), table_row_ok.size() ) );

					const auto predicate = [ &table_row_ok ]( std::size_t i ) { 
						return table_row_ok.contains( i ); 
					};

					reduced_table			  = plots_table_.as_str( predicate );
				}

				if( reduced_table.rows() != plots_.size() )
					throw error_message( std20::format( "Reduced table should have {} rows, but has {}.", 
						plots_.size(), reduced_table.rows() ) );

				{	// Create a Text_table with the metrics and insert it into reduced_table.
					// The order of items must correspond to the order in reduced_table.
					std::stringstream			out;
					
					// Stream the header.
					if( metric_names.size() ) {
						auto					itr = metric_names.begin();
						out << std20::format( "{}",   *itr );
						while( ++itr < metric_names.end() )	
							out << std20::format( merge_items, *itr );
					}

					// Stream those metric rows that have actual metric values.
					const auto					begin = metrics_.begin();
					const auto					end   = metrics_.end();
					for( std::size_t i{}; i<reduced_table.rows(); ++i ) {
						const std::size_t		plot_idx = plot_id_idx.at( std::string( reduced_table[ i, id_col ] ) );
						assert( plots_[ plot_idx ].id() == std::string( reduced_table[ i, id_col ] ) );

						auto				  & metric_aggrs = plots_[ plot_idx ].metric_aggregator();
						auto					itr   = begin;
						out << std20::format( "\n{}", itr->calculate( metric_aggrs ) );
						while( ++itr < end )	out << std20::format( merge_items, itr->calculate( metric_aggrs ) );
					}
					
					// Create a table and insert it into the original.
					out << '\n';
					reduced_table.insert_cols( Text_table< char >{ std::move( out ).str() } );
				}

				// Save the result. 
				reduced_table.save( plot_metrics_dest_ );
			}
		} catch( const std::exception & error_ ) {
			throw error_message( std20::format( "{} (Metrics are {}, destination is '{}'.)", 
				error_.what(), std::span( metric_names ), to_string( plot_metrics_dest_ )
			) );
		}
	}

}	// namespace pax
