//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "plot.hpp"
#include "bbox_indexer.hpp"		// bbox( pdal::PointViewPtr )
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>

// Read a csv file
#include <pax/tables/text-table.hpp>

// pdal stuff
#include <pdal/PointView.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/Dimension.hpp>
#include <pdal/Options.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>


namespace pax {

	/// A simple container for the spacial data of a plot.
	/// It has the plot stuff + an id and a vector of pdal points. 
	class Plot_stuff : public Plot, public metrics::Point_aggregator {
		static constexpr const char *	las_suffix  = ".las";
		static constexpr const char *	laz_suffix  = ".laz";
		static constexpr const char *	file_suffix = laz_suffix;
		
		using Meta = class_meta< Plot_stuff, Plot::coord_type, Plot::coord_type, Plot::coord_type, std::string >;
		using string_view			  = std::string_view;

		static_assert( sizeof( pdal::PointId ) == 8 );
		std::vector< pdal::PointId >	m_points_idx{};
		std::string						m_id{};
		pdal::Dimension::Id				m_height_dimension{ pdal::Dimension::Id::Z };
		bool							m_do_points{ true }, m_do_metrics{ true }, m_has_return_number{ false };

		void save_metrics( const dir_path & plot_metrics_directory_ ) const;

		void save_points( 
			const pdal::PointViewPtr  & view_ptr_,
			const dir_path			  & plot_points_dest_directory_,
			string_view					plot_points_file_format_
		) const;

	public:
		using Plot::coord_type;

		constexpr Plot_stuff()									  = default;
		constexpr Plot_stuff( const Plot_stuff & )				  = default;
		constexpr Plot_stuff( Plot_stuff && )					  = default;
		constexpr Plot_stuff & operator=( const Plot_stuff & )	  = default;
		constexpr Plot_stuff & operator=( Plot_stuff && )		  = default;


		/// The actual constructor. 
		constexpr Plot_stuff(
			const coord_type			east_, 
			const coord_type			north_, 
			const coord_type			radius_, 
			const string_view			id_ = "id"
		) noexcept : 
			Plot{ east_, north_, radius_ }, 
			m_id{ id_.data(), id_.size() }
		{}


		/// Set the stuff that is not found in the csv table.
		constexpr void set_complementary_stuff(
			const bool					do_metrics, 
			const bool					do_points, 
			const bool					has_return_number,
			const pdal::Dimension::Id	height_dimension
		) {
			m_do_metrics			  = do_metrics;
			m_do_points				  = do_points;
			m_has_return_number		  = has_return_number;
			m_height_dimension		  = height_dimension;
		}


		/// This is used to read values from a csv file.
		static constexpr Meta table_meta( const string_view id_column_ )  noexcept	{
			return Meta{ "east", "north", "radius", id_column_ };
		}


		/// Was this plot processed during this run?
		constexpr bool empty()										const noexcept	{
			return m_points_idx.empty();
		}


		/// Process a point.
		bool process( const pdal::PointRef & pt_ ) 					noexcept		{
			const bool					inside = Plot::contains( pt_ );
			if( inside ) {
				if( m_do_points )		m_points_idx.push_back( pt_.pointId() );
				if( m_do_metrics )		Point_aggregator::push_back(
					pt_.getFieldAs< coord_type >( m_height_dimension ),
					!m_has_return_number || pt_.getFieldAs<std::uint8_t>(pdal::Dimension::Id::ReturnNumber) == 1
				);
			}
			return inside;
		}


		/// Access the points.
		void save( 
			const pdal::PointViewPtr  & view_ptr_,
			const dir_path			  & plot_metrics_directory_,
			const dir_path			  & plot_points_dest_directory_,
			string_view					plot_points_file_format_ = laz_suffix
		) const {
			if( !empty() ) {
				if( m_do_metrics && !plot_metrics_directory_.empty() )
					save_metrics( plot_metrics_directory_ );
				if( m_do_points  && !plot_points_dest_directory_.empty() )
					save_points( view_ptr_, plot_points_dest_directory_, plot_points_file_format_ );
			}
		}
	};
	
	
	inline void Plot_stuff::save_metrics( const dir_path & plot_metrics_directory_ ) const {
		const std::filesystem::path	metrics_dest   = plot_metrics_directory_ / ( m_id + ".csv" );
		try{
			

		} catch( const std::exception & error_ ) {
			throw error_message( std20::format( "Plot_stuff: {}. (Saving metrics of plot '{}' to file '{}'.)",
				error_.what(), m_id, to_string( metrics_dest )
			) );
		}
	}

	
	inline void Plot_stuff::save_points( 
		const pdal::PointViewPtr  & view_ptr_,
		const dir_path			  & plot_points_dest_directory_,
		string_view					plot_points_file_format_
	) const {
		const std::filesystem::path	plot_points_dest   = plot_points_dest_directory_ / ( m_id + plot_points_file_format_ );
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
			options.add( "filename", plot_points_dest.native() );
			if( plot_points_file_format_ == laz_suffix ) {
				options.add( "compression",	"laszip" );
				plot_points_file_format_	  = las_suffix;
			}

			pdal::BufferReader		reader;
			reader.addView( points );

			// StageFactory always "owns" the stages it creates. They'll be destroyed with the factory.
			pdal::StageFactory		factory;
			// Next line can not be "writers.laz", but must in that case be "writers.las".
			pdal::Stage			  * writer = factory.createStage( std::string( "writers" ) + plot_points_file_format_ );
			writer->setInput( reader );
			writer->setOptions( options );
			writer->prepare( points->table() );
			writer->execute( points->table() );

		} catch( const std::exception & error_ ) {
			throw error_message( std20::format( "Plot_stuff: {}. (Saving point cloud of plot '{}' to file '{}'.)",
				error_.what(), m_id, to_string( plot_points_dest )
			) );
		}
	}

}	// namespace pax
