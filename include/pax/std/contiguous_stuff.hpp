//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include <cassert>			// assert
#include <algorithm>		// std::min, std::equal, std::lexicographical_compare_three_way
#include "../concepts.hpp"	// pax::Character, pax::String, pax::Contiguous_elements


// From https://lemire.me/blog/2024/07/26/safer-code-in-c-with-lifetime-bounds/
#ifndef __has_cpp_attribute
	#define ada_lifetime_bound
#elif __has_cpp_attribute(msvc::lifetimebound)
	#define ada_lifetime_bound [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
	#define ada_lifetime_bound [[clang::lifetimebound]]
#elif __has_cpp_attribute(lifetimebound)
	#define ada_lifetime_bound [[lifetimebound]]
#else
	#define ada_lifetime_bound
#endif


namespace pax {

	/// Tag for use when doing stuff with newlines. 
	using Newline = struct{};

	/// Name some of the control characters.
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





	/// Returns false. 
	[[nodiscard]] constexpr bool valid( std::nullptr_t ) 	noexcept {
		return false;
	}

	/// Returns ptr_ != nullptr. 
	template< typename T >
	[[nodiscard]] constexpr bool valid( T * ptr_ ) 			noexcept {
		return ptr_ != nullptr;
	}

	/// Returns sp_.data() != nullptr. 
	template< Contiguous_elements V >
	[[nodiscard]] constexpr bool valid( const V & v_ ) 		noexcept {
		using std::data;
		return valid( data( v_ ) );
	}

	/// Return true iff both data() and size() are equal between the two std::spans.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool identic(
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return ( data( v0_ ) == data( v1_ ) ) && ( size( v0_ ) == size( v1_ ) );
	}

	/// Return true iff any element in v1_ by address is also an element in v2_.
	/// Cheap: 1 addition, 2 comparisons, and 2 bolean &&.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool overlap( 
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return	( ( data( v0_ ) > data( v1_ ) )	? ( data( v1_ ) + size( v1_ ) > data( v0_ ) ) 
												: ( data( v0_ ) + size( v0_ ) > data( v1_ ) )	)
			&&	size( v0_ ) && size( v1_ );		// An empty view cannot overlap.
	}

	/// Returns a reference to the first item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto & front( V && v_ ) noexcept {
		using std::data, std::size;
		static_assert( extent_v< V > != 0, "front( v_ ) requires size( v_ ) > 0" );
		if constexpr ( extent_v< V > == dynamic_extent )
			assert( size( v_ ) && "front( v_ ) requires size( v_ ) > 0" );
		return *data( v_ );
	}



	namespace detail {
		template< std::integral Int >
		[[nodiscard]] static constexpr std::size_t subview_offset( 
			const Int			 			offset_, 
			const std::size_t 				size_ 
		) noexcept {
			if constexpr( std::is_unsigned_v< Int > ) {
				return	std::min( offset_, size_ );
			} else {
				return	( offset_ >= 0 )					  ? std::min( std::size_t( offset_ ), size_ ) 
					:	( std::size_t( -offset_ ) < size_ )	  ? size_ - std::size_t( -offset_ )
					:											std::size_t{};
			}
		}

		// Return true iff copying to dest_ requires backward copy.
		template< Contiguous_elements V, typename Itr >
		[[nodiscard]] constexpr bool requires_backward( 
			V			 && sp_,
			Itr 			dst_ 
		) noexcept {
			static constexpr bool same = std::is_same_v< 
				std::remove_cvref_t< Value_type_t< V > >, std::remove_cvref_t< Value_type_t< Itr > > >;
			if constexpr( same )	return ( dst_ >= sp_.begin() ) && ( dst_ < sp_.end() );
			else					return false;
		}

		template< Contiguous_elements V0, Contiguous_elements V1 >
			requires( ( extent_v< V0 > != dynamic_extent ) && ( extent_v< V1 > != dynamic_extent ) )
		constexpr void assert_equal_extent( const V0 &, const V1 & )			noexcept {
			static_assert( extent_v< V0 > == extent_v< V1 >, "V0 and V1 must have same static size." );
		}

		template< Contiguous_elements V0, Contiguous_elements V1 >
		constexpr void assert_equal_extent( [[maybe_unused]] const V0 & v0_, [[maybe_unused]] const V1 & v1_ )	noexcept {
			using std::size;
			assert( size( v0_ ) == size( v1_ ) && "v0_ and v1_ must have same size." );
		}
	}	// namespace detail



	/// Returns 1, if the beginning of `view_` is `t_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		using std::size;
		auto			s = size( v_ );
		if constexpr( Character_array< V > ) 
			s -= bool( s );					// To remove possible trailing '\0'.

		return s && ( v_[ 0 ] == t_ );
	}

	/// Returns the size of `v_` if the beginning of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V0						  & v0_, 
		V1								 && v1_
	) noexcept {
		using std::begin, std::size;
		const auto		b0 = begin( v0_ );
		auto			s0 = size ( v0_ );
		const auto		b1 = begin( v1_ );
		auto			s1 = size ( v1_ );
		if constexpr( Character_array< V0 > ) 
			s0 -= bool( s0 ) && !v0_[ s0 - 1 ];		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			s1 -= bool( s1 ) && !v1_[ s1 - 1 ];		// To remove possible trailing '\0'.

		return ( s1 > s0 ) ? 0u : std::equal( b1, b1 + s1, b0 ) ? s1 : 0u;
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


	/// Returns 1, if the end of `view_` is `t_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		using std::size;
		auto			s = size( v_ );
		if constexpr( Character_array< V > ) 
			s -= bool( s ) && !v_[ s - 1 ];					// To remove possible trailing '\0'.

		return s && ( v_[ s - 1 ] == t_ );
	}

	/// Returns the size of `v_`, if the end of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr std::size_t ends_with( 
		const V0						  & v0_, 
		V1								 && v1_
	) noexcept {
		using std::begin, std::size;
		const auto		b0 = begin( v0_ );
		auto			s0 = size ( v0_ );
		const auto		b1 = begin( v1_ );
		auto			s1 = size ( v1_ );
		if constexpr( Character_array< V0 > ) 
			s0 -= bool( s0 ) && !v0_[ s0 - 1 ];		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			s1 -= bool( s1 ) && !v1_[ s1 - 1 ];		// To remove possible trailing '\0'.

		return ( s1 > s0 ) ? 0u : std::equal( b1, b1 + s1, b0 + ( s0 - s1 ) ) ? s1 : 0u;
	}

	/// Returns 2 if `view_` ends with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( extent_v< V > > 1 ) {
			using std::data, std::size;
			auto			s = size ( v_ );
			if constexpr( Character_array< V > ) 
				s -= bool( s ) && !v_[ s - 1 ];		// To remove possible trailing '\0'.

			const auto last = data( v_ ) + s - ( s > 0 );
			return	( s > 1 )	? newlines  ( *last, *( last - 1 ) )
				:	bool( s )	? is_newline( v_[ 0 ] )
				:	0u;
		} else if constexpr( extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0u;
		}
	}




	/// Returns the offset to the first occurence of sp1_ in sp0_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr std::size_t find(  
		const V0						  & v0_, 
		const V1						  & v1_
	) noexcept {
		using std::begin, std::end;
		const auto		b0 = begin( v0_ );
		auto			e0 = end  ( v0_ );
		const auto		b1 = begin( v1_ );
		auto			e1 = end  ( v1_ );
		if constexpr( Character_array< V0 > ) 
			e0 -= ( b0 != e0 ) && !*( e0 - 1 );		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			e1 -= ( b1 != e1 ) && !*( e1 - 1 );		// To remove possible trailing '\0'.

		return std::search( b0, e0, b1, e1 ) - b0;
	}

	/// Returns the offset to the first occurence of t_ in v_.
	/// - Returns v_.size() if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		return std::find( b, e, t_ ) - b;
	}

	/// Returns the offset to v for the first true occurence of pred_( v ) in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		return std::find_if( b, e, pred_ ) - b;
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

	/// Returns true iff find( v_, x_ ) < size( v_ ).
	template< Contiguous_elements V, typename X >
	[[nodiscard]] constexpr bool contains(  
		const V							  & v_, 
		X								 && x_ 
	) noexcept {
		using std::size;
		auto			s = size( v_ );
		if constexpr( Character_array< V > ) 
			s -= bool( s ) && !v_[ s - 1 ];		// To remove possible trailing '\0'.

		return find( v_, x_ ) < size( v_ );
	}

	/// Returns the offset to the last occurence of t_ in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		using std::size, std::begin, std::end;
		auto		b = std::reverse_iterator( end  ( v_ ) );
		const auto	e = std::reverse_iterator( begin( v_ ) );
		if constexpr( Character_array< V > ) 
			b += ( b != e ) && !*b;

		const auto res = std::find( b, e, t_ );
		return ( res == e ) ? size( v_ ) : e - res - 1;
	}

	/// Returns the offset for the last true occurence of pred_( ... ).
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		using std::size, std::begin, std::end;
		auto		b = std::reverse_iterator( end  ( v_ ) );
		const auto	e = std::reverse_iterator( begin( v_ ) );
		if constexpr( Character_array< V > ) 
			b += ( b != e ) && !*b;

		const auto res = std::find_if( b, e, pred_ );
		return ( res == e ) ? size( v_ ) : e - res - 1;
	}



	/// Returns true iff v_ and u_ are equal.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool equal(  
		const V0						  & v0_, 
		V1								 && v1_
	) noexcept {
		using std::begin, std::end;
		const auto		b0 = begin( v0_ );
		auto			e0 = end  ( v0_ );
		const auto		b1 = begin( v1_ );
		auto			e1 = end  ( v1_ );
		if constexpr( Character_array< V0 > ) 
			e0 -= ( b0 != e0 ) && !*( e0 - 1 );		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			e1 -= ( b1 != e1 ) && !*( e1 - 1 );		// To remove possible trailing '\0'.

		return std::equal( b0, e0, b1, e1 );
	}

}	// namespace pax
