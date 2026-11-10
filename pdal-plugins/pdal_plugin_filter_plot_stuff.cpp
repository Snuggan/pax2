#include <pax/pdal/modules/pdal_plugin_filter_plot_stuff.hpp>
#include <pax/pdal/utilities/pdal.hpp>


// pdal
// #include <pdal/util/FileUtils.hpp>
// #include <pdal/util/ProgramArgs.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>
#if __has_include( <pdal/GDALUtils.hpp> )	// PDAL 2.1
#	include <pdal/GDALUtils.hpp>
#else										// PDAL 2.2
#	include <pdal/private/gdal/GDALError.hpp>
#	include <pdal/private/gdal/GDALUtils.hpp>
#	include <pdal/private/gdal/Raster.hpp>
#endif

#include <pax/reporting/debug.hpp>
constexpr auto log_level = pdal::LogLevel::Debug;
#define DEBUG0 if( debug_active ) pax::Debug{}
#define DEBUG  log()->get( log_level )


#include <unordered_map>
#include <unordered_set>



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

	bool debug_active{ false };


	plot_stuff::~plot_stuff() {}
	

	// This is what prevents this filter to be streaming. 
	// I can not get the metadata/headerinfo of the files to be processed...
	void plot_stuff::setting_needs_PointView( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::setting_needs_PointView start\n";
		m_view_ptr						  = view_ptr_;
		m_plots							  = get_plots( view_ptr_ );
		DEBUG << "plot_stuff::setting_needs_PointView end\n";
	}


	void plot_stuff::addArgs( pdal::ProgramArgs & args ) {
		DEBUG << "plot_stuff::addArgs start\n";
		debug_active				  =	( log()->getLevel() >= log_level );
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
		DEBUG << "plot_stuff::addArgs end\n";
	}


	// Read a plot file and return a vector of the plots in the bbox.
	std::vector< Plot_w_points > plot_stuff::get_plots( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::get_plots start\n";
		using Plot_w_id					  = Circle_w_id< double, 2 >;

		const auto							bbox = box( *view_ptr_ );
		std::vector< Plot_w_points >		plots{};

		// First, read in all plots from the csv file.
		if( ( do_metrics() || do_points() ) && !empty( bbox ) ) {	
			const auto height_dim		  = view_ptr_->hasDim( pdal::Dimension::Id::HeightAboveGround )
									  	  ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z;
			const bool has_return_number  = view_ptr_->hasDim( pdal::Dimension::Id::ReturnNumber );
			m_all_plots_table			  =	Text_table< char >{ m_plot_file };
			std::vector< Plot_w_id >		basic_plots 
				= m_all_plots_table.export_values( Object_meta< Plot_w_id >::value );
	
			// Then, only keep the plots that overlap the bbox.
			// Copy the relevant plots to the begining of 'plots' and...
			auto itr					  = basic_plots.begin();
			for( Plot_w_id & plot : basic_plots )
				if( contains( bbox, plot ) )  * ( itr++ ) = plot;

			// ...resize it to contain just those relevant plots.
			basic_plots.resize( std::size_t( itr - basic_plots.begin() ) );
			plots.reserve( basic_plots.size() );

			// Now, create the Plot_w_points vector.
			for( Plot_w_id & plot : basic_plots ) {
				if( m_plot_buffer > 0 )		plot = Plot_w_id( center( plot ), m_plot_buffer, plot.id() );
				plots.emplace_back( plot, do_metrics(), do_points(), has_return_number, height_dim );
			}
		}
		DEBUG << "plot_stuff::get_plots end\n";
		return plots;
	}


	/// Do pre-flight stuff.
	void plot_stuff::prepared( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "plot_stuff::prepared start\n";
		log()->get( pdal::LogLevel::Debug )
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
		DEBUG << "plot_stuff::prepared end\n";
	}


	/// Do pre-flight stuff.
	void plot_stuff::ready( pdal::PointTableRef /*pt_table_*/ ) {
		DEBUG << "plot_stuff::ready start\n";
		// Check argumeents.
		if( m_plot_buffer < 0 )				m_plot_buffer = 0;;
		DEBUG << "plot_stuff::ready end\n";
	}


	bool plot_stuff::processOne( pdal::PointRef & pt_ ) {
		// DEBUG << "plot_stuff::processOne start\n";	// This generates a lot of output...
		++m_metadata.points_processed;
		for( auto & plot : m_plots )		m_metadata.points_in_plots += plot.process( pt_ );
		// DEBUG << "plot_stuff::processOne end\n";
		return true;
	}


	pdal::PointViewSet plot_stuff::run( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "plot_stuff::run start\n";
		setting_needs_PointView( view_ptr_ );

		// Process the points.
		if( do_points() || do_points() ) {
			auto pt = view_ptr_->point( 0 );
			for( pdal::PointId idx = 0; idx < view_ptr_->size(); ++idx ) {
				pt = view_ptr_->point( idx );
				processOne( pt );
			}
		}

		// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
		pdal::PointViewSet		result;
		result.insert( view_ptr_ );
		DEBUG << "plot_stuff::run end\n";
		return result;
	}


	/// Seve the metrics from each plot to a .csv file, not discarding previous columns. 
	/// It is a bit roundabout, but we need to include the other columns.
	inline void save_metrics(
		const Text_table< char >						  & plots_table_,
		const std::span< Plot_w_points >					plots_,
		const dest_path									  & plot_metrics_dest_,
		const std::span< const metrics::Function_filter >	metrics_,
		const std::string_view								id_col_name_ = "id"
	);


	void plot_stuff::done( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "plot_stuff::done start\n";
		try {
			// Calculate and save metrics.
			if( do_metrics() ) {
				DEBUG << "plot_stuff::done: do_metrics()\n";
				const auto metric_set		  = metrics::metric_set( std::span{ m_metrics }, m_metrics_nilsson );
				DEBUG << "plot_stuff::done: save_metrics()\n";
				save_metrics( m_all_plots_table, m_plots, m_metrics_dest, metric_set, m_id_column );
				DEBUG << "plot_stuff::done: saved_metrics()\n";
			}

			// Seve the plots' points.
			if( do_points() ) {
				DEBUG << "plot_stuff::done: do_points()\n";
				for( const auto & plot : m_plots )	if( plot.num_of_points() )				
					plot.save_plot_points( 
						m_view_ptr, 
						m_points_dest_dir, 
						m_points_format 
					);
			}
		} catch( const std::exception & error_ ) {
			std::cerr << error_.what() << '\n';
			throw;	// Or should we just terminate?
		}
		DEBUG << "plot_stuff::done try clause done\n";

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
		DEBUG << "plot_stuff::done end\n";
	}



	/// Process a point. Return true if it was inside the plot.
	bool Plot_w_points::process( const pdal::PointRef & pt_ ) 			noexcept		{
		// DEBUG0 << "Plot_w_points::process start";	// This generates a lot of output...
		if( contains( *this, point( pt_ ) ) ) {
			if( m_do_points )			m_points_idx.push_back( pt_.pointId() );
			if( m_do_metrics )			m_metric_agg.push_back(
				pt_.getFieldAs< coord_type >( m_height_dimension ),
				!m_has_return_number || pt_.getFieldAs<std::uint8_t>(pdal::Dimension::Id::ReturnNumber) == 1
			);
			return true;
		}
		// DEBUG0 << "Plot_w_points::process end";
		return false;
	}


	/// Save the points of each found plot.
	void Plot_w_points::save_plot_points( 
		const pdal::PointViewPtr		  & view_ptr_,
		const dir_path					  & plot_points_dest_dir_,
		std::string_view					plot_points_file_format_
	) const {
		DEBUG0 << "Plot_w_points::save_plot_points start";
		const std::filesystem::path	plot_points_dest   
			  = plot_points_dest_dir_ / ( Plot_w_id::id() + plot_points_file_format_ );
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
			throw error_message( std::format( "Plot_w_points: {}. (Saving point cloud of plot '{}' to file '{}'.)",
				error_.what(), Plot_w_id::id(), to_string( plot_points_dest )
			) );
		}
		DEBUG0 << "Plot_w_points::save_plot_points end";
	}

	
	/// Seve the metrics from each plot to a .csv file, not discarding previous columns. 
	/// It is a bit roundabout, but we need to include the other columns.
	inline void save_metrics(
		const Text_table< char >						  & plots_table_,
		const std::span< Plot_w_points >					plots_,
		const dest_path									  & plot_metrics_dest_,
		const std::span< const metrics::Function_filter >	metrics_,
		const std::string_view								id_col_name_
	) {
		DEBUG0 << "plot_stuff::save_metrics start";
		DEBUG0 << std::format( "plots_table_:       {} cols, {} rows", plots_table_.cols()(), plots_table_.rows()() );
		DEBUG0 << std::format( "plots_:             {} items", plots_.size() );
		DEBUG0 << std::format( "plot_metrics_dest_: \"{}\"", plot_metrics_dest_.native() );
		static constexpr const std::string_view	merge_items  = ";{}";	// Semicolon separator.
		std::vector< std::string >				metric_names{};
		try{
			DEBUG0 << "plot_stuff::save_metrics what metrics to calculate";
			metric_names.reserve( metrics_.size() );
			for( const auto name : metrics_ )	metric_names.push_back( to_string( name ) );
			DEBUG0 << std::format( "metrics_:           {}", metric_names );

			if( !plots_.empty() && !plot_metrics_dest_.empty() && !metrics_.empty() ) {
				const Col						id_col = Col( plots_table_.header().index( id_col_name_ ) );
				DEBUG0 << std::format( "ID column:          \"{}\" => {}", id_col_name_, id_col() );
				if( id_col() == -1u )
					throw error_message( std::format( "In the plots table, there is no column \"{}\".", id_col_name_ ) );

				// Create an unordered map of all id -> index in plots_.
				DEBUG0 << "plot_stuff::save_metrics Create an unordered map of all id -> index in plots_";
				std::unordered_map< std::string, std::size_t >		plot_id_idx{};
				for( std::size_t i{}; i<plots_.size(); ++i )		plot_id_idx.insert( { plots_[ i ].id(), i } );

				if( plot_id_idx.size() != plots_.size() ) {
					std::cerr << std::format( "plot_id_idx: {}\n", plot_id_idx );
					throw error_message( std::format( 
						"plot_id_idx should have {} elements, but only has {}. "
						"Are you sure that column \"{}\" has unique values on every row, no value ever repeated?", 
						plots_.size(), plot_id_idx.size(), id_col_name_
					) );
				}

				// Usually there are few plots in the plots_table, so we optimize by only working on those few rows.
				// Carculate a reduced Text_table, with only the rows corresponding to the plots in plots_.
				DEBUG0 << "plot_stuff::save_metrics create reduced_table";
				Text_table< char >				reduced_table{};
				{
					// Create an unordered set of all rows in plots_table_ with the same id as any item in plots_
					// and use it as a predicative. 
					std::unordered_set< std::size_t >	table_row_ok{};
					for( Row row{}; row<plots_table_.rows(); ++row ) {
						if( plot_id_idx.contains( std::string( plots_table_[ id_col, row ] ) ) ) {
							DEBUG0 << "Insert row " << row << ": " << std::string( plots_table_[ id_col, row ] );
							table_row_ok.insert( row() );
						}
					}
					if( table_row_ok.size() != plots_.size() ) {
						throw error_message( std::format( "table_row_ok should have {} elements, but has {}.", 
							plots_.size(), table_row_ok.size() ) );
					}
					const auto predicate = [ &table_row_ok ]( Row row ) { 
						return table_row_ok.contains( row() ); 
					};

					reduced_table			  = plots_table_.as_str( predicate );
				}

				if( reduced_table.rows()() != plots_.size() ) {
					pax::Debug{} << std::format( "reduced_table:\n{}", reduced_table );
					pax::Debug{} << "plots_:\n" << plots_;
					throw error_message( std::format( "Reduced table should have {} rows, but has {}.", 
						plots_.size(), reduced_table.rows()() ) );
				}

				DEBUG0 << "plot_stuff::save_metrics insert metrics into reduced_table";
				{	// Create a Text_table with the metrics and insert it into reduced_table.
					// The order of items must correspond to the order in reduced_table.
					std::stringstream			out;
					
					// Stream the header.
					if( metric_names.size() ) {
						auto					itr = metric_names.begin();
						out << std::format( "{}",   *itr );
						while( ++itr < metric_names.end() )	
							out << std::format( merge_items, *itr );
					}

					// Stream those metric rows that have actual metric values.
					const auto					begin = metrics_.begin();
					const auto					end   = metrics_.end();
					for( Row row{}; row<reduced_table.rows(); ++row ) {
						const std::size_t		plot_idx = plot_id_idx.at( std::string( reduced_table[ id_col, row ] ) );
						assert( plots_[ plot_idx ].id() == std::string( reduced_table[ id_col, row ] ) );

						auto				  & metric_aggrs = plots_[ plot_idx ].metric_aggregator();
						auto					itr   = begin;
						out << std::format( "\n{}", itr->calculate( metric_aggrs ) );
						while( ++itr < end )	out << std::format( merge_items, itr->calculate( metric_aggrs ) );
					}
					
					// Create a table and insert it into the original.
					out << '\n';
					reduced_table.insert_cols( Text_table< char >{ std::move( out ).str() } );
				}

				// Save the result. 
				DEBUG0 << "plot_stuff::save_metrics save the reduced_table to \"" << plot_metrics_dest_ << '"';
				reduced_table.save( plot_metrics_dest_ );
			}
		} catch( const std::exception & error_ ) {
			throw error_message( std::format( "{} (Metrics are {}, destination is '{}'.)", 
				error_.what(), std::span( metric_names ), to_string( plot_metrics_dest_ )
			) );
		}
		DEBUG0 << "plot_stuff::save_metrics end";
	}

#undef DEBUG
} // namespace pdal
