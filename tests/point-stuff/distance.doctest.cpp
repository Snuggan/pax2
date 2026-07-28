//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/distance.hpp>
#include <pax/doctest.hpp>
#include <span>


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
			{	// canberra
				DOCTEST_FAST_CHECK_EQ( canberra( v0, v1 ), doctest::Approx( 7.0/3 ) );
				DOCTEST_FAST_CHECK_EQ( canberra( v1, v0 ), doctest::Approx( 7.0/3 ) );
			}
			{	// cosine
				const auto pt0 = point< int >( 3, 2, 0, 5 );
				const auto pt1 = point< int >( 1, 0, 0, 0 );
				DOCTEST_FAST_CHECK_EQ ( cosine( pt0, pt0 ),  doctest::Approx( 0.0 ) );
				DOCTEST_FAST_CHECK_EQ ( cosine( pt0, pt1 ),  doctest::Approx( 0.513336 ) );
				DOCTEST_FAST_CHECK_EQ ( cosine( pt1, pt0 ),  cosine( pt0, pt1 ) );
			}
			{	// kendall_tau
				const auto pt0 = point< int >( 1, 2, 3, 4, 5 );
				const auto pt1 = point< int >( 3, 4, 1, 2, 5 );
				DOCTEST_FAST_CHECK_EQ ( kendall_tau( pt0, pt0 ),  0 );
				DOCTEST_FAST_CHECK_EQ ( kendall_tau( pt0, pt1 ),  4 );
				DOCTEST_FAST_CHECK_EQ ( kendall_tau( pt1, pt0 ),  4 );
				DOCTEST_FAST_CHECK_EQ ( normalized_kendall_tau( pt0, pt1 ), 0.4 );
			}
			{	// hamming
				DOCTEST_FAST_CHECK_EQ ( hamming( v0, v1 ),  3 );
				DOCTEST_FAST_CHECK_EQ ( hamming( v1, v0 ),  3 );
			}
			{	// sorensen_dice
				const char s0[ 19 ] = "algorithms are fun";
				const char s1[ 19 ] = "logarithms are not";
				DOCTEST_FAST_CHECK_EQ ( sorensen_dice( s0, s0 ),	1.0 );
				DOCTEST_FAST_CHECK_EQ ( sorensen_dice( s0, s1 ),	doctest::Approx( 0.58823529411764708 ) );
				DOCTEST_FAST_CHECK_EQ ( sorensen_dice( s1, s0 ),	doctest::Approx( 0.58823529411764708 ) );
				DOCTEST_FAST_CHECK_EQ ( sorensen_dice( std::string( s0 ), std::string( s1 ) ),  
																	doctest::Approx( 0.58823529411764708 ) );
			}
			{	// shannon_index
				static constexpr const auto		v = point< unsigned >( 10, 20, 30, 20, 20 );
				DOCTEST_FAST_CHECK_EQ ( shannon_index( v ),	doctest::Approx( 1.55711 ) );
			}
			{	// jaccard
				static constexpr const auto		v0 = point< int >( 0, 1, 2, 5, 6, 8, 9 );
				static constexpr const auto		v1 = point< int >( 0, 2, 3, 4, 5, 7, 9 );
				DOCTEST_FAST_CHECK_EQ ( jaccard( v0, v0 ),	1.0 );
				DOCTEST_FAST_CHECK_EQ ( jaccard( v0, v1 ),	0.4 );
				DOCTEST_FAST_CHECK_EQ ( jaccard( v1, v0 ),	0.4 );
			}
			{	// jaro_similarity
				DOCTEST_FAST_CHECK_EQ ( jaro_similarity		   ( "CRATE",  "TRACE" ),	doctest::Approx( 0.733333 ) );
				DOCTEST_FAST_CHECK_EQ ( jaro_similarity		   ( "DwAyNE", "DuANE" ),	doctest::Approx( 0.822222 ) );
				DOCTEST_FAST_CHECK_EQ ( jaro_winkler_similarity( "TRATE",  "TRACE" ),	doctest::Approx( 0.906667 ) );
				DOCTEST_FAST_CHECK_EQ ( jaro_winkler_similarity( "DwAyNE", "DuANE" ),	doctest::Approx( 0.84 ) );
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
