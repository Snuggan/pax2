//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/textual/to_string.hpp>
#include <pax/doctest.hpp>

#include <array>
#include <span>


namespace pax {

	constexpr double a0[ 3 ]	= { 1.0, 2.0, 3.0 };
	const auto a 				= &a0[ 0 ];
	constexpr std::array< double, 3 > arr = { 1.0, 2.0, 3.0 };
	using Tuple					= std::tuple< double, const char *, int >;
	const Tuple					  tup{ 3.2, "Hej!", -4 };
	

	DOCTEST_TEST_CASE( "format any" ) {
		DOCTEST_FAST_CHECK_EQ( to_string( true ),					"true" );
		DOCTEST_FAST_CHECK_EQ( to_string( false ),					"false" );
		DOCTEST_FAST_CHECK_EQ( to_string( 1234567890.12345678901 ),	"1234567890.1234567" );
		DOCTEST_FAST_CHECK_EQ( to_string( 1.25 ),					"1.25" );
		DOCTEST_FAST_CHECK_EQ( to_string( 1.25, "{:f}" ),			"1.250000" );
	}
	DOCTEST_TEST_CASE( "format pointer" ) {
		DOCTEST_FAST_CHECK_EQ( to_string( nullptr ),				"0x0" );
		DOCTEST_FAST_CHECK_EQ( to_string( nullptr_t{} ),			"0x0" );
		DOCTEST_FAST_CHECK_EQ( to_string( &arr ).substr( 0, 2 ),	"0x" );
	}
	DOCTEST_TEST_CASE( "format begin->end" ) {
		DOCTEST_FAST_CHECK_EQ( to_string( a, a ),					"[]" );
		DOCTEST_FAST_CHECK_EQ( to_string( a, a+1 ),					"[1]" );
		DOCTEST_FAST_CHECK_EQ( to_string( a, a+3 ),					"[1, 2, 3]" );
		DOCTEST_FAST_CHECK_EQ( to_string( a, a, "{:f}" ),			"[]" );
		DOCTEST_FAST_CHECK_EQ( to_string( a, a+1, "{:f}" ),			"[1.000000]" );
		DOCTEST_FAST_CHECK_EQ( to_string( a, a+3, "{:f}" ),			"[1.000000, 2.000000, 3.000000]" );
		
		DOCTEST_FAST_CHECK_EQ( to_string( arr.begin(), arr.end() ),	"[1, 2, 3]" );
		DOCTEST_FAST_CHECK_EQ( to_string( arr ),					"[1, 2, 3]" );
		DOCTEST_FAST_CHECK_EQ( to_string( arr, "{:f}" ),			"[1.000000, 2.000000, 3.000000]" );

		DOCTEST_FAST_CHECK_EQ( to_string( std::span( a0 ), "'{}'" ),		"['1', '2', '3']" );
		DOCTEST_FAST_CHECK_EQ( std20::format( "'{}'", std::span( a0 ) ),	"'[1, 2, 3]'" );
	}
	DOCTEST_TEST_CASE( "format tuple" ) {
		DOCTEST_FAST_CHECK_EQ( to_string( std::make_tuple( 1 ) ),	"(1)" );
		DOCTEST_FAST_CHECK_EQ( to_string( tup ),					"(3.2, \"Hej!\", -4)" );
		DOCTEST_FAST_CHECK_EQ( to_string( std::make_tuple( tup, "Hej!", -4 ) ),
			"((3.2, \"Hej!\", -4), \"Hej!\", -4)" );
	}

}	// namespace pax
