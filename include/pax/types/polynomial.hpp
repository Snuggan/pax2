//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"
#include <span>
#include <array>
#include <vector>
#include <cmath>		// std::fma
#include <utility>		// std::forward


namespace pax {

	template< floating F >
	[[nodiscard]] constexpr F fma( const F m0_, const F m1_, const F a_ ) noexcept {
#if 	defined( __cpp_lib_constexpr_cmath )
		return std::fma( m0_, m1_, a_ );
#else
#		pragma GCC diagnostic push
#		pragma GCC diagnostic warning "-W#warnings"
#		warning( "std::fma is not constexpr in the std library used." )
#		pragma GCC diagnostic pop
		// Use std::fma during run-time and the naive implementation otherwise.
		// Can't use non-constexpr function at compile time.
		if consteval	{	return m0_*m1_ + a_;				}
		else			{	return std::fma( m0_, m1_, a_ );	}
#endif
	}


	/// Horner's iterative method with std::fma.
	struct horner_fma {
		template< floating F, std::size_t N, arithmetic A >
		[[nodiscard]] static constexpr F operator()(
			std::span< F, N >		p_,
			const A					x_
		) noexcept {
			if		constexpr( N == 0u ) 	return {};
			else if constexpr( N == 1u ) 	return p_[ 0 ];
			else if constexpr( N == 2u ) 	return fma( x_, p_[ 1 ], p_[ 0 ] );
			else if constexpr( N == 3u ) 	return fma( x_, fma( x_, p_[ 2 ], p_[ 1 ] ), p_[ 0 ] );
			else {
				using		V	  = std::remove_cv_t< F >;
				auto		itr	  = p_.end();
				V			res	  = *( --itr );
				while( itr != p_.begin() ) 
							res	  = fma( x_, res, *( --itr ) );
				return res;
			}
		}
	};
	constexpr std::array< double, 5 >	horner_test{{ 1.0, 2.0, 3.0, 4.0, 5.0 }};
	static_assert( horner_fma()( std::span< const double, 0 >{}, 						2.0 ) ==   0.0 );
	static_assert( horner_fma()( std::span< const double, 1 >{ horner_test.data(), 1 }, 2.0 ) ==   1.0 );
	static_assert( horner_fma()( std::span< const double, 2 >{ horner_test.data(), 2 }, 2.0 ) ==   5.0 );
	static_assert( horner_fma()( std::span< const double, 3 >{ horner_test.data(), 3 }, 2.0 ) ==  17.0 );
	static_assert( horner_fma()( std::span< const double, 4 >{ horner_test.data(), 4 }, 2.0 ) ==  49.0 );
	static_assert( horner_fma()( std::span< const double, 5 >{ horner_test.data(), 5 }, 2.0 ) == 129.0 );


	/// Compensated Horner using fma.
	/// It is similar to horner_fma, but more accurate and somewhat slower.
	struct comp_horner_fma {
		template< floating F, std::size_t N, arithmetic A >
		[[nodiscard]] static constexpr F operator()(
			std::span< F, N >		p_,
			const A					x_
		) noexcept {
			if		constexpr( N == 0u ) 	return {};
			else if constexpr( N == 1u ) 	return p_[ 0 ];
			else if constexpr( N == 2u ) 	return fma( x_, p_[ 1 ], p_[ 0 ] );
			else {
				using 		V     = std::remove_cv_t< F >;
				auto		itr   = p_.end();
				V			res   = *( --itr );
				V			cmp{};									// Compensation for rounding errors.
				while( itr != p_.begin() ) {
					const auto	tmp	  = res;
					res =	fma( x_, res, *( --itr ) );
					cmp+=	( tmp * x_ + *itr ) - ( res - cmp );	// Compensate for rounding.
				}
				return res + cmp;
			}
		}
	};
	static_assert( comp_horner_fma()( std::span< const double, 0 >{}, 						 2.0 ) ==   0.0 );
	static_assert( comp_horner_fma()( std::span< const double, 1 >{ horner_test.data(), 1 }, 2.0 ) ==   1.0 );
	static_assert( comp_horner_fma()( std::span< const double, 2 >{ horner_test.data(), 2 }, 2.0 ) ==   5.0 );
	static_assert( comp_horner_fma()( std::span< const double, 3 >{ horner_test.data(), 3 }, 2.0 ) ==  17.0 );
	static_assert( comp_horner_fma()( std::span< const double, 4 >{ horner_test.data(), 4 }, 2.0 ) ==  49.0 );
	static_assert( comp_horner_fma()( std::span< const double, 5 >{ horner_test.data(), 5 }, 2.0 ) == 129.0 );


	/// Implements a polynomial.
	template< typename Algo, floating F = double, std::size_t N = std::dynamic_extent >
	struct Polynomial : public std::array< F, N > {
		using algorithm		  = Algo;
		using base			  = std::array< F, N >;
		using value_type	  = std::remove_cv_t< F >;

		/// The polynomial's degree. An empty polynomial, as well as a constant, has zero degree. 
		[[nodiscard]] static constexpr std::size_t degree()					noexcept		{
			return N ? N - 1u : 0u;
		}

		/// Evaluate the polynomial at x_.
		template< arithmetic A >
		[[nodiscard]] constexpr value_type operator()( const A x_ )			const noexcept	{
			return Algo::operator()( std::span( *this ), x_ );
		}
	};

	template< typename Algo, floating F >
	struct Polynomial< Algo, F, std::dynamic_extent > : public std::vector< F > {
		using algorithm		  = Algo;
		using base			  = std::vector< F >;
		using value_type	  = std::remove_cv_t< F >;

		/// The polynomial's degree. An empty polynomial, as well as a constant, has zero degree. 
		[[nodiscard]] constexpr std::size_t degree()						const noexcept	{
			return base::empty() ? 0u : base::size() - 1u;
		}

		/// Evaluate the polynomial at x_.
		template< arithmetic A >
		[[nodiscard]] constexpr value_type operator()( const A x_ )			const noexcept	{
			return Algo::operator()( std::span( *this ), x_ );
		}
	};


	/// Returns a Polynomial< horner_fma, ... > of size N.
	template< typename... Args >
	[[nodiscard]] constexpr auto polynomial( Args && ...args_ ) 			noexcept		{
		using F = std::remove_cv_t< std::common_type_t< double, Args... > >;
		using P = Polynomial< horner_fma, F, sizeof...( Args ) >;
		return P({ static_cast< F >( std::forward< Args >( args_ ) )... });
	};
	static_assert( polynomial( 2, 1 )( 3.0 ) == 5.0 );

	/// Returns a Polynomial< horner_fma, double, ... > of dynamic size.
	[[nodiscard]] constexpr Polynomial< horner_fma > polynomial() 			noexcept		{	return {};	};
	static_assert( polynomial().degree() == 0u );


	/// Returns a Polynomial< comp_horner_fma, ... >.
	template< typename... Args >
	[[nodiscard]] constexpr auto polynomial_comp( Args && ...args_ )		noexcept		{
		using F = std::remove_cv_t< std::common_type_t< double, Args... > >;
		using P = Polynomial< horner_fma, F, sizeof...( Args ) >;
		return P({ static_cast< F >( std::forward< Args >( args_ ) )... });
	};
	static_assert( polynomial_comp( 2, 1 )[ 0 ] == 2.0 );

	/// Returns a Polynomial< comp_horner_fma, double, ... > of dynamic size.
	[[nodiscard]] constexpr Polynomial< comp_horner_fma > polynomial_comp()	noexcept		{	return {};	};
	static_assert( polynomial_comp().degree() == 0u );


	/// Stream the elements to out_.
	template< typename Out, typename Algo, floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Polynomial< Algo, F, N >	  & poly_ 
	) {
		std::size_t 						pow{};
		bool								first( true );
		for( auto item : poly_ ) {
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

}	// namespace pax
