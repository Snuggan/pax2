//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/point.hpp>
#include <pax/doctest.hpp>


namespace pax {
	
	constexpr Point< int, 5 > pt  = make_point< double >( 0.0, 1, 2u, 3,0f, char( 4 ) );
	constexpr Point< int, 5 > idx = make_point< int    >( 0.0, 1, 2u, 3,0f, char( 4 ) );

	DOCTEST_TEST_CASE( "point accessors" ) {
		DOCTEST_FAST_CHECK_EQ ( x( pt ), 0 );
		DOCTEST_FAST_CHECK_EQ ( y( pt ), 1 );
		DOCTEST_FAST_CHECK_EQ ( z( pt ), 2 );
		DOCTEST_FAST_CHECK_EQ ( east ( pt ), 0 );
		DOCTEST_FAST_CHECK_EQ ( north( pt ), 1 );
		DOCTEST_FAST_CHECK_EQ ( col( idx ), 0 );
		DOCTEST_FAST_CHECK_EQ ( row( idx ), 1 );
	}
	
}	// namespace pax
