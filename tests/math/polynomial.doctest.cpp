//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#include <pax/math/polynomial-named.hpp>
#include <pax/doctest.hpp>
#include <pax/math/comparison.hpp>	// similar


namespace pax {
	static constexpr const auto p = polynomial( 1.0, 2, 3, 4, 5 );

	template< double a, unsigned k, unsigned N >	constexpr double mula = ( a + k )*mula< a, k+1, N >;
	template< double a, unsigned N >				constexpr double mula< a, N, N > = a + N;

	DOCTEST_TEST_CASE( "polynomial horner" ) {
		static constexpr const double poly0 [ 5 ] = { 1.0, 2.0, 3.0, 4.0, 5.0 };
		static constexpr const auto poly		  = std::span{ poly0 };
	
		{	// static
			static_assert( horner_fma     ()( poly.first< 1 >(), 2 ) ==	1 );
			static_assert( horner_fma     ()( poly.first< 2 >(), 2 ) ==	1 + 4 );
			static_assert( horner_fma     ()( poly.first< 3 >(), 2 ) ==	1 + 4 + 12 );
			static_assert( horner_fma     ()( poly.first< 4 >(), 2 ) ==	1 + 4 + 12 + 32 );
			static_assert( horner_fma     ()( poly.first< 5 >(), 2 ) ==	1 + 4 + 12 + 32 + 80 );

			static_assert( horner_fma_even()( poly.first< 1 >(), 2 ) ==	1 );
			static_assert( horner_fma_even()( poly.first< 2 >(), 2 ) ==	1 );
			static_assert( horner_fma_even()( poly.first< 3 >(), 2 ) ==	1 + 12 );
			static_assert( horner_fma_even()( poly.first< 4 >(), 2 ) ==	1 + 12 );
			static_assert( horner_fma_even()( poly.first< 5 >(), 2 ) ==	1 + 12 + 80 );

			static_assert( horner_fma_odd ()( poly.first< 1 >(), 2 ) ==	0 );
			static_assert( horner_fma_odd ()( poly.first< 2 >(), 2 ) ==	4 );
			static_assert( horner_fma_odd ()( poly.first< 3 >(), 2 ) ==	4 );
			static_assert( horner_fma_odd ()( poly.first< 4 >(), 2 ) ==	4 + 32 );
			static_assert( horner_fma_odd ()( poly.first< 5 >(), 2 ) ==	4 + 32 );
		}
		{	// dynamic
			DOCTEST_FAST_CHECK_EQ( horner_fma     ()( poly.first( 1 ), 2 ),		1 );
			DOCTEST_FAST_CHECK_EQ( horner_fma     ()( poly.first( 2 ), 2 ),		1 + 4 );
			DOCTEST_FAST_CHECK_EQ( horner_fma     ()( poly.first( 3 ), 2 ),		1 + 4 + 12 );
			DOCTEST_FAST_CHECK_EQ( horner_fma     ()( poly.first( 4 ), 2 ),		1 + 4 + 12 + 32 );
			DOCTEST_FAST_CHECK_EQ( horner_fma     ()( poly.first( 5 ), 2 ),		1 + 4 + 12 + 32 + 80 );

			DOCTEST_FAST_CHECK_EQ( horner_fma_even()( poly.first( 1 ), 2 ),		1 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_even()( poly.first( 2 ), 2 ),		1 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_even()( poly.first( 3 ), 2 ),		1 + 12 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_even()( poly.first( 4 ), 2 ),		1 + 12 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_even()( poly.first( 5 ), 2 ),		1 + 12 + 80 );

			DOCTEST_FAST_CHECK_EQ( horner_fma_odd ()( poly.first( 1 ), 2 ),		0 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_odd ()( poly.first( 2 ), 2 ),		4 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_odd ()( poly.first( 3 ), 2 ),		4 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_odd ()( poly.first( 4 ), 2 ),		4 + 32 );
			DOCTEST_FAST_CHECK_EQ( horner_fma_odd ()( poly.first( 5 ), 2 ),		4 + 32 );
		}
	}
	DOCTEST_TEST_CASE( "polynomial output" ) {
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial(-1., 2, 3, -4, 5, 1, -1 ) ),	"-1 + 2x + 3x^2 - 4x^3 + 5x^4 + x^5 - x^6" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0. ) ),						"0" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 1. ) ),						"1" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1 ) ),					"x" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1, 0 ) ),					"x" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 0., 1, 0, 3 ) ),				"x + 3x^3" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{}", polynomial( 1., 0 ) ),					"1" );

		// std::cout << "First order Chebychev polynomial:\n\\mbox{}\n";
		// std::cout << "\\\\$T_0(x) = " << poly::chebyshev1< 0 > << "$\n";
		// std::cout << "\\\\$T_1(x) = " << poly::chebyshev1< 1 > << "$\n";
		// std::cout << "\\\\$T_2(x) = " << poly::chebyshev1< 2 > << "$\n";
		// std::cout << "\\\\$T_3(x) = " << poly::chebyshev1< 3 > << "$\n";
		// std::cout << "\\\\$T_4(x) = " << poly::chebyshev1< 4 > << "$\n";
		// std::cout << "\\\\$T_5(x) = " << poly::chebyshev1< 5 > << "$\n";
		// std::cout << "\\\\$T_6(x) = " << poly::chebyshev1< 6 > << "$\n";
		// std::cout << "\\\\$T_7(x) = " << poly::chebyshev1< 7 > << "$\n";
		// std::cout << "\\\\$T_8(x) = " << poly::chebyshev1< 8 > << "$\n";
		// std::cout << "\\\\$T_9(x) = " << poly::chebyshev1< 9 > << "$\n";
		//
		// std::cout << "\nSecond order Chebychev polynomial:\n\\mbox{}\n";
		// std::cout << "\\\\$U_0(x) = " << poly::chebyshev2< 0 > << "$\n";
		// std::cout << "\\\\$U_1(x) = " << poly::chebyshev2< 1 > << "$\n";
		// std::cout << "\\\\$U_2(x) = " << poly::chebyshev2< 2 > << "$\n";
		// std::cout << "\\\\$U_3(x) = " << poly::chebyshev2< 3 > << "$\n";
		// std::cout << "\\\\$U_4(x) = " << poly::chebyshev2< 4 > << "$\n";
		// std::cout << "\\\\$U_5(x) = " << poly::chebyshev2< 5 > << "$\n";
		// std::cout << "\\\\$U_6(x) = " << poly::chebyshev2< 6 > << "$\n";
		// std::cout << "\\\\$U_7(x) = " << poly::chebyshev2< 7 > << "$\n";
		// std::cout << "\\\\$U_8(x) = " << poly::chebyshev2< 8 > << "$\n";
		// std::cout << "\\\\$U_9(x) = " << poly::chebyshev2< 9 > << "$\n";
	}
	DOCTEST_TEST_CASE( "polynomial comparison" ) {
		static_assert( p.degree() == 4 );
		static_assert( polynomial( 4, 0 ).degree() == 0 );
		static_assert( polynomial( 4 ).degree() == 0 );
		static_assert( poly::polynull<>.degree() == 0 );

		static_assert( p == polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p != polynomial( 1.0, 2, 3, 4, 6 ) );
		static_assert( p != polynomial( 2.0, 2, 3, 4, 5 ) );
		static_assert( p != poly::polynull<> );
		static_assert( polynomial( 4 ) != poly::polynull<> );//
		static_assert( polynomial( 0, 0, 0 ) == poly::polynull<> );//
		static_assert( poly::polynull<> == polynomial( 0 ) );

		static_assert( p <= polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p <= polynomial( 1.0, 2, 3, 4, 6, 0, 0 ) );
		static_assert( p <  polynomial( 1.0, 2, 3, 4, 6, 0, 0 ) );
		static_assert( p >= polynomial( 1.0, 2, 3, 4, 5, 0, 0 ) );
		static_assert( p >= polynomial( 1.0, 2, 3, 4, 4, 0, 0 ) );
		static_assert( p >  polynomial( 1.0, 2, 3, 4, 4, 0, 0 ) );
		static_assert( p >  poly::polynull<> );
		static_assert( polynomial(    ) <= poly::polynull<> );
		static_assert( polynomial( -4 ) <  poly::polynull<> );
		static_assert( poly::polynull<> >= polynomial(    ) );
		static_assert( poly::polynull<> >  polynomial( -4 ) );
	}
	DOCTEST_TEST_CASE( "polynomial addition" ) {
		DOCTEST_FAST_CHECK_EQ( polynomial( 0 ) + p, p );
		DOCTEST_FAST_CHECK_EQ( p + polynomial( 0 ), p );
		DOCTEST_FAST_CHECK_EQ( p + p, polynomial( 2, 4, 6, 8, 10 ) );
		DOCTEST_FAST_CHECK_EQ( p + 1, polynomial( 2, 3, 4, 5,  6 ) );
		DOCTEST_FAST_CHECK_EQ( 1 + p, polynomial( 2, 3, 4, 5,  6 ) );
	}
	DOCTEST_TEST_CASE( "polynomial subtraction" ) {
		DOCTEST_FAST_CHECK_EQ( p - polynomial( 0.0 ), p );
		DOCTEST_FAST_CHECK_EQ( p - p + p, p );
		DOCTEST_FAST_CHECK_EQ( p - 1, polynomial( 0,  1,  2,  3,  4 ) );
		DOCTEST_FAST_CHECK_EQ( 1 - p, polynomial( 0, -1, -2, -3, -4 ) );
	}
	DOCTEST_TEST_CASE( "polynomial shift" ) {
		static_assert( shift<  3 >( p ) == polynomial( 0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift<  3 >( p ).extent == 8 );
		static_assert( shift<  0 >( p ) == polynomial( 1.0, 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift<  0 >( p ).extent == 5 );
		static_assert( shift< -1 >( p ) == polynomial( 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift< -1 >( p ).extent == 4 );
		static_assert( shift< -3 >( p ) == polynomial( 4.0, 5.0 ) );
		static_assert( shift< -3 >( p ).extent == 2 );
		static_assert( shift< -9 >( p ) == poly::polynull<> );
		static_assert( shift< -9 >( p ).extent == 0 );
	}
	DOCTEST_TEST_CASE( "polynomial multiplication" ) {
		DOCTEST_FAST_CHECK_EQ( p * polynomial( 1.0 ), p );
		DOCTEST_FAST_CHECK_EQ( p * polynomial( 1.0, 2 ), polynomial( 1.0, 4, 7, 10, 13, 10 ) );
		DOCTEST_FAST_CHECK_EQ( p * 2, polynomial( 2, 4, 6, 8, 10 ) );
		DOCTEST_FAST_CHECK_EQ( 2 * p, polynomial( 2, 4, 6, 8, 10 ) );
	}
	DOCTEST_TEST_CASE( "polynomial division" ) {
		constexpr auto p2			  = polynomial( 1.0, 2, 5 );
		const     auto pp			  = p * p2;
		DOCTEST_FAST_CHECK_EQ( pp / polynomial( 1.0 ), pp );
		DOCTEST_FAST_CHECK_EQ( pp / p2, p );
		DOCTEST_FAST_CHECK_EQ( p / 2, polynomial( 0.5, 1, 1.5, 2, 2.5 ) );
	}
	DOCTEST_TEST_CASE( "polynomial derivation" ) {
		{
			constexpr auto pp	  = Polynomial< horner_fma, double, 0 >();
			static_assert( pp == derivate( pp ) );
		} {
			constexpr auto pp	  = derivate( p );
			static_assert( pp.extent == p.extent - 1 );
			static_assert( pp == polynomial( p[ 1 ]*1.0, p[ 2 ]*2.0, p[ 3 ]*3.0, p[ 4 ]*4.0 ) );
		}
	}
	DOCTEST_TEST_CASE( "polynomial integration" ) {
		static_assert( integrate( p, 1.5 ) == polynomial( 1.5, 1, 1, 1, 1, 1 ) );
		static_assert( derivate( integrate( p ) ) == p );
	}

	// Named polynomials
	DOCTEST_TEST_CASE( "polynomial poly::abel" ) {
		static_assert( !poly::abel< 5 >( 1 ).every_2nd_coeff );
		static_assert( !poly::abel< 6 >( 1 ).every_2nd_coeff );

		static_assert( poly::abel< 0 >( 1 ) == polynomial( 1 ) );
		static_assert( poly::abel< 1 >( 1 ) == polynomial( 0, 1 ) );
		static_assert( poly::abel< 4 >( 1 ) == polynomial( 0.0, -64, 48, -12, 1 ) );
		static_assert( poly::abel< 0 >( 2 ) == polynomial( 1 ) );
		static_assert( poly::abel< 1 >( 1 ) == polynomial( 0, 1 ) );
		static_assert( poly::abel< 6 >( 2 ) == polynomial( 0.0, -248832, 103680, -17280, 1440, -60, 1 ) );

		static_assert( poly::abel< 0 >( 1 )( 1 ) == 1 );
		static_assert( poly::abel< 1 >( 1 )( 1 ) == 1 );
		static_assert( poly::abel< 4 >( 1 )( 1 ) == 0.0 -64 +48 -12 +1 );
		static_assert( poly::abel< 0 >( 2 )( 1 ) == 1 );
		static_assert( poly::abel< 1 >( 1 )( 1 ) == 1 );
		static_assert( poly::abel< 6 >( 2 )( 1 ) == 0.0 -248832 +103680 -17280 +1440 -60 +1 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::bessel" ) {
		static_assert( !poly::bessel< 4 >.every_2nd_coeff );
		static_assert( !poly::bessel< 5 >.every_2nd_coeff );
		static_assert( !poly::bessel_reversed< 4 >.every_2nd_coeff );
		static_assert( !poly::bessel_reversed< 5 >.every_2nd_coeff );

		static_assert( poly::bessel< 4 >			== polynomial(   1.0,  10,  45, 105, 105      ) );
		static_assert( poly::bessel< 5 >			== polynomial(   1.0,  15, 105, 420, 945, 945 ) );
		static_assert( poly::bessel_reversed< 4 >	== polynomial( 105.0, 105,  45,  10,   1      ) );
		static_assert( poly::bessel_reversed< 5 >	== polynomial( 945.0, 945, 420, 105,  15,   1 ) );

		static_assert( poly::bessel< 4 >( 1 )				==   1.0 +  10 +  45 + 105 + 105       );
		static_assert( poly::bessel< 5 >( 1 )				==   1.0 +  15 + 105 + 420 + 945 + 945 );
		static_assert( poly::bessel_reversed< 4 >( 1 )	== 105.0 + 105 +  45 +  10 +   1       );
		static_assert( poly::bessel_reversed< 5 >( 1 )	== 945.0 + 945 + 420 + 105 +  15 +   1 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::chebyshev" ) {
		static_assert( poly::chebyshev1< 5 >.every_2nd_coeff );
		static_assert( poly::chebyshev1< 6 >.every_2nd_coeff );
		static_assert( poly::chebyshev2< 5 >.every_2nd_coeff );
		static_assert( poly::chebyshev2< 6 >.every_2nd_coeff );

		static_assert( poly::chebyshev1< 6 > == polynomial( -1.0,  0, 18,  0, -48,    0, 32      ) );
		static_assert( poly::chebyshev1< 7 > == polynomial(  0.0, -7,  0, 56,   0, -112,  0, 64  ) );
		static_assert( poly::chebyshev2< 6 > == polynomial( -1.0,  0, 24,  0, -80,    0, 64      ) );
		static_assert( poly::chebyshev2< 7 > == polynomial(  0.0, -8,  0, 80,   0, -192,  0, 128 ) );

		static_assert( poly::chebyshev1< 6 >( 1 ) == -1.0 +  0 + 18 +  0 + -48 +    0 + 32       );
		static_assert( poly::chebyshev1< 7 >( 1 ) ==  0.0 + -7 +  0 + 56 +   0 + -112 +  0 +  64 );
		static_assert( poly::chebyshev2< 6 >( 1 ) == -1.0 +  0 + 24 +  0 + -80 +    0 + 64       );
		static_assert( poly::chebyshev2< 7 >( 1 ) ==  0.0 + -8 +  0 + 80 +   0 + -192 +  0 + 128 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::dickson" ) {
		{
			constexpr double a = 1;
			static_assert( poly::dickson1< 5 >( a ).every_2nd_coeff );
			static_assert( poly::dickson1< 6 >( a ).every_2nd_coeff );
			static_assert( poly::dickson2< 5 >( a ).every_2nd_coeff );
			static_assert( poly::dickson2< 6 >( a ).every_2nd_coeff );

			static_assert( poly::dickson1< 4 >( a ) == polynomial( 2*a*a, 0, -4*a, 0, 1 ) );
			static_assert( poly::dickson1< 5 >( a ) == polynomial( 0, 5*a*a, 0, -5*a, 0, 1 ) );
			static_assert( poly::dickson2< 3 >( a ) == polynomial( 0, -2*a, 0, 1 ) );
			static_assert( poly::dickson2< 4 >( a ) == polynomial( a*a, 0, -3.0*a, 0, 1 ) );
			static_assert( poly::dickson1< 4 >( a )( 1 ) == 2*a*a + 0 + -4*a + 0 + 1 );
			static_assert( poly::dickson1< 5 >( a )( 1 ) == 0 + 5*a*a + 0 + -5*a + 0 + 1 );
			static_assert( poly::dickson2< 3 >( a )( 1 ) == 0 + -2*a + 0 + 1 );
			static_assert( poly::dickson2< 4 >( a )( 1 ) == a*a + 0 + -3.0*a + 0 + 1 );
		} {
			constexpr double a = 2;
			static_assert( poly::dickson1< 5 >( a ).every_2nd_coeff );
			static_assert( poly::dickson1< 6 >( a ).every_2nd_coeff );
			static_assert( poly::dickson2< 5 >( a ).every_2nd_coeff );
			static_assert( poly::dickson2< 6 >( a ).every_2nd_coeff );

			static_assert( poly::dickson1< 4 >( a ) == polynomial( 2*a*a, 0, -4*a, 0, 1 ) );
			static_assert( poly::dickson1< 5 >( a ) == polynomial( 0, 5*a*a, 0, -5*a, 0, 1 ) );
			static_assert( poly::dickson2< 3 >( a ) == polynomial( 0, -2*a, 0, 1 ) );
			static_assert( poly::dickson2< 4 >( a ) == polynomial( a*a, 0, -3.0*a, 0, 1 ) );
			static_assert( poly::dickson1< 4 >( a )( 1 ) == 2*a*a + 0 + -4*a + 0 + 1 );
			static_assert( poly::dickson1< 5 >( a )( 1 ) == 0 + 5*a*a + 0 + -5*a + 0 + 1 );
			static_assert( poly::dickson2< 3 >( a )( 1 ) == 0 + -2*a + 0 + 1 );
			static_assert( poly::dickson2< 4 >( a )( 1 ) == a*a + 0 + -3.0*a + 0 + 1 );
		}
	}
	DOCTEST_TEST_CASE( "polynomial poly::fibonacci" ) {
		static_assert( poly::fibonacci< 5 >.every_2nd_coeff );
		static_assert( poly::fibonacci< 6 >.every_2nd_coeff );

		static_assert( poly::fibonacci< 5 > == polynomial( 1.0, 0, 3, 0, 1 ) );
		static_assert( poly::fibonacci< 6 > == polynomial( 0.0, 3, 0, 4, 0, 1 ) );

		static_assert( poly::fibonacci< 5 >( 1 ) == 1 + 0 + 3 + 0 + 1     );
		static_assert( poly::fibonacci< 6 >( 1 ) == 0 + 3 + 0 + 4 + 0 + 1 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::hermite" ) {
		static_assert( poly::hermite1< 5 >.every_2nd_coeff );
		static_assert( poly::hermite1< 6 >.every_2nd_coeff );
		static_assert( poly::hermite2< 5 >.every_2nd_coeff );
		static_assert( poly::hermite2< 6 >.every_2nd_coeff );

		static_assert( poly::hermite1< 5 > == polynomial(    0.0,  15,   0,  -10,    0,  1     ) );
		static_assert( poly::hermite1< 6 > == polynomial(  -15.0,   0,  45,    0,  -15,  0,  1 ) );
		static_assert( poly::hermite2< 5 > == polynomial(    0.0, 120,   0, -160,    0, 32     ) );
		static_assert( poly::hermite2< 6 > == polynomial( -120.0,   0, 720,    0, -480,  0, 64 ) );
		static_assert( poly::hermite1< 5 >( 1 ) ==    0.0 +  15 +   0 +  -10 +    0 +  1     );
		static_assert( poly::hermite1< 6 >( 1 ) ==  -15.0 +   0 +  45 +    0 +  -15 +  0 +  1 );
		static_assert( poly::hermite2< 5 >( 1 ) ==    0.0 + 120 +   0 + -160 +    0 + 32     );
		static_assert( poly::hermite2< 6 >( 1 ) == -120.0 +   0 + 720 +    0 + -480 +  0 + 64 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::jacobi" ) {
		{
			constexpr double a  = 2;
			constexpr double b  = 3;
			constexpr double c1 = (a+2)*(a+b+3)*0.5;
			constexpr double c2 = (a+b+3)*(a+b+4)/8;
			static_assert( !poly::jacobi< 2 >( a, b ).every_2nd_coeff );
			static_assert( poly::jacobi< 2 >( a, b ) == polynomial( (a+1)*(a+2)*0.5 - c1 + c2,  c1 - c2*2,  c2 ) );
			static_assert( poly::jacobi< 2 >( a, b )( 1 )        == (a+1)*(a+2)*0.5 - c1 + c2 + c1 - c2*2 + c2 );
		} {
			constexpr double a  =  2.5;
			constexpr double b  = -0.25;
			constexpr double c1 = (a+2)*(a+b+3)*0.5;
			constexpr double c2 = (a+b+3)*(a+b+4)/8;
			static_assert( !poly::jacobi< 2 >( a, b ).every_2nd_coeff );
			static_assert( poly::jacobi< 2 >( a, b ) == polynomial( (a+1)*(a+2)*0.5 - c1 + c2,  c1 - c2*2,  c2 ) );
			static_assert( poly::jacobi< 2 >( a, b )( 1 )        == (a+1)*(a+2)*0.5 - c1 + c2 + c1 - c2*2 + c2 );
		}
		static_assert( poly::jacobi< 5 >( 0.0, 0.0 ) == poly::legendre< 5 > );
	}
	DOCTEST_TEST_CASE( "polynomial poly::laguerre" ) {
		static_assert( !poly::laguerre< 5 >.every_2nd_coeff );
		static_assert( !poly::laguerre< 6 >.every_2nd_coeff );
		static_assert( !poly::laguerre_generalized< 5 >( 0.5 ).every_2nd_coeff );
		static_assert( !poly::laguerre_generalized< 6 >( 0.5 ).every_2nd_coeff );

		DOCTEST_FAST_CHECK_UNARY( similar( poly::laguerre< 5 >, polynomial(  120.0,  -600,  600,  -200,  25,  -1 )/120 ) );
		DOCTEST_FAST_CHECK_UNARY( similar( poly::laguerre< 6 >, polynomial(  720.0, -4320, 5400, -2400, 450, -36, 1 )/720 ) );

		DOCTEST_FAST_CHECK_EQ( poly::laguerre< 5 >( 1 ), doctest::Approx( ( 120.0 +  -600 +  600 +  -200 +  25 +  -1 )/120 ) );
		DOCTEST_FAST_CHECK_EQ( poly::laguerre< 6 >( 1 ), doctest::Approx( ( 720.0 + -4320 + 5400 + -2400 + 450 + -36 + 1 )/720 ) );

		static_assert( poly::laguerre_generalized< 5 >( 0.0 ) == poly::laguerre< 5 > );
	 	static_assert( poly::laguerre_generalized< 5 >( 0.5 ) == polynomial(
	 			1*mula< 0.5, 1, 5 >, -5*mula< 0.5, 2, 5 >, 10*mula< 0.5, 3, 5 >, -10*mula< 0.5, 4, 5 >, 5*mula< 0.5, 5, 5 >, -1 )/120 );
	 	DOCTEST_FAST_CHECK_EQ( poly::laguerre_generalized< 5 >( 0.5 )( 1 ), doctest::Approx( 
	 			( 1*mula< 0.5, 1, 5 > + -5*mula< 0.5, 2, 5 > + 10*mula< 0.5, 3, 5 > + -10*mula< 0.5, 4, 5 > + 5*mula< 0.5, 5, 5 > -1 )/120 ) );
	}
	DOCTEST_TEST_CASE( "polynomial poly::legendre" ) {
		static_assert( poly::legendre< 5 >.every_2nd_coeff );
		static_assert( poly::legendre< 6 >.every_2nd_coeff );

		static_assert( poly::legendre< 5 > == polynomial(  0.0, 15,   0, -70,    0, 63      )/ 8 );
		static_assert( poly::legendre< 6 > == polynomial( -5.0,  0, 105,   0, -315,  0, 231 )/16 );
		static_assert( poly::legendre< 5 >( 3 ) == 1683 );
		static_assert( poly::legendre< 6 >( 3 ) == 8989 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::lucas" ) {
		static_assert( poly::lucas< 5 >.every_2nd_coeff );
		static_assert( poly::lucas< 6 >.every_2nd_coeff );

		static_assert( poly::lucas< 5 > == polynomial( 0.0, 5, 0, 5, 0, 1 ) );
		static_assert( poly::lucas< 6 > == polynomial( 2.0, 0, 9, 0, 6, 0, 1 ) );

		static_assert( poly::lucas< 5 >( 1 ) == 0.0 + 5 + 0 + 5 + 0 + 1 );
		static_assert( poly::lucas< 6 >( 1 ) == 2.0 + 0 + 9 + 0 + 6 + 0 + 1 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::shapiro" ) {
		static_assert( !poly::shapiro1< 5 >.every_2nd_coeff );
		static_assert( !poly::shapiro1< 6 >.every_2nd_coeff );
		static_assert( !poly::shapiro2< 5 >.every_2nd_coeff );
		static_assert( !poly::shapiro2< 6 >.every_2nd_coeff );

		static_assert( poly::shapiro1< 0 > == polynomial( 0.0 ) );
		static_assert( poly::shapiro1< 1 > == polynomial( 1.0, 1 ) );
		static_assert( poly::shapiro1< 2 > == polynomial( 1.0, 1, 1, -1 ) );
		static_assert( poly::shapiro1< 3 > == polynomial( 1.0, 1, 1, -1, 1, 1, -1, 1 ) );

		static_assert( poly::shapiro1< 0 >( 1 ) == 0.0 );
		static_assert( poly::shapiro1< 1 >( 1 ) == 1.0 + 1 );
		static_assert( poly::shapiro1< 2 >( 1 ) == 1.0 + 1 + 1 + -1 );
		static_assert( poly::shapiro1< 3 >( 1 ) == 1.0 + 1 + 1 + -1 + 1 + 1 + -1 + 1 );

		static_assert( poly::shapiro2< 0 > == polynomial( 0.0 ) );
		static_assert( poly::shapiro2< 1 > == polynomial( 1.0, -1 ) );
		static_assert( poly::shapiro2< 2 > == polynomial( 1.0, 1, -1, 1 ) );
		static_assert( poly::shapiro2< 3 > == polynomial( 1.0, 1, 1, -1, -1, -1, 1, -1 ) );

		static_assert( poly::shapiro2< 0 >( 1 ) == 0.0 );
		static_assert( poly::shapiro2< 1 >( 1 ) == 1.0 + -1 );
		static_assert( poly::shapiro2< 2 >( 1 ) == 1.0 + 1 + -1 + 1 );
		static_assert( poly::shapiro2< 3 >( 1 ) == 1.0 + 1 + 1 + -1 + -1 + -1 + 1 + -1 );
	}
	DOCTEST_TEST_CASE( "polynomial poly::touchard" ) {
		static_assert( !poly::touchard< 5 >.every_2nd_coeff );
		static_assert( !poly::touchard< 6 >.every_2nd_coeff );

		static_assert( poly::touchard< 0 > == poly::polynull<> );
		static_assert( poly::touchard< 5 > == polynomial( 0.0, 1, 15, 25, 10, 1 ) );
		static_assert( poly::touchard< 0 >( 1 ) == 0 );
		static_assert( poly::touchard< 5 >( 1 ) == 0.0 + 1 + 15 + 25 + 10 + 1 );
	}
	DOCTEST_TEST_CASE( "polynomial wilkinsons" ) {
		static_assert( poly::wilkinsons<>( 1.0 ) == 0.0 );
		// DOCTEST_FAST_CHECK_EQ( wilkinsons<>( 2.0 ), 0.0 );
	}

}	// namespace pax
