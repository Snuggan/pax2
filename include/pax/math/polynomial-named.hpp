//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include "polynomial-extras.hpp"


namespace pax {

	namespace detail {
		template< std::size_t N >
		using even_or_odd = Horner< ( N & 1u ) ? Horners::fma_even : Horners::fma_odd >;
	}

	template< is::floating X, typename Algo >	constexpr auto polynull  = Polynomial< Algo, X, 0 >{};

	template< std::size_t N, is::floating X, typename Algo >
	constexpr auto All_one( const X c_ = 1 ) {
		Polynomial< Algo, std::decay_t< X >, N >	res;
		for( auto & c : res )						c = c_;
		return res;
	};

	template< std::size_t N, is::floating X, typename Algo >
	constexpr auto High_one( const X c_ = 1 ) {
		Polynomial< Algo, std::decay_t< X >, N+1 >	res{};
		res.back() = c_;
		return res;
	};

	template< std::size_t N, is::floating X, typename Algo >
	constexpr auto Shapiro( const int alternative_ ) {
		constexpr std::size_t 			Sz = std::size_t( 1u ) << N;
		if constexpr( N == 0 )			return polynull< X, Algo >;
		else if constexpr( N == 1 )		return polynomial< Algo, X >( 1, alternative_ );
		else {
			auto res		  = Polynomial< Algo, X, Sz >();
			res[ 0 ]		  = res[ 1 ] = res[ 2 ] = 1;
			res[ 3 ]		  = -1;
			std::size_t half  = 2u;
			while( ( half <<= 1 ) < Sz ) {
				std::copy( res.begin(), res.begin() + half/2u, res.begin() + half );
				auto ptr0 = res.begin()       + half/2u;
				auto ptr2 = res.begin()+ half + half/2u;
				while( ptr0 < res.begin() + half )	*( ptr2++ ) = -*( ptr0++ );
			}
			if( alternative_ < 0 )
				for( auto ptr = res.begin() + Sz/2; ptr < res.end(); ++ptr )	*ptr = -*ptr;
			return res;
		}
	};

	template< std::size_t N, is::floating X, typename Algo >
	constexpr auto Touchard() {
		if      constexpr( N == 0 )		return polynull< X, Algo >;
		else if constexpr( N == 1 )		return High_one< 1, X, Algo >();
		else {
			constexpr auto temp	  = Touchard< N-1, X, Algo >();
			return High_one< 1, X, Algo >()*( temp + derivate( temp ) );
		}
	};


	/// Abel polynomials: https://en.wikipedia.org/wiki/Abel_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto abel( const std::size_t a ) {
		if constexpr( N == 0 )			return High_one< 0, X, Algo >();
		else {
			Polynomial< Algo, X, N+1 >	res{};
			res[ 1 ]				  = 1;
			for( std::size_t j=2; j<N+1; ++j )
				for( std::size_t i=j; i>0; --i )
					res[ i ]		  = res[ i - 1 ] - a*N*res[ i ];
			return res;
		}
	};
	
	/// Apolynomial with all coefficients equal to one.
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto all_one		  = All_one< N, X, Algo >();
	
	/// x^N, a polynomial with all coefficients equal to zero, except x^N.
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto high_one		  = High_one< N, X, Algo >();
	
	/// Bessel polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel		  = High_one< 1, X, Algo >( 2*N - 1 )*bessel< N-1, X, Algo > + bessel< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 0, X, Algo > = all_one< 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 1, X, Algo > = all_one< 2, X, Algo >;

	/// Bessel reversed polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel_reversed = ( 2*N - 1 )*bessel_reversed< N-1, X, Algo > 
									  + high_one< 2, X, Algo >*bessel_reversed< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 0, X, Algo > = all_one< 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 1, X, Algo > = all_one< 2, X, Algo >;

	/// Chebyshev polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev1	  = High_one< 1, X, Algo >( 2 )*chebyshev1< N-1, X, Algo > - chebyshev1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 0, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 1, X, Algo > = high_one< 1, X, Algo >;

	/// Chebyshev polynomials of the second kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev2	  = High_one< 1, X, Algo >( 2 )*chebyshev2< N-1, X, Algo > - chebyshev2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 0, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 1, X, Algo > = High_one< 1, X, Algo >( 2 );

	/// Dickson polynomials of the first kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson1		  = high_one< 1, X, Algo >*dickson1< N-1, a, X, Algo > - a*dickson1< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 0, a, X, Algo > = polynull<    X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 1, a, X, Algo > = high_one< 1, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 2, a, X, Algo > 
								  = polynomial< Algo, X >( -2.0*a, 0, 1 );

	/// Dickson polynomials of the seconnd kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson2		  = high_one< 1, X, Algo >*dickson2< N-1, a, X, Algo > - a*dickson2< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 0, a, X, Algo > = all_one < 1, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 1, a, X, Algo > = high_one< 1, X, Algo >;

	/// Fibonacci polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N > >
	constexpr auto fibonacci	  = high_one< 1, X, Algo >*fibonacci< N-1, X, Algo > + fibonacci< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 0, X, Algo > = polynull<    X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 1, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 2, X, Algo > = high_one< 1, X, Algo >;

	/// Probabilist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite1		  = high_one< 1, X, Algo >*hermite1< N-1, X, Algo > - (N-1)*hermite1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 0, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 1, X, Algo > = high_one< 1, X, Algo >;

	/// Physicist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite2		  = 2*high_one< 1, X, Algo >*hermite2< N-1, X, Algo > - 2*(N-1)*hermite2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 0, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 1, X, Algo > = high_one< 1, X, Algo >*2;

	/// Jacobi polynomials: https://en.wikipedia.org/wiki/Jacobi_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto jacobi( const X alpha, const X beta ) noexcept {
		if      constexpr( N == 0 )		return High_one< 0, X, Algo >();
		else if constexpr( N == 1 )		return polynomial< Algo, X >( alpha - (alpha+beta)*0.5, (alpha+beta)*0.5 + 1 );
		else {
			const X	a = N + alpha, b = N + beta, c = a + b;
			return ( jacobi< N-1, X, Algo >( alpha, beta ) *(c-1)*polynomial< Algo, X >( (alpha-beta)*(c-2*N), c*(c-2) )
				   - jacobi< N-2, X, Algo >( alpha, beta ) *2*(a-1)*(b-1)*c )/( 2*(c-2)*N*(c-N) );
		}
	}

	/// Generalized Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, is::floating X, X a, typename Algo = horner_fma >
	constexpr auto laguerre_generalized	  = ( polynomial< Algo, X >( 2*N-1+a, -1 )*laguerre_generalized< N-1, X, a, Algo > 
											- (N-1+a)*laguerre_generalized< N-2, X, a, Algo > )/N;
	template< is::floating X, X a, typename Algo >	constexpr auto laguerre_generalized< 0, X, a, Algo > 
										  = high_one< 0, X, Algo >;
	template< is::floating X, X a, typename Algo >	constexpr auto laguerre_generalized< 1, X, a, Algo > 
										  = polynomial< Algo, X >( 1+a, -1 );

	/// Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto laguerre		  = laguerre_generalized< N, X, X{}, Algo >;

	/// Legendre polynomials: https://en.wikipedia.org/wiki/Legendre_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto legendre		  = ( High_one< 1, X, Algo >( 2*N-1 )*legendre< N-1, X, Algo > 
									- (N-1)*legendre< N-2, X, Algo > )/N;
	template< is::floating X, typename Algo >	constexpr auto legendre< 0, X, Algo > = all_one < 1, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto legendre< 1, X, Algo > = high_one< 1, X, Algo >;

	/// Lucas polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto lucas		  = high_one< 1, X, Algo >*lucas< N-1, X, Algo > + lucas< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto lucas< 0, X, Algo > = all_one < 1, X, Algo >*2;
	template< is::floating X, typename Algo >	constexpr auto lucas< 1, X, Algo > = high_one< 1, X, Algo >;

	/// Shapiro polynomials of the first kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro1		  = Shapiro< N, X, Algo >(  1 );

	/// Shapiro polynomials of the second kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro2		  = Shapiro< N, X, Algo >( -1 );

	/// Touchard polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto touchard		  = Touchard< N, X, Algo >();

	/// Wilkinsons polynomial: https://en.wikipedia.org/wiki/Wilkinsons_polynomial
	template< typename Algo = horner_fma >
	constexpr auto wilkinsons		  = polynomial< Algo >( 
		  2'432'902'008'176'640'000.0L, -8752'948'036'761'600'000.0L, 13803'759'753'640'704'000.0L,
		-12'870'931'245'150'988'800.0L,  8037'811'822'645'051'776.0L, -3599'979'517'947'607'200.0L, 
		  1'206'647'803'780'373'360.0L,  -311'333'643'161'390'640.0L,    63'030'812'099'294'896.0L, 
		    -10'142'299'865'511'450.0L,     1'307'535'010'540'395.0L,      -135'585'182'899'530.0L, 
		         11'310'276'995'381.0L,          -756'111'184'500.0L,            40'171'771'630.0L, 
		             -1'672'280'820.0L,                53'327'946.0L,                -1'256'850.0L, 
		                     20'615.0L,                      -210.0L,                         1.0L
	);
}	// namespace pax
