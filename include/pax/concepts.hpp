//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <iterator>
#include <type_traits>


namespace pax::traits {
	template< typename T >		using clean			= std::remove_cvref< T >;
	template< typename T >		using clean_t 		= clean< T >::type;
	template< std::size_t I >	using size_constant = std::integral_constant< std::size_t, I >;

	/// Returns true iff type T is the same as any of the Other types.
	template< typename T, typename ...Other >
	static constexpr bool any_of_v			= ( false || ... || std::is_same_v< T, Other > );

	/// The value that represents a dynamic size, where other values would be the static size.
	constexpr std::size_t dynamic_extent	= -1;

	/// A concept to match T*, T[], T( & )[], T[ N ], or T( & )[ N ].
	template< typename T >
	concept array_like						= std::is_pointer_v< clean_t< T > > || std::rank_v< clean_t< T > > == 1;

	/// Do T store or reference contiguous elements, i.e. std::array, std::string, std::span etc.
	template< typename T >
	concept size_contiguous
		 = std::ranges::contiguous_range	< clean_t< T > >
		|| std::is_bounded_array_v			< clean_t< T > >;	// int[ N ] and int ( & )[ N ]

	/// Either size_contiguous or pointer..
	template< typename T >
	concept contiguous						= size_contiguous< T > || array_like< T >;

	/// Does an instance have a size?
	template< typename T >
	concept has_size						= std::ranges::sized_range< clean_t< T > >;

	/// Does the type T have a [static] size?
	template< typename T >
	concept has_extent
		 =	0u <= std::tuple_size			< clean_t< T > >::value
		||	std::is_bounded_array_v			< clean_t< T > >	// T[ N ], T( & )[ N ]
		||	clean_t< T >::extent != dynamic_extent;				// std::span< T, N > doesn't have tuple_size...

	/// Does an instance have only a dynamic size?
	template< typename T >
	concept has_dynamic_size				= has_size< T > && !has_extent< T >;

	/// Does T have an element_type, [probably] in addition to the value_type?
	template< typename T >
	concept has_declared_element_type		= requires { typename T::element_type; };

	namespace detail {
		template< typename T >	struct Element_type;
		template< typename T >	struct Element_type< const T >		{	using type = const    Element_type< T >::type;	};
		template< typename T >	struct Element_type< volatile T >	{	using type = volatile Element_type< T >::type;	};

		template< typename T >				  requires( std::is_pointer_v< T > )
		struct Element_type< T >			: std::remove_pointer< T > {};

		template< typename T >				  requires( std::is_array_v< T > )
		struct Element_type< T >			: std::remove_all_extents< T > {};

		template< size_contiguous T >		  requires(  has_declared_element_type< T > )
		struct Element_type< T >			{ using type = typename T::element_type; };

		template< size_contiguous T >		  requires( !has_declared_element_type< T > && !array_like< T > )
		struct Element_type< T >			: std::conditional< 
			std::is_same_v< typename T::iterator, typename T::const_iterator >,
			const typename T::value_type,	// std::string_view needs this...
			typename T::value_type
		> {};

		template< typename T >				  struct extent;
	
		template< has_dynamic_size T >
		struct extent< T >					: size_constant< dynamic_extent > {};
	
		template< has_extent T >			  requires( requires{ T::extent; } )
		struct extent< T > 					: size_constant< T::extent > {};

		template< has_extent T >			  requires( std::is_bounded_array_v< clean_t< T > > )
		struct extent< T > 					: std::extent< clean_t< T > > {};

		template< has_extent T >			  requires( !requires{ T::extent; } && 
											  std::tuple_size< clean_t< T > >::value >= 0 )
		struct extent< T > 					: std::tuple_size< clean_t< T > > {};
	}

	/// Get the value type of T.
	template< typename T >
	using element_type_t 					= detail::Element_type< std::remove_reference_t< T > >::type;

	/// Get the value type of T.
	template< typename T >
	using value_type_t 						= clean_t< element_type_t< T > >;

	/// The "size" of a type. 
	/**	- dynamic_extent: for dynamically sized containers with contigous elements.
		- N: for statically sized containers with N contigous elements.
		- Other type with no contigous elements do not have a specialization.		**/
	template< typename T >
	constexpr std::size_t 		extent_v	= detail::extent< std::remove_cvref_t< T > >::value;

	/// Is T a character type. 
	template< typename T >
	concept character						= any_of_v< clean_t< T >, char, signed char, unsigned char, 
																	wchar_t, char8_t, char16_t, char32_t >;

	/// A concept to match character arrays.
	template< typename T >
	concept character_array					= array_like< T > && character< value_type_t< T > >;

	template< typename T >
	concept string							= contiguous< T > && character< value_type_t< T > >;
}	// namespace pax::traits

namespace pax {

	template< typename T >
	[[nodiscard]] constexpr std::size_t shave_zero_suffix( const T &, const std::size_t sz_ )		{	return sz_;		}

	template< traits::string Str >				  requires( traits::character_array< Str > )
	[[nodiscard]] constexpr std::size_t shave_zero_suffix( const Str & str_, const std::size_t sz_ )
	{	return sz_ - ( sz_ && !str_[ sz_ - 1 ] );																		}

}	// namespace pax

namespace std {
	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * begin( Ch * const & c_ )		noexcept	{	return c_;								}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * end( Ch * const & c_ )			noexcept	{
		Ch *				itr = c_;
		if( c_ && *c_ )		while( *( ++itr ) );
		return itr;
	}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * data( Ch * const & c_ )		noexcept	{	return c_;								}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr std::size_t size( Ch * const & c_ )	noexcept	{	return end( c_ ) - c_;					}

	template< pax::traits::character Ch, std::size_t N >		// In char arrays the null character is counted. 
	[[nodiscard]] constexpr std::size_t size( Ch( & c_ )[ N ] )	noexcept	{	return pax::shave_zero_suffix( c_, N );	}

	template< pax::traits::character Ch, std::size_t N >		// In char arrays the null character is counted. 
	[[nodiscard]] constexpr Ch * end( Ch( & c_ )[ N ] )			noexcept	{	return c_ + size( c_ );					}

}	// namespace std
