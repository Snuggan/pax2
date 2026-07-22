//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/point.hpp>
#include <pax/doctest.hpp>


namespace pax {
	
	constexpr Point< double,   5 > pt  = point< double   >( 0.0, 1, 2u, 3.0f, char( 4 ) );
	constexpr Point< unsigned, 5 > idx = point< unsigned >( 0.0, 1, 2u, 3.0f, char( 4 ) );

	DOCTEST_TEST_CASE( "Point accessors" ) {
		DOCTEST_FAST_CHECK_EQ( x    ( pt ),  0 );
		DOCTEST_FAST_CHECK_EQ( y    ( pt ),  1 );
		DOCTEST_FAST_CHECK_EQ( z    ( pt ),  2 );
		DOCTEST_FAST_CHECK_EQ( east ( pt ),  0 );
		DOCTEST_FAST_CHECK_EQ( north( pt ),  1 );
		DOCTEST_FAST_CHECK_EQ( col  ( idx ), 0 );
		DOCTEST_FAST_CHECK_EQ( row  ( idx ), 1 );
		
		auto pt2  = pt;
		auto idx2 = idx;
		x    ( pt2 )	 += 10;
		y    ( pt2 )	 += 10;
		z    ( pt2 )	 += 10;
		east ( pt2 )	 += 10;
		north( pt2 )	 += 10;
		col  ( idx2 )	 += 10;
		row  ( idx2 )	 += 10;
		DOCTEST_FAST_CHECK_EQ( x    ( pt2 ),  20 );
		DOCTEST_FAST_CHECK_EQ( y    ( pt2 ),  21 );
		DOCTEST_FAST_CHECK_EQ( z    ( pt2 ),  12 );
		DOCTEST_FAST_CHECK_EQ( east ( pt2 ),  20 );
		DOCTEST_FAST_CHECK_EQ( north( pt2 ),  21 );
		DOCTEST_FAST_CHECK_EQ( col  ( idx2 ), 10 );
		DOCTEST_FAST_CHECK_EQ( row  ( idx2 ), 11 );
	}
	DOCTEST_TEST_CASE( "Point comparison" ) {
		auto pt2  = point< double >( 1.0, 2, 3u, 4.0f, 5.f );
		DOCTEST_FAST_CHECK_UNARY(  all_lt( pt, pt2 ) );
		z( pt2 )  = z( pt );
		DOCTEST_FAST_CHECK_UNARY( !all_lt( pt, pt2 ) );
		DOCTEST_FAST_CHECK_UNARY(  all_le( pt, pt2 ) );
		z( pt2 ) -= 1;
		DOCTEST_FAST_CHECK_UNARY( !all_le( pt, pt2 ) );

		DOCTEST_FAST_CHECK_EQ( pt, pt  );
		DOCTEST_FAST_CHECK_NE( pt, pt2 );
		DOCTEST_FAST_CHECK_LT( pt, pt2 );
		DOCTEST_FAST_CHECK_LE( pt, pt2 );
	}
	DOCTEST_TEST_CASE( "Point other" ) {
		auto pt2  = Point{ 0.0, 1.0, 4.0, 1.0, 4.0 };
		DOCTEST_FAST_CHECK_EQ( min( pt, pt2 ), Point{ 0.0, 1.0, 2.0, 1.0, 4.0 } );
		DOCTEST_FAST_CHECK_EQ( max( pt, pt2 ), Point{ 0.0, 1.0, 4.0, 3.0, 4.0 } );
		DOCTEST_FAST_CHECK_EQ( pt + pt2, Point{ 0.0, 2.0,  6.0, 4.0, 8.0 } );
		DOCTEST_FAST_CHECK_EQ( pt - pt2, Point{ 0.0, 0.0, -2.0, 2.0, 0.0 } );
		DOCTEST_FAST_CHECK_EQ( cross_product( Point{ 1., 2., 3. }, Point{ 4., 5., 6. } ), Point{ -3.,  6., -3. } );
		DOCTEST_FAST_CHECK_EQ( cross_product( Point{ 4., 5., 6. }, Point{ 1., 2., 3. } ), Point{  3., -6.,  3. } );
	}
	
}	// namespace pax
