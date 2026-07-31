//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"
#include <array>
#include <utility>		// std::forward
#include <format>


namespace pax {
	
	using std::get;
	template< typename T, typename ... U >	struct Table_meta;
	template< typename T >					struct Object_meta;



	/// This is the type used for arithmetic vectors with a fixed size.
	template< arithmetic A, std::size_t N >										requires( is_static< N > )
	using Point = std::array< A, N >;

	using Point2d		  = Point< double, 2 >;
	using Point3d		  = Point< double, 3 >;
	
	/// Create a Point out of a bunch of elements.
	template< arithmetic A, arithmetic ... As >	
	constexpr Point< A, sizeof...( As ) > point_t( As && ... as_ )				noexcept	{
		return { static_cast< A >( std::forward< As >( as_ ) ) ... };
	}
	template< arithmetic ... As >	
	constexpr auto point( As && ... as_ )	noexcept	{
		using A  = std::common_type_t< As ... >;
		return Point< A, sizeof...( As ) >{ static_cast< A >( std::forward< As >( as_ ) ) ... };
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
	
	/// Create a Point out of a bunch of elements.
	template< uinteger ... Uis >	
	constexpr Index< sizeof...( Uis ) > index( Uis && ... uis_ )				noexcept	{
		return { static_cast< std::size_t >( std::forward< Uis >( uis_ ) ) ... };
	}
	


	/// Some access functions.
	/// Using them makes the code clearer and guarantees a consistent mapping name -> index.
	/// col, east, x -> 0, row, north, y -> 1, z -> 2.
	/// @{
	static constexpr std::size_t 	col_idx = 0u;
	template< uinteger U, std::size_t N >				requires( N > col_idx )
	constexpr U   col  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< col_idx >( pt_ );	}
	template< uinteger U, std::size_t N >				requires( N > col_idx )
	constexpr U & col  (       Point< U, N > & pt_ )	noexcept	{	return std::get< col_idx >( pt_ );	}

	static constexpr std::size_t 	row_idx = col_idx + 1u;
	template< uinteger U, std::size_t N >				requires( N > row_idx )
	constexpr U   row  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< row_idx >( pt_ );	}
	template< uinteger U, std::size_t N >				requires( N > row_idx )
	constexpr U & row  (       Point< U, N > & pt_ )	noexcept	{	return std::get< row_idx >( pt_ );	}

	static constexpr std::size_t 	x_idx = 0u;
	template< floating F, std::size_t N >				requires( N > x_idx )
	constexpr F   x    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< x_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > x_idx )
	constexpr F & x    (       Point< F, N > & pt_ )	noexcept	{	return std::get< x_idx >( pt_ );	}

	static constexpr std::size_t 	y_idx = x_idx + 1u;
	template< floating F, std::size_t N >				requires( N > y_idx )
	constexpr F   y    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< y_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > y_idx )
	constexpr F & y    (       Point< F, N > & pt_ )	noexcept	{	return std::get< y_idx >( pt_ );	}

	static constexpr std::size_t 	z_idx = y_idx + 1u;
	template< floating F, std::size_t N >				requires( N > z_idx )
	constexpr F   z    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< z_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > z_idx )
	constexpr F & z    (       Point< F, N > & pt_ )	noexcept	{	return std::get< z_idx >( pt_ );	}

	static constexpr std::size_t 	east_idx = x_idx;
	template< floating F, std::size_t N >				requires( N > east_idx )
	constexpr F   east ( const Point< F, N > & pt_ )	noexcept	{	return std::get< east_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > east_idx )
	constexpr F & east (       Point< F, N > & pt_ )	noexcept	{	return std::get< east_idx >( pt_ );	}

	static constexpr std::size_t 	north_idx = y_idx;
	template< floating F, std::size_t N >				requires( N > north_idx )
	constexpr F   north( const Point< F, N > & pt_ )	noexcept	{	return std::get< north_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > north_idx )
	constexpr F & north(       Point< F, N > & pt_ )	noexcept	{	return std::get< north_idx >( pt_ );	}

	static constexpr std::size_t 	lon_idx = x_idx;
	template< floating F, std::size_t N >				requires( N > lon_idx )
	constexpr F   lon  ( const Point< F, N > & pt_ )	noexcept	{	return std::get< lon_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > lon_idx )
	constexpr F & lon  (       Point< F, N > & pt_ )	noexcept	{	return std::get< lon_idx >( pt_ );	}

	static constexpr std::size_t 	lat_idx = y_idx;
	template< floating F, std::size_t N >				requires( N > lat_idx )
	constexpr F   lat  ( const Point< F, N > & pt_ )	noexcept	{	return std::get< lat_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > lat_idx )
	constexpr F & lat  (       Point< F, N > & pt_ )	noexcept	{	return std::get< lat_idx >( pt_ );	}
	/// @}


	/// Check if all elements in pt0_ are smaller than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_lt( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( true && ... && ( t0 <  t1 ) );
	}

	/// Check if all elements in pt0_ are smaller or equal than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_le( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( true && ... && ( t0 <= t1 ) );
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > min( Point< A, N > pt0_, Point< A, N > pt1_ )		noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return Point< A, N >({ ( ( t0 <= t1 ) ? t0 : t1 ) ... });
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > max( Point< A, N > pt0_, Point< A, N > pt1_ )		noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return Point< A, N >({ ( ( t0 >= t1 ) ? t0 : t1 ) ... });
	}


	/// Add the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > operator+( Point< A, N > pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return Point< A, N >({ ( t0 + t1 ) ... });
	}

	/// Subtract the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > operator-( Point< A, N > pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return Point< A, N >({ ( t0 - t1 ) ... });
	}

	/// Calculate the euclidian distance squared between two points.
	/// Aake the std::sqrt on the result to get the actual euclidian distance.
	template< arithmetic A, std::size_t N >
	constexpr A distance2( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		static constexpr auto square =			  []( A t ){ return t*t; };
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( A{} + ... + square( t0 - t1 ));
	}

	/// Calculate the dot product of two points. 
	template< arithmetic A, std::size_t N >
	constexpr A dot_product( Point< A, N > pt0_, Point< A, N > pt1_ )			noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
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
