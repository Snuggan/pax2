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

}	// namespace pax
