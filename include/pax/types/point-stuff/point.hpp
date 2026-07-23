//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"
#include <array>
#include <utility>		// std::forward
#include <format>


namespace pax {
	
	using std::get;
	template< typename T, typename ... P >	struct Table_meta;
	template< typename F >					struct Object_meta;



	/// This is the type used for arithmetic vectors with a fixed size.
	template< arithmetic A, std::size_t N >										requires( is_static< N > )
	using Point = std::array< A, N >;

	using Point2d		  = Point< double, 2 >;
	using Point3d		  = Point< double, 3 >;
	
	template< typename Out, arithmetic A, std::size_t N >
	constexpr Out & operator<<( Out & out_, const Point< A, N > & pt_ ) {
		return out_ << std::format( "{}", pt_ );
	}

	/// Create a Point out of a bunch of elements.
	template< arithmetic A, arithmetic ... As >	
	constexpr Point< A, sizeof...( As ) > point( As && ... as_ )				noexcept	{
		return { static_cast< A >( std::forward< As >( as_ ) ) ... };
	}

	/// This is used to read Point values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Point< F, 2 > > {
		static constexpr auto value = Table_meta< Point< F, 2 >, F, F >{ "east", "north" };
	};

	/// This is used to read Point values from a csv file using Text_table.
	template< floating F >
	struct Object_meta< Point< F, 3 > > {
		static constexpr auto value = Table_meta< Point< F, 3 >, F, F, F >{ "east", "north", "height" };
	};



	/// This is the type used for a fixed number of multiople "dimensions, i.e. idex into a matrix.
	template< std::size_t N >							requires( is_static< N > )
	using Index = std::array< std::size_t, N >;

	using Index2d		  = Index< 2 >;
	using Index3d		  = Index< 3 >;
	
	template< typename Out, std::size_t N >
	constexpr Out & operator<<( Out & out_, const Index< N > & idx_ ) {
		return out_ << std::format( "{}", idx_ );
	}

	/// Create a Point out of a bunch of elements.
	template< uinteger ... Uis >	
	constexpr Index< sizeof...( Uis ) > index( Uis && ... uis_ )				noexcept	{
		return { static_cast< std::size_t >( std::forward< Uis >( uis_ ) ) ... };
	}
	


	/// Some access functions.
	/// Using them makes the code clearer and guarantees a consistent mapping name -> index.
	/// col, east, x -> 0, row, north, y -> 1, z -> 2.
	/// @{
	template< uinteger U, std::size_t N >				requires( N > 0 )
	constexpr U   col  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< 0 >( pt_ );	}

	template< uinteger U, std::size_t N >				requires( N > 0 )
	constexpr U & col  (       Point< U, N > & pt_ )	noexcept	{	return std::get< 0 >( pt_ );	}

	template< uinteger U, std::size_t N >				requires( N > 1 )
	constexpr U   row  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< 1 >( pt_ );	}

	template< uinteger U, std::size_t N >				requires( N > 1 )
	constexpr U & row  (       Point< U, N > & pt_ )	noexcept	{	return std::get< 1 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F   x    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< 0 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F & x    (       Point< F, N > & pt_ )	noexcept	{	return std::get< 0 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F   y    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< 1 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F & y    (       Point< F, N > & pt_ )	noexcept	{	return std::get< 1 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 2 )
	constexpr F   z    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< 2 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 2 )
	constexpr F & z    (       Point< F, N > & pt_ )	noexcept	{	return std::get< 2 >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F   east ( const Point< F, N > & pt_ )	noexcept	{	return x( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F & east (       Point< F, N > & pt_ )	noexcept	{	return x( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F   north( const Point< F, N > & pt_ )	noexcept	{	return y( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F & north(       Point< F, N > & pt_ )	noexcept	{	return y( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F   lon  ( const Point< F, N > & pt_ )	noexcept	{	return x( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 0 )
	constexpr F & lon  (       Point< F, N > & pt_ )	noexcept	{	return x( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F   lat  ( const Point< F, N > & pt_ )	noexcept	{	return y( pt_ );				}

	template< floating F, std::size_t N >				requires( N > 1 )
	constexpr F & lat  (       Point< F, N > & pt_ )	noexcept	{	return y( pt_ );				}
	/// @}


	/// Check if all elements in pt0_ are smaller than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_lt( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( true && ... && ( t0 <  t1 ) );
	}

	/// Check if all elements in pt0_ are smaller or equal than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_le( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( true && ... && ( t0 <= t1 ) );
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > min( Point< A, N > pt0_, Point< A, N > pt1_ )		noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( ( t0 <= t1 ) ? t0 : t1 ) ... });
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > max( Point< A, N > pt0_, Point< A, N > pt1_ )		noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( ( t0 >= t1 ) ? t0 : t1 ) ... });
	}


	/// Add the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > operator+( Point< A, N > pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( t0 + t1 ) ... });
	}

	/// Subtract the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > operator-( Point< A, N > pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( t0 - t1 ) ... });
	}

	/// Calculate the euclidian distance squared between two points.
	/// Aake the std::sqrt on the result to get the actual euclidian distance.
	template< arithmetic A, std::size_t N >
	constexpr A distance2( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		static constexpr auto square =			  []( A t ){ return t*t; };
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + square( t0 - t1 ));
	}

	/// Calculate the dot product of two points. 
	template< arithmetic A, std::size_t N >
	constexpr A dot_product( Point< A, N > pt0_, Point< A, N > pt1_ )			noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + ( t0*t1 ) );
	}

	/// The vector cross product.
	template< arithmetic A >
	constexpr Point< A, 3 > cross_product(
		Point< A, 3 > pt0_, 
		Point< A, 3 > pt1_ 
	) noexcept {
		return Point{ 
			pt0_[ 1 ]*pt1_[ 2 ] - pt0_[ 2 ]*pt1_[ 1 ],
			pt0_[ 2 ]*pt1_[ 0 ] - pt0_[ 0 ]*pt1_[ 2 ],
			pt0_[ 0 ]*pt1_[ 1 ] - pt0_[ 1 ]*pt1_[ 0 ]
		};
	}

}	// namespace pax
