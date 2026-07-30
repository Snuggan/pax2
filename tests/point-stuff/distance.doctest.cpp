//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/distance.hpp>
#include <pax/doctest.hpp>


namespace pax {

	DOCTEST_TEST_CASE( "distance" ) {	// These could not be static_assert.
		static constexpr const auto		v0 = point< int >( 0, 1, 2, -5, -1 );
		static constexpr const auto		v1 = point< int >( 1, 1, 1,  1, -1 );
		DOCTEST_FAST_CHECK_EQ ( cosine( v0, v1 ),  doctest::Approx( 1.08032 ) );
		DOCTEST_FAST_CHECK_EQ ( shannon_index( point< unsigned >( 10, 20, 30, 20, 20 ) ),	doctest::Approx( 1.55711 ) );
		DOCTEST_FAST_CHECK_EQ ( jaro_similarity		   ( "DwAyNE", "DuANE" ),	doctest::Approx( 0.822222 ) );
		DOCTEST_FAST_CHECK_EQ ( jaro_winkler_similarity( "DwAyNE", "DuANE" ),	doctest::Approx( 0.84 ) );
	}
	
}	// namespace pax
