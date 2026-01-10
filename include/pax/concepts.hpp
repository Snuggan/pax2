//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <iterator>
#include <type_traits>


namespace pax {

	namespace traits {
		template< typename T >		using clean			= std::remove_cvref< T >;
		template< typename T >		using clean_t 		= clean< T >::type;
		template< std::size_t I >	using size_constant = std::integral_constant< std::size_t, I >;


		/// The value that represents a dynamic size, where other values would be the static size.
		constexpr std::size_t dynamic_extent( -1 );

		/// A concept to match pointer, T[], T( & )[], T[ N ], or T( & )[ N ].
		template< typename T >
		concept array_like
			 = std::is_pointer_v				< clean_t< T > > 
			|| std::rank_v						< clean_t< T > > == 1;

		static_assert(  array_like< int[ 4 ] > );
		static_assert(  array_like< int( & )[ 4 ] > );
		static_assert(  array_like< int( & )[] > );
		static_assert(  array_like< int[] > );
		static_assert(  array_like< int * > );

		/// Do T store or reference contiguous elements, i.e. std::array, std::string, std::span etc.
		template< typename T >
		concept has_contiguous_sizeable
			 = std::ranges::contiguous_range	< clean_t< T > >
			|| std::is_bounded_array_v			< clean_t< T > >;	// int[ N ] and int ( & )[ N ]

		static_assert(  has_contiguous_sizeable< int[ 4 ] > );
		static_assert(  has_contiguous_sizeable< int( & )[ 4 ] > );
		static_assert( !has_contiguous_sizeable< int( & )[] > );
		static_assert( !has_contiguous_sizeable< int[] > );
		static_assert( !has_contiguous_sizeable< int * > );
	
		/// Either has_contiguous_sizeable or pointer..
		template< typename T >
		concept has_contiguous
			 = has_contiguous_sizeable			< T >
			|| array_like						< T >;

		static_assert(  has_contiguous< int[ 4 ] > );
		static_assert(  has_contiguous< int( & )[ 4 ] > );
		static_assert(  has_contiguous< int( & )[] > );
		static_assert(  has_contiguous< int[] > );
		static_assert(  has_contiguous< int * > );

		/// Does an instance have a size?
		template< typename T >
		concept has_size
			 = std::ranges::sized_range			< clean_t< T > >;	// int( & )[]

		static_assert(  has_size< int[ 4 ] > );
		static_assert(  has_size< int( & )[ 4 ] > );
		static_assert( !has_size< int( & )[] > );
		static_assert( !has_size< int[] > );
		static_assert( !has_size< int * > );

		/// Does the type T have a [static] size?
		template< typename T >
		concept has_extent
			 = ( 0 <= std::tuple_size			< clean_t< T > >::value ) 
			|| (		std::is_bounded_array_v	< clean_t< T > > 
					&&	1u == std::rank_v		< clean_t< T > >
			   )								  // T[ N ], T( & )[ N ]
			|| ( T::extent != dynamic_extent );	  // std::span< T, N > doesn't have tuple_size...

		static_assert(  has_extent< int[ 4 ] > );
		static_assert(  has_extent< int( & )[ 4 ] > );
		static_assert( !has_extent< int( & )[] > );
		static_assert( !has_extent< int[] > );
		static_assert( !has_extent< int * > );

		/// Does an instance have only a dynamic size?
		template< typename T >
		concept has_dynamic_size				= has_size< T > && !has_extent< T >;

		static_assert( !has_dynamic_size< int[ 4 ] > );
		static_assert( !has_dynamic_size< int( & )[ 4 ] > );
		static_assert( !has_dynamic_size< int( & )[] > );
		static_assert( !has_dynamic_size< int[] > );
		static_assert( !has_dynamic_size< int * > );

		/// Does T have an element_type, [probably] in addition to the value_type?
		template< typename T >
		concept has_declared_element_type		= requires { typename T::element_type; };

		namespace detail {
			template< typename T >	struct Element_type;
			template< typename T >	struct Element_type< T & >			:	Element_type< T > {};
			template< typename T >	struct Element_type< T && >			:	Element_type< T > {};
			template< typename T >	struct Element_type< const T >		{	using type = const Element_type< T >::type;		};
			template< typename T >	struct Element_type< volatile T >	{	using type = volatile Element_type< T >::type;	};

			template< typename T >				  requires( std::is_pointer_v< T > )
			struct Element_type< T >			: std::remove_pointer< T > {};

			template< typename T >				  requires( std::is_array_v< T > )
			struct Element_type< T >			: std::remove_all_extents< T > {};

			template< has_contiguous_sizeable T > requires(  has_declared_element_type< T > && !array_like< T > )
			struct Element_type< T >			{ using type = typename T::element_type; };

			template< has_contiguous_sizeable T > requires( !has_declared_element_type< T > && !array_like< T > )
			struct Element_type< T >			: std::conditional< std::is_same_v< typename T::iterator, typename T::const_iterator >,
				const typename T::value_type,	// std::string_view needs it...
				typename T::value_type
			> {};
		}

		/// Get the value type of T.
		template< typename T >
		using element_type_t 					= detail::Element_type< T >::type;

		/// Get the value type of T.
		template< typename T >
		using value_type_t 						= clean_t< element_type_t< T > >;

		namespace detail {
			template< typename T >	struct extent;
			template< typename T >	struct extent< T & >		: extent< T > {};
			template< typename T >	struct extent< T && >		: extent< T > {};
			template< typename T >	struct extent< const T >	: extent< T > {};
			template< typename T >	struct extent< volatile T >	: extent< T > {};
		
			template< traits::has_dynamic_size T >
			struct extent< T > : size_constant< dynamic_extent > {};
		
			template< traits::has_extent T >	requires( requires{ T::extent; } )
			struct extent< T > : size_constant< T::extent > {};

			template< traits::has_extent T >	requires( std::is_bounded_array_v< clean_t< T > > )
			struct extent< T > : std::extent< clean_t< T > > {};

			template< traits::has_extent T >	requires( std::tuple_size< clean_t< T > >::value >= 0 )
			struct extent< T > : std::tuple_size< clean_t< T > > {};
		}

		/// The "size" of a type. 
		/**	- dynamic_extent: for dynamically sized containers with contigous elements.
			- N: for statically sized containers with N contigous elements.
			- Other type with no contigous elements do not have a specialization.
		**/
		template< typename T >
		constexpr std::size_t 		extent_v	= detail::extent< T >::value;

		static_assert( extent_v< int[ 4 ] >			== 4u );
		static_assert( extent_v< int( & )[ 4 ] >	== 4u );

		namespace detail {
			template< typename T >	struct is_character					: std::false_type {};
			template<>				struct is_character< char >			: std::true_type {};
			template<>				struct is_character< signed char >	: std::true_type {};
			template<>				struct is_character< unsigned char >: std::true_type {};
			template<>				struct is_character< wchar_t >		: std::true_type {};
			template<>				struct is_character< char8_t >		: std::true_type {};
			template<>				struct is_character< char16_t >		: std::true_type {};
			template<>				struct is_character< char32_t >		: std::true_type {};
		}

		/// Is T a character type. 
		template< typename T >
		concept character						= detail::is_character< clean_t< T > >::value;

		/// A concept to match character arrays.
		template< typename T >
		concept character_array					= array_like< T > && character< value_type_t< T > >;

		template< typename T >
		concept string							= has_contiguous< T > && character< value_type_t< T > >;
	}

	constexpr std::size_t dynamic_extent = traits::dynamic_extent;

}	// namespace pax

namespace std {

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * begin( Ch * const & c_ )		noexcept	{	return c_;							}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * end( Ch * const & c_ )			noexcept	{
		Ch *				itr = c_;
		if( c_ && *c_ )		while( *( ++itr ) );
		return itr;
	}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr Ch * data( Ch * const & c_ )		noexcept	{	return c_;							}

	template< pax::traits::character Ch >
	[[nodiscard]] constexpr std::size_t size( Ch * const & c_ )	noexcept	{	return end( c_ ) - c_;				}

	template< pax::traits::character Ch, std::size_t N >		// In char arrays the null character is counted. 
	[[nodiscard]] constexpr std::size_t size( Ch( & c_ )[ N ] )	noexcept	{
		return ( N == 0 ) ? 0u : N - ( c_[ N-1 ] == 0 );
	}

	template< pax::traits::character Ch, std::size_t N >		// In char arrays the null character is counted. 
	[[nodiscard]] constexpr Ch * end( Ch( & c_ )[ N ] )			noexcept	{	return c_ + size( c_ );		}

}	// namespace std

