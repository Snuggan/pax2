#include <pax/pdal/modules/pdal_plugin_filter_raster_metrics2.hpp>
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>
#include <pax/pdal/utilities/bbox_indexer.hpp>

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


static pdal::PluginInfo const s_info {
		"filters.raster_metrics2",
		"Create rasters with specified metrics (statistics). "
		"Execute 'pdal --options filters.raster_metrics' for a list of available parameters and metrics. ",
		"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-raster_metrics.md"
};

CREATE_SHARED_STAGE( pax::Raster_metrics, s_info )
std::string pax::Raster_metrics::getName()	const	{	return s_info.name;		}


std::string function_filter_help() {
	std::ostringstream			stream;
	pax::metrics::Function_filter::help( stream );
	return stream.str();
}


void pax::Raster_metrics::addArgs( pdal::ProgramArgs & args ) {
	
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
}


void pax::Raster_metrics::addDimensions( pdal::PointLayoutPtr layout_ ) {
	layout_->registerDim( pdal::Dimension::Id::X );
	layout_->registerDim( pdal::Dimension::Id::Y );
	
	// "Z" is the standard height, could be either height above ground or height above the geoid (sea level).
	// "HeightAboveGround", if available, is the height abouve ground. It is possible that both heights are available. 
	if( layout_->hasDim( pdal::Dimension::Id::HeightAboveGround ) ) {
		layout_->registerDim( pdal::Dimension::Id::HeightAboveGround );
		pr_height_dimension	  = pdal::Dimension::Id::HeightAboveGround;
	} else {
		layout_->registerDim( pdal::Dimension::Id::Z );
		pr_height_dimension	  = pdal::Dimension::Id::Z;
	}
	
	// Check if the file carry first return information. If not available, no points are treated as first returns. 
	if(( pr_has_return_number = layout_->hasDim( pdal::Dimension::Id::ReturnNumber ) )) {
		layout_->registerDim( pdal::Dimension::Id::Z );
	}
	
	// As this is called when strarting a new processing, it serves as an initiator...
	//////////////////////////////////////////////////////////////////////////////////
	
	// Create the function-filter set. Do it here so that malformed function-filters at once.
	pr_metrics_set			  = metrics::metric_set( std::span{ m_metrics }, m_nilsson );

	// Default m_alignment is m_resolution. 
	if( m_alignment <= 0.0 )	m_alignment = m_resolution;
}


pax::Raster_metrics::~Raster_metrics() {
	// Save the metrics.
#	if PAX_STREAMING
		
#	endif
}


bool pax::Raster_metrics::processOne( pdal::PointRef & /*pt_*/ ) {
	// Read one point.
	// pr_z_accumulators[ bbox.index( pt_ref ) ].push_back(
	// 	pt_ref.getFieldAs< value_type >( pr_height_dimension ),
	// 	pr_has_return_number
	// 		? pt_ref.getFieldAs< std::uint8_t >( pdal::Dimension::Id::ReturnNumber ) == 1
	// 		: false
	// );
	return true;
}


pdal::PointViewSet pax::Raster_metrics::run( pdal::PointViewPtr view_ ) {
	try {
		const PointView_indexer			bbox{ view_, m_alignment };
		{	// Process the points (accumulate the z-values of each pixel). This is the heavy lifting part!!!

			pr_z_accumulators.resize( bbox.pixels() );
			// for( const auto & pt_ref : *view_ ) 		processOne( pt_ref );
			for( const auto & pt_ref : *view_ ) {
				pr_z_accumulators[ bbox.index( pt_ref ) ].push_back( 
					pt_ref.getFieldAs< value_type >( pr_height_dimension ), 
					pr_has_return_number
						? pt_ref.getFieldAs< std::uint8_t >( pdal::Dimension::Id::ReturnNumber ) == 1 
						: false
				);
			}
		}
	} catch( const std::exception & e_ ) {
		std::cerr << "An exception was thrown: " << e_.what() << '\n';
		throw;
	}

	pdal::MetadataNode					metrics_node( "raster_metrics" );
	try {
		// Save metrics' rasters.
	    pdal::gdal::registerDrivers();
		pdal::gdal::GDALError			err;

		{	// Save one raster for each function-filter (metric).
			std::vector< value_type >	pixels;
			pixels.reserve( pr_z_accumulators.size() );
			const PointView_indexer		bbox{ view_, m_alignment };

			for( const auto metric : pr_metrics_set ) {
				const std::filesystem::path		dest{ insert_suffix( m_dest_rasters, to_string( metric ) ) };

				try {
					if( !dest.parent_path().empty() )
						std::filesystem::create_directories( dest.parent_path() );

					// Get the actual metric value for each pixel. 
					pixels.clear();
					for( auto & accumulator : pr_z_accumulators ) {
						pixels.push_back( metric.calculate( accumulator ) );
					}

					// save_metric( acc, m_dest_rasters, m_accumulators );
				    pdal::gdal::Raster	raster( dest, m_drivername, m_srs, bbox.affine_vector() );
					err				  = raster.open( bbox.cols(), bbox.rows(), 1, m_dataType, m_noData, m_options );
					if( err == pdal::gdal::GDALError::None )
						err			  = raster.writeBand( pixels.data(), m_noData, 1, to_string( metric ) );

					// Add metadata of the metrics' destination file. 
					metrics_node.add( to_string( metric ), dest.native() );
					if( err != pdal::gdal::GDALError::None )	throwError( raster.errorMsg() );
				} catch( const std::exception & e_ ) {
					throw error_message( std20::format( "{} (saving metric to raster file {})", e_.what(), dest.native() ) );
				}
			}
		}
	} catch( const std::exception & e_ ) {
		std::cerr << "An exception was thrown: " << e_.what() << '\n';
		throw;
	}


	pdal::PointViewSet					result;
	try {
		// Export metadata.
		pdal::MetadataNode				meta = getMetadata();
		meta.add( metrics_node );
		meta.add( "points-in",			view_->size() );
		meta.add( "has-ReturnNumber",	pr_has_return_number );
		meta.add( "height-dimension",	view_->hasDim( pdal::Dimension::Id::HeightAboveGround ) 
												? "HeightAboveGround" : "Z" );
	
		pdal::MetadataNode				arguments( "arguments" );
		arguments.add( "alignment",		m_alignment );
		arguments.add( "data_type",		interpretationName( m_dataType ) );
		arguments.add( "dest_raster",	m_dest_rasters );
		arguments.add( "gdaldriver",	m_drivername );
		for( const auto & option : m_options )
			arguments.add( "gdalopts",	option );
		for( const auto & metric : m_metrics )
			arguments.add( "metrics",	metric );
		arguments.add( "nilsson_level",	m_nilsson );
		arguments.add( "nodata",		m_noData );
		arguments.add( "resolution",	m_resolution );
		meta.add( arguments );

		result.insert( view_ );

	} catch( const std::exception & e_ ) {
		std::cerr << "An exception was thrown: " << e_.what() << '\n';
		throw;
	}
	return 		    					result;
}
