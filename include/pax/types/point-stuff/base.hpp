//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/concepts.hpp>


#define PAX_SPANTOOLS_VERSION_MAJOR		1
#define PAX_SPANTOOLS_VERSION_MINOR		0
#define PAX_SPANTOOLS_VERSION_PATCH		0


namespace pax {

	template< std::size_t N >
	constexpr bool is_static  = ( N != traits::dynamic_extent );

	template< typename I >	concept integer		  = std::integral< I >;
	template< typename U >	concept uinteger	  = std::unsigned_integral< U >;
	template< typename F >	concept floating	  = std::floating_point< F >;
	template< typename A >	concept arithmetic	  = floating< A > || integer< A >;

	template< typename ... Cont >
	concept all_contiguous		  = ( traits::contiguous< Cont > && ... );
	
	template< typename ... Cont >
	concept all_arithmetic		  =	all_contiguous< Cont ... >
		&&	( arithmetic< traits::value_type_t< Cont > > && ... );
	
	template< typename Cont0, typename Cont1 >
	concept pairwise_comparable	  =	all_contiguous< Cont0, Cont1 >
		&&	std::equality_comparable_with< traits::value_type_t< Cont0 >, traits::value_type_t< Cont1 > >;
	
	template< typename ... Cont >
	concept all_static_extent	  = all_contiguous< Cont ... >
		&&	( is_static< traits::extent_v< Cont > > && ... );
	
	template< typename Cont0, typename Cont1 >
	concept all_same_static_extent	  =	all_contiguous< Cont0, Cont1 > && all_static_extent< Cont0, Cont1 >
		&&	( traits::extent_v< Cont0 > == traits::extent_v< Cont1 > );



	template< traits::contiguous Cont >
	constexpr std::size_t No_null_extent = traits::extent_v< Cont >;

	template< traits::character Char, std::size_t N >
	constexpr std::size_t No_null_extent< Char ( & )[ N ] > = N - 1;



	/// The non-character case. It never has a \0 ending. 
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto no_nullchar_end( Cont && v_ )	{	using std::end; return end( v_ );		}

	/// A string of characters. It may have a \0 ending. 
	template< traits::contiguous Cont >					requires( traits::character< traits::element_type_t< Cont > > )
	[[nodiscard]] constexpr auto no_nullchar_end( Cont && v_ )	{
		using std::end, std::size;
		return end( v_ ) - !( !size( v_ ) || *( end( v_ ) - 1 ) );
	}

	/// A basic array of characters. It always has a \0 ending. 
	template< traits::character Char, std::size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Char const * no_nullchar_end( Char const ( & str_ )[ N ] )	{
		return str_ + N - !str_[ N - 1 ];
	}


	template< typename T >
	[[nodiscard]] constexpr std::size_t no_nullchar_size( T && t_ )	{
		using std::begin; 
		return no_nullchar_end( t_ ) - begin( t_ );
	}
	static_assert( no_nullchar_size( "abc" ) == 3 );

}	// namespace pax
