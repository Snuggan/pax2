//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/format.hpp"
#include <chrono>
#include <string_view>
#include <cmath>		// std::isinf

namespace pax {

	template< typename Rep, typename Ratio >
	constexpr double seconds_to_double( const std::chrono::duration< Rep, Ratio > duration_ ) noexcept {
		if constexpr( Ratio::num == 1 ) {
			if constexpr( Ratio::den == 1 )			return double( duration_.count() );
			else									return double( duration_.count() )/Ratio::den;
		} else if constexpr( Ratio::den == 1 )		return double( duration_.count() )*Ratio::num;
		else										return double( duration_.count() )*Ratio::num/Ratio::den;
	}
	

	inline std::string seconds_to_string( const double secs_, int digits_ = 3 ) {
		std::string					result{ "~zero time" };
		if( digits_ == 0 )			digits_ =  3;
		else if( digits_ ==  1 )	digits_ =  2;
		else if( digits_ == -1 )	digits_ = -2;

		if( !( secs_ <= 0 ) ) {	// To ensure inf and nan execute this.
			static constexpr double	minute	  = 60;
			static constexpr double	hour	  = minute *  60;
			static constexpr double	day		  = hour   *  24;
			static constexpr double	week	  = day    *   7;
			static constexpr double	year	  = day    * 365.25;
			double					duration  = secs_;
			std::string_view		unit{ " " };

			if     ( duration <         100 )	{	unit = " s";								}
			else if( duration <  minute*100 )	{	unit = " min";		duration/= minute;		}
			else if( duration <  day        )	{	unit = " h";		duration/= hour;		}
			else if( duration <  day   * 30 )	{	unit = " days";		duration/= day;			}
			else if( duration <  year       )	{	unit = " weeks";	duration/= week;		}
			else if( duration != duration   ) 	{	/* Handles the NaN case. */	 				}
			else if( std::isinf( duration ) )	{	unit = "  time";							}
			else 								{	unit = " years";	duration/= year;		}

			result = std20::format( runtime_format( "{:.{}g}" ), duration, ( ( digits_ < 0 ) ? -digits_ : digits_ ) );
			result+= ( ( result.back() >= '0' ) && ( result.back() <= '9' ) ) ? unit : unit.substr( 1 );
		}

		if( digits_ < 0 ) {		// Horizontal '.' alignment.
			const int 	ins{ int( digits_ < -2 ) + int( result[ 1 ] == '.' ) - int( result[ 3 ] == '.' ) };
			result.insert( 0, ins, ' ' );
			result.append( 13 - digits_ - result.size() + ( ( result.find( "\u00b5" ) != std::string::npos ) ? 2 : 0 ), ' ' );
		}
		return result;
	}

}	// namespace pax
