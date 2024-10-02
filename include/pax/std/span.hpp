//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"	// pax::Character, pax::String, pax::Contiguous_elements
#include <span>
#include <algorithm>		// std::equal, std::lexicographical_compare_three_way


namespace std {

	template< pax::Character Ch, std::size_t N >
	[[nodiscard]] constexpr Ch * data( Ch( & c_ )[ N ] )	noexcept	{	return c_;											}

	template< pax::Character Ch, std::size_t N >			// In char arrays the null character is counted. 
	[[nodiscard]] constexpr size_t size( Ch( & )[ N ] )		noexcept	{	return ( N == 0 ) ? 0u : N-1u;						}

	template< pax::Character Ch, std::size_t N >
	[[nodiscard]] constexpr Ch * begin( Ch( & c_ )[ N ] )	noexcept	{	return data( c_ );									}

	template< pax::Character Ch, std::size_t N >			// In char arrays the null character is counted. 
	[[nodiscard]] constexpr Ch * end( Ch( & c_ )[ N ] )		noexcept	{	return begin( c_ ) + ( ( N == 0 ) ? 0u : N-1u );	}

	template< pax::Character Ch >
	[[nodiscard]] constexpr Ch * data( Ch * const & c_ )	noexcept	{	return c_;											}

	template< pax::Character Ch >
	[[nodiscard]] constexpr size_t size( Ch * const & c_ )	noexcept	{
		Ch *				itr = c_;
		if( c_ && *c_ )		while( *( ++itr ) );
		return itr - c_;
	}

	template< pax::Character Ch >
	[[nodiscard]] constexpr Ch * begin( Ch * const & c_ )	noexcept	{	return data( c_ );									}

	template< pax::Character Ch >
	[[nodiscard]] constexpr Ch * end( Ch * const & c_ )		noexcept	{	return begin( c_ ) + size( c_ );					}


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
