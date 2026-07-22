//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <concepts>

#define PAX_SPANTOOLS_VERSIONIONER( _MAJOR_, _MINOR_, _PATCH_ ) ( ( _MAJOR_ *100 + _MINOR_ )*10000  + _PATCH_ )
#define PAX_SPANTOOLS_VERSION_MAJOR		1
#define PAX_SPANTOOLS_VERSION_MINOR		0
#define PAX_SPANTOOLS_VERSION_PATCH		0
#define PAX_SPANTOOLS_VERSION	PAX_SPANTOOLS_VERSIONIONER( \
	PAX_SPANTOOLS_VERSION_MAJOR, 							\
	PAX_SPANTOOLS_VERSION_MINOR, 							\
	PAX_SPANTOOLS_VERSION_PATCH )


namespace pax {

	template< typename I >	concept integer		  = std::integral< I >;
	template< typename U >	concept uinteger	  = std::unsigned_integral< U >;
	template< typename F >	concept floating	  = std::floating_point< F >;
	template< typename A >	concept arithmetic	  = floating< A > || integer< A >;
	

	template< std::size_t N >
	constexpr bool is_static  = ( N != traits::dynamic_extent );

	
	template< typename T >
	[[nodiscard]] constexpr auto no_nullchar_end( T && t_ )	{	using std::end; return end( t_ );		}

	template< traits::character Char, std::size_t N >			requires( N > 0 )
	[[nodiscard]] constexpr Char const * no_nullchar_end( Char const ( & str_ )[ N ] )	{
		return str_ + N - !str_[ N - 1 ];
	}

	template< std::ranges::contiguous_range Cont >				requires( traits::character< traits::element_type_t< Cont > > )
	[[nodiscard]] constexpr auto no_nullchar_end( Cont && cont_ )	{
		using std::end, std::empty;
		return end( cont_ ) - !( empty( cont_ ) || *( end( cont_ ) - 1 ) );
	}

}	// namespace pax
