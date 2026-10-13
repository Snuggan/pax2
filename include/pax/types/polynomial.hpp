//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


#include "base.hpp"
#include <span>
#include <array>
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
		[[nodiscard]] static constexpr auto operator()(
			std::span< F, N >						p_,
			const A									x_
		) noexcept {
			using 			V   = std::remove_cv_t< F >;
			auto			itr = p_.end();
			V				res = p_.empty() ? F{} : *( --itr );
			while( itr != p_.begin() ) 
							res = fma( x_, res, *( --itr ) );		// Fused multiply-add.
			return res;
		}
	};


	/// Compensated Horner using fma.
	/// It is similar to horner_fma, but more accurate and somewhat slower.
	struct compensated_horner_fma {
		template< floating F, std::size_t N, arithmetic A >
		[[nodiscard]] static constexpr auto operator()(
			std::span< F, N >						p_,
			const A									x_
		) noexcept {
			using 			V   = std::remove_cv_t< F >;
			auto			itr = p_.end();
			V				res = p_.empty() ? F{} : *( --itr );
			V				cmp{};									// Compensation for rounding errors.
			while( itr != p_.begin() ) {
				const auto	tmp = res;
				res =		fma( x_, res, *( --itr ) );				// Fused multiply-add.
				cmp+=		( tmp * x_ + *itr ) - ( res - cmp );	// Compensate for rounding.
			}
			return res + cmp;
		}
	};


	/// Implements a polynomial.
	template< typename Algo, floating F, std::size_t N >
	struct Polynomial : public std::array< F, N > {
		using algorithm		  = Algo;
		using value_type	  = std::remove_cv_t< F >;

		static constexpr std::size_t degree()									noexcept		{
			return ( N == 0u ) ? 0u : N - 1u;
		}

		/// Evaluate the polynomial at x_.
		[[nodiscard]] constexpr value_type operator()( const value_type x_ )	const noexcept	{
			return Algo::operator()( std::span( *this ), x_ );
		}
	};

	/// Returns a Polynomial< horner_fma, ... >.
	template< typename... Args >
	[[nodiscard]] constexpr auto polynomial( Args && ...args_ ) 				noexcept		{
		using F = std::remove_cv_t< std::common_type_t< double, Args... > >;
		using P = Polynomial< horner_fma, F, sizeof...( Args ) >;
		return P({ static_cast< F >( std::forward< Args >( args_ ) )... });
	};
	static_assert( polynomial( 2, 1 )( 3.0 ) == 5.0 );

	/// Returns a Polynomial< compensated_horner_fma, ... >.
	template< typename... Args >
	[[nodiscard]] constexpr auto polynomial_comp( Args && ...args_ )			noexcept		{
		using F = std::remove_cv_t< std::common_type_t< double, Args... > >;
		using P = Polynomial< horner_fma, F, sizeof...( Args ) >;
		return P({ static_cast< F >( std::forward< Args >( args_ ) )... });
	};
	static_assert( polynomial_comp( 2, 1 )[ 0 ] == 2.0 );


	/// Stream the elements to out_.
	template< typename Out, typename Algo, floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Polynomial< Algo, F, N >	  & poly_ 
	) {
		std::size_t 				pow{};
		bool						first( true );
		for( auto item : poly_ ) {
			if( item ) {
				if( !first ) {
					out_		 << ( ( item > 0 ) ? " + " : " - " );
					item		  = std::abs( item );
				}
				if( pow ) {
					if( item != 1 )	out_ << item;
					out_		 << 'x';
					if( pow > 1 )	out_ << '^' << pow;
				} else				out_ << item;
				first = false;
			}
			++pow;
		}
		if( first )					out_ << 0;
		return out_;
	}

}	// namespace pax
