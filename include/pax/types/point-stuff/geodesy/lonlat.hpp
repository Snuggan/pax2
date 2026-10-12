//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../base.hpp"
#include "spheroid.hpp"
#include <array>
#include <utility>		// std::forward


namespace pax {

	/// This is the type used for arithmetic vectors with a fixed size.
	template< Spheroid Sph, floating F, std::size_t N >
	using Lonlat = std::array< F, N >;

	using Lonlat2d		  = Lonlat< earth::wgs1984, double, 2 >;
	using Lonlat3d		  = Lonlat< earth::wgs1984, double, 3 >;

	/// Create a Lonlat with element type F out of a bunch of elements.
	template< Spheroid Sph, floating F, arithmetic ... Fs >
	constexpr Lonlat< Sph, F, sizeof...( Fs ) > point_t( Fs && ... as_ )						noexcept	{
		return { static_cast< F >( std::forward< Fs >( as_ ) ) ... };
	}

	/// Create a Lonlat out of a bunch of elements.
	template< Spheroid Sph, arithmetic ... Fs >
	constexpr auto lonlat( Fs && ... as_ )	noexcept	{
		using F  = std::common_type_t< Fs ... >;
		return Lonlat< Sph, F, sizeof...( Fs ) >{ static_cast< F >( std::forward< Fs >( as_ ) ) ... };
	}

	/// Create a Lonlat with specified size N and all elements set to a_.
	template< Spheroid Sph, std::size_t N, floating F >
	constexpr auto lonlat( const F a_ )		noexcept	{
		Lonlat< Sph, F, N >		temp;
		temp.fill( a_ );
		return temp;
	}


	enum {	lon_idx  = 0,	lat_idx, 	};
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lon_idx )
	constexpr F   lon( const Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lon_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lon_idx )
	constexpr F & lon(       Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lon_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lat_idx )
	constexpr F   lat( const Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lat_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lat_idx )
	constexpr F & lat(       Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lat_idx >( ll_ );		}

	template< Spheroid Sph, floating F, std::size_t N >
	constexpr F spheroid( const Lonlat< Sph, F, N > & )		noexcept	{	return Sph;								}

	/// The angle between ll0_ and ll1_ expressed in radians. 
	template< Spheroid Sph, floating F, std::size_t N >
	constexpr F angle(
		const Lonlat< Sph, F, N > & ll0_, 
		const Lonlat< Sph, F, N > & ll1_ 
	)	noexcept;

	/// The distance along the surface between ll0_ and ll1_ expressed in meters
	template< Spheroid Sph, floating F, std::size_t N >
	constexpr F distance(
		const Lonlat< Sph, F, N > & ll0_, 
		const Lonlat< Sph, F, N > & ll1_ 
	)	noexcept;

}	// namespace pax
