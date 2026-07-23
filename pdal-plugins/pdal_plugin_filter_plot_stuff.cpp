#include <pax/pdal/modules/pdal_plugin_filter_plot_stuff.hpp>
#include <pax/types/point-stuff/pdal.hpp>
#include <pax/pdal/utilities/plot-stuff.hpp>
#include <pax/types/point-stuff/pdal.hpp>

#include <pdal/util/FileUtils.hpp>
#include <pdal/util/ProgramArgs.hpp>

#include <vector>


// #include <pax/reporting/debug.hpp>
// #define DEBUG Debug{}
#define DEBUG log()->get( pdal::LogLevel::Debug )


static pdal::PluginInfo const s_info {
	"filters.plot_stuff",
	"For each plot within a point cloud file, save the plot's points to a specified file "
	"and also a .csv file containing the metrics for the points of respective plot. "
	"You can specify a buffer so as to also include points near plots. "
	"Execute 'pdal --options filters.plot_stuff' for a list of available parameters. ",
	"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-plot_stuff.md"
};

CREATE_SHARED_STAGE( pax::plot_stuff, s_info )
std::string pax::plot_stuff::getName()	const	{	return s_info.name;		}


namespace pax {


	// This is what prevents this filter to be streaming. 
	// I can not get the metadata/headerinfo of the files to be processed...
	void plot_stuff::setting_needs_PointView( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::setting_needs_PointView start";

		m_view_ptr						  = view_ptr_;
		m_plots							  = get_plots( view_ptr_ );

		DEBUG << "plot_stuff::setting_needs_PointView end";		
	}


	void plot_stuff::addArgs( pdal::ProgramArgs & args ) {
		DEBUG << "plot_stuff::addArgs start";

		std::ostringstream				metrics_help_stream;
		metrics::Function_filter::help( metrics_help_stream );

		// setPositional() Makes the argument required.
		args.add( "plot_file", 			"File path to a csv-type file with at least the required columns 'north', 'east', "
										"'radius' and 'id'. Plots not entirely within the point cloud bounding box are ignored. ",
											m_plot_file ).setPositional();
		args.add( "plot_points_dest",	"Destination path (to a directory) for the plot point cloud files. ", 
											m_points_dest_dir ).setPositional();
		args.add( "plot_metrics_dest",	"Destination path (to a file) for the plot metrics csv files. ", 
											m_metrics_dest ).setPositional();
		args.add( "metrics", 			metrics_help_stream.str(), m_metrics ).setPositional();
		args.add( "nilsson_level", 		"For some metrics, ignore z-values below this. ", m_metrics_nilsson, m_metrics_nilsson );
		args.add( "points_format",		"File format to use for resulting plot point clud files (e.g '.laz'). ", 
											m_points_format, m_points_format );
		args.add( "id_column",			"In what column to find the [unique] plot id, to use as destination file name. ", 
											m_id_column, m_id_column );
		args.add( "plot_buffer",		"How much to enlarge the plot diameters. A zero value (the default) will use the plot diameter. ",
											m_plot_buffer, m_plot_buffer );
		DEBUG << "plot_stuff::addArgs end";
	}


	plot_stuff::~plot_stuff() {}


	/// Return a bounding box for the point cloud.
	inline pdal::BOX2D bbox( const pdal::PointView & view_ ) noexcept {
		pdal::BOX2D					box_;
		view_.calculateBounds( box_ );
		return box_;
	}


	// Read a plot file and return a vector of the plots in the bbox.
	std::vector< Plot_stuff > plot_stuff::get_plots( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::get_plots start";

		const auto							bbox = pax::bbox( *view_ptr_ );
		// const pdal::PointLayoutPtr 			layout = pt_table_.layout();
		const auto height_dim			  = view_ptr_->hasDim( pdal::Dimension::Id::HeightAboveGround )
									  	  ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z;
		const bool has_return_number	  = view_ptr_->hasDim( pdal::Dimension::Id::ReturnNumber );

		std::vector< Plot_stuff >			plots{};

		// First, read in all plots from the csv file.
		if( !empty( bbox ) ) {	
			m_all_plots_table			  =	Text_table< char >{ m_plot_file };
			std::vector< Plot_w_id >		basic_plots = m_all_plots_table.export_values( Plot_w_id::table_meta( m_id_column ) );
		
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
					!m_metrics_dest.empty(), 
					!m_points_dest_dir.empty(), 
					has_return_number, 
					height_dim 
				);
			}
		}

		DEBUG << "plot_stuff::get_plots end";
		return plots;
	}


	/// Do pre-flight stuff.
	void plot_stuff::prepared( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "plot_stuff::prepared start";
		DEBUG
			<< "\n\tPlot stuff arguments:" 
			<< "\n\tplot_file:         " << m_plot_file
			<< "\n\tplot_points_dest:  " << m_points_dest_dir
			<< "\n\tplot_metrics_dest: " << m_metrics_dest
			<< "\n\tnilsson_level:     " << m_metrics_nilsson
			<< "\n\tpoints_format:     " << m_points_format
			<< "\n\tid_column:         " << m_id_column
			<< "\n\tplot_buffer:       " << m_plot_buffer
			<< "\n\tmetrics:           " << std::format( "{}", m_metrics )
			<< "\n";
		DEBUG << "plot_stuff::prepared end";
	}


	/// Do pre-flight stuff.
	void plot_stuff::ready( pdal::PointTableRef /*pt_table_*/ ) {
		DEBUG << "plot_stuff::prepared start";

		// Check argumeents.
		if( m_plot_buffer < 0 )		std::cerr
			<< error_message( "To export points in plots the radius needs to be non-negative." ).what() 
			<< '\n';

		DEBUG << "plot_stuff::prepared end";
	}


	bool plot_stuff::processOne( pdal::PointRef & pt_ ) {
		++m_metadata.points_processed;
		for( auto & plot : m_plots )		m_metadata.points_in_plots += plot.process( pt_ );
		return true;
	}


	// void plot_stuff::filter( pdal::PointView & ) {
	// 	DEBUG << "plot_stuff::filter start";
	// 	DEBUG << "plot_stuff::filter end";
	// }


	pdal::PointViewSet plot_stuff::run( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::run start";

		setting_needs_PointView( view_ptr_ );

		// Process the points.
		auto pt = view_ptr_->point( 0 );
		for( pdal::PointId idx = 0; idx < view_ptr_->size(); ++idx ) {
			pt = view_ptr_->point( idx );
			processOne( pt );
		}

		// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
		pdal::PointViewSet		result;
		result.insert( view_ptr_ );
		return result;

		DEBUG << "plot_stuff::run end";
	}


	void plot_stuff::done( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "plot_stuff::done start";

		try {
			const auto metric_set		  = metrics::metric_set( std::span{ m_metrics }, m_metrics_nilsson );
			save_metrics( m_all_plots_table, m_plots, m_metrics_dest, metric_set, m_id_column );
			for( const auto & plot : m_plots )	if( !plot.empty() )				
				plot.save_plot_points( 
					m_view_ptr, 
					m_points_dest_dir, 
					m_points_format 
				);
		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;	// Or should we just terminate?
		}

		// Export metadata.
		pdal::MetadataNode					meta = getMetadata();

		pdal::MetadataNode					arguments( "arguments" );
		arguments.add( "plot_file",			to_string( m_plot_file ) );
		arguments.add( "plot_points_dest",	to_string( m_points_dest_dir ) );
		arguments.add( "plot_metrics_dest",	to_string( m_metrics_dest ) );
		for( const auto & metric : m_metrics )	arguments.add( "metrics",	metric );
		arguments.add( "nilsson_level", 	m_metrics_nilsson );
		arguments.add( "points_format",		m_points_format );
		arguments.add( "id_column",			m_id_column );
		arguments.add( "plot_buffer",		m_plot_buffer );
		meta.add( arguments );

		pdal::MetadataNode					result( "result" );
		result.add( "plots-processed",		m_plots.size() );
		result.add( "points-processed",		m_metadata.points_processed );
		result.add( "points-in-plots",		m_metadata.points_in_plots );
		meta.add( result );

		DEBUG << "plot_stuff::done end";
	}


} // namespace pdal
