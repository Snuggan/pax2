//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/polynomial-extras.hpp>
#include <pax/doctest.hpp>
#include <pax/math/comparison.hpp>
#include <iostream>


namespace pax {
	
	static constexpr const double poly0 [ 5 ] = { 1.0, 2.0,  3.0,  4.0,   5.0 };
	static constexpr const double answer[ 5 ] = { 1.0, 5.0, 17.0, 49.0, 129.0 };
	static constexpr const auto poly		  = std::span{ poly0 };
	static constexpr const auto p			  = polynomial( poly0 );
	
	static_assert( p[ 3 ] == 4 );

	template< double a, unsigned k, unsigned N >
	constexpr double mula = ( a + k )*mula< a, k+1, N >;

	template< double a, unsigned N >
	constexpr double mula< a, N, N > = a + N;
	


	namespace {	// Static asserts
		template< std::size_t N = std::dynamic_extent > using Sp = std::span< const double, N >;
		constexpr std::array< double, 4 > test{{ 1.0, 2.0, 3.0, 4.0 }};

		static_assert( polynomial().degree() == 0 );
		static_assert( polynomial( 14, 0 ).degree() == 0 );
		static_assert( polynomial( 14, 4, 0 ).degree() == 1 );
		static_assert( polynomial( 14, 4, 8 ).extent == 3 );

		static_assert( polynomial( 2, 1, 0, 0 ) 	== polynomial( 2.0, 1.0 ) );
		static_assert( polynomial() 				== polynomial() );
		static_assert( polynomial()				 	== polynomial( 0.0, 0.0, 0.0 ) );
		static_assert( polynomial( 3, 2, 0, 0 ) 	<  polynomial( 2.0, 3.0 ) );
		static_assert( polynomial( 3, 2, 0, 0 ) 	<  polynomial( 1.0, 2.0, 1.0 ) );

		static_assert( polynomial( 2, 1 )( 3.0 )	== 5.0 );
		static_assert( polynomial()( 5 )			== 0 );
		static_assert( polynomial().extent			== std::dynamic_extent );
		static_assert( polynomial( 2 ).extent		== 1u );
		static_assert( polynomial< comp_horner_fma >( 2, 1 )[ 0 ] 	== 2.0 );
		static_assert( polynomial< comp_horner_fma >()( 5 )			== 0 );
		static_assert( polynomial< comp_horner_fma >().extent		== std::dynamic_extent );
		static_assert( polynomial< comp_horner_fma >( 2 ).extent	== 1u );

		static_assert( horner_fma()     ( Sp< 0 >{}, 				 2 ) ==  0 );
		static_assert( horner_fma()     ( Sp< 1 >{ test.data(), 1 }, 2 ) ==  1 );
		static_assert( horner_fma()     ( Sp< 2 >{ test.data(), 2 }, 2 ) ==  5 );
		static_assert( horner_fma()     ( Sp< 3 >{ test.data(), 3 }, 2 ) == 17 );
		static_assert( horner_fma()     ( Sp< 4 >{ test.data(), 4 }, 2 ) == 49 );
		static_assert( horner_fma()     ( Sp     { test.data(), 4 }, 2 ) == 49 );
		static_assert( comp_horner_fma()( Sp< 0 >{}, 				 2 ) ==  0 );
		static_assert( comp_horner_fma()( Sp< 1 >{ test.data(), 1 }, 2 ) ==  1 );
		static_assert( comp_horner_fma()( Sp< 2 >{ test.data(), 2 }, 2 ) ==  5 );
		static_assert( comp_horner_fma()( Sp< 3 >{ test.data(), 3 }, 2 ) == 17 );
		static_assert( comp_horner_fma()( Sp< 4 >{ test.data(), 4 }, 2 ) == 49 );
		static_assert( comp_horner_fma()( Sp     { test.data(), 4 }, 2 ) == 49 );
	}

	DOCTEST_TEST_CASE( "polynomial output" ) {
		// std::cout << "First order Chebychev polynomial:\n\\mbox{}\n";
		// std::cout << "\\\\$T_0(x) = " << chebyshev1< 0 > << "$\n";
		// std::cout << "\\\\$T_1(x) = " << chebyshev1< 1 > << "$\n";
		// std::cout << "\\\\$T_2(x) = " << chebyshev1< 2 > << "$\n";
		// std::cout << "\\\\$T_3(x) = " << chebyshev1< 3 > << "$\n";
		// std::cout << "\\\\$T_4(x) = " << chebyshev1< 4 > << "$\n";
		// std::cout << "\\\\$T_5(x) = " << chebyshev1< 5 > << "$\n";
		// std::cout << "\\\\$T_6(x) = " << chebyshev1< 6 > << "$\n";
		// std::cout << "\\\\$T_7(x) = " << chebyshev1< 7 > << "$\n";
		// std::cout << "\\\\$T_8(x) = " << chebyshev1< 8 > << "$\n";
		// std::cout << "\\\\$T_9(x) = " << chebyshev1< 9 > << "$\n";
		//
		// std::cout << "\nSecond order Chebychev polynomial:\n\\mbox{}\n";
		// std::cout << "\\\\$U_0(x) = " << chebyshev2< 0 > << "$\n";
		// std::cout << "\\\\$U_1(x) = " << chebyshev2< 1 > << "$\n";
		// std::cout << "\\\\$U_2(x) = " << chebyshev2< 2 > << "$\n";
		// std::cout << "\\\\$U_3(x) = " << chebyshev2< 3 > << "$\n";
		// std::cout << "\\\\$U_4(x) = " << chebyshev2< 4 > << "$\n";
		// std::cout << "\\\\$U_5(x) = " << chebyshev2< 5 > << "$\n";
		// std::cout << "\\\\$U_6(x) = " << chebyshev2< 6 > << "$\n";
		// std::cout << "\\\\$U_7(x) = " << chebyshev2< 7 > << "$\n";
		// std::cout << "\\\\$U_8(x) = " << chebyshev2< 8 > << "$\n";
		// std::cout << "\\\\$U_9(x) = " << chebyshev2< 9 > << "$\n";
		//
		// std::cout << polynomial(-1., 2, 3, -4, 5, 1, -1 ) << '\n';
		// std::cout << polynomial( 0. ) << '\n';
		// std::cout << polynomial( 1. ) << '\n';
		// std::cout << polynomial( 0., 1 ) << '\n';
		// std::cout << polynomial( 0., 1, 0 ) << '\n';
		// std::cout << polynomial( 0., 1, 0, 3 ) << '\n';
		// std::cout << polynomial( 1., 0 ) << '\n';
	}
	DOCTEST_TEST_CASE( "polynomial horner" ) {
		{	// Polynomial object
			constexpr auto	p = polynomial( 1.0, 2.0, 3.0, 4.0, 5.0 );
			DOCTEST_FAST_CHECK_EQ( p( 2 ), answer[ 4 ] );
			// constexpr auto res = p( 2 );
			// DOCTEST_FAST_CHECK_EQ( res,    answer[ 4 ] );
			// static_assert( res == answer[ 4 ] );
		}
		{	// static horner_fma
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first< 1 >(), 2 ),	answer[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first< 2 >(), 2 ),	answer[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first< 3 >(), 2 ),	answer[ 2 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first< 4 >(), 2 ),	answer[ 3 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first< 5 >(), 2 ),	answer[ 4 ] );
			// static_assert( horner_fma( poly.first< 5 >(), 2 )	==	answer[ 4 ] );
		}
		{	// dynamic horner_fma
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first( 1 ), 2 ),		answer[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first( 2 ), 2 ),		answer[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first( 3 ), 2 ),		answer[ 2 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first( 4 ), 2 ),		answer[ 3 ] );
			DOCTEST_FAST_CHECK_EQ( horner_fma()( poly.first( 5 ), 2 ),		answer[ 4 ] );
			// static_assert( horner_fma( poly.first( 5 ), 2 ) == answer[ 4 ] );
		}
	}
	DOCTEST_TEST_CASE( "polynomial compensated horner" ) {
		{	// static comp_horner_fma
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first< 1 >(), 2 ),	answer[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first< 2 >(), 2 ),	answer[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first< 3 >(), 2 ),	answer[ 2 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first< 4 >(), 2 ),	answer[ 3 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first< 5 >(), 2 ),	answer[ 4 ] );
			// static_assert( comp_horner_fma( poly.first< 5 >(), 2 )	==	answer[ 4 ] );
		}
		{	// dynamic comp_horner_fma
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first( 1 ), 2 ),		answer[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first( 2 ), 2 ),		answer[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first( 3 ), 2 ),		answer[ 2 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first( 4 ), 2 ),		answer[ 3 ] );
			DOCTEST_FAST_CHECK_EQ( comp_horner_fma()( poly.first( 5 ), 2 ),		answer[ 4 ] );
			// static_assert( comp_horner_fma( poly.first( 5 ), 2 ) == answer[ 4 ] );
		}
	}
	DOCTEST_TEST_CASE( "polynomial addition" ) {
		DOCTEST_FAST_CHECK_EQ( polynomial( 0.0 ) + p, p );
		DOCTEST_FAST_CHECK_EQ( p + polynomial( 0.0 ), p );
		DOCTEST_FAST_CHECK_EQ( p + p, polynomial( 2.0, 4.0, 6.0, 8.0, 10.0 ) );
	}
	DOCTEST_TEST_CASE( "polynomial subtraction" ) {
		DOCTEST_FAST_CHECK_EQ( p - polynomial( 0.0 ), p );
		DOCTEST_FAST_CHECK_EQ( p - p + p, p );
	}
	DOCTEST_TEST_CASE( "polynomial multiplication" ) {
		DOCTEST_FAST_CHECK_EQ( p * polynomial( 1.0 ), p );
		DOCTEST_FAST_CHECK_EQ( p * polynomial( 1.0, 2 ), polynomial( 1.0, 4, 7, 10, 13, 10 ) );
	}
	DOCTEST_TEST_CASE( "polynomial division" ) {
		constexpr auto p2	  = polynomial( 1.0, 2, 5 );
		const     auto pp	  = p * p2;
		DOCTEST_FAST_CHECK_EQ( pp / polynomial( 1.0 ), pp );
		DOCTEST_FAST_CHECK_EQ( pp / p2, p );
	}
	DOCTEST_TEST_CASE( "polynomial derivation" ) {
		{
			const     auto pp	  = Polynomial< horner_fma, double, 0 >();
			DOCTEST_FAST_CHECK_EQ( pp, derivate( pp ) );
		} {
			const     auto pp	  = derivate( p );
			DOCTEST_FAST_CHECK_EQ( pp.extent, p.extent - 1 );
			DOCTEST_FAST_CHECK_EQ( pp, polynomial( p[ 1 ]*1.0, p[ 2 ]*2.0, p[ 3 ]*3.0, p[ 4 ]*4.0 ) );
		}
	}
	DOCTEST_TEST_CASE( "polynomial integration" ) {
		DOCTEST_FAST_CHECK_EQ( integrate( p, 1.5 ), polynomial( 1.5, 1, 1, 1, 1, 1 ) );
		DOCTEST_FAST_CHECK_EQ( derivate( integrate( p ) ), p );
	}

	DOCTEST_TEST_CASE( "polynomial bessel" ) {
		DOCTEST_FAST_CHECK_EQ( bessel< 4 >, polynomial( 1.0, 10,  45, 105, 105 ) );
		DOCTEST_FAST_CHECK_EQ( bessel< 5 >, polynomial( 1.0, 15, 105, 420, 945, 945 ) );
		DOCTEST_FAST_CHECK_EQ( bessel_reversed< 4 >, polynomial( 105.0, 105,  45,  10,  1 ) );
		DOCTEST_FAST_CHECK_EQ( bessel_reversed< 5 >, polynomial( 945.0, 945, 420, 105, 15, 1 ) );
	}
	DOCTEST_TEST_CASE( "polynomial chebyshev" ) {
		DOCTEST_FAST_CHECK_EQ( chebyshev1< 5 >, polynomial(  0.0, 5,  0, -20,   0, 16 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev1< 6 >, polynomial( -1.0, 0, 18,   0, -48,  0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev2< 5 >, polynomial(  0.0, 6,  0, -32,   0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev2< 6 >, polynomial( -1.0, 0, 24,   0, -80,  0, 64 ) );

		DOCTEST_FAST_CHECK_EQ( chebyshev1_x2< 5 >.polynomial(), polynomial(  0.0, 5,  0, -20,   0, 16 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev1_x2< 6 >.polynomial(), polynomial( -1.0, 0, 18,   0, -48,  0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev2_x2< 5 >.polynomial(), polynomial(  0.0, 6,  0, -32,   0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( chebyshev2_x2< 6 >.polynomial(), polynomial( -1.0, 0, 24,   0, -80,  0, 64 ) );
	}
	DOCTEST_TEST_CASE( "polynomial fibonacci" ) {
		DOCTEST_FAST_CHECK_EQ( fibonacci< 0 >, polynomial( 0.0 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci< 1 >, polynomial( 1.0 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci< 5 >, polynomial( 1.0, 0, 3, 0, 1 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci< 6 >, polynomial( 0.0, 3, 0, 4, 0, 1 ) );

		DOCTEST_FAST_CHECK_EQ( fibonacci_x2< 0 >.polynomial(), polynomial( 0.0 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci_x2< 1 >.polynomial(), polynomial( 1.0 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci_x2< 5 >.polynomial(), polynomial( 1.0, 0, 3, 0, 1 ) );
		DOCTEST_FAST_CHECK_EQ( fibonacci_x2< 6 >.polynomial(), polynomial( 0.0, 3, 0, 4, 0, 1 ) );
	}
	DOCTEST_TEST_CASE( "polynomial hermite" ) {
		DOCTEST_FAST_CHECK_EQ( hermite1< 5 >, polynomial(    0.0,  15,   0,  -10,    0,  1 ) );
		DOCTEST_FAST_CHECK_EQ( hermite1< 6 >, polynomial(  -15.0,   0,  45,    0,  -15,  0,  1 ) );
		DOCTEST_FAST_CHECK_EQ( hermite2< 5 >, polynomial(    0.0, 120,   0, -160,    0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( hermite2< 6 >, polynomial( -120.0,   0, 720,    0, -480,  0, 64 ) );

		DOCTEST_FAST_CHECK_EQ( hermite1_x2< 5 >.polynomial(), polynomial(    0.0,  15,   0,  -10,    0,  1 ) );
		DOCTEST_FAST_CHECK_EQ( hermite1_x2< 6 >.polynomial(), polynomial(  -15.0,   0,  45,    0,  -15,  0,  1 ) );
		DOCTEST_FAST_CHECK_EQ( hermite2_x2< 5 >.polynomial(), polynomial(    0.0, 120,   0, -160,    0, 32 ) );
		DOCTEST_FAST_CHECK_EQ( hermite2_x2< 6 >.polynomial(), polynomial( -120.0,   0, 720,    0, -480,  0, 64 ) );
	}
	DOCTEST_TEST_CASE( "polynomial jacobi" ) {
		{
			constexpr double a  = 2;
			constexpr double b  = 3;
			constexpr double c1 = (a+2)*(a+b+3)*0.5;
			constexpr double c2 = (a+b+3)*(a+b+4)/8;
			DOCTEST_FAST_CHECK_EQ( jacobi< 2, double, a, b >, polynomial( (a+1)*(a+2)*0.5 - c1 + c2, c1 - c2*2, c2 ) );
		} {
			constexpr double a  =  2.5;
			constexpr double b  = -0.25;
			constexpr double c1 = (a+2)*(a+b+3)*0.5;
			constexpr double c2 = (a+b+3)*(a+b+4)/8;
			DOCTEST_FAST_CHECK_EQ( jacobi< 2, double, a, b >, polynomial( (a+1)*(a+2)*0.5 - c1 + c2, c1 - c2*2, c2 ) );
		}
		DOCTEST_FAST_CHECK_EQ( jacobi< 5, double,  0.0,  0.0 >, legendre< 5 > );
	}
	DOCTEST_TEST_CASE( "polynomial laguerre" ) {
		DOCTEST_FAST_CHECK_UNARY( similar( laguerre< 5 >, polynomial(  120.0,  -600,  600,  -200,  25,  -1 )/120 ) );
		DOCTEST_FAST_CHECK_UNARY( similar( laguerre< 6 >, polynomial(  720.0, -4320, 5400, -2400, 450, -36, 1 )/720 ) );
		DOCTEST_FAST_CHECK_EQ( laguerre_generalized< 5, double, 0.0 >, laguerre< 5 > );
	 		DOCTEST_FAST_CHECK_EQ( laguerre_generalized< 5, double, 0.5 >, polynomial(
	 			1*mula< 0.5, 1, 5 >, -5*mula< 0.5, 2, 5 >, 10*mula< 0.5, 3, 5 >, -10*mula< 0.5, 4, 5 >, 5*mula< 0.5, 5, 5 >, -1 )/120 );
	}
	DOCTEST_TEST_CASE( "polynomial legendre" ) {
		DOCTEST_FAST_CHECK_EQ( legendre< 5 >, polynomial(  0.0, 15,   0, -70,    0, 63 )/8 );
		DOCTEST_FAST_CHECK_EQ( legendre< 6 >, polynomial( -5.0,  0, 105,   0, -315,  0, 231 )/16 );
		DOCTEST_FAST_CHECK_EQ( legendre_x2< 5 >.polynomial(), polynomial(  0.0, 15,   0, -70,    0, 63 )/8 );
		DOCTEST_FAST_CHECK_EQ( legendre_x2< 6 >.polynomial(), polynomial( -5.0,  0, 105,   0, -315,  0, 231 )/16 );
	}
	DOCTEST_TEST_CASE( "polynomial lucas" ) {
		DOCTEST_FAST_CHECK_EQ( lucas< 5 >, polynomial( 0.0, 5, 0, 5, 0, 1 ) );
		DOCTEST_FAST_CHECK_EQ( lucas< 6 >, polynomial( 2.0, 0, 9, 0, 6, 0, 1 ) );
		DOCTEST_FAST_CHECK_EQ( lucas_x2< 5 >.polynomial(), polynomial( 0.0, 5, 0, 5, 0, 1 ) );
		DOCTEST_FAST_CHECK_EQ( lucas_x2< 6 >.polynomial(), polynomial( 2.0, 0, 9, 0, 6, 0, 1 ) );
	}
	DOCTEST_TEST_CASE( "polynomial wilkinsons" ) {
		DOCTEST_FAST_CHECK_EQ( wilkinsons< long double >( 1.0 ), 0.0 );
		// DOCTEST_FAST_CHECK_EQ( wilkinsons< long double >( 2.0 ), 0.0 );
	}

	DOCTEST_TEST_CASE( "polynomial_x2" ) {
		{
			constexpr auto 						p0 = polynomial();
			const Polynomial_x2					p2( p0 );
			DOCTEST_FAST_CHECK_EQ( p0( 2 ),		p2( 2 ) );
			DOCTEST_FAST_CHECK_EQ( p0.degree(),	p2.degree() );
			DOCTEST_FAST_CHECK_EQ( p0,			p2.polynomial() );
		} {
			constexpr auto 						p0 = chebyshev1< 8 >;
			const Polynomial_x2					p2( p0 );
			DOCTEST_FAST_CHECK_EQ( p0( 2 ),		p2( 2 ) );
			DOCTEST_FAST_CHECK_EQ( p0.degree(),	p2.degree() );
			DOCTEST_FAST_CHECK_EQ( p0,			p2.polynomial() );
		} {
			constexpr auto 						p0 = chebyshev1< 9 >;
			const Polynomial_x2					p2( p0 );
			DOCTEST_FAST_CHECK_EQ( p0( 2 ), 	p2( 2 ) );
			DOCTEST_FAST_CHECK_EQ( p0.degree(),	p2.degree() );
			DOCTEST_FAST_CHECK_EQ( p0, 			p2.polynomial() );
		}
	}

}	// namespace pax
