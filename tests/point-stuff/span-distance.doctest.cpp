//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/span-distance.hpp>
#include <pax/doctest.hpp>


namespace pax {

	DOCTEST_TEST_CASE( "distance std::span numeric" ) {
/*		int				arr[ 7 ] = { 1, 2, 3, 4, 5, 6, 7 };
		const std::span		v( arr );
		
		{	// distance
			static constexpr const double	a0[ 5 ] = { 0, 1, 2, -5, -1 };
			static constexpr const double	a1[ 5 ] = { 1, 1, 1,  1, -1 };
			static constexpr const auto		v0 = std::span( a0 );
			static constexpr const auto		v1 = std::span( a1 );
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
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "Zyklus" ),		Span( "cykel"   ) ), 4 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "cykel" ),		Span( "Zyklus"  ) ), 4 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "doubt" ),		Span( "doute"   ) ), 2 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "doute"  ),		Span( "doubt"   ) ), 2 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "kitten"  ),		Span( "sitting" ) ), 3 );
				DOCTEST_FAST_CHECK_EQ( levenshtein( Span( "Saturday"  ),	Span( "Sunday"  ) ), 3 );
			}
		}
*/	}
	
}	// namespace pax
