//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/math/power.hpp>
#include <pax/doctest.hpp>


namespace pax { 

	DOCTEST_TEST_CASE( "math functors power" ) {
		static_assert( power( -2.0,  2 ) == 4.00 );
		static_assert( power( -2.0, -2 ) == 0.25 );
		static_assert( power(  4,    3 ) ==   64 );

		DOCTEST_FAST_CHECK_EQ( power( -2,  0.0 ),    1 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  1.0 ),   -2 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  2.0 ),    4 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  3.0 ),   -8 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  3.5 ), std::pow( 2, 3.5 ) );

		DOCTEST_FAST_CHECK_EQ( power( -2,  0 ),    1 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  1 ),   -2 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  2 ),    4 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  3 ),   -8 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  4 ),   16 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  5 ),  -32 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  6 ),   64 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  7 ), -128 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  8 ),  256 );
		DOCTEST_FAST_CHECK_EQ( power( -2,  9 ), -512 );

		DOCTEST_FAST_CHECK_EQ( power(  2,  0 ),    1 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  1 ),    2 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  2 ),    4 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  3 ),    8 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  4 ),   16 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  5 ),   32 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  6 ),   64 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  7 ),  128 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  8 ),  256 );
		DOCTEST_FAST_CHECK_EQ( power(  2,  9 ),  512 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 10 ),    1*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 11 ),    2*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 12 ),    4*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 13 ),    8*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 14 ),   16*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 15 ),   32*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 16 ),   64*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 17 ),  128*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 18 ),  256*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 19 ),  512*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 20 ),    1*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 21 ),    2*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 22 ),    4*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 23 ),    8*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 24 ),   16*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 25 ),   32*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 26 ),   64*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 27 ),  128*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 28 ),  256*1024*1024 );
		DOCTEST_FAST_CHECK_EQ( power(  2, 29 ),  512*1024*1024 );

		DOCTEST_FAST_CHECK_EQ( power( 2.0, -0 ), 1.0/ 1 );
		DOCTEST_FAST_CHECK_EQ( power( 2.0, -1 ), 1.0/ 2 );
		DOCTEST_FAST_CHECK_EQ( power( 2.0, -2 ), 1.0/ 4 );
		DOCTEST_FAST_CHECK_EQ( power( 2.0, -3 ), 1.0/ 8 );
		DOCTEST_FAST_CHECK_EQ( power( 2.0, -4 ), 1.0/16 );
		DOCTEST_FAST_CHECK_EQ( power( 2.0, -5 ), 1.0/32 );

		DOCTEST_FAST_CHECK_EQ( power( 2, power( 2, 3 ) ), 256 );
	}
	DOCTEST_TEST_CASE( "math functors absolute power" ) {
		static_assert( abs_power( -2.0,  1 ) == 2.0, "" );
		static_assert( abs_power( -2.0, -1 ) == 0.5, "" );

		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  0.0 ),   1 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  1.0 ),   2 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  2.0 ),   4 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  3.0 ),   8 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  3.5 ), std::pow( 2, 3.5 ) );
	
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  0 ),   1 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  1 ),   2 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  2 ),   4 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  3 ),   8 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  4 ),  16 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  5 ),  32 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  6 ),  64 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  7 ), 128 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  8 ), 256 );
		DOCTEST_FAST_CHECK_EQ( abs_power( -2,  9 ), 512 );

		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  0 ),   1 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  1 ),   2 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  2 ),   4 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  3 ),   8 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  4 ),  16 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  5 ),  32 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  6 ),  64 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  7 ), 128 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  8 ), 256 );
		DOCTEST_FAST_CHECK_EQ( abs_power(  2,  9 ), 512 );

		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -0 ), 1.0/ 1 );
		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -1 ), 1.0/ 2 );
		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -2 ), 1.0/ 4 );
		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -3 ), 1.0/ 8 );
		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -4 ), 1.0/16 );
		DOCTEST_FAST_CHECK_EQ( abs_power( 2.0, -5 ), 1.0/32 );
	}
	DOCTEST_TEST_CASE( "math functors root" ) {
		static_assert( root( -2.0,  1 ) == -2.0, "" );
		static_assert( root( -2.0, -1 ) == -0.5, "" );
		static_assert( root(  2.0,  1 ) ==  2.0, "" );
		static_assert( root(  2.0, -1 ) ==  0.5, "" );

		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, 1.0 ), 1.0 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, 3.0 ), 3.0 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, 5.0 ), 5.0 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, 3.5 ), 3.5 ),	doctest::Approx(  2 ) );
	
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -1 ),  -1 ),	doctest::Approx( 2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -2 ),  -2 ),	doctest::Approx( 2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -3 ),  -3 ),	doctest::Approx( 2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -4 ),  -4 ),	doctest::Approx( 2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -5 ),  -5 ),	doctest::Approx( 2 ) );

		DOCTEST_FAST_CHECK_EQ( power( root( -2.0,  1 ),   1 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0,  3 ),   3 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0,  5 ),   5 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0,  7 ),   7 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0,  9 ),   9 ),	doctest::Approx( -2 ) );

		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  1 ),   1 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  2 ),   2 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( square( root(  2.0,  2 ) ),		doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( square( square_root(  2.0 ) ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  3 ),   3 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( cube ( root(  2.0,  3 ) ),		doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( cube( cube_root(  2.0 ) ),		doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  4 ),   4 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  5 ),   5 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  6 ),   6 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  7 ),   7 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  8 ),   8 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0,  9 ),   9 ),	doctest::Approx(  2 ) );

		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, -1 ),  -1 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, -3 ),  -3 ),	doctest::Approx( -2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root( -2.0, -5 ),  -5 ),	doctest::Approx( -2 ) );

		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -1 ),  -1 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -2 ),  -2 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -3 ),  -3 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -4 ),  -4 ),	doctest::Approx(  2 ) );
		DOCTEST_FAST_CHECK_EQ( power( root(  2.0, -5 ),  -5 ),	doctest::Approx(  2 ) );
	}

}		// namespace pax
