//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"

// #include <pax/std/algorithm.hpp>
#include <pax/math/power.hpp>

#include <span>
#include <vector>
#include <numeric>
#include <cmath>


namespace pax {
	
	using std::span;


	/// Minkowski P size/distance with no root.
	template< std::size_t P, arithmetic A, std::size_t N > 
	constexpr A minkowskiP( std::array< A, N > pt_ ) 							noexcept	{
		auto [ ... p ]			  = pt_;
		if constexpr( P == 0 )		return std::max({ A{}, ( ( p >= 0 ) ? p : -p ) ... });
		else						return ( abs_power( p, P ) + ... + A{} );
	}

	/// Minkowski P distance with no root.
	template< std::size_t P, arithmetic A, std::size_t N >
	constexpr A minkowskiP(
		const std::array< A, N >  & pt0_,
		const std::array< A, N >  & pt1_
	) noexcept {
		constexpr const auto absdiff = []( A a, A b ){	return ( a > b ) ? a-b : b-a;	};
		auto [ ... p0 ]			  = pt0_;
		auto [ ... p1 ]			  = pt1_;
		if constexpr( P == 0 )		return std::max({ A{}, absdiff( p0, p1 ) ... });
		else						return ( power( absdiff( p0, p1 ), P ) + ... + A{} );
	}

	/// Minkowski P distance.
	template< std::size_t P, typename A, std::size_t N >
	constexpr auto minkowski( std::array< A, N > pt_ ) 							noexcept	{
		using F = std::common_type_t< A, double >;
		if constexpr ( P == 0 )		return			minkowskiP< P >( pt_ );
		else						return root( F( minkowskiP< P >( pt_ ) ), P );
	}

	/// Minkowski P distance.
	template< std::size_t P, arithmetic A, std::size_t N >
	constexpr auto minkowski(
		const std::array< A, N >   & pt0_,
		const std::array< A, N >   & pt1_
	) noexcept {
		using F = std::common_type_t< A, double >;
		if constexpr ( P == 0 )		return			minkowskiP< P >( pt0_, pt1_ );
		else						return root( F( minkowskiP< P >( pt0_, pt1_ ) ), P );
	}

	/// Chebyshev distance (minkowski distance, with infinite P).
	template< typename ... A >
	constexpr auto chebyshev ( A && ...t_ )		noexcept	{	return minkowski < 0 >( std::forward< A >( t_ )... );		}

	/// Manhattan distance (minkowski distance, with P = 1).
	template< typename ... A >
	constexpr auto manhattan ( A && ...t_ )		noexcept	{	return minkowski < 1 >( std::forward< A >( t_ )... );		}

	/// Euclidean distance (minkowski distance, with P = 2).
	template< typename ... A >
	constexpr auto euclidean ( A && ...t_ )		noexcept	{	return minkowski < 2 >( std::forward< A >( t_ )... );		}

	/// Squared euclidean distance (squared minkowski distance, with P = 2).
	template< typename ... A >
	constexpr auto euclidean2( A && ...t_ )		noexcept	{	return minkowskiP< 2 >( std::forward< A >( t_ )... );		}



	/// Hamming distance.
	template< integer Int, std::size_t N >
	constexpr Int hamming(
		const std::array< Int, N >   & pt0_,
		const std::array< Int, N >   & pt1_
	) noexcept {
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( Int{} + ... + ( t0 != t1 ) );
	}


	/// Canberra distance.
	template< arithmetic A, std::size_t N >
	constexpr auto canberra(
		const std::array< A, N >   & pt0_,
		const std::array< A, N >   & pt1_
	) noexcept {
		using F = std::common_type_t< A, double >;
		constexpr const auto f = []( F a, F b ){
			//			absdiff( a, b )		 / (	abs( a ) 			   + 	abs( b ) 			   ): 
			return ( ( a > b ) ? a-b : b-a ) / ( ( ( a >= A{} ) ? a : -a ) + ( ( b >= A{} ) ? b : -b ) );
		};
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + f( t0, t1 ) );
	}



	/// Levenshtein distance.
	/// The Levenshtein distance between two words is the minimum number of single-character edits
	///	(insertions, deletions or substitutions) required to change one word into the other.
	template< integer Int, std::size_t N0, std::size_t N1 >
	constexpr Int levenshtein(
		const span< Int, N0 >	pt0_,
		const span< Int, N1 >	pt1_
	) {
		if( pt1_.size() < pt0_.size() )	levenshtein( pt1_, pt0_ );

		using Idx			  = std::size_t;
		const Idx cols		  = pt0_.size() + 1u;
		const Idx rows		  = pt1_.size() + 1u;

		using Int2 = std::remove_cv_t< Int >;
		std::vector< Int2 >		t( 2*cols );
		auto prev			  = t.data() + cols;
		auto now			  = t.data();
		std::iota( now, prev, Int{} );

		for( Idx r{ 1 }; r<rows; ++r ) {
			std::swap( now, prev );
			*now			  = r;
			for( Idx c{ 1 }; c<cols; ++c ) {
				now[ c ]	  = std::min({
					prev[ c - 1u ] + ( pt0_[ c - 1u ] != pt1_[ r - 1u ] ),
					prev[ c      ] + 1,
					now [ c - 1u ] + 1
				});
			}
		}
		return *( now + cols - 1 );
	}
	
}	// namespace pax
