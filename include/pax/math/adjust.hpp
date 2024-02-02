//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>


namespace pax { 

	/// Returns the modulo of x_ and y_ (floating point or integer).
	/** If y_ == 0, 0 is returned.																	**/
	template< typename T >
	constexpr T modulo( const T x_, const T y_ ) noexcept {
		if constexpr( std::is_floating_point_v< T > )	return y_ ? std::fmod( x_, y_ )	: T{};
		if constexpr( std::is_integral_v< T > )			return y_ ? ( x_ % y_ )			: T{};
	}


	/// Returns the closest number less than or equal to value_ that is evenly divisible by factor_.
	/** If factor_ == 0, value_ is returned.														**/
	template< typename T >
	constexpr T align_le( const T value_, const T factor_ ) noexcept {
		const T	mod{ modulo( value_, factor_ ) };
		return	( ( value_ >= T{} ) || ( mod == T{} ) )	
			? ( value_ - mod )
			: ( value_ - mod + ( ( factor_ >= T{} ) ? -factor_ : factor_ ) );
	}


	/// Returns the closest number greater than or equal to value_ that is evenly divisible by factor_.
	/** If factor_ == 0, value_ is returned.														**/
	template< typename T >
	constexpr T align_ge( const T value_, const T factor_ ) noexcept {
		const T	mod{ modulo( value_, factor_ ) };
		return	( ( value_ <  T{} ) || ( mod == T{} ) )	
			? ( value_ - mod )
			: ( value_ - mod + ( ( factor_ <  T{} ) ? -factor_ : factor_ ) );
	}
	
	
	/// Returns v_ + e, where e is the smalest value to make v_ + e distinguishable from v_ in type T. 
	/** If v_ == Limits::max(), Limits::max() is returned.											**/
	template< typename T >
	constexpr T nudge_up( const T v_ ) noexcept {
		using std::nextafter;
		constexpr auto limit{ std::numeric_limits< T >::max() };
		if constexpr( std::is_integral_v< T > )		return ( v_ < limit ) ? v_+1					: limit;
		else										return ( v_ < limit ) ? nextafter( v_, limit )	: limit;
	}


	/// Returns v_ - e, where e is the smalest value to make v_ - e distinguishable from v_ in type T. 
	/** If v_ == Limits::lowest(), Limits::lowest() is returned.									**/
	template< typename T >
	constexpr T nudge_down( const T v_ ) noexcept {
		using std::nextafter;
		constexpr auto limit{ std::numeric_limits< T >::lowest() };
		if constexpr( std::is_integral_v< T > )		return ( v_ > limit ) ? v_-1					: limit;
		else										return ( v_ > limit ) ? nextafter( v_, limit )	: limit;
	}

}	// namespace pax
