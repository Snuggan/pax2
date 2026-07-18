//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/std/algorithm.hpp>
#include <pax/math/power.hpp>

#include <concepts>
#include <span>
#include <array>
#include <vector>
#include <numeric>


namespace pax {
	
	//	Some point-like utilities.
	//	Point< T, N > arguments are in the form of std::span< T, N >, returned points are std::array< T, N >.


	template< typename T >	concept integer		  = std::integral< T >;
	template< typename T >	concept uinteger	  = std::unsigned_integral< T >;
	template< typename T >	concept floating	  = std::floating_point< T >;
	template< typename T >	concept arithmetic	  = ( floating< T > || integer< T > );
	

	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	using Point = std::array< T, N >;


	template< std::size_t I, arithmetic T, std::size_t N >	requires( N != std::dynamic_extent && N > I )
	constexpr T   get( std::span< const T, N > pt_ )	noexcept	{
		using std::get;
		return get< I >( pt_ );
	}

	template< std::size_t I, arithmetic T, std::size_t N >	requires( N != std::dynamic_extent && N > I )
	constexpr T & get( std::span<       T, N > pt_ )	noexcept	{
		using std::get;
		return get< I >( pt_ );
	}


	template< uinteger T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T   col( std::span< const T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< uinteger T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T & col( std::span<       T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< uinteger T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T   row( std::span< const T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< uinteger T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T & row( std::span<       T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}


	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T   east ( std::span< const T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T & east ( std::span<       T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T   north( std::span< const T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T & north( std::span<       T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}


	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T   x( std::span<     const T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 0 )
	constexpr T & x( std::span<           T, N > pt_ )	noexcept	{	return get< 0 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T   y( std::span<     const T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 1 )
	constexpr T & y( std::span<           T, N > pt_ )	noexcept	{	return get< 1 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 2 )
	constexpr T   z( std::span<     const T, N > pt_ )	noexcept	{	return get< 2 >( pt_ );		}

	template< floating T, std::size_t N >	requires( N != std::dynamic_extent && N > 2 )
	constexpr T & z( std::span<           T, N > pt_ )	noexcept	{	return get< 2 >( pt_ );		}


	/// Create a new point by operating pairwise on the elements.
	template< std::size_t ...Is, arithmetic T, std::size_t N, typename Op >	requires( N != std::dynamic_extent )
	constexpr Point< T, N > operate( 
		std::span< T, N > pt0_, 
		std::span< T, N > pt1_, 
		Op && op_,
		std::index_sequence< Is... >
	) noexcept	{
		using std::get;
		return Point( op_( get< Is >( pt0_ ), get< Is >( pt1_ ) )... );
	}

	/// Add the elements pairwise.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< T, N > operator+( std::span< T, N > pt0_, std::span< T, N > pt1_ )			noexcept	{
		return operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 + t1; } );
	}

	/// Subtract the elements pairwise.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< T, N > operator-( std::span< T, N > pt0_, std::span< T, N > pt1_ )			noexcept	{
		return operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 - t1; } );
	}

	/// Calculate the dot product of two points. 
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr T dot_product( std::span< T, N > pt0_, std::span< T, N > pt1_ )			noexcept	{
		auto [ ...t ] = operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 * t1; } );
		return ( T{} + ... + t );
	}

	/// The cross-product.
	template< arithmetic T >
	constexpr Point< T, 3 > cross_product( std::span< T, 3 > pt0_, std::span< T, 3 > pt1_ ) 	noexcept	{
		return Point({ 
			pt0_[ 1 ]*pt1_[ 2 ] - pt0_[ 2 ]*pt1_[ 1 ],
			pt0_[ 2 ]*pt1_[ 0 ] - pt0_[ 0 ]*pt1_[ 2 ],
			pt0_[ 0 ]*pt1_[ 1 ] - pt0_[ 1 ]*pt1_[ 0 ]
		});
	}

	/// Check if all elements in pt0_ are smaller than the counterpart in pt1_.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr bool all_lt( std::span< T, N > pt0_, std::span< T, N > pt1_ )						noexcept	{
		auto [ ...t ] = operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 < t1; } );
		return ( true && ... && t );
	}

	/// Check if all elements in pt0_ are smaller or equal than the counterpart in pt1_.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr bool all_le( std::span< T, N > pt0_, std::span< T, N > pt1_ )						noexcept	{
		auto [ ...t ] = operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 <= t1; } );
		return ( true && ... && t );
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< T, N > min( std::span< T, N > pt0_, std::span< T, N > pt1_ )				noexcept	{
		auto [ ...p0 ] = pt0_;
		auto [ ...p1 ] = pt1_;
		return Point< T, N >({ ( ( p0 <= p1 ) ? p0 : p1 ) ... });
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr Point< T, N > max( std::span< T, N > pt0_, std::span< T, N > pt1_ )				noexcept	{
		auto [ ...p0 ] = pt0_;
		auto [ ...p1 ] = pt1_;
		return Point< T, N >({ ( ( p0 >= p1 ) ? p0 : p1 ) ... });
	}



	/// Calculate the euclidian distance squared between two points.
	/// Take the std::sqrt on the result to get the actual euclidian distance.
	template< arithmetic T, std::size_t N >	requires( N != std::dynamic_extent )
	constexpr T distance2( std::span< T, N > pt0_, std::span< T, N > pt1_ )			noexcept	{
		static constexpr auto sqr = []( T t ){ return t*t; };
		return operate( pt0_, pt1_, []( T t0, T t1 ){ return sqr( t0 - t1 ); } );
	}


	/// Minkowski P size/distance with no root.
	template< std::size_t P, arithmetic T, std::size_t N > 
	constexpr T minkowskiP( std::span< T, N > pt_ ) noexcept {
		if constexpr( P == 0 )
			return std::accumulate( pt_.begin(), pt_.end(), T{}, []( const T & acc, const T & a ) ->T {
				T t = ( a < 0 ) ? -a : a;
				return ( acc < t ) ? t : acc; 
			} );
		else if constexpr( N > 3 )
			return std::accumulate( pt_.begin(), pt_.end(), T{}, []( const T & acc, T a ) ->T { return acc + abs_power( a, P ); } );
		else {
			auto					acc  = abs_power( pt_.back(),  P );
			if constexpr( N > 2 )	acc += abs_power( pt_[ 1 ],    P );
			if constexpr( N > 1 )	acc += abs_power( pt_.front(), P );
			return	acc;
		}
	}

	/// Minkowski P distance with no root.
	template< std::size_t P, arithmetic T, std::size_t N >
	constexpr T minkowskiP(
		std::span< T, N > 			pt0_,
		std::span< T, N > 			pt1_
	) noexcept {
		constexpr const auto absdiff = []( T a, T b ){	return ( a > b ) ? a-b : b-a;	};
		constexpr const auto max     = []( T a, T b ){	return ( a > b ) ? a   : b; 	};
		T							acc{};
		if constexpr( P == 0 ) {
			on_each_pair( pt0_, pt1_, [ &acc ]( auto a, auto b ){ acc =  max( acc, absdiff( a, b ) ); } );
		} else {
			on_each_pair( pt0_, pt1_, [ &acc ]( auto a, auto b ){ acc += power( absdiff( a, b ), P ); } );
		}
		return acc;
	}

	/// Minkowski P distance.
	template< std::size_t P, typename T, std::size_t N >
	constexpr auto minkowski( std::span< T, N > pt_ ) noexcept {
		using F = std::common_type_t< T, float >;
		if constexpr ( P == 0 )	return          minkowskiP< P >( pt_ );
		else					return root( F( minkowskiP< P >( pt_ ) ), P );
	}

	/// Minkowski P distance.
	template< std::size_t P, arithmetic T, std::size_t N >
	constexpr auto minkowski(
		const std::span< T, N >		pt0_,
		const std::span< T, N >		pt1_
	) noexcept {
		if constexpr ( P == 0 )	return       minkowskiP< P >( pt0_, pt1_ );
		else					return root( minkowskiP< P >( pt0_, pt1_ ), P );
	}

	/// Chebyshev distance (minkowski distance, with infinite P).
	template< typename ...T >
	constexpr auto chebyshev ( T && ...t_ )		noexcept	{	return minkowski < 0 >( std::forward< T >( t_ )... );		}

	/// Manhattan distance (minkowski distance, with P = 1).
	template< typename ...T >
	constexpr auto manhattan ( T && ...t_ )		noexcept	{	return minkowski < 1 >( std::forward< T >( t_ )... );		}

	/// Euclidean distance (minkowski distance, with P = 2).
	template< typename ...T >
	constexpr auto euclidean ( T && ...t_ )		noexcept	{	return minkowski < 2 >( std::forward< T >( t_ )... );		}

	/// Squared euclidean distance (squared minkowski distance, with P = 2).
	template< typename ...T >
	constexpr auto euclidean2( T && ...t_ )		noexcept	{	return minkowskiP< 2 >( std::forward< T >( t_ )... );		}



	/// Hamming distance.
	template< typename T, std::size_t N >
	constexpr T hamming(
		const std::span< T, N >			pt0_,
		const std::span< T, N >			pt1_
	) noexcept {
		auto [ ...t ] = operate( pt0_, pt1_, []( T t0, T t1 ){ return t0 != t1; } );
		return ( T{} + ... + t );
	}


	/// Canberra distance.
	template< floating T, std::size_t N >
	constexpr auto canberra(
		const std::span< T, N >			pt0_,
		const std::span< T, N >			pt1_
	) noexcept {
		constexpr const auto f = []( T a, T b ){
			//			absdiff( a, b )		 / (	abs( a ) 			   + 	abs( b ) 			   ): 
			return ( ( a > b ) ? a-b : b-a ) / ( ( ( a >= T{} ) ? a : -a ) + ( ( b >= T{} ) ? b : -b ) );
		};
		auto [ ...t ] = operate( pt0_, pt1_, f );
		return ( T{} + ... + t );
	}



	/// Levenshtein distance.
	/** The Levenshtein distance between two words is the minimum number of single-character edits
		(insertions, deletions or substitutions) required to change one word into the other.	**/
	template< integer Int, std::size_t N0, std::size_t N1 >
	constexpr Int levenshtein(
		const std::span< Int, N0 >	pt0_,
		const std::span< Int, N1 >	pt1_
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
	

	template< arithmetic T, std::size_t N > class Box;
	

	template< arithmetic T, std::size_t N >			requires( arithmetic< T > && N != std::dynamic_extent )
	class Box< T, N > {
		std::array< std::array< T, N >, 2 >						m_box{};
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = std::array< T, N >;
		using 										Base	  = std::array< Point, 2 >;
		using Point::value_type;

		constexpr Box()										  = default;
		constexpr Box( const Box & )						  = default;
		constexpr Box( Box && )								  = default;
		constexpr Box & operator=( const Box & )			  = default;
		constexpr Box & operator=( Box && )					  = default;
		constexpr Box( std::span< T, N > pt0_, std::span< T, N > pt1_ )	noexcept
			: m_box({ min( pt0_, pt1_ ), max( pt0_, pt1_ ) }) {}

		constexpr const Point & min()							const noexcept	{	return m_box.first();	}
		constexpr const Point & max()							const noexcept	{	return m_box.last();	}

		constexpr Box & grow( std::span< T, N > pt_ )				  noexcept	{
			Base::first() = max( Base::first(), pt_ );
			Base::last () = max( Base::last (), pt_ );
			return *this;
		}

		constexpr bool strictly_inside( std::span< T, N > pt_ )	const noexcept	{
			return all_lt( pt_, max() ) && all_lt( min(), pt_ );
		}

		constexpr bool inside_or_on( std::span< T, N > pt_ )	const noexcept	{
			return all_le( pt_, max() ) && all_le( min(), pt_ );
		}

		constexpr bool in_range( std::span< T, N > pt_ )		const noexcept	{
			return all_lt( pt_, max() ) && all_le( min(), pt_ );
		}
	};


	/// Let the specialisation of unsigned always have zero as minimum. Useful for indeces.
	template< uinteger T, std::size_t N >			requires( uinteger< T > && N != std::dynamic_extent )
	class Box< T, N > {
		Point< T, 2 >											m_max{};
		static constexpr Point< T, 2 >							m_min{};
		using 										Span	  = std::span< T, N >;
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = std::array< T, N >;
		using Point::value_type;

		constexpr Box()										  = default;
		constexpr Box( const Box & )						  = default;
		constexpr Box( Box && )								  = default;
		constexpr Box & operator=( const Box & )			  = default;
		constexpr Box & operator=( Box && )					  = default;
		constexpr Box( std::span< T, N > size_ )				noexcept : m_max( size_ ) {}

		constexpr const Point & min()							const noexcept	{	return m_min;	}
		constexpr const Point & max()							const noexcept	{	return m_max;	}

		constexpr Box & grow( Span pt_ )							  noexcept	{
			m_max = max( m_max, pt_ );
			return *this;
		}

		constexpr bool strictly_inside( Span pt_ )				const noexcept	{	return all_lt( pt_, max() );	}
		constexpr bool inside_or_on( Span pt_ )					const noexcept	{	return all_le( pt_, max() );	}
		constexpr bool in_range( Span pt_ )						const noexcept	{	return strictly_inside( pt_ );	}
		
		/// Calculate an index into a vector for the index represented by pt_.
		template< uinteger ...U >								requires( sizeof...( U ) == N )
		constexpr T operator[]( U && ...u_ )					const noexcept;
		
		/// Calculate an index into a vector for the index represented by pt_.
		constexpr T operator[]( Point pt_ )						const noexcept	{
			auto [ ...t ] = pt_;
			return operator[]( t... );
		}
	};
	


	template< arithmetic T, std::size_t N >						requires( N != std::dynamic_extent )
	class Circle {
		std::array< T, N >										m_center{};
		T														m_radius{};
		
	public:
		static constexpr std::size_t 				rank	  = N;
		using 										Point	  = std::array< T, N >;
		using value_type = Point::value_type;

		constexpr Circle()									  = default;
		constexpr Circle( const Circle & )					  = default;
		constexpr Circle( Circle && )						  = default;
		constexpr Circle & operator=( const Circle & )		  = default;
		constexpr Circle & operator=( Circle && )			  = default;
		constexpr Circle( const std::span< T, N > & center_, const T radius_ ) noexcept
			: m_center( center_ ), m_radius{ ( radius_ >= 0 ) ? radius_ : -radius_ } {}

		constexpr const Point & center()						const noexcept	{	return m_center;	}
		constexpr value_type radius()							const noexcept	{	return m_radius;	}

		constexpr bool strictly_inside( std::span< T, N > pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <  radius()*radius();
		}

		constexpr bool inside_or_on( std::span< T, N > pt_ )	const noexcept	{
			return distance2( center(), pt_ ) <= radius()*radius();
		}
	};
}	// namespace pax
