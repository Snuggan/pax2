//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "spheroid.hpp"
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

}	// namespace pax
