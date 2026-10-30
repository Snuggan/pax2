//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "polynomial.hpp"


namespace pax {
	namespace is {
		template< std::size_t N, std::size_t ...NN >
		constexpr std::size_t newN = ( true && ... && ( NN != std::dynamic_extent ) ) ? N : std::dynamic_extent;
	}


	/// This is a wrapper for Polynomial, to be used with polynomials where every second element is zero. 
	/// It works similar to Polynomial, but is faster as it ignore those zero-valued elements. It does not give 
	/// you access to its innards, you need to convert it back to Polynomial for that (with ´.polynomial()´)
	template< typename Algo, is::floating F, std::size_t N >
	class Polynomial_x2 {
		static constexpr std::size_t 	extent	= ( N == std::dynamic_extent ) ? std::dynamic_extent : N/2 + 1;
		using 							P0		= Polynomial< Algo, F, N >;
		using 							P		= Polynomial< Algo, F, extent >;
		P								m_poly{};
		bool							m_preed{};
	
	public:
		constexpr Polynomial_x2()										noexcept	= default;
		constexpr Polynomial_x2( const Polynomial_x2 & )				noexcept	= default;
		constexpr Polynomial_x2( Polynomial_x2 && )						noexcept	= default;
		constexpr Polynomial_x2 & operator=( const Polynomial_x2 & )	noexcept	= default;
		constexpr Polynomial_x2 & operator=( Polynomial_x2 && )			noexcept	= default;
	
		/// Constructor, from a "normal" polynomial.
		constexpr Polynomial_x2( const P0 & p0_ ) 					{	*this = p0_;			}
	
		/// Operator =, of a "normal" polynomial.
		constexpr Polynomial_x2 & operator=( const P0 & p0_ ) {
			m_poly		  = P{};
			m_preed		  = bool( p0_.size() && !p0_.front() );
			if constexpr( N == std::dynamic_extent )	m_poly.resize( p0_.size()/2 + !m_preed );
			bool on		  = m_preed;
			auto ptr	  = m_poly.data();
			for( const auto e : p0_ ) {
				if(( on = !on ))		*( ptr++ ) = e;
				else if( e != 0 )		throw "Polynomial_x2: Every second term should be zero, but isn't.";
			}
			return *this;
		}
	
		/// Return a "normal" polynomial (with every second term a zero).
		constexpr auto polynomial()										const noexcept	{
			P0 res{};
			if constexpr( N == std::dynamic_extent )	res.resize( m_poly.size()*2 + m_preed - 1 );
			auto ptr	  = res.data() + m_preed;
			for( const auto e : m_poly ) {
				*ptr	  = e;
				ptr		 += 2;
			}
			return res;
		}
	
	
		/// Evaluate the polynomial at x_, using the given algorithm Algo.
		[[nodiscard]] constexpr F operator()( const F x_ )				const noexcept	{
			return Algo::operator()( std::span( m_poly ), x_ * x_ ) * ( m_preed ? x_ : F( 1 ) );
		}
	
		/// The polynomial's degree. Polynomials with zero or one item have degrre 0. 
		[[nodiscard]] constexpr std::size_t degree()					const noexcept	{
			return 2*m_poly.degree() + m_preed;
		}

		/// Stream the elements of a polynomial to out_.
		template< typename Out >
		friend Out & operator<<( Out & out_, const Polynomial_x2 & poly_ ) {
			return out_ << poly_.polynomial();
		}
	};


	/// Stream the elements of sap_ to out_ as id sp_ was a polynomial of x.
	template< typename Out, is::floating F, std::size_t N >
	Out & as_polynomial(
		Out								  & out_, 
		const std::span< F, N >				sp_ 
	) {
		std::size_t 						pow{};
		bool								first( true );
		for( auto item : sp_ ) {
			if( item ) {
				if( !first ) {
					out_						 << ( ( item > 0 ) ? " + " : " - " );
					item				  = std::abs( item );
				}
				if( pow ) {
					if( item != 1 )			out_ << item;
					out_						 << 'x';
					if( pow > 1 )			out_ << '^' << pow;
				} else						out_ << item;
				first = false;
			}
			++pow;
		}
		if( first )							out_ << 0;
		return out_;
	}

	/// Stream the elements of a polynomial to out_.
	template< typename Out, typename Algo, is::floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Polynomial< Algo, F, N >	  & poly_ 
	) {
		return as_polynomial( out_, std::span( poly_ ) );
	}


	template< typename Algo, typename X, std::size_t N0, std::size_t N >
	using Poly = Polynomial< Algo, std::decay_t< X >, ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N >;


	template< typename type0, typename type1 >
	struct polymerge;

	template< typename Algo, is::floating X0, std::size_t N0, 
			  				 is::floating X1, std::size_t N1 >
	struct polymerge< Polynomial< Algo, X0, N0 >, Polynomial< Algo, X1, N1 > > {
		using				X	  = std::decay_t< std::common_type_t< X0, X1 > >;
		using 				type  = Polynomial< Algo, X, std::max( N0, N1 ) >;
	};

	template< typename type0, typename type1 >
	using polymerge_t = polymerge< type0, type1 >::type;



	template< bool Addition, typename Algo, is::floating X0, std::size_t N0, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto addsub(
		const Polynomial< Algo, X0, N0 > & p0_,
		const Polynomial< Algo, X1, N1 > & p1_
	) noexcept {
		polymerge_t< std::decay_t< decltype( p0_ ) >, std::decay_t< decltype( p1_ ) > >	p{};
		if constexpr( p.extent == std::dynamic_extent )	p.resize( std::max( p0_.size(), p1_.size() ) );

		auto 								itr	  = p.begin();
		std::copy( p0_.begin(), p0_.end(), itr );
		if constexpr( Addition )			for( const auto x1 : p1_ )	*( itr++ ) += x1;
		else								for( const auto x1 : p1_ )	*( itr++ ) -= x1;
		return p;
	}

	/// Addition.
	template< typename Algo, typename X0, std::size_t N0, typename X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator+(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const Polynomial< Algo, X1, N1 >	  & p1_
	) noexcept {	return addsub< true >( p0_, p1_ );			}

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		Polynomial< Algo, std::decay_t< X0 >, N0 >	res( p0_.data(), p0_.size() );
		for( auto & x : res )				x += x0_;
		return res;
	}

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ + x0_;							}

	/// Subtraction.
	template< typename Algo, typename X0, std::size_t N0, typename X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const Polynomial< Algo, X1, N1 >	  & p1_
	) noexcept {	return addsub< false >( p0_, p1_ );			}

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ + -x0_;							}

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {
		Polynomial< Algo, std::decay_t< X0 >, N0 >	res( p0_.data(), p0_.size() );
		for( auto & x : res )				x = x0_ - x;
		return res;
	}


	/// Multiplication.
	template< typename Algo, typename X0, std::size_t N0, typename X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator*(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const Polynomial< Algo, X1, N1 >	  & p1_
	) noexcept {
		using									X	= std::decay_t< std::common_type_t< X0, X1 > >;
		static constexpr std::size_t 			N	= is::newN< N0 + !N0 + N1 + !N1 - 1, N0, N1 >;
		Polynomial< Algo, X, N >				p{};
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

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		auto									res = polynomial< Algo, N0 >( p0_.data(), p0_.size() );
		for( auto & x : res )					x *= x0_;
		return res;
	}

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ * x0_;							}

	/// Division.
	/// "A Generalization of Synthetic Division and A General Theorem of Division of Polynomials", by Lianghuo Fan.
	template< typename Algo, typename X0, std::size_t N0, typename X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator/(
		const Polynomial< Algo, X0, N0 >  & p0_,
		const Polynomial< Algo, X1, N1 >  & p1_
	) noexcept {
		using								X	  = std::decay_t< std::common_type_t< X0, X1 > >;
		constexpr std::size_t 				N	  = is::newN< N0 - N1 + 1, N0, N1 >;
		const std::size_t					sz0	  = p0_.degree();
		const std::size_t					sz1	  = p1_.degree();
		const std::size_t					sz    = sz0 - sz1;
		Polynomial< Algo, X, N >			q{};
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

	template< typename Algo, typename X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator/(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		auto									res = polynomial< Algo, N0 >( p0_.data(), p0_.size() );
		for( auto & x : res )					x /= x0_;
		return res;
	}


	/// Derivate a polynomial.
	template< typename Algo, typename X, std::size_t N0 >
	[[nodiscard]] constexpr auto derivate( const Polynomial< Algo, X, N0 > & p0_ ) noexcept {
		static constexpr std::size_t 			N = ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N0 ? N0 - 1 : 0;
		auto									res = Polynomial< Algo, X, N >{};
		if constexpr( N == std::dynamic_extent )
			if( p0_.size() )					res.resize( p0_.size() - 1 );

		std::size_t 							i = 0;
		auto									src = p0_.data();
		for( auto & x : res )					x = *( ++src ) * ++i;
		return res;
	}

	/// Integrate a polynomial.
	template< typename Algo, typename X, std::size_t N0 >
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
		for( auto & x : p0_ )					*( ++dest )	  = x/++i;
		return res;
	}



	template< typename Poly, typename X, std::size_t N >
		requires( N != std::dynamic_extent )
	struct Polyiter {
	private:
		template< std::size_t M >			using pp = Polynomial< typename Poly::algorithm, X, M >;
		static constexpr pp< N   >			p1 = Polyiter< Poly, X, N-1 >::poly;
		static constexpr pp< N-1 >			p2 = Polyiter< Poly, X, N-2 >::poly;

		// The general case.
		template< std::size_t L, std::size_t M >
		static consteval pp< std::max( N+L-1, N+M-2 ) > calc( const pp< L > & c1, const pp< M > & c2 ) noexcept {
			return p1*c1 + p2*c2;
		}

		// Special case for when every second coefficient is zero.
		static consteval pp< N + 1 > calc( const X c1, const X c2 ) noexcept {
			static constexpr std::size_t 	start  = 2 - ( N % 2 );
			pp< N+1 >						p0{};
			p0[ N ]						 =	p1[ N-1 ]*c1;
			p0[ 0 ]						 =	p2[ 0   ]*c2;
			for( std::size_t i=start; i<N-1; i+=2 )
				p0[ i ]					 =	p1[ i-1 ]*c1 + p2[ i ]*c2;
			return p0;
		}

	public:
		static constexpr auto poly		 = calc( Poly::template c1< N >, Poly::template c2< N > )/Poly::template divis < N >;
	};

	template< typename Poly, typename X >
	struct Polyiter< Poly, X, 0 > {	static constexpr auto poly = Poly::poly0;	};

	template< typename Poly, typename X >
	struct Polyiter< Poly, X, 1 > {	static constexpr auto poly = Poly::poly1;	};



	template< typename Algo, typename X, std::size_t N, std::size_t a >
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

	/// Touchard Polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, std::size_t a, typename X = double, typename Algo = horner_fma >
	constexpr auto abel				  = Abel< Algo, X, N, a >();
	


	template< typename X, typename Algo = horner_fma >
	struct Bessel {
		using 							algorithm = Algo;
		template< std::size_t N >		static constexpr auto c1 =  Polynomial< Algo, X, 2 >({ 0, 2*N - 1 });
		template< std::size_t N >		static constexpr auto c2 =  Polynomial< Algo, X, 1 >({ 1 });
		template< std::size_t N >		static constexpr X divis  =  1;
		static constexpr auto			poly0 = Polynomial< Algo, X, 1 >({ 1 });
		static constexpr auto			poly1 = Polynomial< Algo, X, 2 >({ 1, 1 });
	};

	/// Bessel Polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto bessel			  = Polyiter< Bessel< X, Algo >, X, N >::poly;


	template< typename X, typename Algo = horner_fma >
	struct Bessel_reversed : Bessel< X, Algo > {
		template< std::size_t N >		static constexpr auto c1 =  Polynomial< Algo, X, 1 >({ 2*N - 1 });
		template< std::size_t N >		static constexpr auto c2 =  Polynomial< Algo, X, 3 >({ 0, 0, 1 });
	};

	/// Reverse Bessel Polynomials: https://en.wikipedia.org/wiki/Bessel_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto bessel_reversed	  = Polyiter< Bessel_reversed< X, Algo >, X, N >::poly;


	template< typename X, typename Algo = horner_fma >
	struct Chebyshev1 {
		using 							algorithm = Algo;
		template< std::size_t N >		static constexpr X c1 =  2;
		template< std::size_t N >		static constexpr X c2 = -1;
		template< std::size_t N >		static constexpr X divis  =  1;
		static constexpr auto			poly0 = Polynomial< Algo, X, 1 >({ 1 });
		static constexpr auto			poly1 = Polynomial< Algo, X, 2 >({ 0, 1 });
	};

	/// Chebyshev Polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto chebyshev1		  = Polyiter< Chebyshev1< X, Algo >, X, N >::poly;

	/// Chebyshev Polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto chebyshev1_x2	  = Polynomial_x2( chebyshev1< N, X, Algo > );

	template< typename X, typename Algo = horner_fma >
	struct Chebyshev2 : Chebyshev1< X, Algo > {
		static constexpr auto			poly1 = Polynomial< Algo, X, 2 >({ 0, 2 });
	};

	/// Chebyshev Polynomials of the second kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto chebyshev2		  = Polyiter< Chebyshev2< X, Algo >, X, N >::poly;

	/// Chebyshev Polynomials of the first kind: https://en.wikipedia.org/wiki/Chebyshev_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto chebyshev2_x2	  = Polynomial_x2( chebyshev2< N, X, Algo > );


	template< typename X, typename Algo = horner_fma >
	struct Fibonacci : Chebyshev1< X, Algo > {
		template< std::size_t N >		static constexpr X c1 =  1;
		template< std::size_t N >		static constexpr X c2 =  1;
	};

	/// Fibonacci Polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto fibonacci		  = Polyiter< Fibonacci< X, Algo >, X, N-1 >::poly;
	template< typename X = double, typename Algo = horner_fma >
	constexpr auto fibonacci< 0, X, Algo >  = Polynomial< Algo, X, 1 >({ 0 });

	/// Fibonacci Polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto fibonacci_x2		  = Polynomial_x2( fibonacci< N, X, Algo > );


	template< typename X, typename Algo = horner_fma >
	struct Hermite1 : Fibonacci< X, Algo > {
		template< std::size_t N >		static constexpr X c2 = X( 1 ) - N;
	};
	
	/// Pprobabilist's Hermite Polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto hermite1			  = Polyiter< Hermite1< X, Algo >, X, N >::poly;

	/// Pprobabilist's Hermite Polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto hermite1_x2		  = Polynomial_x2( hermite1< N, X, Algo > );


	template< typename X, typename Algo = horner_fma >
	struct Hermite2 : Chebyshev2< X, Algo > {
		template< std::size_t N >		static constexpr X c2 = X( 2 ) - 2*N;
	};

	/// Physicist's Hermite Polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto hermite2			  = Polyiter< Hermite2< X, Algo >, X, N >::poly;

	/// Physicist's Hermite Polynomials: https://en.wikipedia.org/wiki/Hermite_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto hermite2_x2		  = Polynomial_x2( hermite2< N, X, Algo > );


	template< typename X, X a, X b, typename Algo = horner_fma >
	struct Jacobi {
		using 							algorithm = Algo;
		template< std::size_t N >		static constexpr X n2ab  =  2*N + a + b;
		template< std::size_t N >		static constexpr auto c1 =  
											Polynomial< Algo, X, 2 >({ a*a-b*b, n2ab<N>*(n2ab<N>-2) })*(n2ab<N>-1);
		template< std::size_t N >		static constexpr auto c2 =  
											Polynomial< Algo, X, 1 >({ -2*(N+a-1)*(N+b-1)*n2ab<N> });
		template< std::size_t N >		static constexpr X divis  = 2*N*(N+a+b)*(n2ab<N>-2);
		static constexpr auto			poly0 = Polynomial< Algo, X, 1 >({ 1 });
		static constexpr auto			poly1 = Polynomial< Algo, X, 2 >({ a - (a+b)*0.5, (a+b)*0.5 + 1 });
	};

	/// Jacobi Polynomials: https://en.wikipedia.org/wiki/Jacobi_Polynomials
	template< std::size_t N, typename X, X a, X b, typename Algo = horner_fma >
	constexpr auto jacobi			  = Polyiter< Jacobi< X, a, b, Algo >, X, N >::poly;


	template< typename X, X a, typename Algo = horner_fma >
	struct Laguerre_generalized {
		using 							algorithm = Algo;
		template< std::size_t N >		static constexpr auto c1 = Polynomial< Algo, X, 2 >({ 2*N - 1 + a, -1 });
		template< std::size_t N >		static constexpr auto c2 = Polynomial< Algo, X, 1 >({ -a - N + 1 });
		template< std::size_t N >		static constexpr X divis = N;
		static constexpr auto			poly0 = Polynomial< Algo, X, 1 >({ 1 });
		static constexpr auto			poly1 = Polynomial< Algo, X, 2 >({ 1 + a, -1 });
	};

	/// Generalized Laguerre Polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, typename X, X a, typename Algo = horner_fma >
	constexpr auto laguerre_generalized = Polyiter< Laguerre_generalized< X, a, Algo >, X, N >::poly;


	template< typename X, typename Algo = horner_fma >
	struct Laguerre : Laguerre_generalized< X, 0.0, Algo > {};

	/// Laguerre Polynomials: https://en.wikipedia.org/wiki/Laguerre_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto laguerre			  = Polyiter< Laguerre< X, Algo >, X, N >::poly;


	template< typename X, typename Algo = horner_fma >
	struct Legendre : Chebyshev1< X, Algo > {
		template< std::size_t N >		static constexpr X c1 =  X( 2*N - 1 );
		template< std::size_t N >		static constexpr X c2 = -X(   N - 1 );
		template< std::size_t N >		static constexpr X divis  =  N;
	};

	/// Legendre Polynomials: https://en.wikipedia.org/wiki/Legendre_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto legendre			  = Polyiter< Legendre< X, Algo >, X, N >::poly;

	/// Legendre Polynomials: https://en.wikipedia.org/wiki/Legendre_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto legendre_x2		  = Polynomial_x2( legendre< N, X, Algo > );


	template< typename X, typename Algo = horner_fma >
	struct Lucas : Fibonacci< X, Algo > {
		static constexpr auto			poly0 = Polynomial< Algo, X, 1 >({ 2 });
	};

	/// Lucas Polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto lucas			  = Polyiter< Lucas< X, Algo >, X, N >::poly;

	/// Lucas Polynomials: https://en.wikipedia.org/wiki/Fibonacci_Polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto lucas_x2			  = Polynomial_x2( lucas< N, X, Algo > );


	template< typename Algo, typename X, std::size_t N >
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

	/// Touchard Polynomials: https://en.wikipedia.org/wiki/Touchard_polynomials
	template< std::size_t N, typename X = double, typename Algo = horner_fma >
	constexpr auto touchard			  = Touchard< Algo, X, N >();


	template< typename X = long double, typename Algo = horner_fma >
	constexpr auto wilkinsons	  = Polynomial< Algo, X, 21 >{ 
		  2'432'902'008'176'640'000.0L, -8752'948'036'761'600'000.0L, 13803'759'753'640'704'000.0L,
		-12'870'931'245'150'988'800.0L,  8037'811'822'645'051'776.0L, -3599'979'517'947'607'200.0L, 
		  1'206'647'803'780'373'360.0L,  -311'333'643'161'390'640.0L,    63'030'812'099'294'896.0L, 
		    -10'142'299'865'511'450.0L,     1'307'535'010'540'395.0L,      -135'585'182'899'530.0L, 
		         11'310'276'995'381.0L,          -756'111'184'500.0L,            40'171'771'630.0L, 
		             -1'672'280'820.0L,                53'327'946.0L,                -1'256'850.0L, 
		                     20'615.0L,                      -210.0L,                         1.0L
	};
}	// namespace pax
