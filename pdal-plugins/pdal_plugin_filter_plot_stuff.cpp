#include <pax/pdal/modules/pdal_plugin_filter_plot_stuff.hpp>
#include <pax/tables/text-table.hpp>			// Handle a csv file.

#include <pax/pdal/process/plot-points.hpp>

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
		// setPositional() Makes the argument required.
		args.add( "plot_file", 			"File path to a csv-type file with at least the required columns 'north', 'east', "
										"'radius' and 'id'. Plots not entirely within the point cloud bounding box are ignored. ",
											m_plot_file ).setPositional();
		args.add( "dest_plot_points",	"Destination path (to a directory) for the plot point cloud files. ", 
											m_dest_plot_points_directory ).setPositional();
		args.add( "dest_format",		"File format to use for resulting point clud files (e.g '.laz'). ", 
											m_dest_format, m_dest_format );
		args.add( "id_column",			"In what column to find the [unique] plot id, to use as destination file name. ", 
											m_id_column, m_id_column );
		args.add( "max_distance",		"How much to enlarge the plot diameters. A zero value (the default) will use the plot diameter. ",
											m_buffer, m_buffer );
	}


	plot_stuff::~plot_stuff() {
		// Export metadata.
		pdal::MetadataNode					meta = getMetadata();
		meta.add( "plots-processed",		m_metadata.plots_processed );
		meta.add( "points-processed",		m_metadata.points_processed );
		meta.add( "points-found",			m_metadata.points_found );

		pdal::MetadataNode					arguments( "arguments" );
		arguments.add( "plot_file",			to_string( m_plot_file ) );
		arguments.add( "dest_plot_points",	to_string( m_dest_plot_points_directory ) );
		arguments.add( "max_distance",		m_buffer );
		meta.add( arguments );
		
		try {
			for( const auto & plot : m_plots )	if( !plot.empty() )				
				plot.save( m_pt_view_ptr, m_dest_plot_points_directory, m_dest_format );
		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;	// Or should we just terminate?
		}
	}


	// Read a plot file and return a vector of the plots in the bbox.
	template< typename BBox >
	std::vector< Plot_points > plot_stuff::get_plots(
		const std::string_view				plots_table_, 
		const std::string_view 				id_column_,
		const coordinate_type				buffer_,
		const BBox						  & bbox_
	) {
		std::vector< Plot_points >			plots{};

		// First, read in all plots from the csv file.
		if( !empty( bbox_ ) ) {	
			Text_table< char >				plots_table{ plots_table_ };
			plots						  = plots_table.export_values( Plot_points::table_meta( id_column_ ) );
		}
		
		// Then, only keep the plots that overlap the bbox_.
		// Copy the relevant plots to the begining of 'plots' and...
		auto itr						  = plots.begin();
		for( Plot_points & plot : plots )
			if( plot.in_box( bbox_ ) )	  * ( itr++ ) = plot;

		// ...resize it to contain just those relevant plots.
		plots.resize( std::size_t( itr - plots.begin() ) );
		plots.shrink_to_fit();

		// If so stated, set the buffer to the plots.
		if( buffer_ > 0 ) for( Plot_points & plot : plots )
			plot.set_radius( buffer_ );

		return plots;
	}


	/// Do pre-flight stuff.
	void plot_stuff::prepared( pdal::PointTableRef pt_table_ ) {
		m_plots							  = get_plots( m_plot_file, m_id_column, m_buffer, bbox( pt_table_ ) );
		m_metadata.plots_processed		  = m_plots.size();
		m_pt_view_ptr					  = std::make_shared< pdal::PointView >( pt_table_ );
		

		// Check argumeents.
		if( m_buffer < 0 )		std::cerr
			<< error_message( "To export points in plots the radius needs to be non-negative." ).what() 
			<< '\n';
	}


	bool plot_stuff::processOne( pdal::PointRef & pt_ ) {
		++m_metadata.points_processed;
		for( auto & plot : m_plots )		m_metadata.points_found += plot.process( pt_ );
		return true;
	}


	pdal::PointViewSet plot_stuff::run( pdal::PointViewPtr view_ ) {
		// Process the points.
		auto pt = view_->point( 0 );
		for( pdal::PointId idx = 0; idx < view_->size(); ++idx ) {
			pt = view_->point( idx );
			processOne( pt );
		}

		pdal::PointViewSet				result;
		result.insert( view_ );
		return result;
	}


} // namespace pdal
