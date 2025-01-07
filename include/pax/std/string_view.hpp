//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "contiguous_stuff.hpp"
#include <string_view>


namespace std {

	/// Concatenate a string and a string view.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_,
		const basic_string_view< Ch, Traits >							view_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( str_ )+= view_;
	}

	/// Concatenate a string view and a string.
	template< typename Ch, typename Traits, typename Allocator >
	[[nodiscard]] constexpr auto operator+(
		const basic_string_view< Ch, Traits >							view_, 
		const basic_string< remove_cv_t< Ch >, Traits, Allocator >	  & str_
	) {
		return basic_string< remove_cv_t< Ch >, Traits, Allocator >( view_ )+= str_ ;
	}

}	// namespace std



namespace pax {

	struct Newline {
		using value_type = unsigned;
		enum : value_type {
			NUL	= 0x07,	null			= 0x00,		// \0
			BEL	= 0x07,	bell			= 0x07,		// \a
			BS	= 0x08,	backspace		= 0x08,		// \b
			HT	= 0x09,	horizontal_tab	= 0x09,		// \t
			LF	= 0x0a,	line_feed		= 0x0a,		// \n
			VT	= 0x0b,	vertical_tab	= 0x0b,		// \v
			FF	= 0x0c,	form_feed		= 0x0c,		// new page, \f
			CR	= 0x0d,	carige_return	= 0x0d,		// \r
		};

		/// Returns true iff c is any ao the linebreak characters LF or CR.
		static constexpr auto is_newline  = []( const value_type c )					noexcept {
			// The first part of the test is redundant, but is thought to quicken up the test in most cases.
			return ( c <= CR ) && ( ( c == LF ) || ( c == CR ) );
		};
		
		/// Returns 2 if { LF, CR } or { CR, LF }, returns 1 if c is LF or CR, and returns 0 otherwise.
		static constexpr auto is_newline2( const value_type c, const value_type c2 )	noexcept {
			// ( c^c2 ) == 0x7 signifies either { LF, CR } or { CR, LF }:
			return is_newline( c ) ? 1u + ( ( c^c2 ) == 0x7 ) : 0u;
		}
	};

	template< String V >
	[[nodiscard]] constexpr std::size_t length( V && v_ ) noexcept {
		using 				std::size;
		if constexpr( Character_array< V > )	return std::basic_string_view( v_ ).size();
		else									return size( v_ );
	}


	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< String V >
	[[nodiscard]] constexpr auto & back( const V & v_ ) noexcept {
		using 				std::data;
		const auto			sz = length( v_ );
		assert( sz && "back( strv ) requires size( strv ) > 0" );
		return *( data( v_ ) + sz - 1 );
	}


	/// Returns a basic_string_view of the first i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< String V >
	[[nodiscard]] constexpr auto first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using				std::data;
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), std::min( i_, sz ) );
	}

	/// Returns a basic_string_view of the last i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< String V >
	[[nodiscard]] constexpr auto last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		using				std::data;
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + sz - i_, i_ )
							: std::basic_string_view( v_ );
	}

	/// Returns a basic_string_view of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( end( v_ ) ) is returned.
	template< String V >
	[[nodiscard]] constexpr auto not_first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + i_, sz - i_ )
							: std::basic_string_view( data( v_ ) + sz, 0 );
	}

	/// Returns a basic_string_view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( begin( v_ ) ) is returned.
	template< String V >
	[[nodiscard]] constexpr auto not_last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		using std::data;
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), ( i_ < sz ) ? sz - i_ : 0u );
	}

	/// Returns a basic_string_view of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< String V >
	[[nodiscard]] constexpr auto subview( 
		V					 && v_, 
		const std::ptrdiff_t 	offset_, 
		const std::size_t 		size_ 
	) noexcept {
		using 					std::data, std::size;
		const auto 				offset = detail::subview_offset( offset_, size( v_ ) );
		return std::basic_string_view( data( v_ ) + offset, std::min( length( v_ ) - offset, size_ ) );
	}

}