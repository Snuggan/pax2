//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"
#include <array>
#include <cmath>		// std::fma, std::floor, std::abs
#include <utility>		// std::forward
#include <format>


namespace pax {
	
	using std::get;
	template< typename T, typename ... U >	struct Table_meta;
	template< typename T >					struct Object_meta;



	/// This is the type used for arithmetic vectors with a fixed size.
	template< arithmetic A, std::size_t N >									requires( is_static< N > )
	using Point = std::array< A, N >;

	using Point2d		  = Point< double, 2 >;
	using Point3d		  = Point< double, 3 >;
	
	/// Create a Point with element type A out of a bunch of elements.
	template< arithmetic A, arithmetic ... As >	
	constexpr Point< A, sizeof...( As ) > point_t( As && ... as_ )						noexcept	{
		return { static_cast< A >( std::forward< As >( as_ ) ) ... };
	}

	/// Create a Point out of a bunch of elements.
	template< arithmetic ... As >	
	constexpr auto point( As && ... as_ )	noexcept	{
		using A  = std::common_type_t< As ... >;
		return Point< A, sizeof...( As ) >{ static_cast< A >( std::forward< As >( as_ ) ) ... };
	}

	/// Create a Point with specified size N and all elements set to a_.
	template< std::size_t N, arithmetic A >	
	constexpr auto point( const A a_ )		noexcept	{
		Point< A, N >		temp;
		temp.fill( a_ );
		return temp;
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
	
	/// Create an Index out of a bunch of elements.
	template< uinteger ... Uis >	
	constexpr Index< sizeof...( Uis ) > index( Uis && ... uis_ )						noexcept	{
		return { static_cast< std::size_t >( std::forward< Uis >( uis_ ) ) ... };
	}
	


	/// Some access functions.
	/// Using them makes the code clearer and guarantees a consistent mapping name -> index.
	/// col, east, x -> 0, row, north, y -> 1, z -> 2.
	/// @{
	enum {
		col_idx  = 0,		row_idx, 
		x_idx    = 0,		y_idx,			z_idx,
		east_idx = 0,		north_idx,		altitude_idx, 
	};

	template< uinteger U, std::size_t N >				requires( N > col_idx )
	constexpr U   col  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< col_idx >( pt_ );		}
	template< uinteger U, std::size_t N >				requires( N > col_idx )
	constexpr U & col  (       Point< U, N > & pt_ )	noexcept	{	return std::get< col_idx >( pt_ );		}

	template< uinteger U, std::size_t N >				requires( N > row_idx )
	constexpr U   row  ( const Point< U, N > & pt_ )	noexcept	{	return std::get< row_idx >( pt_ );		}
	template< uinteger U, std::size_t N >				requires( N > row_idx )
	constexpr U & row  (       Point< U, N > & pt_ )	noexcept	{	return std::get< row_idx >( pt_ );		}

	template< floating F, std::size_t N >				requires( N > x_idx )
	constexpr F   x    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< x_idx >( pt_ );		}
	template< floating F, std::size_t N >				requires( N > x_idx )
	constexpr F & x    (       Point< F, N > & pt_ )	noexcept	{	return std::get< x_idx >( pt_ );		}

	template< floating F, std::size_t N >				requires( N > y_idx )
	constexpr F   y    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< y_idx >( pt_ );		}
	template< floating F, std::size_t N >				requires( N > y_idx )
	constexpr F & y    (       Point< F, N > & pt_ )	noexcept	{	return std::get< y_idx >( pt_ );		}

	template< floating F, std::size_t N >				requires( N > z_idx )
	constexpr F   z    ( const Point< F, N > & pt_ )	noexcept	{	return std::get< z_idx >( pt_ );		}
	template< floating F, std::size_t N >				requires( N > z_idx )
	constexpr F & z    (       Point< F, N > & pt_ )	noexcept	{	return std::get< z_idx >( pt_ );		}

	template< floating F, std::size_t N >				requires( N > east_idx )
	constexpr F   east ( const Point< F, N > & pt_ )	noexcept	{	return std::get< east_idx >( pt_ );		}
	template< floating F, std::size_t N >				requires( N > east_idx )
	constexpr F & east (       Point< F, N > & pt_ )	noexcept	{	return std::get< east_idx >( pt_ );		}

	template< floating F, std::size_t N >				requires( N > north_idx )
	constexpr F   north( const Point< F, N > & pt_ )	noexcept	{	return std::get< north_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > north_idx )
	constexpr F & north(       Point< F, N > & pt_ )	noexcept	{	return std::get< north_idx >( pt_ );	}

	template< floating F, std::size_t N >				requires( N > altitude_idx )
	constexpr F   altitude( const Point< F, N > & pt_ )	noexcept	{	return std::get< altitude_idx >( pt_ );	}
	template< floating F, std::size_t N >				requires( N > altitude_idx )
	constexpr F & altitude(       Point< F, N > & pt_ )	noexcept	{	return std::get< altitude_idx >( pt_ );	}
	/// @}


	/// Check if all elements in pt0_ are smaller than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_lt( Point< A, N > pt0_, Point< A, N > pt1_ )						noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( true && ... && ( t0 <  t1 ) );
	}

	/// Check if all elements in pt0_ are smaller or equal than the counterpart in pt1_.
	template< arithmetic A, std::size_t N >
	constexpr bool all_le( Point< A, N > pt0_, Point< A, N > pt1_ )						noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( true && ... && ( t0 <= t1 ) );
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > min( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return { ( ( t0 <= t1 ) ? t0 : t1 ) ... };
	}

	/// Return a pairwise min() of the elements of the arguments.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > max( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return { ( ( t0 >= t1 ) ? t0 : t1 ) ... };
	}


	/// Add (+=) the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > & operator+=( Point< A, N > & pt0_, Point< A, N > pt1_ )	noexcept	{
		auto & [ ... t0 ] = pt0_;
		auto   [ ... t1 ] = pt1_;
		( ( t0 += t1 ), ... );
		return pt0_;
	}

	/// Add the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N >   operator+ ( Point< A, N >   pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return { ( t0 + t1 ) ... };
	}

	/// Subtract the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > & operator-=( Point< A, N > & pt0_, Point< A, N > pt1_ )	noexcept	{
		auto & [ ... t0 ] = pt0_;
		auto   [ ... t1 ] = pt1_;
		( ( t0 -= t1 ), ... );
		return pt0_;
	}

	/// Subtract the elements pairwise.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N >   operator- ( Point< A, N >   pt0_, Point< A, N > pt1_ )	noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return { ( t0 - t1 ) ... };
	}

	/// Multiply the elements by a scalar.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N >   operator* ( Point< A, N >   pt_, A a_ )					noexcept	{
		auto [ ... t ] = pt_;
		return { ( t * a_ ) ... };
	}

	/// Multiply the elements by a scalar.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N >   operator* ( A a_, Point< A, N > pt_ )						noexcept	{	return pt_ * a_;	}

	/// Calculate the euclidean vector length squared.
	/// The std::sqrt of the result gives you the euclidian length.
	template< arithmetic A, std::size_t N >
	constexpr A euclidean2( Point< A, N > pt_ )											noexcept	{
		auto [ ... t ] = pt_;
		return ( A{} + ... + ( t*t ) );
	}

	/// Calculate the euclidian distance squared between two points.
	/// The std::sqrt of the result gives you the euclidian distance.
	template< arithmetic A, std::size_t N >
	constexpr A euclidean2( Point< A, N > pt0_, Point< A, N > pt1_ )					noexcept	{
		static constexpr auto square = []( A t ){ return t*t; };
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( A{} + ... + square( t0 - t1 ));
	}

	/// Result = orig_ + direction_*t_, using calls to std::fma. 
	template< floating F, std::size_t N >
	constexpr Point< F, N > movement( 
		const Point< F, N > orig_, 
		const Point< F, N > direction_, 
		const F				t_
	) noexcept {
		auto [ ... orig ] = orig_;
		auto [ ...  dir ] = direction_;
		return { ( std::fma( dir, t_, orig ) ) ... };
	}

	/// Calculate the dot product of two points. 
	template< arithmetic A, std::size_t N >
	constexpr A dot_product( Point< A, N > pt0_, Point< A, N > pt1_ )					noexcept	{
		auto [ ... t0 ] = pt0_;
		auto [ ... t1 ] = pt1_;
		return ( A{} + ... + ( t0*t1 ) );
	}

	/// The vector cross product. Zero for all N other than 3 or 7.
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > cross_product( Point< A, N >, Point< A, N > )				noexcept	{	return {};			}

	/// The vector cross product in R^3.
	template< arithmetic A >
	constexpr Point< A, 3 > cross_product( Point< A, 3 > x_, Point< A, 3 > y_ )			noexcept	{
		return { 
			x_[ 1 ]*y_[ 2 ] - x_[ 2 ]*y_[ 1 ],
			x_[ 2 ]*y_[ 0 ] - x_[ 0 ]*y_[ 2 ],
			x_[ 0 ]*y_[ 1 ] - x_[ 1 ]*y_[ 0 ]
		};
	}

	/// The vector cross product in R^7.
	template< arithmetic A >
	constexpr Point< A, 7 > cross_product( Point< A, 7 > x_, Point< A, 7 > y_ )			noexcept	{
		const auto xy = [ &x_, &y_ ]( std::size_t i, std::size_t j ) {
			return x_[ i ]*y_[ j ] - x_[ j ]*y_[ i ];
		};
		return { 
			xy( 1, 3 ) + xy( 2, 6 ) + xy( 4, 5 ), 
			xy( 2, 4 ) + xy( 3, 0 ) + xy( 5, 6 ), 
			xy( 3, 5 ) + xy( 4, 1 ) + xy( 6, 0 ), 
			xy( 4, 6 ) + xy( 5, 2 ) + xy( 0, 1 ), 
			xy( 5, 0 ) + xy( 6, 3 ) + xy( 1, 2 ), 
			xy( 6, 1 ) + xy( 0, 4 ) + xy( 2, 3 ), 
			xy( 0, 2 ) + xy( 1, 5 ) + xy( 3, 4 )
		};
	}

	/// This gives a number representing the length of the projection of pt0_ in the direction of pt1_.
	template< arithmetic A, std::size_t N >
	constexpr A projection_scalar( Point< A, N > pt0_, Point< A, N > pt1_ )				noexcept	{
		return dot_product( pt0_, pt1_ )/std::sqrt( euclidean2( pt1_ ) );
	}

	/// This gives a vector in the direction of pt1_ that represents the component of pt0_ along pt1_
	template< arithmetic A, std::size_t N >
	constexpr Point< A, N > projection_vector( Point< A, N > pt0_, Point< A, N > pt1_ )	noexcept	{
		return ( dot_product( pt0_, pt1_ )/euclidean2( pt1_ ) )*pt1_;
	}

}	// namespace pax
