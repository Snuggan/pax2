#include <pax/pdal/modules/pdal_plugin_filter_raster_metrics.hpp>
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>
#include <pax/std/file.hpp>


// pdal
#include <pdal/util/FileUtils.hpp>
#include <pdal/util/ProgramArgs.hpp>
#if __has_include( <pdal/GDALUtils.hpp> )
	// PDAL 2.1
#	include <pdal/GDALUtils.hpp>
#else
	// PDAL 2.2
#	include <pdal/private/gdal/GDALError.hpp>
#	include <pdal/private/gdal/GDALUtils.hpp>
#	include <pdal/private/gdal/Raster.hpp>
#endif


// #include <pax/reporting/debug.hpp>
// #define DEBUG Debug{}
#define DEBUG log()->get(pdal::LogLevel::Debug)


static pdal::PluginInfo const s_info {
	"filters.raster_metrics",
	"Create rasters with specified metrics (statistics). "
	"Execute 'pdal --options filters.raster_metrics' for a list of available parameters and metrics. ",
	"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-raster_metrics.md"
};

CREATE_SHARED_STAGE( pax::raster_metrics, s_info )
std::string pax::raster_metrics::getName()	const	{	return s_info.name;		}


std::string function_filter_help() {
	std::ostringstream			stream;
	pax::metrics::Function_filter::help( stream );
	return stream.str();
}


namespace pax {

	// This is what prevents this filter to be streaming. 
	// I can not get the metadata/headerinfo of the files to be processed...
	void raster_metrics::setting_needs_PointView( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "raster_metrics::setting_needs_PointView start";

		pr_bbox	  = Bbox_indexer{ *view_ptr_, m_alignment };
		pr_z_accumulators.resize( pr_bbox.pixels() );

		DEBUG << "raster_metrics::setting_needs_PointView end";		
	}


	void raster_metrics::addArgs( pdal::ProgramArgs & args ) {
		DEBUG << "raster_metrics::addArgs start";
		// setPositional() Makes the argument required.
		args.add( "dest", 
			"Destination directory and file name template for the metric raster files. "
			"(An empty file with the file name template and the name of the metric as the suffix is created with the metric files. "
			"It can be used by 'make' and similar tools as a target file.) "
			"Execute 'pdal --options filters.raster_metrics' for a list of available metrics. ",
			m_dest_rasters ).setPositional();
		args.add( "resolution",			"Size of pixels in the metric rasters. ", m_resolution, m_resolution );
		args.add( "metrics", 			function_filter_help(), m_metrics );
		args.add( "nilsson_level", 		"For some metrics, ignore z-values below this. ", m_nilsson, m_nilsson );
		args.add( "alignment", 			"Raster alignment, raster corner will be aligned. ", m_alignment, m_alignment );
		args.add( "gdaldriver", 		"GDAL writer driver name", m_drivername, m_drivername );
		args.add( "gdalopts", 			"GDAL driver options (name=value,name=value...)", m_options );
		args.add( "data_type", 			"Data type for output raster (\"int8\", \"uint64\", \"float\", etc.)", m_dataType, m_dataType );
		args.add( "nodata", 			"No data value, a sentinal value to say that no value was set for nodata", m_noData, m_noData );
		DEBUG << "raster_metrics::addArgs end";
	}


	/// Do pre-flight stuff.
	void raster_metrics::prepared( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "raster_metrics::prepared start";
		DEBUG
			<< "\n\tRaster metrics arguments:" 
			<< "\n\talignment:         " << m_alignment 
			<< "\n\tdata_type:         " << interpretationName( m_dataType ) 
			<< "\n\tdest_raster:       " << m_dest_rasters 
			<< "\n\tgdaldriver:        " << m_drivername 
			<< "\n\tnilsson_level:     " << m_nilsson 
			<< "\n\tnodata:            " << m_noData 
			<< "\n\tresolution:        " << m_resolution
			<< "\n\tgdalopts:          " << std::format( "{}", m_options )
			<< "\n\tmetrics:           " << std::format( "{}", m_metrics )
			<< "\n";
		DEBUG << "raster_metrics::prepared end";
	}
	
	
	/// This is called just before the pipeline is executed. 
	void raster_metrics::ready( pdal::PointTableRef table_ ) {
		DEBUG << "raster_metrics::ready start";

		// "Z" is the standard height, could be either height above ground or height above the geoid (sea level).
		// "HeightAboveGround", if available, is the height abouve ground. It is possible that both heights are available. 
		const pdal::PointLayoutPtr	layout = table_.layout();
		pr_height_dimension		  = layout->hasDim( pdal::Dimension::Id::HeightAboveGround )
								  ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z;
		// Check if the file carry first return information. If not available, no points are treated as first returns. 
		pr_has_return_number	  = layout->hasDim( pdal::Dimension::Id::ReturnNumber );
		
		// Get the spatial reference system, it is needed when we save metric rasters.
		m_srs					  = table_.spatialReference();

		// Create the function-filter set. Do it here so that malformed function-filters at once.
		pr_metrics_set			  = metrics::metric_set( std::span{ m_metrics }, m_nilsson );

		// Default m_alignment is m_resolution. 
		if( m_alignment <= 0.0 )	m_alignment = m_resolution;

		DEBUG << "raster_metrics::ready end";
	}


	bool raster_metrics::processOne( pdal::PointRef & pt_ ) {
		// Process a point (accumulate the z-values of all pixels). 
		pr_z_accumulators[ pr_bbox.index( pt_ ) ].push_back( 
			pt_.getFieldAs< value_type >( pr_height_dimension ), 
			pr_has_return_number
				? pt_.getFieldAs< std::uint8_t >( pdal::Dimension::Id::ReturnNumber ) == 1 
				: false
		);
		++m_metadata.points_processed;
		return true;
	}


	// void raster_metrics::filter( pdal::PointView & view_ ) {
	// 	DEBUG << "raster_metrics::filter start";
	// 	DEBUG << "raster_metrics::filter end";
	// }


	pdal::PointViewSet raster_metrics::run( pdal::PointViewPtr view_ptr_ ) {
		DEBUG << "raster_metrics::run start";

		setting_needs_PointView( view_ptr_ );

		// Process the points (accumulate the z-values of each pixel). This is the heavy lifting part!!!
		auto pt = view_ptr_->point( 0 );
		for( pdal::PointId idx = 0; idx < view_ptr_->size(); ++idx ) {
			pt = view_ptr_->point( idx );
			processOne( pt );
		}

		// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
		pdal::PointViewSet		result;
		result.insert( view_ptr_ );
		return result;

		DEBUG << "raster_metrics::run end";
	}


	void raster_metrics::done( pdal::PointTableRef /*table_*/ ) {
		DEBUG << "raster_metrics::done start";
		// Save metrics' rasters.
	    pdal::gdal::registerDrivers();
		pdal::gdal::GDALError			err;

		// Save one raster for each function-filter (metric).
		std::vector< value_type >		pixels;
		pixels.reserve( pr_z_accumulators.size() );

		for( const auto metric : pr_metrics_set ) {
			const std::filesystem::path	dest{ insert_suffix( m_dest_rasters, to_string( metric ) ) };
			try {
				if( !dest.parent_path().empty() )
					std::filesystem::create_directories( dest.parent_path() );

				// Get the actual metric value for each pixel. 
				pixels.clear();
				for( auto & accumulator : pr_z_accumulators ) {
					pixels.push_back( metric.calculate( accumulator ) );
				}

				// save_metric( acc, m_dest_rasters, m_accumulators );
			    pdal::gdal::Raster	raster( dest, m_drivername, m_srs, pr_bbox.affine_vector() );
				err					  = raster.open( pr_bbox.cols(), pr_bbox.rows(), 1, m_dataType, m_noData, m_options );
				if( err == pdal::gdal::GDALError::None )
					err				  = raster.writeBand( pixels.data(), m_noData, 1, to_string( metric ) );

				// Add metadata of the metrics' destination file. 
				if( err != pdal::gdal::GDALError::None )	throwError( raster.errorMsg() );
			} catch( const std::exception & e_ ) {
				throw error_message( std20::format( "{} (saving metric to raster file {})", e_.what(), dest.native() ) );
			}
		}
	
		// Export metadata.
		pdal::MetadataNode				meta = getMetadata();
		meta.add( "points-in",			m_metadata.points_processed );
		meta.add( "has-ReturnNumber",	pr_has_return_number );
		meta.add( "height-dimension",	( pr_height_dimension == pdal::Dimension::Id::HeightAboveGround ) 
												? "HeightAboveGround" : "Z" );

		pdal::MetadataNode				arguments( "arguments" );
		arguments.add( "alignment",		m_alignment );
		arguments.add( "data_type",		interpretationName( m_dataType ) );
		arguments.add( "dest_raster",	m_dest_rasters );
		arguments.add( "gdaldriver",	m_drivername );
		for( const auto & option : m_options )	arguments.add( "gdalopts",	option );
		for( const auto & metric : m_metrics )	arguments.add( "metrics",	metric );
		arguments.add( "nilsson_level",	m_nilsson );
		arguments.add( "nodata",		m_noData );
		arguments.add( "resolution",	m_resolution );
		meta.add( arguments );

		pdal::MetadataNode				metrics_node( "raster_metrics" );
		for( const auto metric : pr_metrics_set ) {
			const auto					dest = to_string( insert_suffix( m_dest_rasters, to_string( metric ) ) );
			metrics_node.add( to_string( metric ), dest );
		}
		meta.add( metrics_node );
		DEBUG << "raster_metrics::done end";
	}

}	// namespace pax
