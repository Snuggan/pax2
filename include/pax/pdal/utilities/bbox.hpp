//	Copyright (c) 2014-2021, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pdal/PointView.hpp>


namespace pax {
	
	using Box2	  = pdal::BOX2D;	


	/// Return a bounding box for the point cloud. 
	inline Box2 bbox( const pdal::PointViewPtr view_ptr_ ) noexcept {
		pdal::BOX2D					box_;
		view_ptr_->calculateBounds( box_ );
		return box_;
	}

}	// namespace pax


namespace pdal {

	inline bool empty( const pdal::BOX2D & box_ )	noexcept	{	return box_.empty();	}

}	// namespace pdal
