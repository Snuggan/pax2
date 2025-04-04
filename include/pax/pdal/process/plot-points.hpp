//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "plot-base.hpp"

// Read a csv file
#include <pax/tables/text-table.hpp>

// pdal stuff
#include "../utilities/bbox.hpp"	// bbox( pdal::PointViewPtr )
#include <pdal/PointView.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>


namespace pax {

	/// A simple container for the spacial data of a plot.
	class Plot_points : public Plot_base {
		static constexpr const char *	las_suffix  = ".las";
		static constexpr const char *	laz_suffix  = ".laz";
		static constexpr const char *	file_suffix = laz_suffix;
		
		using string_view			  = std::string_view;

		static_assert( sizeof( pdal::PointId ) == 8 );
		std::vector< pdal::PointId >	m_points_idx{};
		std::string						m_id;


	public:
		using Plot_base::coord_type;

		Plot_points(
			const string_view			id_, 
			const coord_type			east_, 
			const coord_type			north_
		) noexcept : Plot_base{ east_, north_ }, m_id{ id_.data(), id_.size() } {}

		/// Was this plot processed during this run?
		bool empty()											const noexcept	{	return m_points_idx.empty();	}

		/// Process a point.
		bool process(
			pdal::PointRef			  * pt_ptr_, 
			const coord_type			max_distance_
		) noexcept {
			const bool					inside = Plot_base::contains( *pt_ptr_, max_distance_ );
			if( inside )				m_points_idx.push_back( pt_ptr_->pointId() );
			return inside;
		}
		
		/// Access the points.
		void save( 
			const pdal::PointViewPtr  & view_ptr_,
			const dir_path			  & dest_plot_points_directory_,
			string_view					file_format_ = laz_suffix
		) const {
			const std::filesystem::path	dest   = dest_plot_points_directory_ / ( m_id + file_format_ );
			try{
				// Create a new view and add the plot's points to it. 
				pdal::PointViewPtr		points{ view_ptr_->makeNew() };
				pdal::PointRef			pt( *view_ptr_, 0 );
				for( pdal::PointId idx : m_points_idx ) {
					pt.setPointId( idx );
					points->appendPoint( *view_ptr_, pt.pointId() );
				}

				// Save the view to a file.
				pdal::Options			options;
				options.add( "filename", dest.native() );
				if( file_format_ == laz_suffix ) {
					options.add( "compression",	"laszip" );
					file_format_	  = las_suffix;
				}

				pdal::BufferReader		reader;
				reader.addView( points );

				// StageFactory always "owns" the stages it creates. They'll be destroyed with the factory.
				pdal::StageFactory		factory;
				// Next line can not be "writers.laz", but must in that case be "writers.las".
				pdal::Stage			  * writer = factory.createStage( std::string( "writers" ) + file_format_ );
				writer->setInput( reader );
				writer->setOptions( options );
				writer->prepare( points->table() );
				writer->execute( points->table() );

			} catch( const std::exception & error_ ) {
				throw error_message( std20::format( "Plot_points: {}. (Saving point cloud of plot '{}' to file '{}'.)",
					error_.what(), m_id, to_string( dest )
				) );
			}
		}
	};

	template< typename T > class constructor_elements;
	template<> class constructor_elements< Plot_points > {
		static constexpr std::size_t 			N = 3;
		static constexpr std::array				m_names{ "", "east", "north" };
		using coord_type					  = Plot_points::coord_type;

	public:
		using types							  = std::tuple< std::string, coord_type, coord_type >;
		static constexpr auto					names()	{	return std::span( m_names );	};
		static constexpr std::size_t 			size()	{	return N;						};
	};
	static_assert(	constructor_elements< Plot_points >::names().size() == 
					std::tuple_size_v< constructor_elements< Plot_points >::types > );


	/// Process the plots in a plots text file.
	/** 1. Construct (Process_plots_points) the base data from the plots source file (a csv type of file).
		2. Process directories of point cloud files file or individual point cloud files. 
		3. Save a new csv type of file, same as the original, but with new or updated metrics. 
	**/
	class Process_plots_points {
		using coord_type						  = Plot_points::coord_type;
		using string_view						  = std::string_view;
		using meta								  = constructor_elements< Plot_points >;

		pdal::PointViewPtr							m_view_ptr;
		std::vector< Plot_points >					m_plots;				// Binary "table" of plots.
		coord_type									m_max_distance;			// The point cloud radius around each plot.
		std::size_t									m_processed_plots{};	// Number of plots processed.

		// Read a plot file and return the resulting text table.
		[[nodiscard]] static std::vector< Plot_points > read_plots(
			const file_path						  & plots_source_, 
			const string_view						id_column_
		) {
			try {
				const string_view					names[ 3 ] = { id_column_, string_view( meta::names()[ 1 ] ), string_view( meta::names()[ 2 ] ) };
				Text_table< char >					plots_table{ plots_source_ };
				return plots_table.export_values< Plot_points, meta::types >( std::span( names ) );
			} catch( const std::exception & error_ ) {
				throw error_message( std20::format( "Plot_points: {}. (Reading text table '{}'.)",
					error_.what(), to_string( plots_source_ ) 
				) );
			}
		}

		/// Construct the base data from the plots source file (a csv type of file).
		Process_plots_points(
			const file_path						  & plots_source_, 
			const coord_type						max_disdance_, 
			const pdal::PointViewPtr				view_ptr_, 
			const string_view						id_column_
		) : m_view_ptr{ view_ptr_ }, 
			m_plots{ read_plots( plots_source_, id_column_ ) }, 
			m_max_distance{ max_disdance_ }
		{}

		/// Process an individual point cloud file. 
		template< typename BBox >
		void process( const BBox & bbox_ ) {
			// Find the plots that are in bbox_.
			if( !empty( bbox_ ) ) {
				std::vector< Plot_points * >		active_plots;
				for( Plot_points & plot : m_plots )
					if( plot.in_box( bbox_, m_max_distance ) ) 
						active_plots.push_back( &plot );

				// Accumulate points at the activated plots (if there are any).
				if( !active_plots.empty() ) {
					m_processed_plots			 += active_plots.size();
					pdal::PointRef					pt( *m_view_ptr, 0 );
					for( pdal::PointId idx{}; idx < m_view_ptr->size(); ++idx ) {
						pt.setPointId( idx );
						for( auto plot_ptr : active_plots ) 
							plot_ptr->process( &pt, m_max_distance );
					}
				} 
			}
		}

		/// Save the plots' point couds in the specified directory. 
		/** Returns the number of plots processed.		**/
		std::size_t save(
			const dir_path		  & dest_plot_points_directory_, 
			const string_view		output_file_format_
		) const {
			if( m_processed_plots ) 
				for( const Plot_points & plot : m_plots ) 
					if( !plot.empty() )				plot.save( m_view_ptr, dest_plot_points_directory_, output_file_format_ );
			return m_processed_plots;
		}

		/// Return metadata of result. 
		template< typename Json >
		[[nodiscard]] Json json( const dir_path & dest_plot_points_directory_ ) const {
			return Json{
				{	"plots",						{
					{	"destination",				dest_plot_points_directory_		},
					{	"total",					m_plots.size()					},
					{	"saved-plots",				m_processed_plots				}, 
					{	"plot-inclusion",			Plot_points::inclusion_id()		}
				}}
			};
		}


	public:
		static std::size_t process(
			const pdal::PointViewPtr				view_ptr_, 
			const file_path						  & plots_source_, 
			const dir_path						  & dest_plot_points_directory_, 
			const coord_type						max_disdance_, 
			const string_view						id_column_			  = "id",
			const string_view						output_file_format_	  = ".laz"
		) {
			Process_plots_points					plots{ plots_source_, max_disdance_, view_ptr_, id_column_ };
			plots.process( bbox( view_ptr_ ) );
			return plots.save( dest_plot_points_directory_, output_file_format_ );
		}
	};

}	// namespace pax
