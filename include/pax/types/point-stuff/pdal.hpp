//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "box.hpp"
#include <pdal/PointView.hpp>			// PointViewPtr, PointId


namespace pdal {
	
	// Access a coordinate in an easier way.
	inline constexpr double x    ( PointRef pt_ )			{	return pt_.getFieldAs< double >( Dimension::Id::X );	}
	inline constexpr double y    ( PointRef pt_ )			{	return pt_.getFieldAs< double >( Dimension::Id::Y );	}
	inline constexpr double z    ( PointRef pt_ )			{	return pt_.getFieldAs< double >( Dimension::Id::Z );	}
	inline constexpr double east ( PointRef pt_ ) 			{	return x( pt_ );										}
	inline constexpr double north( PointRef pt_ ) 			{	return y( pt_ );										}

	inline constexpr double x    ( PointViewPtr view_, PointId idx_ )	{
		return view_->getFieldAs< double >( Dimension::Id::X, idx_ );
	}
	inline constexpr double y    ( PointViewPtr view_, PointId idx_ )	{
		return view_->getFieldAs< double >( Dimension::Id::Y, idx_ );
	}
	inline constexpr double z    ( PointViewPtr view_, PointId idx_ )	{
		return view_->getFieldAs< double >( Dimension::Id::Z, idx_ );
	}
	inline constexpr double east ( PointViewPtr view_, PointId idx_ )	{	return x( view_, idx_ );					}
	inline constexpr double north( PointViewPtr view_, PointId idx_ )	{	return y( view_, idx_ );					}


	// Convert pdal point to pax::Point.
	inline constexpr pax::Point< double, 2 > point  ( PointRef pt_ )	{	return { x( pt_ ), y( pt_ ) };				}
	inline constexpr pax::Point< double, 3 > point3d( PointRef pt_ )	{	return { x( pt_ ), y( pt_ ), z( pt_ ) };	}
	inline constexpr pax::Point< double, 2 > point  ( PointViewPtr view_, PointId idx_ ) {
		return { x( view_, idx_ ), y( view_, idx_ ) };
	}
	inline constexpr pax::Point< double, 3 > point3d( PointViewPtr view_, PointId idx_ ) {
		return { x( view_, idx_ ), y( view_, idx_ ), z( view_, idx_ ) };
	}


	// Get corners of pdal boxes.
	inline constexpr pax::Point< double, 2 > min( const BOX2D & box_ )	{	return { box_.minx, box_.miny			 };	}
	inline constexpr pax::Point< double, 2 > max( const BOX2D & box_ )	{	return { box_.maxx, box_.maxy			 };	}
	inline constexpr pax::Point< double, 3 > min( const BOX3D & box_ )	{	return { box_.minx, box_.miny, box_.minz };	}
	inline constexpr pax::Point< double, 3 > max( const BOX3D & box_ )	{	return { box_.maxx, box_.maxy, box_.maxz };	}

	// Convert a pdal box to pax::Box.
	inline constexpr pax::Box  < double, 2 > box( const BOX2D & box_ )	{	return { min( box_ ), max( box_ )		 };	}
	inline constexpr pax::Box  < double, 3 > box( const BOX3D & box_ )	{	return { min( box_ ), max( box_ )		 };	}

	// Convert pdal PointView to pax::Box.
	inline constexpr pax::Box< double, 2 > box( const PointView & view_ )	{
		BOX2D					pdal_box{};
		view_.calculateBounds( pdal_box );
		return box( pdal_box );
	}
	inline constexpr pax::Box< double, 3 > box3d( const PointView & view_ )	{
		BOX3D					pdal_box{};
		view_.calculateBounds( pdal_box );
		return box( pdal_box );
	}

	// Convert pax::Box to pdal box.
	inline constexpr auto pdal_box( const pax::Box< double, 2 > & box_ )			{
		return BOX2D(	pax::x( min( box_ ) ), pax::y( min( box_ ) ), 
						pax::x( max( box_ ) ), pax::y( max( box_ ) ) );
	}
	inline constexpr auto pdal_box( const pax::Box< double, 3 > & box_ )			{
		return BOX3D(	pax::x( min( box_ ) ), pax::y( min( box_ ) ), pax::z( min( box_ ) ), 
						pax::x( max( box_ ) ), pax::y( max( box_ ) ), pax::z( max( box_ ) ) );
	}
	
	
	inline constexpr bool empty( const BOX2D & box_ )	{	return box_.empty();	}
	inline constexpr bool empty( const BOX3D & box_ )	{	return box_.empty();	}

}	// namespace pax
