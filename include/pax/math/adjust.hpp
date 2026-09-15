//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>


namespace pax { 

	/// Returns the closest number less than or equal to v_ that is evenly divisible by abs( res_ ).
	/// If res_ == 0, v_ is returned.
	template< typename T >
	constexpr T align_le( const T v_, T res_ ) noexcept {
		res_		  = std::abs( res_ );
		const T temp  = res_ ? res_ * std::floor( v_ / res_ ) : v_;
		if constexpr( std::is_floating_point_v< T > )	return temp;
		else											return temp - ( ( temp > v_ ) ? res_ : T{} );
	}


	/// Returns the closest number greater than or equal to v_ that is evenly divisible by abs( res_ ).
	/// If res_ == 0, v_ is returned.
	template< typename T >
	constexpr T align_ge( const T v_, T res_ ) noexcept {
		res_ = std::abs( res_ );
		const T temp = res_ ? ( res_ * std::ceil( v_ / res_ ) ) : v_;
		if constexpr( std::is_floating_point_v< T > )	return temp;
		else											return temp + ( ( temp < v_) ? res_ : T{} );
	}
	
	
	/// Returns v_ + e, where e is the smalest value to make v_ + e distinguishable from v_ in type T. 
	/// If v_ == Limits::max(), Limits::max() is returned.
	template< typename T >
	constexpr T nudge_up( const T v_ ) noexcept {
		using std::nextafter, std::numeric_limits;
		static constexpr auto highest{ numeric_limits< T >::max() };
		if constexpr( std::is_integral_v< T > )		return ( v_ < highest ) ? v_+1						: highest;
		else										return ( v_ < highest ) ? nextafter( v_, highest )	: highest;
	}


	/// Returns v_ - e, where e is the smalest value to make v_ - e distinguishable from v_ in type T. 
	/// If v_ == Limits::lowest(), Limits::lowest() is returned.
	template< typename T >
	constexpr T nudge_down( const T v_ ) noexcept {
		using std::nextafter, std::numeric_limits;
		static constexpr auto lowest{ numeric_limits< T >::lowest() };
		if constexpr( std::is_integral_v< T > )		return ( v_ > lowest ) ? v_-1						: lowest;
		else										return ( v_ > lowest ) ? nextafter( v_, lowest )	: lowest;
	}

}	// namespace pax
