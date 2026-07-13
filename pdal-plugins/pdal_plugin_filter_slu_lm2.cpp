#include <pax/pdal/modules/pdal_plugin_filter_slu_lm2.hpp>
#include <pax/pdal/utilities/classification.hpp>	// normal_lm_filter()
#include <pax/pdal/utilities/bbox_indexer.hpp>

#include <pdal/pdal_internal.hpp>


namespace pax {

	static pdal::PluginInfo const s_info {
		"filters.slu_lm2",
		"Filtering according to z-values and point type, see the options below for details. "
		"If there is a dimension HeightAboveGround, its values are copied into dimension Z. "
		"Execute 'pdal --options filters.slu_lm' for a list of available parameters. ",
		"https://github.com/Snuggan/pax2/blob/main/documentation/pdal-slu_lm.md"
	};

	CREATE_SHARED_STAGE( Slu_lm2, s_info )
	std::string Slu_lm2::getName()		const	{	return s_info.name;		}


	void Slu_lm2::addArgs( pdal::ProgramArgs& args_ ) {
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


	void Slu_lm2::addDimensions( pdal::PointLayoutPtr layout_ ) {
		layout_->registerDim( pdal::Dimension::Id::Classification );
		
		// The pdal hag_dem filter puts normalized points into a specific dimension: HeightAboveGround. 
		// We want to overwrite unnormalized z-values with normalized.
		const bool	has_hag_dim{ layout_->hasDim( pdal::Dimension::Id::HeightAboveGround ) };
		m_height_id		  = has_hag_dim ? pdal::Dimension::Id::HeightAboveGround : pdal::Dimension::Id::Z;
		if( has_hag_dim )	layout_->registerDim( pdal::Dimension::Id::HeightAboveGround );
		else				layout_->registerDim( pdal::Dimension::Id::Z );

		// No z-value filtering?
		// Do not place in constructor, as the argument values probably are not set then.
		if( m_max_z < m_min_z ) {
			m_min_z = std::numeric_limits< decltype( m_min_z ) >::lowest();
			m_max_z = std::numeric_limits< decltype( m_max_z ) >::max();
		}
	}


	Slu_lm2::~Slu_lm2() {
		// Create metadata.
		pdal::MetadataNode					meta = getMetadata();
		
		// Metadata: general:
		meta.add( "points-processed",		m_metadata.points_in );
		meta.add( "points-passed",			m_metadata.points_out );
		
		// Metadata: arguments:
		pdal::MetadataNode					arguments( "arguments" );
		arguments.add( "min_z",				m_min_z );
		arguments.add( "max_z",				m_max_z );
		arguments.add( "lm_filter",			m_lm_filter );
		meta.add( arguments );
		
		// Metadata: filtering:
		pdal::MetadataNode					filtering( "filtering" );
		filtering.add( "z-made-zero",		m_metadata.z_negative );
		filtering.add( "z-to-small",		m_metadata.z_small );
		filtering.add( "z-to-large",		m_metadata.z_large );
		filtering.add( "not-lm",			m_metadata.not_lm );
		filtering.add( "points-removed",	m_metadata.points_in - m_metadata.points_out );
		meta.add( filtering );
	}


	bool Slu_lm2::processOne( pdal::PointRef & pt_ ) {
		++m_metadata.points_in;
		const auto	z		  = pt_.getFieldAs< coordinate_type >( m_height_id );
		const auto	classif	  = asprs::Classification( 	// From pax/pdal/utilities/classification.hpp
									pt_.getFieldAs< unsigned    >( pdal::Dimension::Id::Classification ) );
		if (	( z >= m_min_z ) 
			&&	( z <= m_max_z ) 
			&&	( !m_lm_filter || asprs::normal_lm_filter( classif ) ) 
		) {	
			// Include the point.
			++m_metadata.points_out;
			if( z < 0 ) {	// Negative height.
				pt_.setField( pdal::Dimension::Id::Z, 0.0 );
				++m_metadata.z_negative;
			} else {
				pt_.setField( pdal::Dimension::Id::Z, z );
			}
			return true;
		}
		// Exclude the point, but count the reason for exclusion.
		( z < m_min_z )	? ++m_metadata.z_small : 0;
		( z > m_max_z )	? ++m_metadata.z_large : 0;
		!normal_lm_filter( classif ) ? ++m_metadata.not_lm : 0;
		return false;
	}


	pdal::PointViewSet Slu_lm2::run( pdal::PointViewPtr view_ ) {
		// Create a new empty point cloud.
		pdal::PointViewPtr		points{ view_->makeNew() };

		// Filter the points.
		for( pdal::PointId idx = 0; idx < view_->size(); ++idx ) {
			auto pt = view_->point( idx );
			if( processOne( pt ) ) 
				points->appendPoint( *view_, idx );
		}

		// Create new point cloud (pdal::PointViewSet) with the result (pdal::PointViewPtr) and return it.
		pdal::PointViewSet		result;
		result.insert( points );
		return result;
	}

}	// namespace pax
