//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once
#include "polynomial.hpp"
#include <format>


namespace std {
	template< typename Algo, pax::is::floating F, std::size_t N >
	struct formatter< pax::Polynomial< Algo, F, N > > : formatter< string > {
		auto format(  pax::Polynomial< Algo, F, N > sp_, format_context & ctx ) const {
			string					res{};
			res.reserve( sp_.size()*( Algo::every_2nd_coeff ? 6 : 10 ) );
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
}	// namespace std
namespace pax {
	namespace is {
		template< std::size_t N, std::size_t ...NN >
		constexpr std::size_t newN = ( true && ... && ( NN != std::dynamic_extent ) ) ? N : std::dynamic_extent;

		template< typename A0, typename A1 >	struct Merge_algo		{	using type = horner_fma;		};
		template< typename A > struct Merge_algo< A, A >				{	using type = A;					};
		template< typename A0, typename A1 >	using  Merge_algo_t		=	typename Merge_algo< A0, A1 >::type;
	}

	/// Stream the elements of a polynomial to out_.
	template< typename Out, typename Algo, is::floating F, std::size_t N >
	Out & operator<<(
		Out								  & out_, 
		const Polynomial< Algo, F, N >	  & poly_ 
	) {	return out_ << std::format( "{}", poly_ );							}

	/// Stream the elements of sp_ to out_ as id sp_ was a polynomial of x.
	template< typename Out, is::floating F, std::size_t N >
	Out & as_polynomial(
		Out								  & out_, 
		const std::span< F, N >				sp_ 
	) {	return out_ << std::format( "{}", polynomial( sp_ ) );				}


	template< bool Addition, typename Algo0, is::floating X0, std::size_t N0, 
							 typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto addsub(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, std::decay_t< std::common_type_t< X0, X1 > >, 
												std::max( N0, N1 ) > p{};
		if constexpr( !p.is_static )			p.resize( std::max( p0_.size(), p1_.size() ) );
		auto 									itr = p.begin();
		std::copy( p0_.begin(), p0_.end(), itr );
		if constexpr( Addition )				for( const auto x1 : p1_ )	*( itr++ ) += x1;
		else									for( const auto x1 : p1_ )	*( itr++ ) -= x1;
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
	) noexcept -> Polynomial< is::Merge_algo_t< Algo0, Algo1 >, std::decay_t< std::common_type_t< X0, X1 > >, 
												is::newN< N0 + !N0 + N1 + !N1 - 1, N0, N1 > >
	{
		if( Algo1::every_2nd_coeff && ( !Algo0::every_2nd_coeff || ( p1_.size() > p0_.size() ) ) )
			return p1_*p0_;						// It is more advantageous to do it in reverse order.
		else {
			Polynomial< is::Merge_algo_t< Algo0, Algo1 >, std::decay_t< std::common_type_t< X0, X1 > >, 
												is::newN< N0 + !N0 + N1 + !N1 - 1, N0, N1 > > p{};
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

	template< typename Algo, is::floating X0, std::size_t N0 >
	[[nodiscard]] constexpr auto & operator*=(
		Polynomial< Algo, X0, N0 >			  & p0_,
		const std::type_identity_t< X0 >		x0_
	) noexcept {
		for( auto & x : p0_ )					x *= x0_;
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
	[[nodiscard]] constexpr auto division(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, std::decay_t< std::common_type_t< X0, X1 > >, 
												is::newN< N0 - N1 + 1, N0, N1 > >	q{};
		const std::size_t				d0	  = p0_.degree();
		const std::size_t				d1	  = p1_.degree();
		const std::size_t				d	  = d0 - d1;
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

	/// Division.
	/// "A Generalization of Synthetic Division and A General Theorem of Division of Polynomials", by Lianghuo Fan.
	template< typename Algo0, is::floating X0, std::size_t N0, 
			  typename Algo1, is::floating X1, std::size_t N1 >
	[[nodiscard]] constexpr auto operator/(
		const Polynomial< Algo0, X0, N0 >	  & p0_,
		const Polynomial< Algo1, X1, N1 >	  & p1_
	) noexcept {
		Polynomial< is::Merge_algo_t< Algo0, Algo1 >, std::decay_t< std::common_type_t< X0, X1 > >, 
												is::newN< N0 - N1 + 1, N0, N1 > >	q{};
		const std::size_t				d0	  = p0_.degree();
		const std::size_t				d1	  = p1_.degree();
		const std::size_t				d	  = d0 - d1;
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
