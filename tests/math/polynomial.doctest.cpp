//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#include <pax/math/polynomial-named.hpp>
#include <pax/doctest.hpp>


namespace pax::test {
	static constexpr const auto p  = polynomial( 1.0, 2, 3, 4, 5 );

	DOCTEST_TEST_CASE( "polynomial output" ) {
		// static_assert( std::format( "{}", polynomial(-1., 2, 3, -4, 5, 1, -1 ) ) ==	"-1 + 2x + 3x^2 - 4x^3 + 5x^4 + x^5 - x^6" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", pax::poly::polynull<> ),					"0" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0. ) ),						"0" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 1. ) ),						"1" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1 ) ),					"x" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1, 0 ) ),					"x" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1, 0, 3 ) ),				"x + 3x^3" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 1., 0 ) ),					"1" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial(-1., 2, 3, -4, 5, 1, -1 ) ),	"-1 + 2x + 3x^2 - 4x^3 + 5x^4 + x^5 - x^6" );
	}

	namespace poly::horner_ {
		static constexpr const auto sp = std::span{ p };
	
		// Statically sized cases
		static_assert( horner_fma     ()( sp.first< 1 >(), 2 ) ==	1 );
		static_assert( horner_fma     ()( sp.first< 2 >(), 2 ) ==	1 + 4 );
		static_assert( horner_fma     ()( sp.first< 3 >(), 2 ) ==	1 + 4 + 12 );
		static_assert( horner_fma     ()( sp.first< 4 >(), 2 ) ==	1 + 4 + 12 + 32 );
		static_assert( horner_fma     ()( sp.first< 5 >(), 2 ) ==	1 + 4 + 12 + 32 + 80 );

		static_assert( horner_fma_even()( sp.first< 1 >(), 2 ) ==	1 );
		static_assert( horner_fma_even()( sp.first< 2 >(), 2 ) ==	1 );
		static_assert( horner_fma_even()( sp.first< 3 >(), 2 ) ==	1 + 12 );
		static_assert( horner_fma_even()( sp.first< 4 >(), 2 ) ==	1 + 12 );
		static_assert( horner_fma_even()( sp.first< 5 >(), 2 ) ==	1 + 12 + 80 );

		static_assert( horner_fma_odd ()( sp.first< 1 >(), 2 ) ==	0 );
		static_assert( horner_fma_odd ()( sp.first< 2 >(), 2 ) ==	4 );
		static_assert( horner_fma_odd ()( sp.first< 3 >(), 2 ) ==	4 );
		static_assert( horner_fma_odd ()( sp.first< 4 >(), 2 ) ==	4 + 32 );
		static_assert( horner_fma_odd ()( sp.first< 5 >(), 2 ) ==	4 + 32 );

		// Dynamically sized cases
		static_assert( horner_fma     ()( sp.first( 1 ), 2 ) ==	1 );
		static_assert( horner_fma     ()( sp.first( 2 ), 2 ) ==	1 + 4 );
		static_assert( horner_fma     ()( sp.first( 3 ), 2 ) ==	1 + 4 + 12 );
		static_assert( horner_fma     ()( sp.first( 4 ), 2 ) ==	1 + 4 + 12 + 32 );
		static_assert( horner_fma     ()( sp.first( 5 ), 2 ) ==	1 + 4 + 12 + 32 + 80 );

		static_assert( horner_fma_even()( sp.first( 1 ), 2 ) ==	1 );
		static_assert( horner_fma_even()( sp.first( 2 ), 2 ) ==	1 );
		static_assert( horner_fma_even()( sp.first( 3 ), 2 ) ==	1 + 12 );
		static_assert( horner_fma_even()( sp.first( 4 ), 2 ) ==	1 + 12 );
		static_assert( horner_fma_even()( sp.first( 5 ), 2 ) ==	1 + 12 + 80 );

		static_assert( horner_fma_odd ()( sp.first( 1 ), 2 ) ==	0 );
		static_assert( horner_fma_odd ()( sp.first( 2 ), 2 ) ==	4 );
		static_assert( horner_fma_odd ()( sp.first( 3 ), 2 ) ==	4 );
		static_assert( horner_fma_odd ()( sp.first( 4 ), 2 ) ==	4 + 32 );
		static_assert( horner_fma_odd ()( sp.first( 5 ), 2 ) ==	4 + 32 );
	}
	namespace poly::comparison_ {
		using pax::poly::polynull;

		static_assert( p.degree() == 4 );
		static_assert( polynomial( 4, 0 ).degree() == 0 );
		static_assert( polynomial( 4 ).degree() == 0 );
		static_assert( polynull<>.degree() == 0 );

		static_assert( p == polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p != polynomial( 1.0, 2, 3, 4, 6 ) );
		static_assert( p != polynomial( 2.0, 2, 3, 4, 5 ) );
		static_assert( p != polynull<> );
		static_assert( polynomial( 4 ) != polynull<> );//
		static_assert( polynomial( 0, 0, 0 ) == polynull<> );//
		static_assert( polynull<> == polynomial( 0 ) );

		static_assert( p <= polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p <= polynomial( 1.0, 2, 3, 4, 6, 0, 0 ) );
		static_assert( p <  polynomial( 1.0, 2, 3, 4, 6, 0, 0 ) );
		static_assert( p >= polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p >= polynomial( 1.0, 2, 3, 4, 4, 0, 0 ) );
		static_assert( p >  polynomial( 1.0, 2, 3, 4, 4, 0, 0 ) );
		static_assert( p >  polynull<> );
		static_assert( polynomial(    ) <= polynull<> );
		static_assert( polynomial( -4 ) <  polynull<> );
		static_assert( polynull<> >= polynomial(    ) );
		static_assert( polynull<> >  polynomial( -4 ) );
	}
	namespace poly::addition_ {
		static_assert( polynomial( 0 ) + p == p );
		static_assert( p + polynomial( 0 ) == p );
		static_assert( p + p == polynomial( 2, 4, 6, 8, 10 ) );
		static_assert( p + 1 == polynomial( 2, 3, 4, 5,  6 ) );
		static_assert( 1 + p == polynomial( 2, 3, 4, 5,  6 ) );
	}
	namespace poly::subtraction_ {
		static_assert( p - polynomial( 0.0 ) == p );
		static_assert( p - p + p == p );
		static_assert( p - 1 == polynomial( 0,  1,  2,  3,  4 ) );
		static_assert( 1 - p == polynomial( 0, -1, -2, -3, -4 ) );
	}
	namespace poly::shift_ {
		static_assert( shift<  3 >( p ) == polynomial( 0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift<  3 >( p ).extent == 8 );
		static_assert( shift<  0 >( p ) == polynomial( 1.0, 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift<  0 >( p ).extent == 5 );
		static_assert( shift< -1 >( p ) == polynomial( 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift< -1 >( p ).extent == 4 );
		static_assert( shift< -3 >( p ) == polynomial( 4.0, 5.0 ) );
		static_assert( shift< -3 >( p ).extent == 2 );
		static_assert( shift< -9 >( p ) == pax::poly::polynull<> );
		static_assert( shift< -9 >( p ).extent == 0 );
	}
	namespace poly::multiplication_ {
		static_assert( p * polynomial( 1.0 ) == p );
		static_assert( p * polynomial( 1.0, 2 ) == polynomial( 1.0, 4, 7, 10, 13, 10 ) );
		static_assert( p * 2 == polynomial( 2, 4, 6, 8, 10 ) );
		static_assert( 2 * p == polynomial( 2, 4, 6, 8, 10 ) );
	}
	namespace poly::division_ {
		constexpr auto p2			  = polynomial( 1.0, 2, 5 );
		constexpr auto pp			  = p * p2;
		static_assert( pp / polynomial( 1.0 ) == pp );
		static_assert( pp / p2 == p );
		static_assert( p / 2 == polynomial( 0.5, 1, 1.5, 2, 2.5 ) );
	}
	namespace poly::derivation {
		constexpr auto pp0	  = Polynomial< horner_fma, double, 0 >();
		static_assert( pp0 == derivate( pp0 ) );

		constexpr auto pp	  = derivate( p );
		static_assert( pp.extent == p.extent - 1 );
		static_assert( pp == polynomial( p[ 1 ]*1.0, p[ 2 ]*2.0, p[ 3 ]*3.0, p[ 4 ]*4.0 ) );
	}
	namespace poly::integration {
		static_assert( integrate( p, 1.5 ) == polynomial( 1.5, 1, 1, 1, 1, 1 ) );
		static_assert( derivate( integrate( p ) ) == p );
	}
}	// namespace pax
