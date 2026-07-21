//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"


namespace pax {
	
	template< arithmetic A, std::size_t N >			requires( is_static< N > )
	class Box {
		std::array< Point< A, N >, 2 >				m_box{};
		
	public:
		static constexpr std::size_t 				rank		  = N;
		using 										Pt			  = Point< A, N >;
		using 										Base		  = std::array< Pt, 2 >;
		using 										value_type	  = Pt::value_type;

		constexpr Box()											  = default;
		constexpr Box( const Box & )							  = default;
		constexpr Box( Box && )									  = default;
		constexpr Box & operator=( const Box & )				  = default;
		constexpr Box & operator=( Box && )						  = default;
		constexpr Box( const Point< A, N > & pt0_, const Point< A, N > & pt1_ )	noexcept
			: m_box({ pax::min( pt0_, pt1_ ), pax::max( pt0_, pt1_ ) }) {}

		constexpr const Pt & min()									const noexcept	{	return m_box.front();	}
		constexpr const Pt & max()									const noexcept	{	return m_box.back();	}
		friend constexpr const Pt & min( const Box & b_ )			noexcept		{	return b_.min();		}
		friend constexpr const Pt & max( const Box & b_ )			noexcept		{	return b_.max();		}

		constexpr Box grow( const Point< A, N > & pt_ )				const noexcept	{
			return { pax::min( min(), pt_ ), pax::max( max(), pt_ ) };
		}

		constexpr bool strictly_inside( const Point< A, N > & pt_ )	const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		constexpr bool inside_or_on( const Point< A, N > & pt_ )	const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		constexpr bool in_range( const Point< A, N > & pt_ )		const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}
	};

}	// namespace pax
