//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"


namespace pax {
	
	using std::array, std::span;


	template< arithmetic A, std::size_t N >						requires( N != std::dynamic_extent )
	class Box {
		std::array< Point< A, N >, 2 >				m_box{};
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = Point< A, N >;
		using 										Base	  = std::array< Point, 2 >;
		using Point::value_type;

		constexpr Box()										  = default;
		constexpr Box( const Box & )						  = default;
		constexpr Box( Box && )								  = default;
		constexpr Box & operator=( const Box & )			  = default;
		constexpr Box & operator=( Box && )					  = default;
		constexpr Box( span< A, N > pt0_, span< A, N > pt1_ )	noexcept
			: m_box({ pax::min( pt0_, pt1_ ), pax::max( pt0_, pt1_ ) }) {}

		constexpr const Point & min()							const noexcept	{	return m_box.first();	}
		constexpr const Point & max()							const noexcept	{	return m_box.last();	}
		friend constexpr const Point & min( const Box & b_ )	noexcept		{	return b_.min();		}
		friend constexpr const Point & max( const Box & b_ )	noexcept		{	return b_.max();		}

		constexpr Box grow( span< A, N > pt_ )					const noexcept	{
			return { pax::min( min(), pt_ ), pax::max( max(), pt_ ) };
		}

		constexpr bool strictly_inside( span< A, N > pt_ )		const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		constexpr bool inside_or_on( span< A, N > pt_ )			const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		constexpr bool in_range( span< A, N > pt_ )				const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}
	};

}	// namespace pax
