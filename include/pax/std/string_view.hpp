//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "contiguous_stuff.hpp"
#include <string_view>


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


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
	
	using Newline = struct{};

	enum Ascii : unsigned {
		NUL	= 0x00,		null			= NUL,		// \0
		BEL	= 0x07,		bell			= BEL,		// \a
		BS	= 0x08,		backspace		= BS,		// \b
		HT	= 0x09,		horizontal_tab	= HT,		// \t
		LF	= 0x0a,		line_feed		= LF,		// \n
		VT	= 0x0b,		vertical_tab	= VT,		// \v
		FF	= 0x0c,		form_feed		= FF,		// new page, \f
		CR	= 0x0d,		carige_return	= CR,		// \r
	};

	/// Returns true iff c is any ao the linebreak characters LF or CR.
	static constexpr auto is_newline  = []( const unsigned c )						noexcept {
		// The first part of the test is redundant, but is thought to quicken up the test in most cases.
		return ( c <= Ascii::CR ) && ( ( c == Ascii::LF ) || ( c == Ascii::CR ) );
	};
	
	/// Returns 2 if { LF, CR } or { CR, LF }, returns 1 if c is LF or CR, and returns 0 otherwise.
	static constexpr auto newlines( const unsigned c, const unsigned c2 )			noexcept {
		// ( c^c2 ) == 0x7 signifies either { LF, CR } or { CR, LF }:
		return is_newline( c ) ? 1u + ( ( c^c2 ) == 0x7 ) : 0u;
	}


	/// Returns a descriptive string view for invisible characters and an empty one for visible ones.
	template< Character Ch2 >
	static constexpr std::basic_string_view< Ch2 > invisible_character_name( const Ch2 c_ )	noexcept {
		using string_view	  = std::basic_string_view< Ch2 >;
		static constexpr		unsigned		specialsN = 35;
		static constexpr		string_view		specials[ specialsN ] = { 
			"\\0",   "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "\\a",
			"\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "<SO>",  "<SI>",
			"<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>", 
			"<CAN>", "<EM>",  "<SUB>", "\\e",   "<FS>",  "<GS>",  "<RS>",  "<US>",
			" ",	 "!",	  "\\\""
		};
		switch( c_ ) {
			case '\\': 			return "\\\\";
			case 0x7f:			return "<DEL>";
			default:			return ( unsigned( c_ ) >= specialsN ) ? string_view{} : specials[ unsigned( c_ ) ];
		}
	}



	template< Character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch c_ )	noexcept	{
		const auto view 	= invisible_character_name( c_ );
		return view.size()	? std::basic_string< Ch >{ view }
							: std::basic_string< Ch >{{ c_ }};
	}

	template< String S >
	constexpr auto as_ascii( const S & str_ )					noexcept	{
		using std::size, std::begin, std::end;
		using 				Ch	  = typename S::value_type;
		using				Str	  = std::basic_string     < Ch >;
		using				StrV  = std::basic_string_view< Ch >;

		auto				itr	  = begin( str_ );
		const auto			stop  = end  ( str_ );
		StrV				sub{};
		Str					result{};
							result.reserve( size( str_ ) );

		while( itr != stop ) {
			auto itr2	  = itr;
			while( ( ( sub = invisible_character_name( *itr2 ) ).size() == 0u ) && ( ++itr2 != stop ) );
			if( itr2 != itr ) {			// A [bunch of] visible character[s]. 
				result	 +=	StrV{ itr, itr2 };
				itr		  = itr2;
			} else if( itr2 != stop ) {	// An invisible character is substituted by a string. 
				result	 +=	sub;
				++itr;
			}
		}
		return result;
	}

	template< Character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch *c_ )	noexcept	{
		return as_ascii( std::basic_string_view( c_ ) );
	}




	template< String V >
	[[nodiscard]] constexpr std::size_t length( V && v_ )		noexcept	{
		using 				std::size;
		if constexpr( Character_array< V > )	return std::basic_string_view( v_ ).size();
		else									return size( v_ );
	}


	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< String V >
	[[nodiscard]] constexpr auto & back( const V & v_ )			noexcept	{
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

	/// Return the first newline used in view_ (`"\n"`, `"\r"`, `"\n\r"`, or `"\r\n"`).
	/// - If none is found, `"\n"` is returned.
	template< String V >
	[[nodiscard]] constexpr auto identify_newline( const V & str_ ) noexcept {
		using my_view = std::basic_string_view< Value_type_t< V > >;
		static constexpr const my_view			 	res = { "\n\r\n" };
		const auto 									temp = not_first( str_, find( str_, Newline{} ) );
		const std::size_t 							sz = starts_with( temp, Newline{} );
		return	sz ? subview( res, temp.front() == '\r', sz ) : first( res, 1 );
	}



	/// Returns the offset to the first occurence of either `'\n'` or `'\r'` in `view_`.
	/// - Returns `view_.size()` if there is no such.
	template< String V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Newline	
	) noexcept {
		return find( v_, is_newline );
	}

	/// Returns 2 if `view_` starts with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( extent_v< V > > 1 ) {
			using std::data, std::size;
			return	( size( v_ ) > 1 )	? newlines  ( v_[ 0 ], v_[ 1 ] )
				:	  size( v_ )		? is_newline( v_[ 0 ] )
				:						  0;
		} else if constexpr( extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0;
		}
	}

}