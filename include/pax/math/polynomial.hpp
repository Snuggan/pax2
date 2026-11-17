// Implements a polynomial class.
// - For static sizes (`N != dynamic_extent`) it is all `constexpr`, so `Polynomial` can be used/initialised during compilation.
// - Everything is noexcept.
// - You can use the Horner template for evaluating olynomials. There are to variants, horner_fma_even and horner_fma_odd, that
//	 igonres the other terms. 
// - The class is simple and does not have extensive constructors. Use the ´polynomial(...)´ functions instead.
// - It uses base class std::array for static sizes and std::vector for dynamic. Their full functionality is available.
// - This file implements the actual class. The polynomial-extras.hpp implements lots of bells and whistles for it,
//   such as addition, subtraction, multiplication, division, derivation, and integration. You can also generate
//   a number of named polynomials, such as Bessel, Chebyshev, Fibonacci, Hermite, etc.
//
//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include <span>
#include <array>
#include <vector>
#include <algorithm>	// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <cmath>		// std::fma
#include <cassert>		// assert


namespace pax {
	namespace is {
		template< typename ... F >	concept floating	= ( std::floating_point < std::decay_t< F > > && ... );
		template< typename ... A >	concept arithmetic	= ( std::is_arithmetic_v< std::decay_t< A > > && ... );
	}

	template< is::floating F >
	[[nodiscard]] constexpr F fma( const F m0_, const F m1_, const F a_ ) noexcept {
#		if defined( __cpp_lib_constexpr_cmath )	// Can use std::fma at compile time.
			return std::fma( m0_, m1_, a_ );
#		else									// Can't use std::fma at compile time.
#			warning( "std::fma is not constexpr in the std library used." )
			// Use std::fma during run-time and the naive implementation otherwise.
			if consteval	{	return m0_*m1_ + a_;				}
			else			{	return std::fma( m0_, m1_, a_ );	}
#		endif
	}

	enum Horners {	fma_all = 0, fma_odd, fma_even	};

	/// Horner's iterative method with std::fma.
	template< Horners variant = fma_all >
	struct Horner {
		static  constexpr bool odd_coeffs		  = variant & ( Horners::fma_odd  );
		static  constexpr bool even_coeffs		  = variant & ( Horners::fma_even );
		static  constexpr bool every_2nd_coeff	  = odd_coeffs || even_coeffs;

	private:
		template< std::size_t N, is::floating F, std::size_t N0 >
		static constexpr F quick( const std::span< F, N0 > p_, const auto x_ ) noexcept {
			if constexpr( N <= 1+every_2nd_coeff+odd_coeffs )	return p_[ N0-1 ];
			else return fma( x_, quick< N-1-every_2nd_coeff >( p_, x_ ), p_[ N0-N+odd_coeffs ] );
		}

	public:
		template< is::floating F, std::size_t N >
		[[nodiscard]] static constexpr F operator()(
			const std::span< F, N >			p_,
			const std::type_identity_t< F >	x_
		) noexcept {
			std::decay_t< F >		res;
			const auto xx		  = every_2nd_coeff ? x_*x_ : x_;	// Optimisation when enery 2nd coeff is zero.
			if 		constexpr( N <= 0+odd_coeffs )	return {};
			else if constexpr( N != std::dynamic_extent ) {
				static constexpr auto	NN = N - ( every_2nd_coeff && ( ( N & 1u ) == odd_coeffs ) );
				res				  = quick< NN >( std::span< F, NN >{ p_.data(), NN }, xx );
			} else {												// If every_2nd_coeff, p_.end() might be out of sync...
				auto	itr		  = p_.end() - ( every_2nd_coeff && ( ( p_.size() & 1u ) == odd_coeffs ) );
				res				  = ( p_.begin() == itr ) ? 0 : *( --itr );
				while( itr > p_.begin() + odd_coeffs ) {
					if constexpr( every_2nd_coeff )		 --itr;		// Optimisation when enery 2nd coeff is zero.
					res			  =	fma( xx, res, *( --itr ) );
				}
			}
			return odd_coeffs ? res*x_ : res;
		}
	};

	using horner_fma	  = Horner< Horners::fma_all  >;	///< Quick Horner algorithm using fma, if available.
	using horner_fma_even = Horner< Horners::fma_even >;	///< Quicker, use this when odd coeffs may be ignored.
	using horner_fma_odd  = Horner< Horners::fma_odd  >;	///< Quicker, use this when even coeffs may be ignored.

	
	/// Implements a polynomial that uses Algo to calculate the polynomial for a given x.
	/// Available algorithms for Algo are horner_fma and comp_horner_fma, above.
	template< typename Algo = horner_fma, is::floating F = double, std::size_t N = std::dynamic_extent >
	struct Polynomial : public std::conditional_t< N == std::dynamic_extent,
		std::vector< std::decay_t< F > >, std::array< std::decay_t< F >, N > >
	{
		using algorithm						  = Algo;		///< Algorithm used to calculate a value at x.
		static constexpr std::size_t extent	  = N;			///< Could be std::dynamic_extent.
		static constexpr bool is_static		  = N != std::dynamic_extent;
		
		/// Evaluate the polynomial at x_, using the given algorithm Algo.
		[[nodiscard]] constexpr F operator()( const F x_ )									const noexcept	{
			return Algo::operator()( std::span< const F, N >( *this ), x_ );
		}
		
		/// Compare two polynomials for equality.
		template< typename Algo1, is::floating F1, std::size_t N1 >
		[[nodiscard]] constexpr bool operator==( const Polynomial< Algo1, F1, N1 > & o_ ) 	const noexcept	{
			const auto	itr0 =    last_non_zero();
			const auto	itr1 = o_.last_non_zero();
			return ( this->crend() - itr0 == o_.crend() - itr1 )
				? std::ranges::equal( itr0, this->crend(), itr1, o_.crend() )
				: false;
		}
		
		/// Lexicographically compare two polynomials. The comparison starts with the highest degree items. 
		template< typename Algo1, is::floating F1, std::size_t N1 >
		[[nodiscard]] constexpr auto operator<=>( const Polynomial< Algo1, F1, N1 > & o_ ) 	const noexcept	{
			const auto	itr0 =    last_non_zero();
			const auto	itr1 = o_.last_non_zero();
			return ( this->crend() - itr0 == o_.crend() - itr1 )
				? std::lexicographical_compare_three_way( itr0, this->crend(), itr1, o_.crend() )
				: ( this->crend() - itr0 <=> o_.crend() - itr1 );
		}

		/// The polynomial's degree. Polynomials with zero or one item have degrre 0. 
		[[nodiscard]] constexpr std::size_t degree()										const noexcept	{
			const std::size_t sz =  this->crend() - last_non_zero();
			return sz ? sz - 1u : 0u;
		}
	
		[[nodiscard]] constexpr auto last_non_zero()										const noexcept	{
			auto	itr = this->crbegin();
			while( ( itr != this->crend() ) && !*itr )	++itr;
			return itr;
		}
	};	
	

	/// Returns a Polynomial< Algo, ... > of size sizeof...( Args ).
	template< typename Algo = horner_fma, is::floating min_type = float, is::arithmetic... Args >
	[[nodiscard]] constexpr auto polynomial( Args && ...args_ ) 							noexcept		{
		static constexpr std::size_t N = sizeof...( Args );
		using F = std::decay_t< std::common_type_t< min_type, Args... > >;
		using P = Polynomial< Algo, F, N ? N : std::dynamic_extent >;
		return P{{ static_cast< F >( std::forward< Args >( args_ ) )... }};
	}

	/// Returns a Polynomial< Algo, ... > of size sp_.size() with the contents of sp_.
	template< typename Algo = horner_fma, is::floating F, std::size_t N >
	[[nodiscard]] constexpr auto polynomial( const std::span< F, N > sp_ ) 					noexcept		{
		auto res = Polynomial< Algo, std::decay_t< F >, N >{};
		if constexpr( N == std::dynamic_extent )		res.resize( sp_.size() );
		std::ranges::copy_n( sp_.data(), sp_.size(), res.data() );
		return res;
	}

	/// Returns a Polynomial< Algo, ... > of statical size N with the contents of arr_.
	template< typename Algo = horner_fma, std::size_t N, is::floating F >
	[[nodiscard]] constexpr auto polynomial( F( & arr_ )[ N ] ) 							noexcept		{
		return polynomial< Algo >( std::span< const F, N >{ arr_, N } );
	}

	/// Returns a Polynomial< Algo, ... > of dynamic size with the contents at ptr_.
	template< typename Algo = horner_fma, std::size_t N = std::dynamic_extent, is::floating F >
	[[nodiscard]] constexpr auto polynomial( const F * ptr_, const std::size_t sz_ ) 						{
		assert( N >= sz_ );
		return polynomial< Algo >( std::span< const F, N >{ ptr_, sz_ } );
	}

	/// Returns a Polynomial< horner_fma, ... > of dynamic size with the contents at [begin_, end_[.
	template< typename Algo = horner_fma, std::size_t N = std::dynamic_extent, is::floating F >
	[[nodiscard]] constexpr auto polynomial( const F * begin_, const F * end_ ) 							{
		assert( N >= end_ - begin_ );
		return polynomial< Algo >( std::span< const F, N >{ begin_, end_ } );
	}

}	// namespace pax
