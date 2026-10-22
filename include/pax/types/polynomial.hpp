//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"
#include <span>
#include <array>
#include <vector>
#include <algorithm>	// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <cmath>		// std::fma
#include <utility>		// std::forward


namespace pax {

	template< floating F >
	[[nodiscard]] constexpr F fma( const F m0_, const F m1_, const F a_ ) noexcept {
#		if defined( __cpp_lib_constexpr_cmath )
			// Can use std::fma at compile time.
			return std::fma( m0_, m1_, a_ );
#		else
			// Can't use std::fma at compile time.
#			warning( "std::fma is not constexpr in the std library used." )

			// Use std::fma during run-time and the naive implementation otherwise.
			if consteval	{	return m0_*m1_ + a_;				}
			else			{	return std::fma( m0_, m1_, a_ );	}
#		endif
	}


	/// Horner's iterative method with std::fma.
	struct horner_fma {
		template< floating F, std::size_t N >
		[[nodiscard]] static constexpr F operator()(
			std::span< F, N >				p_,
			const std::type_identity_t< F >	x_
		) noexcept {
			if		constexpr( N == 0u ) 	return {};
			else if constexpr( N == 1u ) 	return p_[ 0 ];
			else if constexpr( N == 2u ) 	return fma( x_, p_[ 1 ], p_[ 0 ] );
			else if constexpr( N == 3u ) 	return fma( x_, fma( x_, p_[ 2 ], p_[ 1 ] ), p_[ 0 ] );
			else {
				auto		itr	  = p_.end();
				auto		res	  = p_.empty() ? 0 : *( --itr );
				while( itr != p_.begin() ) 
							res	  = fma( x_, res, *( --itr ) );
				return res;
			}
		}
	};

	/// Compensated Horner using fma.
	/// It is similar to horner_fma, but more accurate and somewhat slower.
	struct comp_horner_fma {
		template< floating F, std::size_t N >
		[[nodiscard]] static constexpr F operator()(
			std::span< F, N >				p_,
			const std::type_identity_t< F >	x_
		) noexcept {
			if		constexpr( N == 0u ) 	return {};
			else if constexpr( N == 1u ) 	return p_[ 0 ];
			else if constexpr( N == 2u ) 	return fma( x_, p_[ 1 ], p_[ 0 ] );
			else {
				using 		V     = std::remove_cv_t< F >;
				auto		itr   = p_.end();
				auto		res	  = p_.empty() ? 0 : *( --itr );
				V			cmp{};			// Compensation for rounding errors.
				while( itr != p_.begin() ) {
					auto	tmp	  = res;
					res			  =	fma( x_, res, *( --itr ) );
					cmp			 +=	( tmp * x_ + *itr ) - ( res - cmp );
				}
				return res + cmp;
			}
		}
	};
	
	
	template< floating F, std::size_t N >
	using polynomial_base_t = std::conditional_t< N == std::dynamic_extent,
		std::vector< std::remove_cv_t< F > >, std::array< std::remove_cv_t< F >, N > >;

	/// Implements a polynomial that uses Algo to calculate the polynomial for a given x.
	/// Available algorithms for Algo are horner_fma and comp_horner_fma, above.
	template< typename Algo = horner_fma, floating F = double, std::size_t N = std::dynamic_extent >
	struct Polynomial : public polynomial_base_t< F , N > {
		using algorithm						  = Algo;
		static constexpr std::size_t extent	  = N;
		static constexpr bool is_static		  = N != std::dynamic_extent;
		using base							  = polynomial_base_t< F , N >;
		
		/// Evaluate the polynomial at x_.
		[[nodiscard]] constexpr F operator()( const std::type_identity_t< F > x_ )	const noexcept	{
			return Algo::operator()( std::span( *this ), F( x_ ) );
		}
	};

	template< floating F, std::size_t N >
	Polynomial( F( & )[ N ] )				-> Polynomial< horner_fma, F, N >;
	
	template< floating F >
	Polynomial( F *, F * )					-> Polynomial< horner_fma, F, std::dynamic_extent >;
	
	template< floating F >
	Polynomial( const F *, std::size_t )	-> Polynomial< horner_fma, F, std::dynamic_extent >;

	template< floating F, std::size_t N >
	Polynomial( std::span< F, N > )			-> Polynomial< horner_fma, F, std::dynamic_extent >;
	
	

	/// A polynomial's degree. Polynomials with fewer items than two have degrre 0, etc. 
	template< typename Algo, floating F, std::size_t N >
	[[nodiscard]] constexpr F degree( const Polynomial< Algo, F, N > & p_ )			noexcept		{
		return p_.size() ? p_.size() - 1u : 0u;
	}


	/// Returns a Polynomial< horner_fma, ... > of size sizeof...( Args ).
	template< arithmetic... Args >
	[[nodiscard]] constexpr auto polynomial( Args && ...args_ ) 					noexcept		{
		constexpr std::size_t N = sizeof...( Args );
		using F = std::remove_cv_t< std::common_type_t< float, Args... > >;
		using P = Polynomial< horner_fma, F, N ? N : std::dynamic_extent >;
		return P{{ static_cast< F >( std::forward< Args >( args_ ) )... }};
	}

	/// Returns a Polynomial< Algo, ... > of size sp_.size() with the contents of sp_.
	template< typename Algo = horner_fma, floating F, std::size_t N >
	[[nodiscard]] constexpr auto polynomial( const std::span< F, N > sp_ ) 			noexcept		{
		auto res = Polynomial< Algo, std::remove_cv_t< F >, N >{};
		if constexpr( N == std::dynamic_extent )		res.resize( sp_.size() );
		std::ranges::copy_n( sp_.data(), sp_.size(), res.data() );
		return res;
	}

	/// Returns a Polynomial< Algo, ... > of statical size N with the contents of arr_.
	template< typename Algo = horner_fma, floating F, std::size_t N >
	[[nodiscard]] constexpr auto polynomial( F( & arr_ )[ N ] ) 					noexcept		{
		auto res = Polynomial< Algo, std::remove_cv_t< F >, N >{};
		std::ranges::copy_n( arr_, N, res.data() );
		return res;
	}

	/// Returns a Polynomial< Algo, ... > of dynamic size with the contents at ptr_.
	template< typename Algo = horner_fma, floating F >
	[[nodiscard]] constexpr auto polynomial( const F * ptr_, const std::size_t sz_ ) noexcept		{
		return polynomial< Algo >( std::span{ ptr_, sz_ } );
	}

	/// Returns a Polynomial< horner_fma, ... > of dynamic size with the contents at [begin_, end_[.
	template< typename Algo = horner_fma, floating F >
	[[nodiscard]] constexpr auto polynomial( const F * begin_, const F * end_ ) 	noexcept		{
		return polynomial< Algo >( std::span{ begin_, end_ } );
	}

	/// Returns a Polynomial< comp_horner_fma, ... >.
	template< typename... Args >
	[[nodiscard]] constexpr auto polynomial_comp( Args && ...args_ )				noexcept		{
		constexpr std::size_t N = sizeof...( Args );
		using F = std::remove_cv_t< std::common_type_t< float, Args... > >;
		using P = Polynomial< comp_horner_fma, F, N ? N : std::dynamic_extent >;
		return P({ static_cast< F >( std::forward< Args >( args_ ) )... });
	}


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


	namespace {
		template< std::size_t N = std::dynamic_extent > using Sp = std::span< const double, N >;
		constexpr std::array< double, 4 > test{{ 1.0, 2.0, 3.0, 4.0 }};

		static_assert( polynomial( 2, 1 )( 3.0 )	== 5.0 );
		static_assert( polynomial()( 5 )			== 0 );
		static_assert( polynomial().extent			== std::dynamic_extent );
		static_assert( polynomial( 2 ).extent		== 1u );
		static_assert( polynomial_comp( 2, 1 )[ 0 ] == 2.0 );
		static_assert( polynomial_comp()( 5 )		== 0 );
		static_assert( polynomial_comp().extent		== std::dynamic_extent );
		static_assert( polynomial_comp( 2 ).extent	== 1u );

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

}	// namespace pax
