//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"


namespace pax {
	
	using std::span;


	template< arithmetic A, std::size_t N >						requires( N != std::dynamic_extent )
	class Circle {
		Point< A, N >											m_center{};
		A														m_radius{};

		constexpr const Point< A, N > & center()				const noexcept	{	return m_center;	}
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = Point< A, N >;
		using value_type = Point::value_type;

		constexpr Circle()									  = default;
		constexpr Circle( const Circle & )					  = default;
		constexpr Circle( Circle && )						  = default;
		constexpr Circle & operator=( const Circle & )		  = default;
		constexpr Circle & operator=( Circle && )			  = default;
		constexpr Circle( const span< A, N > & center_, const A radius_ ) noexcept
			: m_center( center_ ), m_radius{ ( radius_ >= 0 ) ? radius_ : -radius_ } {}

		friend constexpr const Circle & center( const Circle & c_ )   noexcept	{	return c_.center();	}
		constexpr value_type radius()							const noexcept	{	return m_radius;	}

		friend constexpr const Circle & min   ( const Circle & c_ )   noexcept	{
			return c_.center() - Point( c_.radius() );
		}

		friend constexpr const Circle & max   ( const Circle & c_ )   noexcept	{
			return c_.center() + Point( c_.radius() );
		}

		constexpr bool strictly_inside( span< A, N > pt_ )		const noexcept	{
			return distance2( center(), pt_ ) <  radius()*radius();
		}

		constexpr bool inside_or_on( span< A, N > pt_ )			const noexcept	{
			return distance2( center(), pt_ ) <= radius()*radius();
		}
	};

}	// namespace pax
