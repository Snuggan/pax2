#include <pax/pdal/modules/pdal_plugin_filter_slu_lm.hpp>
#include <pax/pdal/utilities/classification.hpp>	// normal_lm_filter()
#include <pax/pdal/utilities/bbox_indexer.hpp>

#include <pdal/pdal_internal.hpp>


namespace pax {

	static pdal::PluginInfo const s_info {
		"filters.slu_lm",
		"Filtering according to z-values and point type, see the options below for details. "
		"If there is a dimension HeightAboveGround, its values are copied into dimension Z. "
		"Execute 'pdal --options filters.slu_lm' for a list of available parameters. ",
		"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-slu_lm.md"
	};

	CREATE_SHARED_STAGE( Slu_lm, s_info )
	std::string Slu_lm::getName()		const	{	return s_info.name;		}


	void Slu_lm::addArgs( pdal::ProgramArgs& args_ ) {
		args_.add(	"min_z",
					"Minimum z: remove points with a z-value smaller than this value. " 
					"Also, sets all remaining negative z-values to zero. You probably don't want to do use this option "
					"unless the points are normalized. If 'max_z' has a value smaller than 'min_z', no z-filtering will occur.",
					m_min_z, m_min_z
		);
		args_.add(	"max_z",
					"Maximum z: remove points with a z-value larger than this value. " 
					"You probably don't want to do use this option unless the points are normalized. "
					"If 'max_z' has a value smaller than 'min_z', no z-filtering will occur.",
					m_max_z, m_max_z
		);
		args_.add(	"lm_filter",
					"If false, all point classes will pass. " 
					"If true, only these point classes will pass: "
					"never classified (0), unspecified (1), and point on ground (2). ",
					m_lm_filter, m_lm_filter
		);
	}


	void Slu_lm::addDimensions( pdal::PointLayoutPtr layout_ ) {
		// Don't know if this is necessary, as we only use standard dimensions...
		layout_->registerDim( pdal::Dimension::Id::Classification );
		layout_->registerDim( pdal::Dimension::Id::Z );
	}



	inline asprs::Classification get_classification(
		const pdal::PointViewPtr	view_, 
		const pdal::PointId 		idx_ 
	) {
		return static_cast< asprs::Classification >( 
			view_->getFieldAs< asprs::classification_type >( pdal::Dimension::Id::Classification, idx_ ) );
	}
	
	

	pdal::PointViewSet Slu_lm::run( pdal::PointViewPtr view_ ) {
		// No z-value filtering?
		if( m_max_z < m_min_z ) {
			m_min_z = std::numeric_limits< decltype( m_min_z ) >::lowest();
			m_max_z = std::numeric_limits< decltype( m_max_z ) >::max();
		}

		// Produce new (filtered) point cloud.
		const pdal::PointViewPtr		filtered{ slu_filter( view_ ) };
		
		// Create metadata.
		pdal::MetadataNode				meta = getMetadata();
		meta.add( "points-in",			view_->size() );
		meta.add( "points-out",			filtered->size() );
		meta.add( "points-removed",		view_->size() - filtered->size() );
		
		// Export metadata.
		pdal::MetadataNode				arguments( "arguments" );
		arguments.add( "min_z",			m_min_z );
		arguments.add( "max_z",			m_max_z );
		arguments.add( "lm_filter",		m_lm_filter );
		meta.add( arguments );

		// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
		pdal::PointViewSet				result;
		result.insert( filtered );
		return 		    				result;
	}



	pdal::PointViewPtr Slu_lm::slu_filter( pdal::PointViewPtr view_ ) const {
		// The pdal hag_dem filter puts normalized points into a specific dimension: HeightAboveGround. 
		// We want to overwrite unnormalized z-values with normalized.
		const bool				has_hag_dim{ view_->hasDim( pdal::Dimension::Id::HeightAboveGround ) };
		const auto				hag_dim{ has_hag_dim ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z };

		// Create a new empty point cloud.
		pdal::PointViewPtr		points{ view_->makeNew() };
		auto					pts_idx{ points->size() };

		// For metadata generation. 
		std::size_t 			z_negative{}, z_small{}, z_large{}, not_lm{};

		// Filter the points.
		for( pdal::PointId idx = 0; idx < view_->size(); ++idx ) {
			const auto			z             { view_->getFieldAs< coordinate_type >( hag_dim, idx ) };
			const auto			classification{ get_classification( view_, idx ) };
			if (	( z >= m_min_z ) 
				&&	( z <= m_max_z ) 
				&&	( !m_lm_filter || normal_lm_filter( main_classification( classification ) ) ) 
			) {
				// Include the point.
				points->appendPoint( *view_, idx );
				( z < 0 )		? ++z_negative : 0;
				points->setField( pdal::Dimension::Id::Z, pts_idx, ( z < 0 ) ? 0.0 : z );
				++pts_idx;
			} else {
				// Exclude the point, but count the reason for exclusion.
				( z < m_min_z )	? ++z_small : 0;
				( z > m_max_z )	? ++z_large : 0;
				!normal_lm_filter( classification ) ? ++not_lm : 0;
			}
		}

		// Generate metadata.
		pdal::MetadataNode				filtering( "filtering" );
		filtering.add( "z-made-zero",	z_negative );
		filtering.add( "z-small",		z_small );
		filtering.add( "z-large",		z_large );
		filtering.add( "not-lm",		not_lm );
		pdal::MetadataNode				meta = getMetadata();
		meta.add( filtering );

		// Return the point cloud (pdal::PointViewPtr).
	    return points;
	}

}	// namespace pax
