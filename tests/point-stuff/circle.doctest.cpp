//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/circle.hpp>
#include <pax/doctest.hpp>


namespace pax {

	constexpr auto circ			  = Circle3d( { 1., 2., 3. }, 1. );

	DOCTEST_TEST_CASE( "Circle object" ) {
		DOCTEST_FAST_CHECK_EQ( circ,  				circ );
		DOCTEST_FAST_CHECK_EQ( min( circ ),  		Point{ 0., 1., 2. } );
		DOCTEST_FAST_CHECK_EQ( center( circ ),  	Point{ 1., 2., 3. } );
		DOCTEST_FAST_CHECK_EQ( max( circ ),  		Point{ 2., 3., 4. } );
		{
			constexpr Point		in{ 1., 2., 3. };
			constexpr Point		on{ 1., 2., 4. };
			constexpr Point		ut{ 2., 3., 4. };
			DOCTEST_FAST_CHECK_UNARY(  circ.strictly_inside( in ) );
			DOCTEST_FAST_CHECK_UNARY( !circ.strictly_inside( on ) );
			DOCTEST_FAST_CHECK_UNARY( !circ.strictly_inside( ut ) );
			DOCTEST_FAST_CHECK_UNARY(  circ.inside_or_on( in ) );
			DOCTEST_FAST_CHECK_UNARY(  circ.inside_or_on( on ) );
			DOCTEST_FAST_CHECK_UNARY( !circ.inside_or_on( ut ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( circ, in ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( circ, on ) );
			DOCTEST_FAST_CHECK_UNARY( !contains( circ, ut ) );
		} {
			constexpr auto bx	  = Box3d{ { 0., 0., 0. }, { 3., 4., 5. } };
			DOCTEST_FAST_CHECK_UNARY( !contains( bx, Circle( Point{ 7., 7., 7. }, 1. ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap ( bx, Circle( Point{ 7., 7., 7. }, 1. ) ) );
			DOCTEST_FAST_CHECK_UNARY( !contains( bx, Circle( Point{ 3., 4., 5. }, 1. ) ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap ( bx, Circle( Point{ 3., 4., 5. }, 1. ) ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( bx, Circle( Point{ 2., 3., 4. }, 1. ) ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap ( bx, Circle( Point{ 2., 3., 4. }, 1. ) ) );
		}
	}
	DOCTEST_TEST_CASE( "Circle_w_id object" ) {
		constexpr auto circ2		  = Circle_w_id3d( { 1., 2., 3. }, -1., "my_id" );
		DOCTEST_FAST_CHECK_EQ( circ2,  				circ2 );
		DOCTEST_FAST_CHECK_EQ( min( circ2 ),  		Point{ 0., 1., 2. } );
		DOCTEST_FAST_CHECK_EQ( center( circ2 ),  	Point{ 1., 2., 3. } );
		DOCTEST_FAST_CHECK_EQ( max( circ2 ),  		Point{ 2., 3., 4. } );
		DOCTEST_FAST_CHECK_EQ( circ2.id(),  		"my_id" );
		{
			constexpr Point		in{ 1., 2., 3. };
			constexpr Point		on{ 1., 2., 4. };
			constexpr Point		ut{ 2., 3., 4. };
			DOCTEST_FAST_CHECK_UNARY(  circ2.strictly_inside( in ) );
			DOCTEST_FAST_CHECK_UNARY( !circ2.strictly_inside( on ) );
			DOCTEST_FAST_CHECK_UNARY( !circ2.strictly_inside( ut ) );
			DOCTEST_FAST_CHECK_UNARY(  circ2.inside_or_on( in ) );
			DOCTEST_FAST_CHECK_UNARY(  circ2.inside_or_on( on ) );
			DOCTEST_FAST_CHECK_UNARY( !circ2.inside_or_on( ut ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( circ2, in ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( circ2, on ) );
			DOCTEST_FAST_CHECK_UNARY( !contains( circ2, ut ) );
		} {
			constexpr auto bx	  = Box3d{ { 0., 0., 0. }, { 3., 4., 5. } };
			DOCTEST_FAST_CHECK_UNARY( !contains( bx, Circle_w_id( Point{ 7., 7., 7. }, 1., "" ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap ( bx, Circle_w_id( Point{ 7., 7., 7. }, 1., "" ) ) );
			DOCTEST_FAST_CHECK_UNARY( !contains( bx, Circle_w_id( Point{ 3., 4., 5. }, 1., "" ) ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap ( bx, Circle_w_id( Point{ 3., 4., 5. }, 1., "" ) ) );
			DOCTEST_FAST_CHECK_UNARY(  contains( bx, Circle_w_id( Point{ 2., 3., 4. }, 1., "" ) ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap ( bx, Circle_w_id( Point{ 2., 3., 4. }, 1., "" ) ) );
		}
	}
	
}	// namespace pax
