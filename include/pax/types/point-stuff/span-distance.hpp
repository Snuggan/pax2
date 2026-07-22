//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"

// #include <pax/std/algorithm.hpp>
#include <pax/math/power.hpp>

#include <span>
#include <vector>
#include <numeric>


namespace pax {
	
	using std::span;


	/// Minkowski P size/distance with no root.
	template< std::size_t P, arithmetic A, std::size_t N > 
	constexpr A minkowskiP( span< A, N > pt_ ) 											noexcept	{
		if constexpr( P == 0 )
			return std::accumulate( pt_.begin(), pt_.end(), A{}, []( const A & acc, const A & a ) ->A {
				A t = ( a < 0 ) ? -a : a;
				return ( acc < t ) ? t : acc; 
			} );
		else if constexpr( N > 3 )
			return std::accumulate( pt_.begin(), pt_.end(), A{}, []( const A & acc, A a ) ->A { return acc + abs_power( a, P ); } );
		else {
			auto					acc  = abs_power( pt_.back(),  P );
			if constexpr( N > 2 )	acc += abs_power( pt_[ 1 ],    P );
			if constexpr( N > 1 )	acc += abs_power( pt_.front(), P );
			return	acc;
		}
	}

	/// Minkowski P distance with no root.
	template< std::size_t P, arithmetic A, std::size_t N >
	constexpr A minkowskiP(
		std::array< A, N > 			pt0_,
		std::array< A, N > 			pt1_
	) noexcept {
		constexpr const auto absdiff = []( A a, A b ){	return ( a > b ) ? a-b : b-a;	};
		constexpr const auto max     = []( A a, A b ){	return ( a > b ) ? a   : b; 	};
		A						acc{};
		// auto [ ... p0 ]		  = pt0_;
		// auto [ ... p1 ]		  = pt1_;
		if constexpr( P == 0 ) {
			// { ( acc  = max( acc, absdiff( p0, p1 ) ) ... };
			on_each_pair( pt0_, pt1_, [ &acc ]( auto a, auto b ){ acc =  max( acc, absdiff( a, b ) ); } );
		} else {
			// { ( acc += power( absdiff( p0, p1 ), P ) ) ... };
			on_each_pair( pt0_, pt1_, [ &acc ]( auto a, auto b ){ acc += power( absdiff( a, b ), P ); } );
		}
		return acc;
	}

	/// Minkowski P distance.
	template< std::size_t P, typename A, std::size_t N >
	constexpr auto minkowski( span< A, N > pt_ ) 											noexcept	{
		using F = std::common_type_t< A, float >;
		if constexpr ( P == 0 )	return          minkowskiP< P >( pt_ );
		else					return root( F( minkowskiP< P >( pt_ ) ), P );
	}

	/// Minkowski P distance.
	template< std::size_t P, arithmetic A, std::size_t N >
	constexpr auto minkowski(
		const span< A, N >		pt0_,
		const span< A, N >		pt1_
	) noexcept {
		if constexpr ( P == 0 )	return       minkowskiP< P >( pt0_, pt1_ );
		else					return root( minkowskiP< P >( pt0_, pt1_ ), P );
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
	template< arithmetic A, std::size_t N >
	constexpr A hamming(
		const span< A, N >			pt0_,
		const span< A, N >			pt1_
	) noexcept {
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( A{} + ... + ( t0 != t1 ) );
	}


	/// Canberra distance.
	template< floating F, std::size_t N >
	constexpr auto canberra(
		const span< F, N >			pt0_,
		const span< F, N >			pt1_
	) noexcept {
		constexpr const auto f = []( F a, F b ){
			//			absdiff( a, b )		 / (	abs( a ) 			   + 	abs( b ) 			   ): 
			return ( ( a > b ) ? a-b : b-a ) / ( ( ( a >= F{} ) ? a : -a ) + ( ( b >= F{} ) ? b : -b ) );
		};
		auto [ ...t0 ] = pt0_;		auto [ ...t1 ] = pt1_;
		return ( F{} + ... + f( t0 - t1 ) );
	}



	/// Levenshtein distance.
	/// The Levenshtein distance between two words is the minimum number of single-character edits
	///	(insertions, deletions or substitutions) required to change one word into the other.
	template< integer Int, std::size_t N0, std::size_t N1 >
	constexpr Int levenshtein(
		const span< Int, N0 >	pt0_,
		const span< Int, N1 >	pt1_
	) {
		constexpr auto min	  = []( Int a, const Int b, const Int c ) noexcept {
			a	 = ( a < b ) ? a : b;
			return ( a < c ) ? a : c;
		};

		if( pt1_.size() < pt0_.size() )	levenshtein( pt1_, pt0_ );

		using Idx			  = std::size_t;
		const Idx cols		  = pt0_.size() + 1u;
		const Idx rows		  = pt1_.size() + 1u;

		std::vector< Int >		t( 2*cols );
		auto prev			  = t.data() + cols;
		auto now			  = t.data();
		std::iota( now, prev, Int{} );

		for( Idx r{ 1 }; r<rows; ++r ) {
			std::swap( now, prev );
			*now			  = r;
			for( Idx c{ 1 }; c<cols; ++c ) {
				now[ c ]	  = min(
					prev[ c - 1u ] + ( pt0_[ c - 1u ] != pt1_[ r - 1u ] ),
					prev[ c      ] + 1,
					now [ c - 1u ] + 1
				);
			}
		}
		return *( now + cols - 1 );
	}
	
}	// namespace pax
