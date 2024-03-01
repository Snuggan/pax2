#include <pax/pdal/modules/pdal_plugin_filter_plot_metrics.hpp>
#include <pax/pdal/utilities/bbox.hpp>
#include <pax/pdal/process/plot-metrics.hpp>

#include <pdal/util/FileUtils.hpp>
#include <pdal/util/ProgramArgs.hpp>


namespace pax {

	static pdal::PluginInfo const s_info {
			"filters.plot_metrics",
			"Calculate metrics (statistics) for all points within the plots. "
			"Execute 'pdal --options filters.plot_metrics' for a list of available parameters and metrics. ",
			"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-plot_metrics.md"
	};

	CREATE_SHARED_STAGE( plot_metrics, s_info )
	std::string plot_metrics::getName()	const	{	return s_info.name;		}
	
	
	
	std::string function_filter_help() {
		std::ostringstream			stream;
		metrics::Function_filter::help( stream );
		return stream.str();
	}



	void plot_metrics::addArgs( pdal::ProgramArgs & args ) {
		
		// setPositional() Makes the argument required.
		args.add( "plot_file", 			"File path to a csv-type file with at least the required columns 'north', 'east', 'radius' and 'id'. "
										"Plots not entirely within the point cloud bounding box are ignored. ",
										m_plot_file, m_plot_file ).setPositional();
		args.add( "dest_plot_metrics",	"Destination path for the plot metric file. ", m_dest_plots ).setPositional();
		args.add( "metrics", 			function_filter_help(), m_metrics ).setPositional();
		args.add( "nilsson_level", 		"For some metrics, ignore z-values below this. ", m_nilsson, m_nilsson );
	}


	void plot_metrics::addDimensions( pdal::PointLayoutPtr layout_ ) {
		// Don't know if this is necessary, as we only use standard dimensions...
		layout_->registerDim( pdal::Dimension::Id::X );
		layout_->registerDim( pdal::Dimension::Id::Y );
		layout_->registerDim( pdal::Dimension::Id::Z );
	}



	pdal::PointViewSet plot_metrics::run( pdal::PointViewPtr view_ ) {
		pdal::PointViewSet					result;
		try {
			// Create the function-filter set. Do it here so that malformed function-filters at once.
			const auto metrics_set		  = metrics::metric_set( std::span{ m_metrics }, m_nilsson );

			// Process plots. 
			Process_plots_metrics			plots{ m_plot_file };
			plots.process( view_, bbox( view_ ) );
			plots.save( m_dest_plots, std::span( metrics_set ) );


			// Export metadata.
			pdal::MetadataNode				metrics_node( "plot_metrics" );		
			pdal::MetadataNode				meta = getMetadata();
			meta.add( metrics_node );
			meta.add( "points-in",			view_->size() );
		
			pdal::MetadataNode				arguments( "arguments" );
			arguments.add( "plot_file",		to_string( m_plot_file ) );
			arguments.add( "dest_plot_metrics",	to_string( m_dest_plots ) );
			for( const auto & metric : m_metrics )
				arguments.add( "metrics",	metric );
			arguments.add( "nilsson_level",	m_nilsson );
			meta.add( arguments );

			result.insert( view_ );

		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;
		}
		return 		    					result;
	}


} // namespace pdal
