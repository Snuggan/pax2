#include <pax/pdal/modules/pdal_plugin_filter_remove_overlap.hpp>
#include <pax/pdal/utilities/bbox_indexer.hpp>

#include <pdal/pdal_internal.hpp>


static pdal::PluginInfo const s_info {
	"filters.remove_overlap",
	"Removes overlap by only accepting points with the same source id as the pixel's source id. "
	"The pixel's source id is the source id of the point with the smallest scan angle within the pixel "
	"(so different pixels might have different source id). " 
	"Execute 'pdal --options filters.remove_overlap' for a list of available parameters. ",
	"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-remove_overlap.md"
};

CREATE_SHARED_STAGE( pax::Remove_overlap, s_info )
std::string pax::Remove_overlap::getName()		const	{	return s_info.name;		}


void pax::Remove_overlap::addArgs( pdal::ProgramArgs & args_ ) {
	args_.add(	"overlap_resolution", 
				"The pixel size. " 
				"The filter removes overlap by only accepting points with the same source id as the pixel's source id. "
				"The pixel's source id is the source id of the point with the smallest scan angle in the poixel "
				"(so different pixels might have different source id)." 
				"You probably do not want overlap filtering on photogrammetry files. ",
				m_overlap_resolution, m_overlap_resolution
	);
}


void pax::Remove_overlap::addDimensions( pdal::PointLayoutPtr layout_ ) {
	// Don't know if this is necessary, as we only use standard dimensions...
	layout_->registerDim( pdal::Dimension::Id::PointSourceId );
	layout_->registerDim( pdal::Dimension::Id::ScanAngleRank );
	layout_->registerDim( pdal::Dimension::Id::X );
	layout_->registerDim( pdal::Dimension::Id::Y );
}




pdal::PointViewSet pax::Remove_overlap::run( pdal::PointViewPtr view_ ) {
	// Produce new point cloud with correct size (with empty points).
	const pdal::PointViewPtr		filtered{ view_->size() ? overlap_filter( view_ ) : view_ };
	
	// Create metadata.
	pdal::MetadataNode				meta = getMetadata();
	meta.add( "points-in",			view_->size() );
	meta.add( "points-out",			filtered->size() );
	meta.add( "points-removed",		view_->size() - filtered->size() );
	
	// Export metadata.
	pdal::MetadataNode				arguments( "arguments" );
	arguments.add( "resolution",	m_overlap_resolution );
	meta.add( arguments );

	// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
	pdal::PointViewSet				result;
	result.insert( filtered );
	return 		    				result;
}



pdal::PointViewPtr pax::Remove_overlap::overlap_filter( pdal::PointViewPtr view_ ) {
	// Pre-calculating some constants. 
	using ID					  = pdal::Dimension::Id;
	const auto 						size0{ view_->size() };
	const bool						active{
			( m_overlap_resolution > 0.0 ) 
		&&	view_->hasDim( ID::PointSourceId ) 
		&&	view_->hasDim( ID::ScanAngleRank )
	};

	// Generate metadata.
	pdal::MetadataNode				meta = getMetadata();
	pdal::MetadataNode				status( "status" );
	status.add( "has-resolution",	m_overlap_resolution > 0.0 );
	status.add( "has-PointSourceId",view_->hasDim( ID::PointSourceId ) );
	status.add( "has-ScanAngleRank",view_->hasDim( ID::ScanAngleRank ) );
	status.add( "active",			active );
	meta.add( status );

	if(	active ) {
		// Get the bbox, aligned as specified. 
		const PointView_indexer		bbox{ view_, m_overlap_resolution };
		std::vector< Angle_source >	min_angle{ bbox.pixels(), Angle_source{} };
		
		// Create a raster of minimal angle/point-id pairs.
		for( const auto & pt : *view_ ) {
			min_angle[ bbox.index( pt ) ].update(
				pt.getFieldAs< angle_type     >( ID::ScanAngleRank ), 
				pt.getFieldAs< source_id_type >( ID::PointSourceId )
			);
		}
		
		// Remove all but the minimum points in each raster cell.
		pdal::PointViewPtr			points{ view_->makeNew() };
		for( pdal::PointId idx = 0; idx < size0; ++idx ) {
			const source_id_type	source_id{ view_->getFieldAs< source_id_type >( ID::PointSourceId, idx ) };
			if( min_angle[ bbox.index( view_, idx ) ].source_id() == source_id ) 
				points->appendPoint( *view_, idx );
		}

		// Return the filtered set of points. 
	    return points;
	}

	// We didn't do anything, return the original. 
    return view_;
}
