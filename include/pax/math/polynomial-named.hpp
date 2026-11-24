//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include "polynomial.hpp"


namespace pax::poly {

	template< std::size_t N, poly::floating X >
	constexpr auto All_one( const X c_ = 1 ) {
		Polynomial< horner_fma, std::decay_t< X >, N >	res;
		for( auto & c : res )							c = c_;
		return res;
	};

	template< std::size_t N, poly::floating X >
	constexpr auto High_one( const X c_ = 1 ) {
		Polynomial< horner_even_or_odd< ( N+1 ) >, std::decay_t< X >, N+1 >	res{};
		res.back() = c_;
		return res;
	};

	template< std::size_t N, poly::floating X >
	constexpr auto shapiro( const int alternative_ ) {
		constexpr std::size_t 			Sz = std::size_t( 1u ) << N;
		if      constexpr( N == 0 )		return polynull< X, horner_fma >;
		else if constexpr( N == 1 )		return polynomial< horner_fma, X >( 1, alternative_ );
		else {
			auto res		  = Polynomial< horner_fma, X, Sz >();
			res[ 0 ]		  = res[ 1 ] = res[ 2 ] = 1;
			res[ 3 ]		  = -1;
			std::size_t half  = 2u;
			while( ( half <<= 1 ) < Sz ) {
				std::copy(    res.begin(), res.begin() + half/2u, res.begin() + half );
				auto ptr0   = res.begin()        + ( half >> 1 );
				auto ptr2   = res.begin() + half + ( half >> 1 );
				while( ptr0 < res.begin() + half )	*( ptr2++ ) = -*( ptr0++ );
			}
			if( alternative_ < 0 )
				for( auto ptr = res.begin() + Sz/2; ptr < res.end(); ++ptr )	*ptr = -*ptr;
			return res;
		}
	};

	template< std::size_t N, poly::floating X >
	constexpr auto Touchard() {
		if      constexpr( N == 0 )		return polynull< X, horner_fma_even >;
		else if constexpr( N == 1 )		return High_one< 1, X >();
		else {
			constexpr auto temp		  = Touchard< N-1, X >();
			return shift< 1 >( temp + derivate( temp ) );
		}
	};


	/// Abel polynomials: https://en.wikipedia.org/wiki/Abel_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto abel( const std::size_t a ) {
		if constexpr( N == 0 )			return High_one< 0, X >();
		else {
			Polynomial< horner_fma, X, N+1 >	res{};
			res[ 1 ]						  = 1;
			for( std::size_t j=2; j<N+1; ++j )
				for( std::size_t i=j; i>0; --i )
					res[ i ]				  = res[ i - 1 ] - a*N*res[ i ];
			return res;
		}
	};
	
	/// Apolynomial with all coefficients equal to one.
	template< std::size_t N, poly::floating X = double >
	constexpr auto all_one		  = All_one< N, X >();
	
	/// x^N, a polynomial with all coefficients equal to zero, except x^N.
	template< std::size_t N, poly::floating X = double >
	constexpr auto high_one		  = High_one< N, X >();
	
	/// Bessel polynomials: https://en.wikipedia.org/wiki/Bessel_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto bessel		  = shift< 1 >( ( (N<<1) - 1 )*bessel< N-1, X > ) + bessel< N-2, X >;
	template< poly::floating X >	constexpr auto bessel< 0, X > = all_one< 1, X >;
	template< poly::floating X >	constexpr auto bessel< 1, X > = all_one< 2, X >;

	/// Bessel reversed polynomials: https://en.wikipedia.org/wiki/Bessel_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto bessel_reversed = ( (N<<1) - 1 )*bessel_reversed< N-1, X > + shift< 2 >( bessel_reversed< N-2, X > );
	template< poly::floating X >	constexpr auto bessel_reversed< 0, X > = all_one< 1, X >;
	template< poly::floating X >	constexpr auto bessel_reversed< 1, X > = all_one< 2, X >;

	/// Chebyshev polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto chebyshev1	  = shift< 1 >( 2*chebyshev1< N-1, X > ) - chebyshev1< N-2, X >;
	template< poly::floating X >	constexpr auto chebyshev1< 0, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto chebyshev1< 1, X > = high_one< 1, X >;

	/// Chebyshev polynomials of the second kind: https://en.wikipedia.org/wiki/Chebyshev_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto chebyshev2	  = shift< 1 >( 2*chebyshev2< N-1, X > ) - chebyshev2< N-2, X >;
	template< poly::floating X >	constexpr auto chebyshev2< 0, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto chebyshev2< 1, X > = High_one< 1, X >( 2 );

	/// Dickson polynomials of the first kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto dickson1( const X a ) noexcept {
		if      constexpr( N == 0 )	return polynull< X, horner_fma_even >;
		else if constexpr( N == 1 )	return high_one< 1, X >;
		else if constexpr( N == 2 )	return polynomial< horner_fma_even, X >( -2*a, 0, 1 );
		else						return shift< 1 >( dickson1< N-1, X >( a ) ) - a*dickson1< N-2, X >( a );
	}

	/// Dickson polynomials of the seconnd kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto dickson2( const X a ) noexcept {
		if      constexpr( N == 0 )	return high_one< 0, X >;
		else if constexpr( N == 1 )	return high_one< 1, X >;
		else						return shift< 1 >( dickson2< N-1, X >( a ) ) - a*dickson2< N-2, X >( a );
	}

	/// Fibonacci polynomials: https://en.wikipedia.org/wiki/Fibonacci_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto fibonacci	  = shift< 1 >( fibonacci< N-1, X > ) + fibonacci< N-2, X >;
	template< poly::floating X >	constexpr auto fibonacci< 0, X > = polynull< X, horner_even_or_odd< 0 > >;
	template< poly::floating X >	constexpr auto fibonacci< 1, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto fibonacci< 2, X > = high_one< 1, X >;

	/// Probabilist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto hermite1		  = shift< 1 >( hermite1< N-1, X > ) - (N-1)*hermite1< N-2, X >;
	template< poly::floating X >	constexpr auto hermite1< 0, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto hermite1< 1, X > = high_one< 1, X >;

	/// Physicist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto hermite2		  = shift< 1 >( 2*hermite2< N-1, X > ) - 2*(N-1)*hermite2< N-2, X >;
	template< poly::floating X >	constexpr auto hermite2< 0, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto hermite2< 1, X > = High_one< 1, X >( 2 );

	/// Jacobi polynomials: https://en.wikipedia.org/wiki/Jacobi_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto jacobi( const X alpha, const X beta ) noexcept {
		if      constexpr( N == 0 )	return high_one< 0, X >;
		else if constexpr( N == 1 )	return polynomial< horner_fma, X >( (alpha-beta)*0.5, 1 + (alpha+beta)*0.5 );
		else {
			const X	a = N + alpha, b = N + beta, c = a + b;
			return ( jacobi< N-1, X >( alpha, beta ) *(c-1)*polynomial< horner_fma, X >( (alpha-beta)*(c-2*N), c*(c-2) )
				   - jacobi< N-2, X >( alpha, beta ) *2*(a-1)*(b-1)*c )/( 2*(c-2)*N*(c-N) );
		}
	}

	/// Generalized Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto laguerre_generalized( const X a ) noexcept {
		if      constexpr( N == 0 )	return high_one< 0, X >;
		else if constexpr( N == 1 )	return polynomial< horner_fma, X >( 1 + a, -1 );
		else {
			return (  laguerre_generalized< N-1, X >( a ) * polynomial< horner_fma, X >( ( N << 1 ) - 1 + a, -1 )
			  		- laguerre_generalized< N-2, X >( a ) * ( N - 1 + a ) )/N;
		}
	}

	/// Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto laguerre		  = laguerre_generalized< N, X >( X{} );

	/// Legendre polynomials: https://en.wikipedia.org/wiki/Legendre_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto legendre		  = ( shift< 1 >( ( 2*N-1 )*legendre< N-1, X > ) - ( N-1 )*legendre< N-2, X > )/N;
	template< poly::floating X >	constexpr auto legendre< 0, X > = high_one< 0, X >;
	template< poly::floating X >	constexpr auto legendre< 1, X > = high_one< 1, X >;

	/// Lucas polynomials: https://en.wikipedia.org/wiki/Fibonacci_polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, poly::floating X = double >
	constexpr auto lucas		  = shift< 1 >( lucas< N-1, X > ) + lucas< N-2, X >;
	template< poly::floating X >	constexpr auto lucas< 0, X > = High_one< 0, X >( 2 );
	template< poly::floating X >	constexpr auto lucas< 1, X > = high_one< 1, X >;

	/// Shapiro polynomials of the first kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto shapiro1		  = shapiro< N, X >(  1 );

	/// Shapiro polynomials of the second kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto shapiro2		  = shapiro< N, X >( -1 );

	/// Touchard polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, poly::floating X = double >
	constexpr auto touchard		  = Touchard< N, X >();

	/// Wilkinsons polynomial: https://en.wikipedia.org/wiki/Wilkinson%27s_polynomial
	constexpr auto wilkinsons	  = polynomial< horner_fma >( 
		  2'432'902'008'176'640'000.0L, -8752'948'036'761'600'000.0L, 13803'759'753'640'704'000.0L,
		-12'870'931'245'150'988'800.0L,  8037'811'822'645'051'776.0L, -3599'979'517'947'607'200.0L, 
		  1'206'647'803'780'373'360.0L,  -311'333'643'161'390'640.0L,    63'030'812'099'294'896.0L, 
		    -10'142'299'865'511'450.0L,     1'307'535'010'540'395.0L,      -135'585'182'899'530.0L, 
		         11'310'276'995'381.0L,          -756'111'184'500.0L,            40'171'771'630.0L, 
		             -1'672'280'820.0L,                53'327'946.0L,                -1'256'850.0L, 
		                     20'615.0L,                      -210.0L,                         1.0L
	);
}	// namespace pax::poly

namespace pax::poly::test {
#if !defined( NDEBUG )
	namespace abel_ {
		static_assert( !abel< 6 >( 1 ).every_2nd_coeff );
		static_assert( abel< 6 >( 2 ) == polynomial( 0.0, -248832, 103680, -17280, 1440, -60, 1 ) );
		static_assert( abel< 6 >( 2 )( 1 ) == -161051 );
	}
	namespace bessel_ {
		static_assert( !bessel< 5 >.every_2nd_coeff );
		static_assert(  bessel< 5 >				== polynomial(   1.0,  15, 105, 420, 945, 945 ) );
		static_assert(  bessel< 5 >( 1 )			==   1.0 +  15 + 105 + 420 + 945 + 945 );

		static_assert( !bessel_reversed< 5 >.every_2nd_coeff );
		static_assert(  bessel_reversed< 5 >		== polynomial( 945.0, 945, 420, 105,  15,   1 ) );
		static_assert(  bessel_reversed< 5 >( 1 )	== 945.0 + 945 + 420 + 105 +  15 +   1 );
	}
	namespace chebyshev_ {
		static_assert( chebyshev1< 6 >.every_2nd_coeff );
		static_assert( chebyshev1< 6 > == polynomial( -1.0,  0, 18,  0, -48,    0, 32      ) );
		static_assert( chebyshev1< 6 >( 1 ) == -1.0 +  0 + 18 +  0 + -48 +    0 + 32       );

		static_assert( chebyshev1< 7 >.every_2nd_coeff );
		static_assert( chebyshev1< 7 > == polynomial(  0.0, -7,  0, 56,   0, -112,  0, 64  ) );
		static_assert( chebyshev1< 7 >( 1 ) ==  0.0 + -7 +  0 + 56 +   0 + -112 +  0 +  64 );

		static_assert( chebyshev2< 6 >.every_2nd_coeff );
		static_assert( chebyshev2< 6 > == polynomial( -1.0,  0, 24,  0, -80,    0, 64      ) );
		static_assert( chebyshev2< 6 >( 1 ) == -1.0 +  0 + 24 +  0 + -80 +    0 + 64       );

		static_assert( chebyshev2< 7 >.every_2nd_coeff );
		static_assert( chebyshev2< 7 > == polynomial(  0.0, -8,  0, 80,   0, -192,  0, 128 ) );
		static_assert( chebyshev2< 7 >( 1 ) ==  0.0 + -8 +  0 + 80 +   0 + -192 +  0 + 128 );
	}
	namespace dickson1_ {
		constexpr double a = 1;

		static_assert( dickson1< 4 >( a ).every_2nd_coeff );
		static_assert( dickson1< 4 >( a ) == polynomial( 2*a*a, 0, -4*a, 0, 1 ) );
		static_assert( dickson1< 4 >( a )( 1 ) == 2*a*a + 0 + -4*a + 0 + 1 );

		static_assert( dickson1< 5 >( a ).every_2nd_coeff );
		static_assert( dickson1< 5 >( a ) == polynomial( 0, 5*a*a, 0, -5*a, 0, 1 ) );
		static_assert( dickson1< 5 >( a )( 1 ) == 0 + 5*a*a + 0 + -5*a + 0 + 1 );

		static_assert( dickson2< 3 >( a ).every_2nd_coeff );
		static_assert( dickson2< 3 >( a ) == polynomial( 0, -2*a, 0, 1 ) );
		static_assert( dickson2< 3 >( a )( 1 ) == 0 + -2*a + 0 + 1 );

		static_assert( dickson2< 4 >( a ).every_2nd_coeff );
		static_assert( dickson2< 4 >( a ) == polynomial( a*a, 0, -3.0*a, 0, 1 ) );
		static_assert( dickson2< 4 >( a )( 1 ) == a*a + 0 + -3.0*a + 0 + 1 );
	} 
	namespace dickson2_ {
		constexpr double a = 2;

		static_assert( dickson1< 4 >( a ).every_2nd_coeff );
		static_assert( dickson1< 4 >( a ) == polynomial( 2*a*a, 0, -4*a, 0, 1 ) );
		static_assert( dickson1< 4 >( a )( 1 ) == 2*a*a + 0 + -4*a + 0 + 1 );

		static_assert( dickson1< 5 >( a ).every_2nd_coeff );
		static_assert( dickson1< 5 >( a ) == polynomial( 0, 5*a*a, 0, -5*a, 0, 1 ) );
		static_assert( dickson1< 5 >( a )( 1 ) == 0 + 5*a*a + 0 + -5*a + 0 + 1 );

		static_assert( dickson2< 3 >( a ).every_2nd_coeff );
		static_assert( dickson2< 3 >( a ) == polynomial( 0, -2*a, 0, 1 ) );
		static_assert( dickson2< 3 >( a )( 1 ) == 0 + -2*a + 0 + 1 );

		static_assert( dickson2< 4 >( a ).every_2nd_coeff );
		static_assert( dickson2< 4 >( a ) == polynomial( a*a, 0, -3.0*a, 0, 1 ) );
		static_assert( dickson2< 4 >( a )( 1 ) == a*a + 0 + -3.0*a + 0 + 1 );
	}
	namespace fibonacci_ {
		static_assert( fibonacci< 5 >.every_2nd_coeff );
		static_assert( fibonacci< 5 > == polynomial( 1.0, 0, 3, 0, 1 ) );
		static_assert( fibonacci< 5 >( 1 ) == 1 + 0 + 3 + 0 + 1     );

		static_assert( fibonacci< 6 >.every_2nd_coeff );
		static_assert( fibonacci< 6 > == polynomial( 0.0, 3, 0, 4, 0, 1 ) );
		static_assert( fibonacci< 6 >( 1 ) == 0 + 3 + 0 + 4 + 0 + 1 );
	}
	namespace hermite_ {
		static_assert( hermite1< 5 >.every_2nd_coeff );
		static_assert( hermite1< 5 > == polynomial(    0.0,  15,   0,  -10,    0,  1     ) );
		static_assert( hermite1< 5 >( 1 ) ==    0.0 +  15 +   0 +  -10 +    0 +  1     );

		static_assert( hermite1< 6 >.every_2nd_coeff );
		static_assert( hermite1< 6 > == polynomial(  -15.0,   0,  45,    0,  -15,  0,  1 ) );
		static_assert( hermite1< 6 >( 1 ) ==  -15.0 +   0 +  45 +    0 +  -15 +  0 +  1 );

		static_assert( hermite2< 5 >.every_2nd_coeff );
		static_assert( hermite2< 5 > == polynomial(    0.0, 120,   0, -160,    0, 32     ) );
		static_assert( hermite2< 5 >( 1 ) ==    0.0 + 120 +   0 + -160 +    0 + 32     );

		static_assert( hermite2< 6 >.every_2nd_coeff );
		static_assert( hermite2< 6 > == polynomial( -120.0,   0, 720,    0, -480,  0, 64 ) );
		static_assert( hermite2< 6 >( 1 ) == -120.0 +   0 + 720 +    0 + -480 +  0 + 64 );
	}
	namespace jacobi_one {
		constexpr double a  = 2;
		constexpr double b  = 3;
		constexpr double c1 = (a+2)*(a+b+3)*0.5;
		constexpr double c2 = (a+b+3)*(a+b+4)/8;
		static_assert( !jacobi< 2 >( a, b ).every_2nd_coeff );
		static_assert(  jacobi< 2 >( a, b ) == polynomial( (a+1)*(a+2)*0.5 - c1 + c2,  c1 - c2*2,  c2 ) );
		static_assert(  jacobi< 2 >( a, b )( 1 ) == (a+1)*(a+2)*0.5 - c1 + c2 + c1 - c2*2 + c2 );
	}
	namespace jacobi_two {
		constexpr double a  =  2.5;
		constexpr double b  = -0.25;
		constexpr double c1 = (a+2)*(a+b+3)*0.5;
		constexpr double c2 = (a+b+3)*(a+b+4)/8;
		static_assert( !jacobi< 2 >( a, b ).every_2nd_coeff );
		static_assert(  jacobi< 2 >( a, b ) == polynomial( (a+1)*(a+2)*0.5 - c1 + c2,  c1 - c2*2,  c2 ) );
		static_assert(  jacobi< 2 >( a, b )( 1 ) == (a+1)*(a+2)*0.5 - c1 + c2 + c1 - c2*2 + c2 );

		static_assert( jacobi< 5 >( 0.0, 0.0 ) == legendre< 5 > );
	}
	namespace laguerre_ {
		template< double a, unsigned k, unsigned N >	constexpr double mula = ( a + k )*mula< a, k+1, N >;
		template< double a, unsigned N >				constexpr double mula< a, N, N > = a + N;
			
		static_assert( !laguerre< 5 >.every_2nd_coeff );
		static_assert(  laguerre< 5 > == laguerre_generalized< 5 >( 0.0 ) );
		static_assert(  laguerre< 5 >( 1 ) == laguerre_generalized< 5 >( 0.0 )( 1 ) );

		static_assert( !laguerre_generalized< 5 >( 0.5 ).every_2nd_coeff );
	 	static_assert(  laguerre_generalized< 5 >( 0.5 ) == polynomial( 1*mula< 0.5, 1, 5 >, -5*mula< 0.5, 2, 5 >, 
										  10*mula< 0.5, 3, 5 >,  -10*mula< 0.5, 4, 5 >,  5*mula< 0.5, 5, 5 >, -1 )/120 );
	 	static_assert( -laguerre_generalized< 5 >( 0.5 )( 1 ) + ( 1*mula< 0.5, 1, 5 > + -5*mula< 0.5, 2, 5 >
	 									+ 10*mula< 0.5, 3, 5 > + -10*mula< 0.5, 4, 5 > + 5*mula< 0.5, 5, 5 >  -1 )/120 < 1e-15 );
	}
	namespace legendre_ {
		static_assert( legendre< 5 >.every_2nd_coeff );
		static_assert( legendre< 5 > == polynomial(  0.0, 15,   0, -70,    0, 63      )/ 8 );
		static_assert( legendre< 5 >( 3 ) == 1683 );

		static_assert( legendre< 6 >.every_2nd_coeff );
		static_assert( legendre< 6 > == polynomial( -5.0,  0, 105,   0, -315,  0, 231 )/16 );
		static_assert( legendre< 6 >( 3 ) == 8989 );
	}
	namespace lucas_ {
		static_assert( lucas< 5 >.every_2nd_coeff );
		static_assert( lucas< 5 > == polynomial( 0.0, 5, 0, 5, 0, 1 ) );
		static_assert( lucas< 5 >( 1 ) == 0.0 + 5 + 0 + 5 + 0 + 1 );

		static_assert( lucas< 6 >.every_2nd_coeff );
		static_assert( lucas< 6 > == polynomial( 2.0, 0, 9, 0, 6, 0, 1 ) );
		static_assert( lucas< 6 >( 1 ) == 2.0 + 0 + 9 + 0 + 6 + 0 + 1 );
	}
	namespace shapiro_ {
		static_assert( !shapiro1< 3 >.every_2nd_coeff );
		static_assert(  shapiro1< 0 > == polynomial( 0.0 ) );
		static_assert(  shapiro1< 3 > == polynomial( 1.0, 1, 1, -1, 1, 1, -1, 1 ) );
		static_assert(  shapiro1< 0 >( 1 ) == 0.0 );
		static_assert(  shapiro1< 3 >( 1 ) == 1.0 + 1 + 1 + -1 + 1 + 1 + -1 + 1 );

		static_assert( !shapiro2< 6 >.every_2nd_coeff );
		static_assert(  shapiro2< 0 > == polynomial( 0.0 ) );
		static_assert(  shapiro2< 3 > == polynomial( 1.0, 1, 1, -1, -1, -1, 1, -1 ) );
		static_assert(  shapiro2< 3 >( 1 ) == 1.0 + 1 + 1 + -1 + -1 + -1 + 1 + -1 );
	}
	namespace touchard_ {
		static_assert( !touchard< 5 >.every_2nd_coeff );
		static_assert(  touchard< 0 > == polynull<> );
		static_assert(  touchard< 5 > == polynomial( 0.0, 1, 15, 25, 10, 1 ) );
		static_assert(  touchard< 5 >( 1 ) == 0.0 + 1 + 15 + 25 + 10 + 1 );
	}
	namespace wilkinsons_ {
		static_assert( wilkinsons( 1.0 ) == 0.0 );
	}
#endif
}	// namespace pax::poly::test

