//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>


namespace pax { 

	/// Returns the closest number less than or equal to value_ that is evenly divisible by abs( factor_ ).
	/// If factor_ == 0, value_ is returned.
	template< typename T >
	constexpr T align_le( const T value_, T factor_ ) noexcept {
		factor_ = std::abs( factor_ );
		const T temp = factor_ ? ( factor_ * std::floor( value_ / factor_ ) ) : value_;
		return  temp - ( ( temp > value_ ) ? factor_ : T{} );
	}


	/// Returns the closest number greater than or equal to value_ that is evenly divisible by abs( factor_ ).
	/// If factor_ == 0, value_ is returned.
	template< typename T >
	constexpr T align_ge( const T value_, T factor_ ) noexcept {
		factor_ = std::abs( factor_ );
		const T temp = factor_ ? ( factor_ * std::floor( value_ / factor_ ) ) : value_;
		return temp + ( ( temp < value_) ? factor_ : T{} );
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
