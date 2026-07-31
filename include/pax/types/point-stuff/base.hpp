//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/concepts.hpp>


// From https://lemire.me/blog/2024/07/26/safer-code-in-c-with-lifetime-bounds/
#ifndef __has_cpp_attribute
	#define ada_lifetime_bound
#elif __has_cpp_attribute(msvc::lifetimebound)
	#define ada_lifetime_bound [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
	#define ada_lifetime_bound [[clang::lifetimebound]]
#elif __has_cpp_attribute(lifetimebound)
	#define ada_lifetime_bound [[lifetimebound]]
#else
	#define ada_lifetime_bound
#endif


namespace pax {
	using traits::contiguous, traits::value_type_t, traits::extent_v, traits::character;

	template< typename ... I >	concept integer		= ( std::integral< I > && ... );
	template< typename ... U >	concept uinteger	= ( std::unsigned_integral< U > && ... );
	template< typename ... F >	concept floating	= ( std::floating_point< F > && ... );
	template< typename ... A >	concept arithmetic	= ( ( floating< A > || integer< A > ) && ... );

	template< std::size_t N >
	constexpr bool is_static		= ( N != traits::dynamic_extent );

	template< typename ... V >
	concept all_arithmetic			= contiguous< V ... > && arithmetic< value_type_t< V > ... >;
	
	template< typename ... V >
	concept static_extent			= contiguous< V ... > && ( is_static< extent_v< V > > && ... );
	
	template< typename V0, typename V1 >
	concept same_static_extent		= contiguous< V0, V1 > 
		&& static_extent< V0, V1 > && ( extent_v< V0 > == extent_v< V1 > );
	
	template< typename V0, typename V1 >
	concept pairwise_comparable		= contiguous< V0, V1 >
		&& std::equality_comparable_with< value_type_t< V0 >, value_type_t< V1 > >;



	template< contiguous V >
	constexpr std::size_t No_null_extent = extent_v< V >;

	template< character Char, std::size_t N >
	constexpr std::size_t No_null_extent< Char ( & )[ N ] > = N - 1;



	/// The non-character case. It never has a \0 ending. 
	template< contiguous V >	
	[[nodiscard]] constexpr auto no_nullchar_end( V && v_ )	{
		using std::end; 
		return end( v_ );
	}

	/// A string of characters. It may have a \0 ending.
	template< traits::string V >	
	[[nodiscard]] constexpr auto no_nullchar_end( V && str_ ) {
		using std::end, std::size;
		return end( str_ ) - ( size( str_ ) && !*( end( str_ ) - 1 ) );
	}

	/// A basic array of characters. It always has a \0 ending. 
	template< character Char, std::size_t N >
	[[nodiscard]] constexpr Char * no_nullchar_end( Char ( & str_ )[ N ] )	{
		return str_ + N - ( N && !*( str_ + N - 1 ) );
	}

	template< typename T >
	[[nodiscard]] constexpr std::size_t no_nullchar_size( T && t_ )	{
		using std::begin; 
		return no_nullchar_end( t_ ) - begin( t_ );
	}
	static_assert( no_nullchar_size( "abc" ) == 3 );

}	// namespace pax
