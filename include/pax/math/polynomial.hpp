// Implements a polynomial class with comparison, arithmetic operators, and various support functions.
// - For static sizes (`N != dynamic_extent`) it is all `constexpr`, so `Polynomial` can be used/initialised during compilation.
// - Everything is noexcept.
// - You can use the Horner template for evaluating polynomials, but other templates or structs may be used. 
// - The Horner template implements three variants. horner_fma_even and horner_fma_odd igonre every 2nd term for better 
//   efficiency. horner_fma use all coefficients. 
// - The template is simple and does not have other constructors than those in the base templates: std::vector for 
//   N == std::dynamic_extent and std::array otherwise. Use the ´polynomial(...)´ functions instead.
// - The full functionality of the base templates is available.
// - This file polynomials-named.hpp implements a number of named polynomials, such as Bessel, Chebyshev, Hermite...
//
//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include <span>
#include <array>
#include <vector>
#include <algorithm>	// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <cmath>		// std::fma
#include <format>


// Todo
// - Check what happens when shifting, multiplying, or dividing by a polynull.


namespace pax {
	namespace poly {
		template< typename ... F >	concept floating	= ( std::floating_point < std::decay_t< F > > && ... );
		template< typename ... A >	concept arithmetic	= ( std::is_arithmetic_v< std::decay_t< A > > && ... );
		
		template< typename A >	struct what_all_coeffs;
		template< typename A >	struct what_even_coeffs;
		template< typename A >	struct what_odd_coeffs;

		template< typename A >	constexpr bool is_all_coeffs			=	std::is_same_v< A, typename what_all_coeffs< A >::type >;
		template< typename A >	constexpr bool is_even_coeffs			=	is_all_coeffs< A > || std::is_same_v< A, typename what_even_coeffs< A >::type >;
		template< typename A >	constexpr bool is_odd_coeffs			=	is_all_coeffs< A > || std::is_same_v< A, typename what_odd_coeffs< A >::type >;
		template< typename A >	constexpr bool is_every_2nd_coeff_v		=	1 == is_even_coeffs< A > + is_odd_coeffs< A >;

		/// Change odd for even and vice versa.
		template< std::size_t n, typename Algo >
		class flip_even_and_odd {
			static_assert( n != std::dynamic_extent, "You need to rethink..." );
			template< bool, bool >	struct meta;
			template< typename Meta, int i >			struct flip		{	using type = what_all_coeffs < Algo >::type;	};
			template< int i >	struct flip< meta< true, false >, i >	{	using type = what_even_coeffs< Algo >::type;	};
			template< int i >	struct flip< meta< false, true >, i >	{	using type = what_odd_coeffs < Algo >::type;	};
		public:
			using type   = flip< std::conditional_t< n & 1,	meta< is_odd_coeffs< Algo >, is_even_coeffs< Algo > >, 
															meta< is_even_coeffs< Algo >, is_odd_coeffs< Algo > > >, 42 >::type;
		};
		template< std::size_t n, typename Algo >	using flip_even_and_odd_t = flip_even_and_odd< n, Algo >::type;
	}

	template< poly::floating F >
	[[nodiscard]] constexpr F fma( const F m0_, const F m1_, const F a_ ) noexcept {
#		if defined( __cpp_lib_constexpr_cmath )	// Can use std::fma at compile time.
			return std::fma( m0_, m1_, a_ );
#		else									// Can't use std::fma at compile time.
#			warning( "std::fma is not constexpr in the std library used." )
			// Use std::fma during run-time and the naive implementation otherwise.
			if consteval	{	return			 m0_ * m1_ + a_;	}
			else			{	return std::fma( m0_,  m1_,  a_ );	}
#		endif
	}

	/// Horner's iterative method using std::fma. Specify if even or odd coeffs only – or both.
	template< bool even_coeffs, bool odd_coeffs >	requires( even_coeffs || odd_coeffs )
	class Horner {
		static  constexpr bool odd_coeffs_only	  = !even_coeffs &&  odd_coeffs;
		static  constexpr bool every_2nd_coeff	  = ( even_coeffs + odd_coeffs == 1 );

		template< std::size_t N, poly::floating F, std::size_t N0 >
		static constexpr F quick( const std::span< F, N0 > p_, const auto x_ ) noexcept {
			if constexpr( N <= 1+every_2nd_coeff+odd_coeffs_only )	return p_[ N0-1 ];
			else return fma( x_, quick< N-1-every_2nd_coeff >( p_, x_ ), p_[ N0-N+odd_coeffs_only ] );
		}

	public:
		template< poly::floating F, std::size_t N >
		[[nodiscard]] static constexpr F operator()(
			const std::span< F, N >			p_,
			const std::type_identity_t< F >	x_
		) noexcept {
			std::decay_t< F >		res;
			const auto xx		  = every_2nd_coeff ? x_*x_ : x_;	// Optimisation when enery 2nd coeff is zero.
			if 		constexpr( N <= 0+odd_coeffs_only )	return {};
			else if constexpr( N != std::dynamic_extent ) {
				static constexpr auto	NN = N - ( every_2nd_coeff && ( ( N & 1u ) == odd_coeffs_only ) );
				res				  = quick< NN >( std::span< F, NN >{ p_.data(), NN }, xx );
			} else {												// If every_2nd_coeff, p_.end() might be out of sync...
				auto	itr		  = p_.end() - ( every_2nd_coeff && ( ( p_.size() & 1u ) == odd_coeffs_only ) );
				res				  = ( p_.begin() == itr ) ? 0 : *( --itr );
				while( itr > p_.begin() + odd_coeffs_only ) {
					if constexpr( every_2nd_coeff )	 --itr;			// Optimisation when enery 2nd coeff is zero.
					res			  =	fma( xx, res, *( --itr ) );
				}
			}
			return odd_coeffs_only ? res*x_ : res;
		}
	};

	using horner_fma	  = Horner<  true,  true >;		///< Quick Horner algorithm using fma, if available.
	using horner_fma_even = Horner<  true, false >;		///< Quicker, use this when odd coeffs may be ignored.
	using horner_fma_odd  = Horner< false,  true >;		///< Quicker, use this when even coeffs may be ignored.

	namespace poly {
		template< bool E, bool O >	struct what_all_coeffs	< Horner< E, O > >	{	using type = horner_fma;		};
		template< bool E, bool O >	struct what_even_coeffs < Horner< E, O > >	{	using type = horner_fma_even;	};
		template< bool E, bool O >	struct what_odd_coeffs	< Horner< E, O > >	{	using type = horner_fma_odd;	};
	}


	/// Implements a polynomial that uses Algo to calculate the polynomial for a given x.
	template< typename Algo = horner_fma, poly::floating F = double, std::size_t N = std::dynamic_extent >
	struct Polynomial : public std::conditional_t< N == std::dynamic_extent,
		std::vector< std::decay_t< F > >, std::array< std::decay_t< F >, N > >
	{
		static constexpr std::size_t extent	  = N;			///< Could be std::dynamic_extent.
		static constexpr bool is_static		  = extent != std::dynamic_extent;
		using algorithm						  = Algo;		///< Algorithm used to calculate a value at x.
		static constexpr bool every_2nd_coeff = poly::is_every_2nd_coeff_v< algorithm >;

		/// Evaluate the polynomial at x_, using the algorithm Algo.
		[[nodiscard]] constexpr F operator()( const F x_ )									const noexcept	{
			return Algo::operator()( std::span< const F, N >( *this ), x_ );
		}

		/// Compare two polynomials for equality.
		template< typename Algo1, poly::floating F1, std::size_t N1 >
		[[nodiscard]] constexpr bool operator==( const Polynomial< Algo1, F1, N1 > & o_ ) 	const noexcept	{
			const std::size_t 		deg0{ this->degree() }, deg1{ o_.degree() };
			return  ( deg0 || deg1 )
				?	std::ranges::equal(	this->begin(), this->begin() + deg0 + !this->empty(), 
										o_.   begin(), o_.   begin() + deg1 + !   o_.empty() )
				:	( this->empty() ? F{} : this->front() ) == ( o_.empty() ? F1{} : o_.front() );
		}

		/// Lexicographically compare two polynomials. The comparison starts with the highest degree items. 
		template< typename Algo1, poly::floating F1, std::size_t N1 >
		[[nodiscard]] constexpr auto operator<=>( const Polynomial< Algo1, F1, N1 > & o_ ) 	const noexcept	{
			const std::size_t 		deg0{ this->degree() }, deg1{ o_.degree() };
			return  ( deg0 || deg1 )
				?	std::lexicographical_compare_three_way(	this->begin(),	this->begin() + deg0 + !this->empty(), 
															o_.   begin(),	o_.   begin() + deg1 + !   o_.empty() )
				:	( this->empty() ? F{} : this->front() ) <=> ( o_.empty() ? F1{} : o_.front() );
		}

		/// The polynomial's degree. Polynomials with zero or one item have degrre 0. 
		[[nodiscard]] constexpr std::size_t degree()										const noexcept	{
			auto	 itr  = this->end();
			while( ( itr != this->begin() ) && !*( --itr ) );
			return   itr -  this->begin();
		}
	};	


	/// An empty polynomial, same as all coefficients are zero.
	template< poly::floating X = double >
	constexpr auto polynull  = Polynomial< horner_fma_even, X, 0 >{};

	/// Returns a Polynomial< Algo, ... > of size sizeof...( Args ) from a bunch of arithmetic values.
	template< typename Algo = horner_fma, poly::floating min_type = float, poly::arithmetic... Args >
	[[nodiscard]] constexpr auto polynomial( Args && ...args_ ) 							noexcept		{
		static constexpr std::size_t N = sizeof...( Args );
		using F = std::decay_t< std::common_type_t< min_type, Args... > >;
		using P = Polynomial< Algo, F, N ? N : std::dynamic_extent >;
		return P{{ static_cast< F >( std::forward< Args >( args_ ) )... }};
	}

	/// Returns a Polynomial< Algo, ... > of size sp_.size() with the contents of sp_.
	template< typename Algo = horner_fma, poly::floating F, std::size_t N >
	[[nodiscard]] constexpr auto polynomial( const std::span< F, N > sp_ ) 					noexcept		{
		auto res = Polynomial< Algo, std::decay_t< F >, N >{};
		if constexpr( N == std::dynamic_extent )		res.resize( sp_.size() );
		std::ranges::copy_n( sp_.data(), sp_.size(), res.data() );
		return res;
	}

	/// Returns a Polynomial< Algo, ... > of statical size N with the contents of arr_.
	template< typename Algo = horner_fma, std::size_t N, poly::floating F >
	[[nodiscard]] constexpr auto polynomial( F( & arr_ )[ N ] ) 							noexcept		{
		return polynomial< Algo >( std::span< const F, N >{ arr_, N } );
	}



} namespace std {
	template< typename Algo, pax::poly::floating F, std::size_t N >
	struct formatter< pax::Polynomial< Algo, F, N > > : formatter< string > {
		constexpr auto format( const pax::Polynomial< Algo, F, N > & sp_, format_context & ctx ) const {
			string					res{};
			res.reserve( sp_.size()*( pax::poly::is_every_2nd_coeff_v< Algo > ? 6 : 10 ) );
			size_t 					pow{};
			for( auto item : sp_ ) {
				if( item ) {
					if( res.size() ) {
						res		 += ( ( item > 0 ) ? " + " : " - " );
						item	  = abs( item );
					}
					if( pow == 0 || item != 1 )	res += std::format( "{}",   item );
					if( pow == 1 )				res += 'x';
					else if( pow > 1 )			res += std::format( "x^{}", pow );
				}
				++pow;
			}
			return formatter< string >::format( res.empty() ? std::format( "{}", F{} ) : res, ctx );
		}
	};

} namespace pax {

	/// Stream the elements of a polynomial to out_.
	template< typename Out, typename Algo, poly::floating F, std::size_t N >
	Out & operator<<(
		Out									  & out_, 
		const Polynomial< Algo, F, N >		  & poly_ 
	) {	return out_ << std::format( "{}", poly_ );							}

	/// Stream the elements of sp_ to out_ as id sp_ was a polynomial of x.
	template< typename Out, poly::floating F, std::size_t N >
	Out & as_polynomial(
		Out									  & out_, 
		const std::span< F, N >					sp_ 
	) {	return out_ << std::format( "{}", polynomial( sp_ ) );				}


	namespace poly {
		template< bool Addition, typename Algo0, poly::floating X0, std::size_t N0, 
								 typename Algo1, poly::floating X1, std::size_t N1 >
		[[nodiscard]] constexpr auto do_add_sub(
			const Polynomial< Algo0, X0, N0 >	  & p0_,
			const Polynomial< Algo1, X1, N1 >	  & p1_
		) noexcept {
			// If Alg0 and Algo1 are the same, use it.
			using Algo = std::conditional_t< std::is_same_v< Algo0, Algo1 >, Algo0, typename what_all_coeffs< Algo0 >::type >;
			Polynomial< Algo, std::decay_t< std::common_type_t< X0, X1 > >, std::max( N0, N1 ) >	p{};
			if constexpr( !p.is_static )			p.resize( std::max( p0_.size(), p1_.size() ) );

			auto 									itr = p.begin();
			std::copy( p0_.begin(), p0_.end(), itr );
			if constexpr( Addition )				for( const auto x1 : p1_ )	*( itr++ ) += x1;
			else									for( const auto x1 : p1_ )	*( itr++ ) -= x1;
			return p;
		}

		template<	typename Algo0, poly::floating X0, std::size_t N0, 
					typename Algo1, poly::floating X1, std::size_t N1, std::size_t N >
		class merge_mul_div { 
			template< bool, bool >	struct meta;
			template< typename Meta0, typename Meta1 >			struct mul_div	{	using type = what_all_coeffs < Algo0 >::type;	};
			template< bool A >	struct mul_div< meta< A, !A >, meta<  A, !A > >	{	using type = what_even_coeffs< Algo0 >::type;	};
			template< bool A >	struct mul_div< meta< A, !A >, meta< !A,  A > >	{	using type = what_odd_coeffs < Algo0 >::type;	};
			using mul_div_t   = mul_div<	meta< is_even_coeffs< Algo0 >, is_odd_coeffs< Algo0 > >, 
											meta< is_even_coeffs< Algo1 >, is_odd_coeffs< Algo1 > > >::type;
		public:
			using type		  = Polynomial<	mul_div_t, std::decay_t< std::common_type_t< X0, X1 > >, 
				 							( N0 != std::dynamic_extent && N1 != std::dynamic_extent ) ? N : std::dynamic_extent >;	
		};
	}

	/// Addition.
	template< typename Algo0, poly::floating X0, std::size_t N0, 
			  typename Algo1, poly::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator+(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {	return poly::do_add_sub< true >( p0_, p1_ );	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator+=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )			   x += x0_;
		return p0_;
	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ += x0_;							}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator+(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ + x0_;							}

	/// Subtraction.
	template< typename Algo0, poly::floating X0, std::size_t N0, 
			  typename Algo1, poly::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {	return poly::do_add_sub< false >( p0_, p1_ );	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator-=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ += -x0_;							}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const Polynomial< Algo, X0, N0 >	  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ + -x0_;							}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator-(
		const std::type_identity_t< X0 >		x0_,
		Polynomial< Algo, X0, N0 >				p0_
	) noexcept {
		for( auto & x : p0_ )				x = x0_ - x;
		return p0_;
	}

	/// Same as (but quicker) multiplying by x^n.
	template< int n, typename Algo, poly::floating X, std::size_t N0 >
	[[nodiscard]] constexpr auto shift( const Polynomial< Algo, X, N0 > & p_ )	noexcept	{
		static constexpr std::size_t 		N = ( N0 == std::dynamic_extent ) ? N0 : std::max< int >( int( N0 ) + n, 0 );
		using Algo2						  = poly::flip_even_and_odd_t< ( n<0 ) ? -n : n, Algo >;
		Polynomial< Algo2, X, N >			res{};
		if constexpr( !res.is_static )		res.resize( std::max( p_.size() + n, 0 ) );
		if constexpr( n >= 0 )				std::copy( p_.begin(),     p_.end(), res.begin() + n );
		else if( p_.size() > -n )			std::copy( p_.begin() - n, p_.end(), res.begin()     );
		return res;
	}
	template< int n, typename Algo, poly::floating X >
	[[nodiscard]] constexpr auto shift( const Polynomial< Algo, X, 0 > ) 		noexcept	{	return polynull< X >;		}

	/// Multiplication.
	template< typename Algo0, poly::floating X0, std::size_t N0, 
			  typename Algo1, poly::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator*(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept -> typename poly::merge_mul_div< Algo0, X0, N0, Algo1, X1, N1, N0 + !N0 + N1 + !N1 - 1 >::type {
		if( poly::is_every_2nd_coeff_v< Algo1 > && ( !poly::is_every_2nd_coeff_v< Algo0 > || ( p1_.size() < p0_.size() ) ) )
			return p1_*p0_;						// It is more advantageous to do it in reverse order.
		else {
			typename poly::merge_mul_div< Algo0, X0, N0, Algo1, X1, N1, N0 + !N0 + N1 + !N1 - 1 >::type		p{};
			if constexpr( !p.is_static )		p.resize( p0_.size() + p1_.size() - 1 );
			auto 								itr = p.begin();
			for( const auto c0 : p0_ ) {
				if( c0 ) {						// Skip zoro-valued coeffs.
					for( const auto c1 : p1_ ) {
						*itr = fma( c0, c1, *itr );
						++itr;
					}
					itr -= p1_.size() - 1;
				} else	++itr;
			}
			return p;
		}
	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator*=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )					x *= x0_;
		return p0_;
	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ *= x0_;							}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator*(
		const std::type_identity_t< X0 >		x0_,
		const Polynomial< Algo, X0, N0 >	  & p0_
	) noexcept {	return p0_ * x0_;							}

	/// Division.
	/// "A Generalization of Synthetic Division and A General Theorem of Division of Polynomials", by Lianghuo Fan.
	template< typename Algo0, poly::floating X0, std::size_t N0, 
			  typename Algo1, poly::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator/(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {
		const std::size_t				d0	  = p0_.degree();
		const std::size_t				d1	  = p1_.degree();
		const std::size_t				d	  = d0 - d1;
		typename poly::merge_mul_div< Algo0, X0, N0, Algo1, X1, N1, N0 - N1 + 1 >::type	q{};
		if constexpr( !q.is_static )			q.resize( d + 1 );
		q[ d ]								  = p0_[ d0 ] / p1_[ d1 ];
		for( std::size_t i=d; i>0; ) {
			--i;
			q[ i ]							  = p0_[ d1 + i ];
			const std::size_t 			stop  = ( ( d < d1 ) || ( d0-i <= d1+d1 ) ) ? d-i : d1;
			for( std::size_t j=1; j<=stop; ++j ) 
				q[ i ] -= q[ i + j ]*p1_[ d1 - j ];
			q[ i ] 							 /= p1_[ d1 ];
		}
		return q;
	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator/=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )			   x /= x0_;
		return p0_;
	}

	template< typename Algo, poly::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto operator/(
		Polynomial< Algo, X0, N0 >				p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {	return p0_ /= x0_;							}


	/// Derivate a polynomial.
	template< typename Algo, poly::floating X, std::size_t N0 >
	[[nodiscard]] constexpr auto derivate( const Polynomial< Algo, X, N0 > & p0_ ) noexcept {
		static constexpr std::size_t 			N	  = ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N0 ? N0 - 1 : 0;
		auto									res	  = Polynomial< poly::flip_even_and_odd_t< 1, Algo >, X, N >{};
		if constexpr( N == std::dynamic_extent )
			if( p0_.size() )					res.resize( p0_.size() - 1 );

		std::size_t 							i	  = 0;
		auto									src	  = p0_.data();
		for( auto & x : res )					x	  = *( ++src ) * ++i;
		return res;
	}

	/// Integrate a polynomial.
	template< typename Algo, poly::floating X, std::size_t N0 >
	[[nodiscard]] constexpr auto integrate(
		const Polynomial< Algo, X, N0 >		  & p0_, 
		const std::type_identity_t< X >			x0_	  = 0
	) noexcept {
		static constexpr std::size_t 			N	  = ( N0 == std::dynamic_extent ) ? std::dynamic_extent : N0+1;
		auto									res	  = Polynomial< poly::flip_even_and_odd_t< 1, Algo >, X, N >{};
		if constexpr( N == std::dynamic_extent )res.resize( p0_.size() + 1 );

		res.front()									  = x0_;
		std::size_t 							i	  = 0;
		auto									dest  = res.data();
		for( auto & x : p0_ )					*( ++dest ) = x/++i;
		return res;
	}
	template< typename Algo, poly::floating X >
	[[nodiscard]] constexpr auto integrate(
		const Polynomial< Algo, X, 0 >, 
		const std::type_identity_t< X >		  = 0
	) noexcept {
		return polynull< X >;
	}

}	// namespace pax

namespace pax::poly::test {
#if !defined( NDEBUG )
	static constexpr const auto p  = polynomial( 1.0, 2, 3, 4, 5 );

	namespace poly::horner_ {
		static constexpr const auto sp = std::span{ p };
		static_assert( sp.first< 1 >().extent == 1 );
		static_assert( sp.first( 1 )  .extent == std::dynamic_extent );
	
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
		using pax::polynull;

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
		static_assert( polynull<> + p == p );
		static_assert( p + polynull<> == p );
		static_assert( polynomial( 0 ) + p == p );
		static_assert( p + polynomial( 0 ) == p );
		static_assert( p + p == polynomial( 2, 4, 6, 8, 10 ) );
		static_assert( p + 1 == polynomial( 2, 3, 4, 5,  6 ) );
		static_assert( 1 + p == polynomial( 2, 3, 4, 5,  6 ) );
	}
	namespace poly::subtraction_ {
		static_assert( polynull<> - p == 0 - p );
		static_assert( p - polynull<> == p );
		static_assert( p - polynomial( 0.0 ) == p );
		static_assert( p - p + p == p );
		static_assert( p - 1 == polynomial( 0,  1,  2,  3,  4 ) );
		static_assert( 1 - p == polynomial( 0, -1, -2, -3, -4 ) );
	}
	namespace poly::shift_ {
		static_assert( shift<  3 >( p ) == polynomial( 0.0, 0.0, 0.0, 1.0, 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift<  3 >( p ).extent == 8 );
		static_assert( shift<  0 >( p ) == p );
		static_assert( shift<  0 >( p ).extent == p.extent );
		static_assert( shift< -1 >( p ) == polynomial( 2.0, 3.0, 4.0, 5.0 ) );
		static_assert( shift< -1 >( p ).extent == 4 );
		static_assert( shift< -3 >( p ) == polynomial( 4.0, 5.0 ) );
		static_assert( shift< -3 >( p ).extent == 2 );
		static_assert( shift< -9 >( p ) == pax::polynull<> );
		static_assert( shift< -9 >( p ).extent == 0 );

		static_assert( shift<  3 >( polynull<> ).extent == 0 );

		// Check handling of odd/even.
		constexpr auto even = polynomial< horner_fma_even >( 1.0, 0.0, 3.0, 0.0, 5.0 );
		static_assert( even( 1 ) == 9 );
		static_assert( shift< 1 >( even ) == polynomial( 0.0, 1.0, 0.0, 3.0, 0.0, 5.0 ) );
		static_assert( shift< 1 >( even )( 1 ) == 9 );
		static_assert( shift< 2 >( even ) == polynomial( 0.0, 0.0, 1.0, 0.0, 3.0, 0.0, 5.0 ) );
		static_assert( shift< 2 >( even )( 1 ) == 9 );
	}

	constexpr auto even3 = polynomial< horner_fma_even >( 1.0, 0, 3 );
	constexpr auto even5 = polynomial< horner_fma_even >( 1.0, 0, 3, 0, 5 );
	constexpr auto even6 = even5 * even3;
	constexpr auto odd4  = polynomial< horner_fma_odd  >( 0.0, 2, 0, 4 );
	constexpr auto odd6  = polynomial< horner_fma_odd  >( 0.0, 2, 0, 4, 0, 6 );
	constexpr auto odd7  = odd6 * even3;
	constexpr auto odd8  = odd4 *  odd6;
	namespace poly::multiplication_ {
		static_assert( p * polynull<> == polynull<> );
		static_assert( polynull<> * p == polynull<> );
		static_assert( p * polynomial( 1.0 ) == p );
		static_assert( p * polynomial( 1.0, 2 ) == polynomial( 1.0, 4, 7, 10, 13, 10 ) );
		static_assert( p * 2 == polynomial( 2, 4, 6, 8, 10 ) );
		static_assert( 2 * p == polynomial( 2, 4, 6, 8, 10 ) );

		namespace {	// Preservation of even/odd.
			constexpr auto even_mul = even5 * even5;
			static_assert( even_mul.every_2nd_coeff );
			static_assert( even_mul( 2 ) != 0 );

			constexpr auto odd_mul = odd6 * odd6;
			static_assert( odd_mul.every_2nd_coeff );
			static_assert( odd_mul( 2 ) != 0 );

			constexpr auto odd_even_mul = odd6 * even5;
			static_assert( odd_even_mul.every_2nd_coeff );
			static_assert( odd_even_mul( 2 ) != 0 );
		}
	}
	namespace poly::division_ {
		constexpr auto p2			  = polynomial( 1.0, 2, 5 );
		constexpr auto pp			  = p * p2;
		// static_assert( pp / polynull<> == polynull<> );
		// static_assert( polynull<> / pp == polynull<> );
		static_assert( pp / polynomial( 1.0 ) == pp );
		static_assert( pp / p2 == p );
		static_assert( p / 2 == polynomial( 0.5, 1, 1.5, 2, 2.5 ) );

		namespace {	// Preservation of even/odd.
			constexpr auto even_div = even6 / even5;
			static_assert( even_div.every_2nd_coeff );
			static_assert( even_div( 2 ) != 0 );
			static_assert( even_div( 2 ) == even3( 2 ) );
			static_assert( even_div == even3 );

			constexpr auto odd_div = odd8 / odd6;
			static_assert( odd_div.every_2nd_coeff );
			static_assert( odd_div( 2 ) != 0 );
			static_assert( odd_div( 2 ) == odd4( 2 ) );
			static_assert( odd_div == odd4 );

			constexpr auto odd_even_div = odd7 / even3;
			static_assert( odd_even_div.every_2nd_coeff );
			static_assert( odd_even_div( 2 ) != 0 );
			static_assert( odd_even_div( 2 ) == odd6( 2 ) );
			static_assert( odd_even_div == odd6 );
		}
	}
	namespace poly::derivation {
		constexpr auto pp0	  = Polynomial< horner_fma, double, 0 >();
		static_assert( pp0 == derivate( pp0 ) );
		static_assert( derivate( polynull<> ) == polynull<> );
		static_assert( derivate( polynull<> ).extent == 0 );

		constexpr auto pp	  = derivate( p );
		static_assert( pp.extent == p.extent - 1 );
		static_assert( pp == polynomial( p[ 1 ]*1.0, p[ 2 ]*2.0, p[ 3 ]*3.0, p[ 4 ]*4.0 ) );
	}
	namespace poly::integration {
		static_assert( integrate( p, 1.5 ) == polynomial( 1.5, 1, 1, 1, 1, 1 ) );
		static_assert( derivate( integrate( p ) ) == p );
		static_assert( integrate( polynull<> ) == polynull<> );
		static_assert( integrate( polynull<> ).extent == 0 );
	}
#endif
}	// namespace pax::poly::test
