//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <span>
#include <array>
#include <concepts>


namespace pax {
	
	using std::get, std::array, std::span;

	// Make it possible to incude other types in the concepts below:
	template< typename U >	struct is_uinteger	  : std::false_type  {};
	template< typename I >	struct is_integer	  : is_uinteger< I > {};
	template< typename F >	struct is_floating	  : std::false_type  {};

	template< typename I >	concept integer		  = std::integral< I >			|| is_integer < I >::value;
	template< typename U >	concept uinteger	  = std::unsigned_integral< U > || is_uinteger< U >::value;
	template< typename F >	concept floating	  = std::floating_point< F >	|| is_floating< F >::value;
	template< typename A >	concept arithmetic	  = floating< A > || integer< A >;
	

	template< arithmetic A, std::size_t N >					requires( N != std::dynamic_extent )
	using Point = array< A, N >;
	

	/// Create a Point out of a bunch of elements.
	template< arithmetic A, arithmetic ... As >	
	constexpr Point< A, sizeof...( As ) > make_point( As ... as_ )					noexcept	{
		return { static_cast< A >( std::forward( as_ ) ) ... };
	}
	
	/// Create a Point out of a span.
	template< arithmetic A, std::size_t N >					requires( N != std::dynamic_extent )
	constexpr Point< A, N > make_point( span< A, N > span_ )						noexcept	{
		return { span_.begin(), span_.end() };
	}
	


	/// Some access functions.
	/// Using them makes the code clearer and guarantees a consistent mapping name -> index.
	/// col, east, x -> 0, row, north, y -> 1, z -> 2.
	/// @{
	template< uinteger U, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr U   col  ( span< const U, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< uinteger U, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr U & col  ( span<       U, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< uinteger U, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr U   row  ( span< const U, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< uinteger U, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr U & row  ( span<       U, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}


	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr F   east ( span< const F, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr F & east ( span<       F, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr F   north( span< const F, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr F & north( span<       F, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}


	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr F   x    ( span< const F, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr F & x    ( span<     F, N > pt_ )		noexcept	{	return get< 0 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr F   y    ( span< const F, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr F & y    ( span<       F, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 2 )
	constexpr F   z    ( span< const F, N > pt_ )	noexcept	{	return get< 2 >( pt_ );		}

	template< floating F, std::size_t N >	requires( N != std::dynamic_extent && N > 2 )
	constexpr F & z    ( span<       F, N > pt_ )	noexcept	{	return get< 2 >( pt_ );		}
	/// @}


	/// Add the elements pairwise.
	template< arithmetic A, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< A, N > operator+( span< A, N > pt0_, span< A, N > pt1_ )		noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( t0 + t1 ) ... });
	}

	/// Subtract the elements pairwise.
	template< arithmetic A, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< A, N > operator-( span< A, N > pt0_, span< A, N > pt1_ )		noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return Point< A, N >({ ( t0 - t1 ) ... });
	}

	/// Calculate the euclidian distance squared between two points.
	/// Aake the std::sqrt on the result to get the actual euclidian distance.
	template< arithmetic A, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr A distance2( span< A, N > pt0_, span< A, N > pt1_ )					noexcept	{
		static constexpr auto square =			  []( A t ){ return t*t; };
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + square( t0 - t1 ));
	}

	/// Calculate the dot product of two points. 
	template< arithmetic A, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr A dot_product( span< A, N > pt0_, span< A, N > pt1_ )					noexcept	{
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + ( t0*t1 ) );
	}

	/// The cross-product.
	template< arithmetic A >
	constexpr Point< A, 3 > cross_product( span< A, 3 > pt0_, span< A, 3 > pt1_ ) 	noexcept	{
		return Point({ 
			pt0_[ 1 ]*pt1_[ 2 ] - pt0_[ 2 ]*pt1_[ 1 ],
			pt0_[ 2 ]*pt1_[ 0 ] - pt0_[ 0 ]*pt1_[ 2 ],
			pt0_[ 0 ]*pt1_[ 1 ] - pt0_[ 1 ]*pt1_[ 0 ]
		});
	}

}	// namespace pax
