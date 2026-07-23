//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/types/point-stuff/box.hpp>
#include <pdal/PointView.hpp>			// PointViewPtr, PointId


namespace pax {
	using	pdal::BOX2D, pdal::BOX3D, pdal::PointRef, 
			pdal::PointView, pdal::PointViewPtr, pdal::PointId, pdal::Dimension::Id;
	
	// Access a coordinate in an easier way.
	inline constexpr double x( PointRef pt_ )					{	return pt_. getFieldAs< double >( Id::X );		}
	inline constexpr double y( PointRef pt_ )					{	return pt_. getFieldAs< double >( Id::Y );		}
	inline constexpr double z( PointRef pt_ )					{	return pt_. getFieldAs< double >( Id::Z );		}
	inline constexpr double x( PointViewPtr vw_, PointId ix_ )	{	return vw_->getFieldAs< double >( Id::X, ix_ );	}
	inline constexpr double y( PointViewPtr vw_, PointId ix_ )	{	return vw_->getFieldAs< double >( Id::Y, ix_ );	}
	inline constexpr double z( PointViewPtr vw_, PointId ix_ )	{	return vw_->getFieldAs< double >( Id::Z, ix_ );	}


	// Convert pdal point to Point.
	inline constexpr Point< double, 2 > point  ( PointRef pt_ )	{	return { x( pt_ ), y( pt_ ) };					}
	inline constexpr Point< double, 2 > point  ( PointViewPtr vw_, PointId ix_ ) 
																{	return { x( vw_, ix_ ), y( vw_, ix_ ) };		}


	// Get corners of pdal boxes.
	inline constexpr Point< double, 2 > min( const BOX2D & b_ )	{	return { b_.minx, b_.miny			};			}
	inline constexpr Point< double, 2 > max( const BOX2D & b_ )	{	return { b_.maxx, b_.maxy			};			}
	inline constexpr Point< double, 3 > min( const BOX3D & b_ )	{	return { b_.minx, b_.miny, b_.minz	};			}
	inline constexpr Point< double, 3 > max( const BOX3D & b_ )	{	return { b_.maxx, b_.maxy, b_.maxz	};			}

	// Convert a pdal box to Box.
	inline constexpr Box  < double, 2 > box( const BOX2D & b_ )	{	return { min( b_ ), max( b_ )		 };			}
	inline constexpr Box  < double, 3 > box( const BOX3D & b_ )	{	return { min( b_ ), max( b_ )		 };			}

	// Convert pdal PointView to Box.
	inline constexpr Box< double, 2 > box( const PointView & vw_ )	{
		BOX2D					pdal_box{};
		vw_.calculateBounds( pdal_box );
		return box( pdal_box );
	}

}	// namespace pax
