//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"	// pax::Character, pax::String, pax::Contiguous_elements
#include <span>
#include <algorithm>		// std::equal, std::lexicographical_compare_three_way


namespace pax {

	template< Contiguous_elements A >
	constexpr auto make_span( A && a_ ) noexcept {
		using span = std::span< Value_type_t< A >, extent_v< A > >;
		using std::begin, std::end;
		return span{ begin( a_ ), end( a_ ) };
	}

	template< typename T, std::size_t N >
	constexpr auto make_span( T( & c_ )[ N ] ) noexcept {
		using span = std::span< std::remove_reference_t< T >, N >;
		return span{ c_, N };
	}

	template< Character Ch, std::size_t N >
	constexpr auto make_span( Ch( & c_ )[ N ] ) noexcept {
		constexpr std::size_t 	sz = ( N == 0 ) ? 0u : N-1u;
		using span = std::span< std::remove_reference_t< Ch >, sz >;
		return span{ c_, sz };
	}

	template< Character Ch >
	constexpr auto make_span( Ch * const & c_ ) noexcept {
		using span = std::span< std::remove_reference_t< Ch > >;
		using std::size;
		return span{ c_, size( c_ ) };
	}

}	// namespace pax



namespace std {

	/// Stream the contents of `sp_` to `dest_` in the form `[i0, i1, ...]`.
	/// - If T is a string type, each item is surrounded by '"'.
	template< typename Dest, typename T, std::size_t N >
	constexpr Dest & operator<<(
		Dest					  & dest_,
		const std::span< T, N >		sp_
	) {
		if constexpr( !pax::Character< T > ) {
			constexpr auto			delimit  = pax::String< T > ? "\", \"" : ", ";
			dest_ << '[';
			if constexpr( N > 0 ) if( sp_.size() > 0 ) {
				if constexpr( pax::String< T > )	dest_ << '"' << sp_.front();
				else								dest_ << sp_.front();
				if constexpr( N > 1 ) if( sp_.size() > 1 ) 
					for( const auto & item : sp_.template subspan< 1 >() ) 
						dest_ << delimit << item;
				if constexpr( pax::String< T > )	dest_ << '"';
			}
			dest_ << ']';
		} else if constexpr( N > 0 ) if( sp_.size() > 0 ) {
			dest_.write( sp_.data(), sp_.size() - !sp_.back() );
		}
		return dest_;
	}

	/// Check if all elements of `sp_` are equal to the corresponding elements of `v_`.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr bool operator==(
		const std::span< T, N >		sp_,
		V						 && v_
	) noexcept {
		using std::begin, std::end;
		return std::equal( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Lexicographically compare `sp_` with `v_` using three-way comparison.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr auto operator<=>(
		const std::span< T, N >		sp_,
		V						 && v_
	) noexcept {
		using std::begin, std::end;
		return std::lexicographical_compare_three_way( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Implement `get` for statically sized `std::span`.
	/// Negative `I` will acces `I + N`.
	template< std::ptrdiff_t I, typename T, std::size_t N >
		requires( ( N != std::dynamic_extent ) && ( ( ( I < 0 ) ? -I : I ) < N ) )
	[[nodiscard]] constexpr T & get( const std::span< T, N > v_ )  				noexcept	{
		return *( v_.data() + ( ( I >= 0 ) ? I : I + N ) );
	}

}	// namespace std
