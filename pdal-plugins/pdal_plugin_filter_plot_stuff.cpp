#include <pax/pdal/modules/pdal_plugin_filter_plot_stuff.hpp>

#include <pax/pdal/utilities/plot-stuff.hpp>

#include <pdal/util/FileUtils.hpp>
#include <pdal/util/ProgramArgs.hpp>

#include <vector>


namespace pax {

	static pdal::PluginInfo const s_info {
		"filters.plot_stuff",
		"Save all points within plots [plus an optional buffer] to individual files. "
		"Execute 'pdal --options filters.plot_stuff' for a list of available parameters. ",
		"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-plot_stuff.md"
	};

	CREATE_SHARED_STAGE( plot_stuff, s_info )
	std::string plot_stuff::getName()	const	{	return s_info.name;		}



	void plot_stuff::addArgs( pdal::ProgramArgs & args ) {
		std::ostringstream				metrics_help_stream;
		metrics::Function_filter::help( metrics_help_stream );

		// setPositional() Makes the argument required.
		args.add( "plot_file", 			"File path to a csv-type file with at least the required columns 'north', 'east', "
										"'radius' and 'id'. Plots not entirely within the point cloud bounding box are ignored. ",
											m_plot_file ).setPositional();
		args.add( "plot_points_dest",	"Destination path (to a directory) for the plot point cloud files. ", 
											m_points_dest_directory ).setPositional();
		args.add( "plot_metrics_dest",	"Destination path (to a directory) for the plot metric csv files. ", 
											m_metrics_dest_directory ).setPositional();
		args.add( "metrics", 			metrics_help_stream.str(), m_metrics ).setPositional();
		args.add( "nilsson_level", 		"For some metrics, ignore z-values below this. ", m_metrics_nilsson, m_metrics_nilsson );
		args.add( "points_format",		"File format to use for resulting plot point clud files (e.g '.laz'). ", 
											m_points_format, m_points_format );
		args.add( "id_column",			"In what column to find the [unique] plot id, to use as destination file name. ", 
											m_points_id_column, m_points_id_column );
		args.add( "plot_buffer",		"How much to enlarge the plot diameters. A zero value (the default) will use the plot diameter. ",
											m_plot_buffer, m_plot_buffer );
	}


	plot_stuff::~plot_stuff() {
		try {
			const auto metric_set		  = metrics::metric_set( std::span{ m_metrics }, m_metrics_nilsson );
			save_metrics( m_all_plots_table, m_plots, m_metrics_dest_directory, metric_set );
			for( const auto & plot : m_plots )	if( !plot.empty() )				
				plot.save_plot_points( 
					m_view_ptr, 
					m_points_dest_directory, 
					m_points_format 
				);
		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;	// Or should we just terminate?
		}

		// Export metadata.
		pdal::MetadataNode					meta = getMetadata();
		meta.add( "plots-processed",		m_plots.size() );
		meta.add( "points-processed",		m_metadata.points_processed );
		meta.add( "points-found",			m_metadata.points_in_plots );

		pdal::MetadataNode					arguments( "arguments" );
		arguments.add( "plot_file",			to_string( m_plot_file ) );
		arguments.add( "plot_points_dest",	to_string( m_points_dest_directory ) );
		arguments.add( "plot_metrics_dest",	to_string( m_metrics_dest_directory ) );
		for( const auto & metric : m_metrics )	arguments.add( "metrics",	metric );
		arguments.add( "nilsson_level", 	m_metrics_nilsson );
		arguments.add( "points_format",		m_points_format );
		arguments.add( "id_column",			m_points_id_column );
		arguments.add( "plot_buffer",		m_plot_buffer );
		meta.add( arguments );
	}


	// Read a plot file and return a vector of the plots in the bbox.
	template< typename Point_table >
	std::vector< Plot_stuff > plot_stuff::get_plots( Point_table & pt_table_ ) {
		const pdal::PointLayoutPtr 			layout = pt_table_.layout();
		const auto							bbox = pax::bbox( pt_table_ );
		const auto height_dim			  = layout->hasDim( pdal::Dimension::Id::HeightAboveGround )
									  	  ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z;
		const bool has_return_number	  = layout->hasDim( pdal::Dimension::Id::ReturnNumber );

		std::vector< Plot_stuff >			plots{};

		// First, read in all plots from the csv file.
		if( !empty( bbox ) ) {	
			m_all_plots_table			  =	Text_table< char >{ m_plot_file };
			std::vector< Plot_w_id >		basic_plots = m_all_plots_table.export_values( Plot_w_id::table_meta( m_points_id_column ) );
		
			// Then, only keep the plots that overlap the bbox.
			// Copy the relevant plots to the begining of 'plots' and...
			auto itr					  = basic_plots.begin();
			for( Plot_w_id & plot : basic_plots )
				if( plot.in_box( bbox ) )  * ( itr++ ) = plot;

			// ...resize it to contain just those relevant plots.
			basic_plots.resize( std::size_t( itr - basic_plots.begin() ) );
			plots.reserve( basic_plots.size() );

			// Now, create the Plot_stuff vector.
			for( Plot_w_id & plot : basic_plots ) {
				if( m_plot_buffer > 0 )		plot.set_radius( m_plot_buffer );
				plots.emplace_back(
					plot, 
					!m_metrics_dest_directory.empty(), 
					!m_points_dest_directory.empty(), 
					has_return_number, 
					height_dim 
				);
			}
		}

		return plots;
	}


	/// Do pre-flight stuff.
	void plot_stuff::prepared( pdal::PointTableRef pt_table_ ) {
		m_view_ptr						  = std::make_shared< pdal::PointView >( pt_table_ );
		m_plots							  = get_plots( pt_table_ );
		

		// Check argumeents.
		if( m_plot_buffer < 0 )		std::cerr
			<< error_message( "To export points in plots the radius needs to be non-negative." ).what() 
			<< '\n';
	}


	bool plot_stuff::processOne( pdal::PointRef & pt_ ) {
		++m_metadata.points_processed;
		for( auto & plot : m_plots )		m_metadata.points_in_plots += plot.process( pt_ );
		return true;
	}


	pdal::PointViewSet plot_stuff::run( pdal::PointViewPtr view_ ) {
		// Process the points.
		auto pt = view_->point( 0 );
		for( pdal::PointId idx = 0; idx < view_->size(); ++idx ) {
			pt = view_->point( idx );
			processOne( pt );
		}

		pdal::PointViewSet					result;
		result.insert( view_ );
		return result;
	}


} // namespace pdal
