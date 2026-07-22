//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "box.hpp"
#include <string>
#include <span>


namespace pax {

	template< typename T, typename ... P >	struct Table_meta;
	template< floating F >					struct Object_meta;


	
	/// Implements an object with coordinates and a scalar value.
	/// It can have any rank you please, but two is probably usual.
	template< floating F, std::size_t N >						requires( is_static< N > )
	class Circle {
		Point< F, N >											m_center{};
		F														m_radius{};

		constexpr const Point< F, N > & center()				const noexcept	{	return m_center;	}
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Pt		  = Point< F, N >;
		using value_type = Pt::value_type;

		constexpr Circle()									  = default;
		constexpr Circle( const Circle & )					  = default;
		constexpr Circle( Circle && )						  = default;
		constexpr Circle & operator=( const Circle & )		  = default;
		constexpr Circle & operator=( Circle && )			  = default;
		constexpr Circle( const Point< F, N > & center_, const F radius_ ) noexcept
			: m_center( center_ ), m_radius{ ( radius_ >= 0 ) ? radius_ : -radius_ } {}

		template< floating ... Coords >							requires( sizeof...( Coords ) == rank )
		constexpr Circle(
			Coords				 && ... coords_,
			const F					radius_
		) noexcept : m_center{ F( std::forward< Coords >( coords_ ) ) ... }, m_radius{ radius_ } {}


		/// The center coordinates.
		friend constexpr const Circle & center( const Circle & c_ ) noexcept		{	return c_.center();	}
		constexpr value_type radius()								const noexcept	{	return m_radius;	}

		/// The minimal coordinates of a bounding box of circle's.
		friend constexpr const Circle & min   ( const Circle & c_ )	noexcept		{
			return c_.center() - Pt( c_.radius() );
		}

		/// The maximal coordinates of a bounding box of circle's.
		friend constexpr const Circle & max   ( const Circle & c_ )	noexcept		{
			return c_.center() + Pt( c_.radius() );
		}

		/// The point is inside, but not on, the circle.
		constexpr bool strictly_inside( const Point< F, N > & pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <  radius()*radius();
		}

		/// The point is either inside or on the circle.
		constexpr bool inside_or_on( const Point< F, N > & pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <= radius()*radius();
		}

		/// The point is either inside or on the circle.
		friend constexpr bool contains( 
			const Circle			  & circle_,
			const Pt				  & pt_
		) noexcept {
			return	circle_.inside_or_on( pt_ );
		}
	};

	/// This is used to read Circle values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Circle< F, 2 > > {
		static constexpr auto value = Table_meta< Circle< F, 2 >, F, F, F >{ "east", "north", "radius" };
	};


	/// Does the Box contain the Circle_? (They may touch.)
	template< floating F, std::size_t N >						requires( is_static< N > )
	constexpr bool contains( 
		const Box< F, N >		  & box_, 
		const Circle< F, N >	  & circle_ 
	) noexcept {
		return	all_le( min( box_    ),	min( circle_ ) )
			&&	all_le( max( circle_ ),	max( box_    ) );
	}


	/// Does the Box contain the Circle_? (They may touch.)
	template< floating F, std::size_t N >						requires( is_static< N > )
	constexpr bool overlap( 
		const Box< F, N >		  & box_, 
		const Circle< F, N >	  & circle_ 
	) noexcept {
		return	all_lt( min( box_    ), max( circle_ ) )
			&&	all_lt( min( circle_ ), max( box_    ) );
	}
	



	/// A simple container for the spacial data of a plot/circle with an id.
	template< floating F, std::size_t N >
	class Plot_w_id : public Circle< F, N > {
		std::string						m_id{};
		using Base					  = Circle< F, N >;

	public:
		constexpr Plot_w_id()								  = default;
		constexpr Plot_w_id( const Plot_w_id & )			  = default;
		constexpr Plot_w_id( Plot_w_id && )					  = default;
		constexpr Plot_w_id & operator=( const Plot_w_id & )  = default;
		constexpr Plot_w_id & operator=( Plot_w_id && )		  = default;

		constexpr Plot_w_id(
			const Base				  & circle_,
			const std::string_view		id_
		) noexcept : Base{ circle_ }, m_id{ id_ } {}

		template< floating ... Coords >							requires( sizeof...( Coords ) == N )
		constexpr Plot_w_id(
			Coords					 && ...coords_,
			const F						radius_,
			const std::string_view		id_
		) noexcept : Plot_w_id{ Base{ std::forward< Coords >( coords_ ) ..., radius_ }, id_ } {}		
		
		/// Return the [unique] id. No other circle/plot may have the same. 
		constexpr std::string id() 								const noexcept	{	return m_id;	}
	};
	
	/// This is used to read values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Plot_w_id< F, 2 > > {
		static constexpr auto value = Table_meta< Plot_w_id< F, 2 >, F, F, F, std::string >
			{ "east", "north", "radius", "id" };
	};

}	// namespace pax
