//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pdal/PointRef.hpp>
#include <array>


namespace pax {
	
	inline constexpr double lower_x( const pdal::BOX2D & box_ )	noexcept	{	return box_.minx;		}
	inline constexpr double upper_x( const pdal::BOX2D & box_ )	noexcept	{	return box_.maxx;		}
	inline constexpr double lower_y( const pdal::BOX2D & box_ )	noexcept	{	return box_.miny;		}
	inline constexpr double upper_y( const pdal::BOX2D & box_ )	noexcept	{	return box_.maxy;		}

	template< typename Box >
	constexpr double lower_x( const Box & box_ )	noexcept	{	return get< 0 >( std::get< 0 >( box_ ) );	}

	template< typename Box >
	constexpr double upper_x( const Box & box_ )	noexcept	{	return get< 0 >( std::get< 1 >( box_ ) );	}

	template< typename Box >
	constexpr double lower_y( const Box & box_ )	noexcept	{	return get< 1 >( std::get< 0 >( box_ ) );	}

	template< typename Box >
	constexpr double upper_y( const Box & box_ )	noexcept	{	return get< 1 >( std::get< 1 >( box_ ) );	}
	


	/// A simple container for the spacial data of a plot.
	class Plot_base : public std::array< double, 2 > {
	protected:
		using coord_type			  = double;
		using point_type			  = std::array< coord_type, 2 >;
		
	private:
		enum class Inclusion			{	overlapped, contained, 	};
		static constexpr Inclusion		inclusion{ Inclusion::contained };
		static constexpr const char *	inclusion_id_[ 2 ] =	{	"overlapped", "contained"	};

		static_assert( sizeof( pdal::PointRef ) == 32 );

		static constexpr coord_type square( const coord_type v_ )	noexcept		{	return v_*v_;	}
	
		/// Does the plot overlap any part of box_?
		template< typename BBox >
		constexpr bool overlapped(
			const BBox			  & bbox_, 
			const coord_type		max_distance_
		) const noexcept {
			using std::get;
			return	( lower_x( bbox_ ) <  get< 0 >( *this ) + max_distance_ )
				&&	( lower_y( bbox_ ) <  get< 1 >( *this ) + max_distance_ )
				&&	( upper_x( bbox_ ) >  get< 0 >( *this ) - max_distance_ )
				&&	( upper_y( bbox_ ) >  get< 1 >( *this ) - max_distance_ );
		}

		/// Is the plot completely inside box_?
		template< typename BBox >
		constexpr bool contained(
			const BBox			  & bbox_, 
			const coord_type		max_distance_
		) const noexcept {
			using std::get;
			return	( lower_x( bbox_ ) <= get< 0 >( *this ) - max_distance_ )
				&&	( lower_y( bbox_ ) <= get< 1 >( *this ) - max_distance_ )
				&&	( upper_x( bbox_ ) >= get< 0 >( *this ) + max_distance_ )
				&&	( upper_y( bbox_ ) >= get< 1 >( *this ) + max_distance_ );
		}

	public:
		constexpr Plot_base(
			const coord_type		east_, 
			const coord_type		north_
		) noexcept : point_type{ east_, north_ } {}


		/// 
		static constexpr auto inclusion_id() noexcept {
			return inclusion_id_[ std::size_t( inclusion ) ];
		}


		/// Should the plot recieve points from file_ (with the bbox_)?
		template< typename BBox >
		constexpr bool in_box( 
			const BBox			  & bbox_, 
			const coord_type		max_distance_
		) const noexcept {
			if      constexpr( inclusion == Inclusion::overlapped )		return overlapped( bbox_, max_distance_ );
			else if constexpr( inclusion == Inclusion::contained )		return contained ( bbox_, max_distance_ );
		}


		/// Is pt_ inside the area?
		bool contains(
			const pdal::PointRef	pt_, 
			const coord_type		max_distance_
		) const noexcept {
			using std::get;
			return	square( get< 0 >( *this ) - pt_.getFieldAs< coord_type >( pdal::Dimension::Id::X ) ) 
				+	square( get< 1 >( *this ) - pt_.getFieldAs< coord_type >( pdal::Dimension::Id::Y ) ) 
				<=  max_distance_*max_distance_;
		}
	};

}	// namespace pax
