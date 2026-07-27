//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/distance.hpp>
#include <pax/doctest.hpp>


namespace pax {

	DOCTEST_TEST_CASE( "distance std::span numeric" ) {
		{	// distance
			static constexpr const auto		v0 = point< int >( 0, 1, 2, -5, -1 );
			static constexpr const auto		v1 = point< int >( 1, 1, 1,  1, -1 );
			{	// static
		#		if defined( __IS_CLANG__ )
					static_assert( chebyshev( v0, v1 ) == 6 );
					static_assert( manhattan( v0, v1 ) == 8 );
					static_assert( euclidean2( v0, v1 ) == 38 );
					static_assert( hamming( v0, v1 ) == 3 );
		#		endif
			}
			{	// minkowski
				DOCTEST_FAST_CHECK_EQ ( chebyshev( v0 ),  5 );
				DOCTEST_FAST_CHECK_EQ ( chebyshev( v1 ),  1 );
				DOCTEST_FAST_CHECK_EQ ( chebyshev( v0, v1 ),  6 );

				DOCTEST_FAST_CHECK_EQ ( manhattan( v0 ),  9 );
				DOCTEST_FAST_CHECK_EQ ( manhattan( v1 ),  5 );
				DOCTEST_FAST_CHECK_EQ ( manhattan( v0, v1 ),  8 );

				DOCTEST_FAST_CHECK_EQ ( euclidean( v0 ), std::sqrt( 31.0 ) );
				DOCTEST_FAST_CHECK_EQ ( euclidean( v1 ), std::sqrt(  5.0 ) );
				DOCTEST_FAST_CHECK_EQ ( euclidean( v0, v1 ), std::sqrt( 38.0 ) );

				DOCTEST_FAST_CHECK_EQ ( euclidean2( v0 ), 31.0 );
				DOCTEST_FAST_CHECK_EQ ( euclidean2( v1 ),  5.0 );
				DOCTEST_FAST_CHECK_EQ ( euclidean2( v0, v1 ), 38.0 );
			}
			{	// hamming
				DOCTEST_FAST_CHECK_EQ ( hamming( v0, v1 ),  3 );
				DOCTEST_FAST_CHECK_EQ ( hamming( v1, v0 ),  3 );
			}
			{	// canberra
				DOCTEST_FAST_CHECK_EQ( canberra( v0, v1 ), doctest::Approx( 7.0/3 ) );
				DOCTEST_FAST_CHECK_EQ( canberra( v1, v0 ), doctest::Approx( 7.0/3 ) );
			}
			{	// levenshtein
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "Zyklus" ),		std::span( "cykel"   ) ), 4 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "cykel" ),		std::span( "Zyklus"  ) ), 4 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "doubt" ),		std::span( "doute"   ) ), 2 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "doute"  ),		std::span( "doubt"   ) ), 2 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "kitten"  ),		std::span( "sitting" ) ), 3 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( std::span( "Saturday"  ),	std::span( "Sunday"  ) ), 3 );
			}
		}
	}
	
}	// namespace pax
