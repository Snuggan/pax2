//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include "polynomial.hpp"

#include <format>


namespace std {
	template< typename Algo, typename F, std::size_t N >
	struct formatter< pax::Polynomial< Algo, F, N > > : formatter< string > {
		auto format(  pax::Polynomial< Algo, F, N > sp_, format_context & ctx ) const {
			string					res{};
			size_t 					pow{};
			bool					first( true );
			for( auto item : sp_ ) {
				if( item ) {
					if( !first ) {
						res				 += ( ( item > 0 ) ? " + " : " - " );
						item			  = abs( item );
					}
					if( item == 1 ) switch( pow ) {
						case 0:		res	  = std::format( "{}{}",	 res, item );		break;
						case 1:		res	  = std::format( "{}x",		 res );				break;
						default:	res	  = std::format( "{}x^{}",	 res, pow );		break;
					} else switch( pow ) {
						case 0:		res	  = std::format( "{}{}",	 res, item );		break;
						case 1:		res	  = std::format( "{}{}x",	 res, item );		break;
						default:	res	  = std::format( "{}{}x^{}", res, item, pow );	break;
					}
					first				  = false;
				}
				++pow;
			}
			return formatter< string >::format( !first ? res : std::format( "{}", 0.0 ), ctx );
		}
	};
}
namespace pax {
	namespace is {
		template< std::size_t N, std::size_t ...NN >
		constexpr std::size_t newN = ( true && ... && ( NN != std::dynamic_extent ) ) ? N : std::dynamic_extent;

		template< typename A0, typename A1 >	struct Merge_algo		{	using type = horner_fma;		};
		template< typename A > struct Merge_algo< A, A >				{	using type = A;					};
		template< typename A0, typename A1 >	using  Merge_algo_t		=	typename Merge_algo< A0, A1 >::type;
	}


	/// Stream the elements of sp_ to out_ as id sp_ was a polynomial of x.
	template< typename Out, is::floating F, std::size_t N >
	Out & as_polynomial(
		Out								  & out_, 
		const std::span< F, N >				sp_ 
	) {	return out_ << std::format( "{}", polynomial( sp_ ) );				}

	/// Stream the elements of a polynomial to out_.
	template< typename Out, typename Algo, is::floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Polynomial< Algo, F, N >	  & poly_ 
	) {	return out_ << std::format( "{}", poly_ );							}


	template< bool Addition, typename Algo0, is::floating X0, std::size_t N0, 
							 typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto addsub(
		const Polynomial< Algo0, X0, N0 > & p0_,
		const Polynomial< Algo1, X1, N1 > & p1_
	) noexcept {
		using						X	  = std::decay_t< std::common_type_t< X0, X1 > >;
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, X, std::max( N0, N1 ) >	p{};
		if constexpr( p.extent == std::dynamic_extent )	p.resize( std::max( p0_.size(), p1_.size() ) );

		auto 						itr	  = p.begin();
		std::copy( p0_.begin(), p0_.end(), itr );
		if constexpr( Addition )	for( const auto x1 : p1_ )	*( itr++ ) += x1;
		else						for( const auto x1 : p1_ )	*( itr++ ) -= x1;
		return p;
	}

	/// Addition.
	template< typename Algo0, is::floating X0, std::size_t N0, 
			  typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator+(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {	return addsub< true >( p0_, p1_ );			}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator+=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )			   x += x0_;
		return p0_;
	}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ += x0_;							}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ + x0_;							}

	/// Subtraction.
	template< typename Algo0, is::floating X0, std::size_t N0, 
			  typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {	return addsub< false >( p0_, p1_ );			}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator-=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ += -x0_;							}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ + -x0_;							}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const std::type_identity_t< X0 >		x0_,
		Polynomial< Algo, X0, N0 >				p0_
	) noexcept {
		for( auto & x : p0_ )				x = x0_ - x;
		return p0_;
	}

	/// Multiplication.
	template< typename Algo0, is::floating X0, std::size_t N0, 
			  typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator*(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {
		using									X	= std::decay_t< std::common_type_t< X0, X1 > >;
		static constexpr std::size_t 			N	= is::newN< N0 + !N0 + N1 + !N1 - 1, N0, N1 >;
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, X, N >	p{};
		if constexpr( N == std::dynamic_extent ) p.resize( p0_.size() + p1_.size() - 1 );

		auto 									itr = p.begin();
		for( const auto c0 : p0_ ) {
			for( const auto c1 : p1_ ) {
				*itr = fma( c0, c1, *itr );
				++itr;
			}
			itr -= p1_.size() - 1;
		}
		return p;
	}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator*=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )			   x *= x0_;
		return p0_;
	}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ *= x0_;							}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ * x0_;							}

	/// Division.
	/// "A Generalization of Synthetic Division and A General Theorem of Division of Polynomials", by Lianghuo Fan.
	template< typename Algo0, is::floating X0, std::size_t N0, 
			  typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator/(
		const Polynomial< Algo0, X0, N0 >  & p0_,
		const Polynomial< Algo1, X1, N1 >  & p1_
	) noexcept {
		using								X	  = std::decay_t< std::common_type_t< X0, X1 > >;
		constexpr std::size_t 				N	  = is::newN< N0 - N1 + 1, N0, N1 >;
		const std::size_t					sz0	  = p0_.degree();
		const std::size_t					sz1	  = p1_.degree();
		const std::size_t					sz    = sz0 - sz1;
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, X, N >	q{};
		if constexpr( N == std::dynamic_extent )	q.resize( sz + 1 );

		q[ sz ] 								  = p0_[ sz0 ] / p1_[ sz1 ];
		for( std::size_t i=sz; i>0; ) {
			--i;
			q[ i ]								  = p0_[ sz1 + i ];
			const std::size_t 				stop  = ( ( sz < sz1 ) || ( sz0-i <= sz1+sz1 ) ) ? sz-i : sz1;
			for( std::size_t j=1; j<=stop; ++j ) 
				q[ i ] -= q[ i + j ]*p1_[ sz1 - j ];
			q[ i ] 								 /= p1_[ sz1 ];
		}
		return q;
	}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator/=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )			   x /= x0_;
		return p0_;
	}

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator/(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ /= x0_;							}


	/// Derivate a polynomial.
	template< typename Algo, is::floating X, std::size_t N0 >
	[[nodiscard]] constexpr auto derivate( const Polynomial< Algo, X, N0 > & p0_ ) noexcept {
		static constexpr std::size_t 			N	  = ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N0 ? N0 - 1 : 0;
		auto									res	  = Polynomial< Algo, X, N >{};
		if constexpr( N == std::dynamic_extent )
			if( p0_.size() )					res.resize( p0_.size() - 1 );

		std::size_t 							i	  = 0;
		auto									src	  = p0_.data();
		for( auto & x : res )					x	  = *( ++src ) * ++i;
		return res;
	}

	/// Integrate a polynomial.
	template< typename Algo, is::floating X, std::size_t N0 >
	[[nodiscard]] constexpr auto integrate(
		const Polynomial< Algo, X, N0 >		  & p0_, 
		const std::type_identity_t< X >			x0_	  = 0
	) noexcept {
		static constexpr std::size_t 			N	  = ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N0+1;
		auto									res	  = Polynomial< Algo, X, N >{};
		if constexpr( N == std::dynamic_extent )res.resize( p0_.size() + 1 );

		res.front()									  = x0_;
		std::size_t 							i	  = 0;
		auto									dest  = res.data();
		for( auto & x : p0_ )					*( ++dest ) = x/++i;
		return res;
	}


	namespace detail {
		template< std::size_t N >
		using even_or_odd = Horner< ( N & 1u ) ? Horners::fma_even : Horners::fma_odd >;

		template< typename Algo, is::floating X, std::size_t N, std::size_t a >
		constexpr auto Abel() {
			if constexpr( N == 0 )			return polynomial< Algo, X >( 1 );
			else {
				Polynomial< Algo, X, N + 1 >	res{};
				res[ 1 ] = 1;
				for( std::size_t j=2; j<N+1; ++j )
					for( std::size_t i=j; i>0; --i )
						res[ i ] = res[ i - 1 ] - a*N*res[ i ];
				return res;
			}
		};

		template< std::size_t N, is::floating X, X alpha, X beta, typename Algo = horner_fma >
		constexpr auto Jacobi() noexcept {
			if      constexpr( N == 0 )		return polynomial< Algo, X >( 1 );
			else if constexpr( N == 1 )		return polynomial< Algo, X >( alpha - (alpha+beta)*0.5, (alpha+beta)*0.5 + 1 );
			else {
				static constexpr X	a = N + alpha, b = N + beta, c = a + b;
				return ( Jacobi< N-1, X, alpha, beta, Algo >() *(c-1)*polynomial< Algo, X >( (a-b)*(c-2*N), c*(c-2) )
					   - Jacobi< N-2, X, alpha, beta, Algo >() *2*(a-1)*(b-1)*c )/( 2*N*(c-N)*(c-2) );
			}
		}

		template< bool Alternative, typename Algo, is::floating X, std::size_t N >
		constexpr auto Shapiro() {
			constexpr std::size_t 			Sz = std::size_t( 1u ) << N;
			if constexpr( N == 0 )			return polynomial< Algo, X >( 0 );
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
			if constexpr( N == 0 )			return polynomial< Algo, X >( 0 );
			else if constexpr( N == 1 )		return polynomial< Algo, X >( 0, 1 );
			else {
				auto previous			  = Touchard< Algo, X, N-1 >();
				previous				  = previous + derivate( previous );
				Polynomial< Algo, X, N+1 >	res{};
				std::copy( previous.begin(), previous.end(), res.begin() + 1 );
				return res;
			}
		};
	}


	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_0  = Polynomial< Algo, X, 0 >{};
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_1  = polynomial< Algo, X >( 1 );
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_01 = polynomial< Algo, X >( 0, 1 );
	template< typename Algo = horner_fma, is::floating X = double >	constexpr auto polynomial_11 = polynomial< Algo, X >( 1, 1 );


	/// Touchard polynomials: https://en.wikipedia.org/wiki/Abel_polynomials
	template< std::size_t N, std::size_t a, is::floating X = double, typename Algo = horner_fma >
	constexpr auto abel				  = detail::Abel< Algo, X, N, a >();
	
	/// Bessel polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel			  = polynomial< Algo, X >( 0, 2*N - 1 )*bessel< N-1, X, Algo > + bessel< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto bessel< 1, X, Algo >  = polynomial_11< Algo, X >;

	/// Bessel reversed polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto bessel_reversed	  = polynomial< Algo, X >( 2*N - 1 )*bessel_reversed< N-1, X, Algo > 
									  + polynomial< Algo, X >( 0, 0, 1 )*bessel_reversed< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto bessel_reversed< 1, X, Algo >  = polynomial_11< Algo, X >;

	/// Chebyshev polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev1		  = 2*polynomial_01< Algo, X >*chebyshev1< N-1, X, Algo > - chebyshev1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev1< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Chebyshev polynomials of the second kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto chebyshev2		  = 2*polynomial_01< Algo, X >*chebyshev2< N-1, X, Algo > - chebyshev2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto chebyshev2< 1, X, Algo >  = polynomial_01< Algo, X >*2;

	/// Dickson polynomials of the first kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson1			  = polynomial_01< Algo, X >*dickson1< N-1, a, X, Algo > - a*dickson1< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 0, a, X, Algo >  = polynomial_0 < Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 1, a, X, Algo >  = polynomial_01< Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson1< 2, a, X, Algo >  = polynomial   < Algo, X >( -2.0*a, 0, 1 );

	/// Dickson polynomials of the seconnd kind: https://en.wikipedia.org/wiki/Dickson_polynomial
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, double a = 1, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto dickson2			  = polynomial_01< Algo, X >*dickson2< N-1, a, X, Algo > - a*dickson2< N-2, a, X, Algo >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 0, a, X, Algo >  = polynomial_1 < Algo, X >;
	template< double a, is::floating X, typename Algo >	constexpr auto dickson2< 1, a, X, Algo >  = polynomial_01< Algo, X >;

	/// Fibonacci polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N > >
	constexpr auto fibonacci		  = polynomial_01< Algo, X >*fibonacci< N-1, X, Algo > + fibonacci< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 0, X, Algo >  = polynomial_0 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 1, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto fibonacci< 2, X, Algo >  = polynomial_01< Algo, X >;

	/// Probabilist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite1			  = polynomial_01< Algo, X >*hermite1< N-1, X, Algo > - (N-1)*hermite1< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto hermite1< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Physicist's Hermite polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto hermite2			  = 2*polynomial_01< Algo, X >*hermite2< N-1, X, Algo > - 2*(N-1)*hermite2< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto hermite2< 1, X, Algo >  = polynomial_01< Algo, X >*2;

	/// Jacobi polynomials: https://en.wikipedia.org/wiki/Jacobi_Polynomials
	template< std::size_t N, is::floating X, X a, X b, typename Algo = horner_fma >
	constexpr auto jacobi			  = detail::Jacobi< N, X, a, b, Algo >();
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
	constexpr auto laguerre			  = laguerre_generalized< N, X, X{}, Algo >;

	/// Legendre polynomials: https://en.wikipedia.org/wiki/Legendre_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N + 1 > >
	constexpr auto legendre			  = ( polynomial< Algo, X >( 0, 2*N-1 )*legendre< N-1, X, Algo > 
										- (N-1)*legendre< N-2, X, Algo > )/N;
	template< is::floating X, typename Algo >	constexpr auto legendre< 0, X, Algo >  = polynomial_1 < Algo, X >;
	template< is::floating X, typename Algo >	constexpr auto legendre< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Lucas polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	/// Uses quicker horner algorithm that ignores the zero valued coefficients.
	template< std::size_t N, is::floating X = double, typename Algo = detail::even_or_odd< N+1 > >
	constexpr auto lucas			  = polynomial_01< Algo, X >*lucas< N-1, X, Algo > + lucas< N-2, X, Algo >;
	template< is::floating X, typename Algo >	constexpr auto lucas< 0, X, Algo >  = polynomial_1 < Algo, X >*2;
	template< is::floating X, typename Algo >	constexpr auto lucas< 1, X, Algo >  = polynomial_01< Algo, X >;

	/// Shapiro polynomials of the first kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro1			  = detail::Shapiro< false, Algo, X, N >();

	/// Shapiro polynomials of the second kind: https://en.wikipedia.org/wiki/Shapiro_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto shapiro2			  = detail::Shapiro<  true, Algo, X, N >();

	/// Touchard polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, is::floating X = double, typename Algo = horner_fma >
	constexpr auto touchard			  = detail::Touchard< Algo, X, N >();

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
