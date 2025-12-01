//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "../concepts.hpp"	// pax::Character, pax::String, pax::Contiguous_elements
#include <algorithm>		// std::min, std::equal, std::lexicographical_compare_three_way
#include <cassert>			// assert


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
	struct Newline{};

	/// Returns false. 
	[[nodiscard]] constexpr bool valid( std::nullptr_t ) 	noexcept	{	return false;				}

	/// Returns ptr_ != nullptr. 
	template< typename T >
	[[nodiscard]] constexpr bool valid( T * ptr_ ) 			noexcept	{	return ptr_ != nullptr;		}

	/// Returns sp_.data() != nullptr. 
	template< Contiguous_elements V >
	[[nodiscard]] constexpr bool valid( const V & v_ ) 		noexcept	{
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
		// Return true iff copying to dest_ requires backward copy.
		template< Contiguous_elements V, typename Itr >
		[[nodiscard]] constexpr bool requires_backward( 
			V							 && sp_,
			Itr 							dst_ 
		) noexcept {
			static constexpr bool same = std::is_same_v< 
				std::remove_cvref_t< Value_type_t< V > >, std::remove_cvref_t< Value_type_t< Itr > > >;
			return same && ( dst_ >= sp_.begin() ) && ( dst_ < sp_.end() );
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
			s -= bool( s ) && !v_[ s - 1 ];			// To remove possible trailing '\0'.

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


	/// Returns 1, if the end of `view_` is `t_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		using std::size;
		auto			s = size( v_ );
		if constexpr( Character_array< V > ) 
			s -= bool( s ) && !v_[ s - 1 ];			// To remove possible trailing '\0'.

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
		return find( v_, []( const unsigned c )	noexcept 
			{
				// The first part of the test is redundant, but is thought to quicken up the test in most cases.
				return ( c <= 0x0d ) && ( ( c == 0x0a ) || ( c == 0x0d ) );
			}
		);
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
			s -= bool( s ) && !v_[ s - 1 ];			// To remove possible trailing '\0'.

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



	///	Sorts the elements in v in non-descending order.
	///	- The order of equal elements is not guaranteed to be preserved.
	/// - https://en.cppreference.com/w/cpp/algorithm/sort
	template< Contiguous_elements V >
	constexpr void sort( V & v_ ) {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		std::sort( b, e );
	}



	/// A tool to apply binary_ on all pairs of items from v0_/v1_. 
	/// v0_ and v1_ must have the same same size.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_v< Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr void on_each_pair(
		const V0	  & v0_,
		const V1	  & v1_,
		Binary		 && binary_
	) noexcept {
		using std::begin, std::size;
		auto			b0 = begin( v0_ );
		auto			s0 = size ( v0_ );
		auto			b1 = begin( v1_ );
		auto			s1 = size ( v1_ );
		if constexpr( Character_array< V0 > ) 
			s0 -= bool( s0 ) && !v0_[ s0 - 1 ];		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			s1 -= bool( s1 ) && !v1_[ s1 - 1 ];		// To remove possible trailing '\0'.

		assert( ( s0 == s1 ) && "The containers must have the same size" );
		const auto		e0 = b0 + s0;
		while( b0 != e0 ) 	binary_( *( b0++ ), *( b1++ ) );
	}

	/// Applies the function f_ on all elements while f_ returns true.
	/** - Returns true iff all pairs are iterated. 
		- Terminates unless v0_.size() == v1_.size().
		- This could be used to mutate elements or aggregate/count in different ways.
	**/
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr bool on_each_pair_while(
		const V0	  & v0_,
		const V1	  & v1_,
		Binary		 && binary_
	) noexcept {
		using std::begin, std::size;
		auto			b0 = begin( v0_ );
		auto			s0 = size ( v0_ );
		auto			b1 = begin( v1_ );
		auto			s1 = size ( v1_ );
		if constexpr( Character_array< V0 > ) 
			s0 -= bool( s0 ) && !v0_[ s0 - 1 ];		// To remove possible trailing '\0'.
		if constexpr( Character_array< V1 > ) 
			s1 -= bool( s1 ) && !v1_[ s1 - 1 ];		// To remove possible trailing '\0'.

		assert( ( s0 == s1 ) && "The containers must have the same size" );
		const auto		e0 = b0 + s0;
		while( ( b0 != e0 ) && binary_( *( b0++ ), *( b1++ ) ) );
		return b0 == e0;
	}

	///	Checks if unary predicate p returns true for all elements in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool all_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		return std::all_of( b, e, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for all i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool all_of(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		return on_each_pair_while( v0_, v1_, 
			[ binary_ ]( 
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return binary_( t0_, t1_ );
			}
		);
	}

	///	Checks if unary predicate p returns true for at least one element in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool any_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		return std::any_of( b, e, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for any i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool any_of(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		return !on_each_pair_while( v0_, v1_, 
			[ binary_ ]( 
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return !binary_( t0_, t1_ );
			}
		);
	}

	///	Checks if unary predicate p returns true for no elements in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool none_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		using std::begin, std::end;
		const auto		b = begin( v_ );
		auto			e = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( b != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		return std::none_of( b, e, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for no i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool none_of(
  		const V0						  & v0_,
  		const V1						  & v1_,
  		Binary							 && binary_
	) noexcept {
		return on_each_pair_while( v0_, v1_, 
			[ binary_ ](
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return !binary_( t0_, t1_ );
			}
		);
	}

}	// namespace pax
