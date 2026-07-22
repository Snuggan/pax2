//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "box.hpp"
#include <pdal/PointView.hpp>			// PointViewPtr, PointId


namespace pax {
	
	// PDAL has no class[es] for points per see. It only uses scalar [separate] coordinates.
	// But you get them, of course:

	inline constexpr Point< double, 2 > point(
		const pdal::PointViewPtr	view_, 
		const pdal::PointId			idx_ 
	) {
		return {	view_->getFieldAs< double >( pdal::Dimension::Id::X, idx_ ),
					view_->getFieldAs< double >( pdal::Dimension::Id::Y, idx_ ) };
	}

	inline constexpr Point< double, 3 > point3d(
		const pdal::PointViewPtr	view_, 
		const pdal::PointId			idx_ 
	) {
		return {	view_->getFieldAs< double >( pdal::Dimension::Id::X, idx_ ),
					view_->getFieldAs< double >( pdal::Dimension::Id::Y, idx_ ),
					view_->getFieldAs< double >( pdal::Dimension::Id::Z, idx_ ) };
	}

	inline constexpr Point< double, 2 > point( const pdal::PointRef & pt_ref_ ) {
		return {	pt_ref_.getFieldAs< double >( pdal::Dimension::Id::X ),
					pt_ref_.getFieldAs< double >( pdal::Dimension::Id::Y ) };
	}

	inline constexpr Point< double, 3 > point3d( const pdal::PointRef & pt_ref_ ) {
		return {	pt_ref_.getFieldAs< double >( pdal::Dimension::Id::X ),
					pt_ref_.getFieldAs< double >( pdal::Dimension::Id::Y ),
					pt_ref_.getFieldAs< double >( pdal::Dimension::Id::Z ) };
	}



	inline constexpr Box< double, 2 > box( const pdal::BOX2D & box_ )			noexcept	{
		return Box< double, 2 >(	pax::Point< double, 2 >({	box_.minx, box_.miny	}),
									pax::Point< double, 2 >({	box_.maxx, box_.maxy	})	);
	}

	inline constexpr Box< double, 3 > box( const pdal::BOX3D & box_ )			noexcept	{
		return Box< double, 3 >(	pax::Point< double, 3 >({	box_.minx, box_.miny, box_.minz		}),
									pax::Point< double, 3 >({	box_.maxx, box_.maxy, box_.maxz		})	);
	}

	inline constexpr Box< double, 2 > box( const pdal::PointView & view_ )		noexcept	{
		pdal::BOX2D					pdal_box{};
		view_.calculateBounds( pdal_box );
		return pax::box( pdal_box );
	}

	inline constexpr Box< double, 3 > box3d( const pdal::PointView & view_ )				{
		pdal::BOX3D					pdal_box{};
		view_.calculateBounds( pdal_box );
		return pax::box( pdal_box );
	}

	inline constexpr auto pdal_box( const Box< double, 2 > & box_ )				noexcept	{
		return pdal::BOX2D(	x( min( box_ ) ), y( min( box_ ) ), 
							x( max( box_ ) ), y( max( box_ ) ) );
	}

	inline constexpr auto pdal_box( const Box< double, 3 > & box_ )				noexcept	{
		return pdal::BOX3D(	x( min( box_ ) ), y( min( box_ ) ), z( min( box_ ) ), 
							x( max( box_ ) ), y( max( box_ ) ), z( max( box_ ) ) );
	}
	
	
	inline constexpr bool empty( const pdal::BOX2D & box_ )		noexcept	{	return box_.empty();	}
	inline constexpr bool empty( const pdal::BOX3D & box_ )		noexcept	{	return box_.empty();	}

}	// namespace pax
