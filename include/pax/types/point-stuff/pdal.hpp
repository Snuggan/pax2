//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "box.hpp"
#include <pdal/PointView.hpp>			// PointViewPtr, PointId


namespace pax {
	
	inline constexpr bool empty( const pdal::BOX2D & box_ )		noexcept	{	return box_.empty();	}
	inline constexpr bool empty( const pdal::BOX3D & box_ )		noexcept	{	return box_.empty();	}


	inline constexpr Box< double, 2 > box( const pdal::BOX2D & box_ )		noexcept	{
		return Box< double, 2 >(	pax::Point< double, 2 >({	box_.minx, box_.miny	}),
									pax::Point< double, 2 >({	box_.maxx, box_.maxy	})	);
	}

	inline constexpr Box< double, 3 > box( const pdal::BOX3D & box_ )		noexcept	{
		return Box< double, 3 >(	pax::Point< double, 3 >({	box_.minx, box_.miny, box_.minz		}),
									pax::Point< double, 3 >({	box_.maxx, box_.maxy, box_.maxz		})	);
	}

	inline constexpr Box< double, 2 > box( const pdal::PointView & view_ )	noexcept	{
		pdal::BOX2D					pdal_box{};
		view_.calculateBounds( pdal_box );
		return pax::box( pdal_box );
	}

	inline constexpr auto pdal_box( const Box< double, 2 > & box_ )			noexcept	{
		return pdal::BOX2D(	x( min( box_ ) ), y( min( box_ ) ), 
							x( max( box_ ) ), y( max( box_ ) ) );
	}

	inline constexpr auto pdal_box( const Box< double, 3 > & box_ )			noexcept	{
		return pdal::BOX3D(	x( min( box_ ) ), y( min( box_ ) ), z( min( box_ ) ), 
							x( max( box_ ) ), y( max( box_ ) ), z( max( box_ ) ) );
	}

}	// namespace pax
