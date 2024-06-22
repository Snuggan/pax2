//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>


namespace pax { 

	// Implements the power of N by clever repeated multiplication.
	template< typename T, typename Int > 
		requires std::is_integral_v< Int >
	constexpr T power_detail( T v_, Int n_ )		noexcept	{
		switch( n_ ) {
			case  0: 	return T( 1 );
			case  1: 	return v_;
			case  2: 	return v_*v_;
			case  3: 	return v_*v_*v_;
		}
		if constexpr( std::is_signed_v< Int > )
			if( n_ < 0 ) return T( 1 )/power_detail( v_, -n_ );
	
		// Remove the power of two part of n_.
		while( !( n_ & 1 ) ) {	// While n_ is even...
			n_>>= 1;			// Half n_.
			v_*= v_;			// Square v_.
		}

		// If n_ was a power of two exactly, we are done.
		if( n_ == 1 )			return v_;

		// For any power n_...
		T	r = v_;				// After the above, we know that n_ is odd.
		while( true ) {
			n_>>= 1;			// Half n_.
			v_*= v_;			// Square v_.
			if( n_ & 1 ) {		// If n_ is odd...
				r*=v_;
				if( n_ == 1 )	return r;
			}
		}
	}

	/// Calculates the power of N by clever repeated multiplication.
	/** - If T is complex and n_ is a non-integer value, you need to include <complex> before including this file.	**/
	template< typename T, typename P > 
	constexpr auto power( const T & v_, P n_ )		noexcept	{
		if constexpr( std::is_integral_v< P > )	return power_detail( v_, n_ );
		else return ( n_ == int( n_ ) )				 ? power_detail( v_, int( n_ ) )
													 : std::pow( v_, n_ );
	}

	/// Calculates the square: v_ * v_.
	template< typename T > 
	constexpr auto square( const T & v_ ) 			noexcept	{	return v_ * v_;									}

	/// Calculates the cube: v_ * v_ * v_.
	template< typename T > 
	constexpr auto cube( const T & v_ ) 			noexcept	{	return v_ * v_ * v_;							}

	/// Calculates the absolute value of the power of N by clever repeated multiplication.
	template< typename T, typename P > 
	constexpr auto abs_power( const T & v_, P n_ )	noexcept	{	return power( ( ( v_ < 0 ) ? -v_ : v_ ), n_ );	}
	
	



	template< typename T > 
	constexpr auto cbrt( const T & v_ )						noexcept	{	return std::cbrt( v_ );					}

	// What We handle both real and complex numbers...
	template< typename T > 
	struct root_detail_t {
		using type = std::remove_cv_t< T >;
		static constexpr bool is_negative( const T & t_ )	noexcept	{	return t_ < T{};						}
	};

	template< typename T > 
	constexpr auto root_detail( const T & v_, int n_ )		noexcept	{
		constexpr typename root_detail_t< T >::type			one = 1;
		using std::pow;
		switch( n_ ) {
			case  0: 	return std::numeric_limits< T >::quiet_NaN();		// Undefined, actually...
			case  1: 	return v_;
			case  2: 	return std::sqrt( v_ );
			case  3: 	return pax::cbrt( v_ );
			default: 	return ( n_ < 0 )
								? root_detail( T( 1 )/v_, -n_ )
								: ( root_detail_t< T >::is_negative( v_ ) && ( n_ & 1 ) )	
									? -pow( -v_, one/n_ )
									:  pow(  v_, one/n_ );
		}
	}


	/// Calculates the n_:th root of v_.
	/** \returns
		- n_ <  0: 1/std::cbrt( v_ ).
		- n_ == 0: NaN.
		- n_ == 1: v_.
		- n_ == 2: std::sqrt( v_ ).
		- n_ == 3: std::cbrt( v_ ).
		- n_ >  3 and v_ >= 0: std::pow( v_, 1/n_ ).
		- n_ >  3 and v_ <  0 and odd n_: -std::pow( -v_, 1/n_ ).
	**/
	template< typename T, typename P > 
	constexpr auto root( const T & v_, P n_ )	noexcept	{
		if constexpr( std::is_integral_v< P > )	return root_detail( v_, n_ );
		else return ( n_ == int( n_ ) ) 			 ? root_detail( v_, int( n_ ) )
													 : std::pow( v_, 1/n_ );	// v_ must be non-complex.
	}

	/// Returns the square root of v_.
	template< typename T > 
	constexpr auto square_root( const T & v_ )	noexcept	{	return std::sqrt( v_ );								}

	/// Returns the cube root of v_.
	template< typename T > 
	constexpr auto cube_root( const T & v_ )	noexcept	{	return cbrt( v_ );									}

}	// namespace pax
