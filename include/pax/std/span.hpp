//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../types/point-stuff/contiguous.hpp"


namespace pax {
	template< typename T >
	concept Not_character_array = traits::contiguous< T > && !traits::character_array< T >;


	/// Returns a std::span.
	template< traits::character Ch, std::size_t N >
	[[nodiscard]] constexpr auto make_span( Ch( & c_ )[ N ] ) 						noexcept {
		constexpr std::size_t 	sz = ( N == 0 ) ? 0u : N-1u;
		return std::span< std::remove_reference_t< Ch >, sz >{ c_, sz };
	}

	/// Returns a std::span.
	template< traits::character Ch >
	[[nodiscard]] constexpr auto make_span( Ch * const & c_ ) 						noexcept {
		using std::size;
		return std::span< std::remove_reference_t< Ch > >{ c_, size( c_ ) };
	}

	/// Returns a view with const elements.
	template< Not_character_array V >
	[[nodiscard]] constexpr auto make_const_span( V && v_ ) 						noexcept {
		using std::data, std::size;
		return std::span< const traits::element_type_t< V >, traits::extent_v< V > >( data( v_ ), size( v_ ) );
	}

	/// Returns a dynamically sized std::span.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr auto make_dynamic_span( const std::span< T, N > sp_ )	noexcept {
		if constexpr( N == traits::dynamic_extent )		return sp_;
		else											return std::span( sp_.data(), N );
	}

	/// Returns a dynamically sized std::span.
	template< traits::contiguous V >
	[[nodiscard]] constexpr auto make_dynamic_span( V && v_ ) 						noexcept {
		return make_dynamic_span( make_span( v_ ) );
	}


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< traits::contiguous V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}

}	// namespace pax
