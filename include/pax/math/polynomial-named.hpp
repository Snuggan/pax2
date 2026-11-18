//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include "polynomial-extras.hpp"


namespace pax {

	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_0  = Polynomial< Algo, X, 0 >{};
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_1  = polynomial< Algo, X >( 1 );
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_01 = polynomial< Algo, X >( 0, 1 );
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_11 = polynomial< Algo, X >( 1, 1 );

	namespace detail {
		template< std::size_t N >
		using even_or_odd = Horner< ( N & 1u ) ? Horners::fma_even : Horners::fma_odd >;

		template< typename Algo, is::floating X, std::size_t N, std::size_t a >
		constexpr auto Abel() {
			if constexpr( N == 0 )			return polynomial_1< Algo, X >;
			else {
				Polynomial< Algo, X, N+1 >	res{};
				res[ 1 ]				  = 1;
				for( std::size_t j=2; j<N+1; ++j )
					for( std::size_t i=j; i>0; --i )
						res[ i ]		  = res[ i - 1 ] - a*N*res[ i ];
				return res;
			}
		};

		template< std::size_t N, is::floating X, X alpha, X beta, typename Algo = horner_fma >
		constexpr auto Jacobi() noexcept {
			if      constexpr( N == 0 )		return polynomial_1< Algo, X >;
			else if constexpr( N == 1 )		return polynomial< Algo, X >( alpha - (alpha+beta)*0.5, (alpha+beta)*0.5 + 1 );
			else {
				static constexpr X	a = N + alpha, b = N + beta, c = a + b;
				return ( Jacobi< N-1, X, alpha, beta, Algo >() *(c-1)*polynomial< Algo, X >( (alpha-beta)*(c-2*N), c*(c-2) )
					   - Jacobi< N-2, X, alpha, beta, Algo >() *2*(a-1)*(b-1)*c )/( 2*(c-2)*N*(c-N) );
			}
		}

		template< bool Alternative, typename Algo, is::floating X, std::size_t N >
		constexpr auto Shapiro() {
			constexpr std::size_t 			Sz = std::size_t( 1u ) << N;
			if constexpr( N == 0 )			return polynomial_0< Algo, X >;
			else if constexpr( N == 1 )		return polynomial< Algo, X >( 1, Alternative ? -1 : 1 );
			else {
				auto res		  = Polynomial< Algo, X, Sz >();
				res[ 0 ]		  = res[ 1 ] = res[ 2 ] = 1;
				res[ 3 ]		  = -1;
				std::size_t half  = 2u;
				while( ( half *= 2 ) < Sz ) {
					std::copy( res.begin(), res.begin() + half/2u, res.begin() + half );
					auto ptr0 = res.begin()       + half/2u;
					auto ptr2 = res.begin()+ half + half/2u;
					while( ptr0 < res.begin() + half )	*( ptr2++ ) = -*( ptr0++ );
				}
				if constexpr( Alternative )
					for( auto ptr = res.begin() + Sz/2; ptr < res.end(); ++ptr )	*ptr = -*ptr;
				return res;
			}
		};

		template< typename Algo, is::floating X, std::size_t N >
		constexpr auto Touchard() {
			if      constexpr( N == 0 )		return polynomial_0 < Algo, X >;
			else if constexpr( N == 1 )		return polynomial_01< Algo, X >;
			else {
				constexpr auto temp	  = Touchard< Algo, X, N-1 >();
				return polynomial_01< Algo, X >*( temp + derivate( temp ) );
			}
		};
	}	// namespace detail


	/// Touchard polynomials: https://en.wikipedia.org/wiki/Abel_polynomials
	template< std::size_t N, std::size_t a, is::floating X = double, typename Algo = horner_fma >
	constexpr auto abel			  = detail::Abel< Algo, X, N, a >();
	
	/// Bessel polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel		  = polynomial< Algo, X >( 0, 2*N - 1 )*bessel< N-1, X, Algo > + bessel< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 1, X, Algo >  = polynomial_11< Algo, X >;

	/// Bessel reversed polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel_reversed = polynomial< Algo, X >( 2*N - 1 )*bessel_reversed< N-1, X, Algo > 
									  + polynomial< Algo, X >( 0, 0, 1 )*bessel_reversed< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 1, X, Algo >  = polynomial_11< Algo, X >;

	/// Chebyshev polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev1	  = 2*polynomial_01< Algo, X >*chebyshev1< N-1, X, Algo > - chebyshev1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Chebyshev polynomials of the second kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev2	  = 2*polynomial_01< Algo, X >*chebyshev2< N-1, X, Algo > - chebyshev2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 1, X, Algo >  = polynomial_01< Algo, X >*2;

	/// Dickson polynomials of the first kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson1		  = polynomial_01< Algo, X >*dickson1< N-1, a, X, Algo > - a*dickson1< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 0, a, X, Algo >  = polynomial_0 < Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 1, a, X, Algo >  = polynomial_01< Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 2, a, X, Algo >  = polynomial   < Algo, X >( -2.0*a, 0, 1 );

	/// Dickson polynomials of the seconnd kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson2		  = polynomial_01< Algo, X >*dickson2< N-1, a, X, Algo > - a*dickson2< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 0, a, X, Algo >  = polynomial_1 < Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 1, a, X, Algo >  = polynomial_01< Algo, X >;

	/// Fibonacci polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N > >
	constexpr auto fibonacci	  = polynomial_01< Algo, X >*fibonacci< N-1, X, Algo > + fibonacci< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 0, X, Algo >  = polynomial_0 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 1, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 2, X, Algo >  = polynomial_01< Algo, X >;

	/// Probabilist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite1		  = polynomial_01< Algo, X >*hermite1< N-1, X, Algo > - (N-1)*hermite1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Physicist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite2		  = 2*polynomial_01< Algo, X >*hermite2< N-1, X, Algo > - 2*(N-1)*hermite2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 1, X, Algo >  = polynomial_01< Algo, X >*2;

	/// Jacobi polynomials: https://en.wikipedia.org/wiki/Jacobi_Polynomials
	template< std::size_t N, is::floating X, X a, X b, typename Algo = horner_fma >
	constexpr auto jacobi		  = detail::Jacobi< N, X, a, b, Algo >();
	template< is::floating X, X a, X b, typename Algo >	constexpr auto jacobi< 0, X, a, b, Algo >  = polynomial_1< Algo, X >;
	template< is::floating X, X a, X b, typename Algo >	constexpr auto jacobi< 1, X, a, b, Algo >  = polynomial  < Algo, X >( a - (a+b)*0.5, (a+b)*0.5 + 1 );

	/// Generalized Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, is::floating X, X a, typename Algo = horner_fma >
	constexpr auto laguerre_generalized	  = ( polynomial< Algo, X >( 2*N-1+a, -1 )*laguerre_generalized< N-1, X, a, Algo > 
											- (N-1+a)*laguerre_generalized< N-2, X, a, Algo > )/N;
	template< is::floating X, X a, typename Algo >	constexpr auto laguerre_generalized< 0, X, a, Algo >  = polynomial_1< Algo, X >;
	template< is::floating X, X a, typename Algo >	constexpr auto laguerre_generalized< 1, X, a, Algo >  = polynomial  < Algo, X >( 1 + a, -1 );

	/// Laguerre polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto laguerre		  = laguerre_generalized< N, X, X{}, Algo >;

	/// Legendre polynomials: https://en.wikipedia.org/wiki/Legendre_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto legendre		  = ( polynomial< Algo, X >( 0, 2*N-1 )*legendre< N-1, X, Algo > 
										- (N-1)*legendre< N-2, X, Algo > )/N;
	template< is::floating X, typename Algo >	constexpr auto legendre< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto legendre< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Lucas polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto lucas		  = polynomial_01< Algo, X >*lucas< N-1, X, Algo > + lucas< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto lucas< 0, X, Algo >  = polynomial_1 < Algo, X >*2;
	template< is::floating X, typename Algo >	constexpr auto lucas< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Shapiro polynomials of the first kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro1		  = detail::Shapiro< false, Algo, X, N >();

	/// Shapiro polynomials of the second kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro2		  = detail::Shapiro<  true, Algo, X, N >();

	/// Touchard polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto touchard		  = detail::Touchard< Algo, X, N >();

	/// Wilkinsons polynomial: https://en.wikipedia.org/wiki/Wilkinsons_polynomial
	template< is::floating X = long double, typename Algo = horner_fma >
	constexpr auto wilkinsons		  = polynomial< Algo, X >( 
		  2'432'902'008'176'640'000.0L, -8752'948'036'761'600'000.0L, 13803'759'753'640'704'000.0L,
		-12'870'931'245'150'988'800.0L,  8037'811'822'645'051'776.0L, -3599'979'517'947'607'200.0L, 
		  1'206'647'803'780'373'360.0L,  -311'333'643'161'390'640.0L,    63'030'812'099'294'896.0L, 
		    -10'142'299'865'511'450.0L,     1'307'535'010'540'395.0L,      -135'585'182'899'530.0L, 
		         11'310'276'995'381.0L,          -756'111'184'500.0L,            40'171'771'630.0L, 
		             -1'672'280'820.0L,                53'327'946.0L,                -1'256'850.0L, 
		                     20'615.0L,                      -210.0L,                         1.0L
	);
}	// namespace pax
