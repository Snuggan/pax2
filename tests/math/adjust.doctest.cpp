//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/math/adjust.hpp>
#include <pax/doctest.hpp>


namespace pax { 

	DOCTEST_TEST_CASE( "math modulo" ) {
		DOCTEST_FAST_CHECK_EQ( modulo( +5,    4   ),  1 );
		DOCTEST_FAST_CHECK_EQ( modulo( -5,    4   ), -1 );
		DOCTEST_FAST_CHECK_EQ( modulo( +5.0,  4.0 ),  1 );
		DOCTEST_FAST_CHECK_EQ( modulo( -5.0,  4.0 ), -1 );
	}
	DOCTEST_TEST_CASE( "math align_le" ) {
		DOCTEST_FAST_CHECK_EQ( std::fmod( 17.0, 12.5 ),  std::fmod( 17.0, -12.5 ) );
		DOCTEST_FAST_CHECK_EQ( align_le(  12.8,   0.0 ),  12.8 );
		DOCTEST_FAST_CHECK_EQ( align_le(  12.5,  12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le( -12.5,  12.5 ), -12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le(  17.0,  12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le( -17.0,  12.5 ), -25.0 );
		DOCTEST_FAST_CHECK_EQ( align_le(  12.5, -12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le( -12.5, -12.5 ), -12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le(  17.0, -12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_le( -17.0, -12.5 ), -25.0 );
	}
	DOCTEST_TEST_CASE( "math align_ge" ) {
		DOCTEST_FAST_CHECK_EQ( align_ge(  12.8,   0.0 ),  12.8 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  12.5,  12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -12.5,  12.5 ), -12.5 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  17.0,  12.5 ),  25.0 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -17.0,  12.5 ), -12.5 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  12.5, -12.5 ),  12.5 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -12.5, -12.5 ), -12.5 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  17.0, -12.5 ),  25.0 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -17.0, -12.5 ), -12.5 );
	}
	DOCTEST_TEST_CASE( "math align_le" ) {
		DOCTEST_FAST_CHECK_EQ( align_le(  10,   0 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_le(  10,  10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_le( -10,  10 ), -10 );
		DOCTEST_FAST_CHECK_EQ( align_le(  17,  10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_le( -17,  10 ), -20 );
		DOCTEST_FAST_CHECK_EQ( align_le(  10, -10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_le( -10, -10 ), -10 );
		DOCTEST_FAST_CHECK_EQ( align_le(  17, -10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_le( -17, -10 ), -20 );
	}
	DOCTEST_TEST_CASE( "math align_ge" ) {
		DOCTEST_FAST_CHECK_EQ( align_ge(  10,   0 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  10,  10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -10,  10 ), -10 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  17,  10 ),  20 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -17,  10 ), -10 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  10, -10 ),  10 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -10, -10 ), -10 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  17, -10 ),  20 );
		DOCTEST_FAST_CHECK_EQ( align_ge( -17, -10 ), -10 );
	}
	DOCTEST_TEST_CASE( "math align_ge from life" ) {
		DOCTEST_FAST_CHECK_EQ( align_le(  810000.040, 12.5 ),  810000.0 );
		DOCTEST_FAST_CHECK_EQ( align_ge(  812500.040, 12.5 ),  812512.5 );
		DOCTEST_FAST_CHECK_EQ( align_le( 7177500.020, 12.5 ), 7177500.0 );
		DOCTEST_FAST_CHECK_EQ( align_ge( 7180000.020, 12.5 ), 7180012.5 );
	}
	DOCTEST_TEST_CASE( "math nudge" ) {
		DOCTEST_FAST_CHECK_EQ( nudge_up  ( 0   ),  1 );
		DOCTEST_FAST_CHECK_EQ( nudge_down( 0   ), -1 );
		DOCTEST_FAST_CHECK_EQ( nudge_up  ( 0u  ),  1 );
		DOCTEST_FAST_CHECK_EQ( nudge_down( 0u  ),  0 );
		DOCTEST_FAST_CHECK_EQ( nudge_up  ( 1.0 ), 1.0 + std::numeric_limits< double >::epsilon() );
		DOCTEST_FAST_CHECK_EQ( nudge_up  ( nudge_down( 1.0 ) ), 1.0 );
		DOCTEST_FAST_CHECK_EQ( nudge_down( nudge_up  ( 1.0 ) ), 1.0 );
	}

}		// namespace pax
