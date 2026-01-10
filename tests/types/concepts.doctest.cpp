//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/concepts.hpp>
#include <pax/doctest.hpp>

#include <array>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include <list>
#include <map>


namespace pax::traits {

	DOCTEST_TEST_CASE_TEMPLATE( "concepts for static contiguous", T, 
		char[ 4 ]
	) {
		static_assert(  has_contiguous< T > );
		static_assert(  has_size< T > );
		static_assert(  has_extent< T > );
		static_assert( !has_dynamic_size< T > );
		static_assert(  std::is_same_v< value_type_t< T >, char > );	// decltype( "abc" ) is const char
		static_assert(  extent_v< T > == 4 );
		static_assert(  traits::string< T > );

		static_assert(  std::is_same_v< char,		element_type_t< std::array< char, 4 > > > );
		static_assert(  std::is_same_v< const char, element_type_t< std::string_view > > );
		static_assert(  std::is_same_v< const char, element_type_t< std::initializer_list< char > > > );
	}

	DOCTEST_TEST_CASE_TEMPLATE( "concepts for static contiguous", T, 
		char(&)[ 4 ], decltype( "abc" ), std::array< char, 4 >, std::span< char, 4 >
	) {
		static_assert(  has_contiguous< T > );
		static_assert(  has_size< T > );
		static_assert(  has_extent< T > );
		static_assert( !has_dynamic_size< T > );
		static_assert(  std::is_same_v< value_type_t< T >, char > );	// decltype( "abc" ) is const char
		static_assert(  extent_v< T > == 4 );
		static_assert(  traits::string< T > );

		static_assert(  std::is_same_v< char, 		element_type_t< std::array< char, 4 > > > );
		static_assert(  std::is_same_v< const char, element_type_t< std::string_view > > );
		static_assert(  std::is_same_v< const char, element_type_t< std::initializer_list< char > > > );
	}

	DOCTEST_TEST_CASE_TEMPLATE( "concepts for dynamic contiguous", T, 
		std::span< char >, std::string, std::string_view, std::vector< char >
	) {
		static_assert(  has_contiguous< T > );
		static_assert(  has_size< T > );
		static_assert( !has_extent< T > );
		static_assert(  has_dynamic_size< T > );
		static_assert(  std::is_same_v< value_type_t< T >, char > );	// std::string_view is const char
		static_assert(  extent_v< T > == dynamic_extent );
		static_assert(  string< T > );
	}

	DOCTEST_TEST_CASE_TEMPLATE( "concepts for contiguous with no size", T, 
		char*, char[], char(&)[]
	) {
		static_assert(  has_contiguous< T > );
		static_assert( !has_size< T > );
		static_assert( !has_extent< T > );
		static_assert( !has_dynamic_size< T > );
		static_assert(  std::is_same_v< value_type_t< T >, char > );
		// static_assert(  extent_v< T > == 0u );	// Not specialized for these types.
		static_assert(  string< T > );
	}

	DOCTEST_TEST_CASE( "concepts has_contiguous_sizeable" ) {
		static_assert( !has_contiguous_sizeable< std::tuple< int, int > > );
		static_assert( !has_contiguous_sizeable< std::pair< int, int > > );
		static_assert( !has_contiguous_sizeable< std::list< int > > );
		static_assert( !has_contiguous_sizeable< std::map< int, int > > );
	}
	DOCTEST_TEST_CASE( "concepts traits::character or not traits::string" ) {
		static_assert(  character< char > );
		static_assert( !string< char > );

		static_assert( !string< int > );
		static_assert( !string< int(&)[] > );
		static_assert( !string< int(&)[ 4 ] > );
		static_assert( !string< int16_t* > );
		static_assert( !string< int[] > );
		static_assert( !string< int[ 4 ] > );
		static_assert( !string< const int(&)[ 4 ] > );
		static_assert( !string< const int[ 4 ] > );
		static_assert( !string< std::array< int, 4 > > );
		static_assert( !string< std::span< int > > );
		static_assert( !string< std::span< int, 4 > > );
		static_assert( !string< std::vector< int > > );
	}

}	// namespace pax
