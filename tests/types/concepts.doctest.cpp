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

	static_assert(  array_like< int[ 4 ] > );
	static_assert(  array_like< int( & )[ 4 ] > );
	static_assert(  array_like< int( & )[] > );
	static_assert(  array_like< int[] > );
	static_assert(  array_like< int * > );
	static_assert( !array_like< std::array< int, 4 > > );
	static_assert( !array_like< std::vector< int > > );
	static_assert( !array_like< std::span< int, 4 > > );
	static_assert( !array_like< std::span< int > > );

	static_assert(  size_contiguous< int[ 4 ] > );
	static_assert(  size_contiguous< int( & )[ 4 ] > );
	static_assert( !size_contiguous< int( & )[] > );
	static_assert( !size_contiguous< int[] > );
	static_assert( !size_contiguous< int * > );
	static_assert(  size_contiguous< std::array< int, 4 > > );
	static_assert(  size_contiguous< std::vector< int > > );
	static_assert(  size_contiguous< std::span< int, 4 > > );
	static_assert(  size_contiguous< std::span< int > > );

	static_assert(  traits::contiguous< int[ 4 ] > );
	static_assert(  traits::contiguous< int( & )[ 4 ] > );
	static_assert(  traits::contiguous< int( & )[] > );
	static_assert(  traits::contiguous< int[] > );
	static_assert(  traits::contiguous< int * > );
	static_assert(  traits::contiguous< std::array< int, 4 > > );
	static_assert(  traits::contiguous< std::vector< int > > );
	static_assert(  traits::contiguous< std::span< int, 4 > > );
	static_assert(  traits::contiguous< std::span< int > > );

	static_assert(  has_size< int[ 4 ] > );
	static_assert(  has_size< int( & )[ 4 ] > );
	static_assert( !has_size< int( & )[] > );
	static_assert( !has_size< int[] > );
	static_assert( !has_size< int * > );
	static_assert(  has_size< std::array< int, 4 > > );
	static_assert(  has_size< std::vector< int > > );
	static_assert(  has_size< std::span< int, 4 > > );
	static_assert(  has_size< std::span< int > > );

	static_assert(  has_extent< int[ 4 ] > );
	static_assert(  has_extent< int( & )[ 4 ] > );
	static_assert( !has_extent< int( & )[] > );
	static_assert( !has_extent< int[] > );
	static_assert( !has_extent< int * > );
	static_assert(  has_extent< std::array< int, 4 > > );
	static_assert( !has_extent< std::vector< int > > );
	static_assert(  has_extent< std::span< int, 4 > > );
	static_assert( !has_extent< std::span< int > > );

	static_assert( !has_dynamic_size< int[ 4 ] > );
	static_assert( !has_dynamic_size< int( & )[ 4 ] > );
	static_assert( !has_dynamic_size< int( & )[] > );
	static_assert( !has_dynamic_size< int[] > );
	static_assert( !has_dynamic_size< int * > );
	static_assert( !has_dynamic_size< std::array< int, 4 > > );
	static_assert(  has_dynamic_size< std::vector< int > > );
	static_assert( !has_dynamic_size< std::span< int, 4 > > );
	static_assert(  has_dynamic_size< std::span< int > > );

	static_assert( 4u == extent_v< int[ 4 ] > );
	static_assert( 4u == extent_v< int( & )[ 4 ] > );
	static_assert( 4u == extent_v< std::array< int, 4 > > );
	static_assert( 4u == extent_v< std::span< int, 4 > > );

	static_assert(  string< char[ 4 ] > );
	static_assert(  string< char( & )[ 4 ] > );
	static_assert(  string< char( & )[] > );
	static_assert(  string< char[] > );
	static_assert(  string< char * > );
	static_assert(  string< std::array< char, 4 > > );
	static_assert(  string< std::vector< char > > );
	static_assert(  string< std::span< char, 4 > > );
	static_assert(  string< std::span< char > > );
	static_assert(  string< std::string > );
	static_assert(  string< std::string_view > );

	DOCTEST_TEST_CASE_TEMPLATE( "concepts for static contiguous", T, 
		char[ 4 ]
	) {
		static_assert(  traits::contiguous< T > );
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
		static_assert(  traits::contiguous< T > );
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
		static_assert(  traits::contiguous< T > );
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
		static_assert(  traits::contiguous< T > );
		static_assert( !has_size< T > );
		static_assert( !has_extent< T > );
		static_assert( !has_dynamic_size< T > );
		static_assert(  std::is_same_v< value_type_t< T >, char > );
		// static_assert(  extent_v< T > == 0u );	// Not specialized for these types.
		static_assert(  string< T > );
	}

	DOCTEST_TEST_CASE( "concepts size_contiguous" ) {
		static_assert( !size_contiguous< std::tuple< int, int > > );
		static_assert( !size_contiguous< std::pair< int, int > > );
		static_assert( !size_contiguous< std::list< int > > );
		static_assert( !size_contiguous< std::map< int, int > > );
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
	DOCTEST_TEST_CASE( "concepts any_of" ) {
		static_assert( !any_of_v< int, bool, char, long > );
		static_assert(  any_of_v< int, bool, char, long, int > );
	}

}	// namespace pax
