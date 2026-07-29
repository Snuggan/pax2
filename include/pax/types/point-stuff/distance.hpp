//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "point.hpp"
#include <pax/math/power.hpp>

#include <cassert>
#include <vector>
#include <algorithm>		// std::sort, std::unique
#include <numeric>			// std::iota


namespace pax {
	
	using traits::contiguous, traits::sized_contiguous;


	/// Minkowski P size/distance with no root.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - Must have a static extent, the present implementation depends on it. 
	template< std::size_t P, sized_contiguous Cont > 
		requires( all_arithmetic< Cont > && all_static_extent< Cont > )
	constexpr auto minkowskiP( 
		Cont		 && v_
	) noexcept {
		auto [ ... p ]		  = v_;
		if constexpr( P == 0 )	return std::max({ 0, ( ( p >= 0 ) ? p : -p ) ... });
		else					return ( abs_power( p, P ) + ... + 0 );
	}

	/// Minkowski P distance with no root.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - Must have a static extent, the present implementation depends on it. 
	template< std::size_t P, sized_contiguous Cont0, sized_contiguous Cont1 >
		requires( all_arithmetic< Cont0, Cont1 > && all_same_static_extent< Cont0, Cont1 > )
	constexpr auto minkowskiP(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		static constexpr const auto absdiff = []( auto a, auto b ){	return ( a > b ) ? a-b : b-a;	};
		auto [ ... p0 ] = v0_;	
		auto [ ... p1 ] = v1_;
		if constexpr( P == 0 )	return std::max({ 0, absdiff( p0, p1 ) ... });
		else					return ( power( absdiff( p0, p1 ), P ) + ... + 0 );
	}

	/// Minkowski P distance.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - (The containers must have the same number of elements, as the distance is defined that way.) 
	/// - Must have a static extent, the present implementation depends on it. 
	template< std::size_t P, sized_contiguous ... Cont >
		requires( all_arithmetic< Cont ... > && all_static_extent< Cont ... > )
	constexpr auto minkowski(
		Cont		 && ... v_
	) noexcept {
		using F = std::common_type_t< double, traits::value_type_t< Cont > ... >;
		if constexpr ( P == 0 )	return			minkowskiP< P >( std::forward< Cont >( v_ )... );
		else					return root( F( minkowskiP< P >( std::forward< Cont >( v_ )... ) ), P );
	}

	/// Chebyshev distance (minkowski distance, with infinite P).
	/// "Chessboard distance": the maximum difference between the coordinates of two points along any single axis.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - (The containers must have the same number of elements, as the distance is defined that way.) 
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous ... Cont >		requires( all_arithmetic< Cont ... > && all_static_extent< Cont ... > )
	constexpr auto chebyshev ( Cont && ... v_ )	noexcept	{	return minkowski < 0 >( std::forward< Cont >( v_ ) ... );	}

	/// Manhattan distance (minkowski distance, with P = 1).
	/// The sum of the absolute differences between the coordinates of two points.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - (The containers must have the same number of elements, as the distance is defined that way.) 
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous ... Cont >		requires( all_arithmetic< Cont ... > && all_static_extent< Cont ... > )
	constexpr auto manhattan ( Cont && ... v_ )	noexcept	{	return minkowski < 1 >( std::forward< Cont >( v_ ) ... );	}

	/// Euclidean distance (minkowski distance, with P = 2).
	/// The Euclidean distance is the straight-line distance between two points in a flat space.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - (The containers must have the same number of elements, as the distance is defined that way.) 
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous ... Cont >		requires( all_arithmetic< Cont ... > && all_static_extent< Cont ... > )
	constexpr auto euclidean ( Cont && ... v_ )	noexcept	{	return minkowski < 2 >( std::forward< Cont >( v_ ) ... );	}

	/// Squared euclidean distance (squared minkowski distance, with P = 2).
	/// Same as the euclidean distance squared. Useful when comparing distances. 
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - (The containers must have the same number of elements, as the distance is defined that way.) 
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous ... Cont >		requires( all_arithmetic< Cont ... > && all_static_extent< Cont ... > )
	constexpr auto euclidean2( Cont && ... v_ )	noexcept	{	return minkowskiP< 2 >( std::forward< Cont >( v_ ) ... );	}


	/// Canberra distance.
	/// The weighted version of Manhattan distance.
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	/// - The containers must have the same number of elements, as the distance is defined that way.
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous Cont0, sized_contiguous Cont1 >
	requires( all_arithmetic< Cont0, Cont1 > && all_same_static_extent< Cont0, Cont1 > )
	constexpr auto canberra(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		using F = std::common_type_t< double, traits::value_type_t< Cont0 >, traits::value_type_t< Cont1 > >;
		constexpr const auto f = []( F a, F b ) {	// absdiff( a, b ) / (	abs( a ) + abs( b ) ): 
			return ( ( a > b ) ? a-b : b-a ) / ( ( ( a >= F{} ) ? a : -a ) + ( ( b >= F{} ) ? b : -b ) );
		};
		auto [ ...t0 ] = v0_;
		auto [ ...t1 ] = v1_;
		return ( F{} + ... + f( t0, t1 ) );
	}


	/// Cosine distance.
	/// The cosine of the angle between the vectors: the dot product divided by the product of their lengths.
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - The elements must be an arithmetic type, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( all_arithmetic< Cont0, Cont1 > )
	constexpr double cosine(
		Cont0		 && v0_,
		Cont1		 && v1_
	) {
		using 			std::begin, std::size;
		double			dot{}, nmA{}, nmB{};
		auto			itrA = begin( v0_ );
		auto			itrB = begin( v1_ );
		const auto		endA = no_nullchar_end( v0_ );
		assert( ( endA - itrA == no_nullchar_end( v1_ ) - itrB ) && "The containers must have the same number of elements." );
		while( itrA < endA ) {
			dot		 += *itrA * *itrB;
			nmA		 += *itrA * *itrA;
			nmB 	 += *itrB * *itrB;
			++itrA;	++itrB;
		}
		return 1 - dot / ( std::sqrt( nmA ) * std::sqrt( nmB ) );
	}


	/// Kendall Tao distance.
	/// A metric that measures the number of pairwise disagreements between two ranking lists.
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr std::size_t kendall_tau(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		using std::begin, std::size;
		std::size_t 		count{};
		const std::size_t 	n = no_nullchar_size( v0_ );

		assert( ( n == no_nullchar_size( v1_) ) && "The containers must have the same number of elements." );
		for( std::size_t i = 0; i < n; ++i )
			for( std::size_t j = i + 1; j < n; ++j )
				// Check if the order is different in the two rankings
				count	+=	( v0_[ i ] < v0_[ j ] && v1_[ i ] > v1_[ j ] ) 
						||	( v0_[ i ] > v0_[ j ] && v1_[ i ] < v1_[ j ] );
		return count;
	}


	/// Normalized Kendall Tao distance.
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr double normalized_kendall_tau(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		using std::size;
		return kendall_tau( v0_, v1_ )*2.0 / ( size( v0_ )*( size( v0_ ) - 1.0 ) );
	}


	/// Hamming distance.
	/// Hamming distance measures the number of positions at which two strings of equal length differ.
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - Must have a static extent, the present implementation depends on it. 
	template< sized_contiguous Cont0, sized_contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > && all_same_static_extent< Cont0, Cont1 > )
	constexpr std::size_t hamming(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		auto [ ...t0 ] = v0_;
		auto [ ...t1 ] = v1_;
		return ( int{} + ... + ( t0 != t1 ) );
	}
	
	
	/// Sorensen-Dice distance. 
	/// - The containers must have the same number of elements, as the distance is defined that way.. 
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr double sorensen_dice(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		using std::begin, std::size;
		std::size_t 		matches{};
		auto				itr0 = begin( v0_ );
		auto				itr1 = begin( v1_ );
		const auto			end0 = no_nullchar_end( v0_ ) - 1;
		const std::size_t 	len  = end0 - itr0;
		assert( ( end0 - itr0 + 1 == no_nullchar_end( v1_ ) - itr1 ) && "The containers must have the same number of elements." );
		if( itr0 + 1u > end0 )	return 0.0;

		while( itr0 < end0 ) {
			matches		 += ( *itr0 == *itr1 ) && ( *( itr0 + 1 ) == *( itr1 + 1 ) );
			++itr0;	++itr1;
		}
		return matches / double( len );
	}


	/// Shannon index.
	/// A diversity index in the ecological literature. 
	/// - The elements must be an unsigned integer type, as the index is defined that way.
	template< contiguous Cont >
		requires( uinteger< traits::value_type_t< Cont > > )
	double shannon_index(
		Cont		 && v_
	) {
		double			H{};
		std::size_t		total{};

		// Total number of individuals
		for( std::size_t count : v_ )		total += count;

		// Calculate Shannon's index
		for( std::size_t count : v_ ) 
			if( count > 0 ) {
				const double p = static_cast< double >( count )/total;
				H			  -= p * std::log( p );
			}
		return H;
	}
	

	/// First sorts a copy of v_ and then remove doubles. Returns a std::vector.
	template< contiguous Cont >
	constexpr auto sorted_unique( Cont && v_ ) {
		using std::begin;
		std::vector< traits::value_type_t< Cont > >		arr( no_nullchar_size( v_ ) );
		std::copy( begin( v_ ), no_nullchar_end( v_ ), arr.begin() );
		std::sort( arr.begin(), arr.end() );
		const auto itr = std::unique( arr.begin(), arr.end() );
		arr.resize( itr - arr.begin() );
		return arr;
	}


	/// Jaccard distance. 
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr double jaccard(
		Cont0		 && v0_,
		Cont1		 && v1_
	) noexcept {
		std::size_t 	common{};
		const auto		arr0 = sorted_unique( v0_ );
		auto			itr0 = arr0.begin();
		const auto		end0 = no_nullchar_end( arr0 );
		const auto		arr1 = sorted_unique( v1_ );
		auto			itr1 = arr1.begin();
		const auto		end1 = no_nullchar_end( arr1 );
		while( ( itr0 < end0 ) && ( itr1 < end1 ) ) {
			if( *itr0 == *itr1 ) {
				++common;
				++itr0;
				++itr1;
			} else if( *itr0 < *itr1 )		++itr0;
			else							++itr1;
		}
		return common / double( arr0.size() + arr1.size() - common );
	}


	/// Jaro similarity. 
	/// A string metric measuring an edit distance between two sequences.
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr double jaro_similarity(
		Cont0		 && v0_,
		Cont1		 && v1_
	) {
		const std::size_t 		sz0 = no_nullchar_size( v0_ );
		const std::size_t 		sz1 = no_nullchar_size( v1_ );
	
		if( !sz0 && !sz1 )		return 1.0;		// Both strings are empty
		if( !sz0 || !sz1 )		return 0.0;		// One string is empty

		const int max_dist	  = std::floor( std::max( sz0, sz1 ) / 2.0 ) - 1.0;
		std::vector< bool >		matched1( sz0, false );
		std::vector< bool >		matched2( sz1, false );
		std::size_t 			matches{};

		// Count matching characters.
		for( std::size_t i = 0; i < sz0; ++i ) {
			for( std::size_t j = std::max( std::int64_t{}, std::int64_t( i ) - max_dist ); 
				 			 j < std::min( sz1, i + max_dist + 1 ); 
							 ++j 
			) {
				if( !matched2[ j ] && ( v0_[ i ] == v1_[ j ] ) ) {
					matched1[ i ] = matched2[ j ] = true;
					++matches;
					break;
				}
			}
		}
		if( matches == 0 )		return 0.0;		// No matches found

		// Count transpositions.
		std::size_t 			t{};
		std::size_t 			pt{};
		for( std::size_t i = 0; i < sz0; ++i ) {
			if( matched1[ i ] ) {
				while( !matched2[ pt ] )	++pt;
				t 			 += v0_[ i ] != v1_[ pt ];
				++pt;
			}
		}
		t /= 2;		// Each transposition is counted twice

		return (	// Calculate Jaro similarity
				static_cast< double >( matches ) / sz0 
			+	static_cast< double >( matches ) / sz1 
			+	static_cast< double >( matches - t ) / matches 
			) / 3.0;
	}


	/// Jaro-Winkler similarity. 
	/// A string metric measuring an edit distance between two sequences.
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr double jaro_winkler_similarity(
		Cont0		 && v0_,
		Cont1		 && v1_
	) {
		// Calculate common prefix length (up to 4 characters)
		std::size_t 			prefix_length{};
		const std::size_t 		stop = std::min({ std::size_t( 4 ), no_nullchar_size( v0_ ), no_nullchar_size( v1_ ) });
		for( std::size_t i = 0; i < stop && ( v0_[ i ] == v1_[ i ] ); ++i ) 
			++prefix_length;

		const double			jaro_sim = jaro_similarity( v0_, v1_ );
		return jaro_sim + ( 0.1 * prefix_length * ( 1.0 - jaro_sim ) );
	}


	/// Levenshtein distance.
	/// The Levenshtein distance between two words is the minimum number of single-character edits
	///	(insertions, deletions or substitutions) required to change one word into the other.
	/// - The elements must have pairwise comparable types, as the distance is defined that way.
	template< contiguous Cont0, contiguous Cont1 >
		requires( pairwise_comparable< Cont0, Cont1 > )
	constexpr std::size_t levenshtein(
		Cont0		 && v0_,
		Cont1		 && v1_
	) {
		using std::data, std::size_t;
		if( size( v1_ ) < size( v0_ ) )	levenshtein( v1_, v0_ );

		using Idx			  = std::size_t;
		const Idx cols		  = size( v0_ ) + 1u;
		const Idx rows		  = size( v1_ ) + 1u;

		using Int = std::common_type_t< traits::value_type_t< Cont0 >, traits::value_type_t< Cont1 > >;
		std::vector< Int >		t( 2*cols );
		auto prev			  = t.data() + cols;
		auto now			  = t.data();
		std::iota( now, prev, Int{} );

		for( Idx r{ 1 }; r<rows; ++r ) {
			std::swap( now, prev );
			*now			  = r;
			for( Idx c{ 1 }; c<cols; ++c ) {
				now[ c ]	  = std::min({
					prev[ c - 1u ] + ( v0_[ c - 1u ] != v1_[ r - 1u ] ),
					prev[ c	  ] + 1,
					now [ c - 1u ] + 1
				});
			}
		}
		return *( now + cols - 1 );
	}

}	// namespace pax
