#include <pax/pdal/modules/pdal_plugin_filter_plot_points.hpp>
#include <pax/pdal/process/plot-points.hpp>

#include <pdal/util/FileUtils.hpp>
#include <pdal/util/ProgramArgs.hpp>


namespace pax {

	static pdal::PluginInfo const s_info {
		"filters.plot_points",
		"Save all points within plots [plus an optional buffer] to individual files. "
		"Execute 'pdal --options filters.plot_points' for a list of available parameters. ",
		"<no link yet>"		// Example: "http://pdal.io/stages/writers.text.html"
	};

	CREATE_SHARED_STAGE( plot_points, s_info )
	std::string plot_points::getName()	const	{	return s_info.name;		}



	void plot_points::addArgs( pdal::ProgramArgs & args ) {
		
		// setPositional() Makes the argument required.
		args.add( "plot_file", 			"File path to a csv-type file with at least the required columns 'north', 'east', 'radius' and 'id'. "
										"Plots not entirely within the point cloud bounding box are ignored. ",
											m_plot_file ).setPositional();
		args.add( "dest_plot_points",	"Destination path (to a directory) for the plot point cloud files. ", m_dest_plot_points ).setPositional();
		args.add( "dest_format",		"File format to use for resulting point clud files (e.g '.laz'). ", m_dest_format, m_dest_format );
		args.add( "id_column",			"In what column to find the [unique] plot id, to use as destination file name. ", m_id_column, m_id_column );
		args.add( "max_distance",		"How much to enlarge the plot diameters. A zero value (the default) will use the plot diameter. ",
											m_buffer, m_buffer );
	}



	pdal::PointViewSet plot_points::run( pdal::PointViewPtr view_ ) {
		pdal::PointViewSet						result;
		try {
			if( m_buffer > 0 ) {

				Process_plots_points::process( view_, m_plot_file, m_dest_plot_points, m_buffer, m_id_column, m_dest_format );

				// Export metadata.
				pdal::MetadataNode				metrics_node( "plot_points" );		
				pdal::MetadataNode				meta = getMetadata();
				meta.add( metrics_node );
				meta.add( "points-in",			view_->size() );
		
				pdal::MetadataNode				arguments( "arguments" );
				arguments.add( "plot_file",		to_string( m_plot_file ) );
				arguments.add( "dest_plot_points",	to_string( m_dest_plot_points ) );
				arguments.add( "max_distance",	m_buffer );
				meta.add( arguments );
			}
			result.insert( view_ );

		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;
		}
		return result;
	}


} // namespace pdal
