//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "span.hpp"
#include "string_view.hpp"


namespace pax {

	/// What std::span or std::string_view is suitable for a type?
	/// Candidates are std::span< T >, std::span< T, N >, or std::string_view< T >.
	template< Contiguous_elements C, bool Dynamic = false >
	struct view_type {
		using type = std::span< Value_type_t< C >, Dynamic ? std::dynamic_extent : extent_v< C > >;
	};

	template< typename T, std::size_t N, bool Dynamic >
	struct view_type< std::span< T, N >, Dynamic > {
		using type = std::span< T, Dynamic ? std::dynamic_extent : N >;
	};

	template< String S, bool Dynamic >
	struct view_type< S, Dynamic > {
		using type = std::basic_string_view< std::remove_cvref_t< Value_type_t< S > > >;
	};

	template< String S, bool Dynamic >
		requires( std::remove_cvref_t< S >::traits_type )
	struct view_type< S, Dynamic > {
		using Ch = std::remove_cvref_t< Value_type_t< S > >;
		using Tr = typename std::remove_cvref_t< S >::traits_type;
		using type = std::basic_string_view< Ch, Tr >;
	};

	template< typename T, bool Dynamic >
	struct view_type< const T, Dynamic > : view_type< T, Dynamic > {};

	template< Contiguous_elements C, bool Dynamic = false >
	using view_type_t = typename view_type< C, Dynamic >::type;


	/// Turns a Contiguous_elements into its view type. 
	template< Contiguous_elements A >
	constexpr auto view( A && a_ )				noexcept	{
		return view_type_t< A >( a_ );
	}

	/// Turns an array of characters into a string view. Does not include a final '\0' character.
	template< Character Ch, std::size_t N >	
	constexpr auto view( Ch( & c_ )[ N ] )		noexcept	{
		return view_type_t< Ch( & )[ N ] >( c_, ( N == 0 ) ? 0u : N - !( *( c_ + N - 1 ) ) );
	}

	/// Turns a Contiguous_elements into its view type. 
	template< Contiguous_elements A >
	constexpr auto dview( A && a_ )				noexcept	{
		using std::size, std::data;
		return view_type_t< A, true >( a_ );
	}

	/// Turns an array of characters into a string view. Does not include a final '\0' character.
	template< Character Ch, std::size_t N >		// To keep constness, ampersand is necessary:
	constexpr auto dview( Ch( & c_ )[ N ] )		noexcept	{
		return view_type_t< Ch( & )[ N ], true >( c_, ( N == 0 ) ? 0u : N - !( *( c_ + N - 1 ) ) );
	}




	/// Returns the offset to the first occurence of sp1_ in sp0_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr std::size_t find(  
		const V0						  & v0_, 
		const V1						  & v1_
	) noexcept {
		if constexpr( Character_array< V0 > ) {
			return find( std::basic_string_view( v0_ ), v1_ );			// To remove possible trailing '\0'.
		} else if constexpr( Character_array< V1 > ) {
			return find( v0_, std::basic_string_view( v1_ ) );			// To remove possible trailing '\0'.
		} else {
			using std::begin, std::end;
			return std::search( begin( v0_ ), end( v0_ ), begin( v1_ ), end( v1_ ) ) - begin( v0_ );
		}
	}

	/// Returns the offset to the first occurence of t_ in v_.
	/// - Returns v_.size() if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return find( std::basic_string_view( v_ ), t_ );				// To remove possible trailing '\0'.
		} else {
			using std::begin, std::end;
			return std::find( begin( v_ ), end( v_ ), t_ ) - begin( v_ );
		}
	}

	/// Returns the offset to v for the first true occurence of pred_( v ) in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return find( std::basic_string_view( v_ ), pred_ );			// To remove possible trailing '\0'.
		} else {
			using std::begin, std::end;
			return std::find_if( begin( v_ ), end( v_ ), pred_ ) - begin( v_ );
		}
	}
	
	/// Returns the offset to the first occurence of either `'\n'` or `'\r'` in `view_`.
	/// - Returns `view_.size()` if there is no such.
	template< String V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Newline	
	) noexcept {
		return find( v_, Newline::is_newline );
	}


	/// Returns the offset to the last occurence of t_ in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return rfind( std::basic_string_view( v_ ), t_ );				// To remove possible trailing '\0'.
		} else {
			using std::size, std::rbegin, std::rend;
			const auto res = std::find( rbegin( v_ ), rend( v_ ), t_ );
			return ( res == rend( v_ ) ) ? size( v_ ) : rend( v_ ) - res - 1;
		}
	}

	/// Returns the offset for the last true occurence of pred_( ... ).
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return rfind( std::basic_string_view( v_ ), pred_ );			// To remove possible trailing '\0'.
		} else {
			using std::size, std::rbegin, std::rend;
			const auto res = std::find_if( rbegin( v_ ), rend( v_ ), pred_ );
			return ( res == rend( v_ ) ) ? size( v_ ) : rend( v_ ) - res - 1;
		}
	}


	/// Returns true iff find( v_, x_ ) < size( v_ ).
	template< Contiguous_elements V, typename X >
	[[nodiscard]] constexpr bool contains(  
		const V							  & v_, 
		X								 && x_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return find( v_, x_ ) < size( std::basic_string_view( v_ ) );	// To remove possible trailing '\0'.
		} else {
			using std::size;
			return find( v_, x_ ) < size( v_ );
		}
	}


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< Contiguous_elements V, typename U >
	constexpr auto until(  
		V								 && v_, 
		U								 && until_this_ 
	) noexcept {
		if constexpr( Character_array< V > ) {			// To remove possible trailing '\0'.
			return view_type_t< V, true >( data( v_ ), find( std::basic_string_view( v_ ), until_this_ ) );
		} else {
			using std::data;
			return view_type_t< V, true >( data( v_ ), find( v_, until_this_ ) );
		}
	}




	/// Returns true iff v_ and u_ are equal.
	template< Contiguous_elements V, Contiguous_elements U >
	[[nodiscard]] constexpr bool equal(  
		const V							  & v_, 
		U								 && u_
	) noexcept {
		if constexpr( Character_array< V > || Character_array< U > ) {
			return equal( std::basic_string_view( v_  ), std::basic_string_view( u_  ) );	// To remove possible trailing '\0'.
		} else {
			using std::begin, std::end;
			return std::equal( begin( v_ ), end( v_ ), begin( u_ ), end( u_ ) );
		}
	}


	/// Returns 1, if the beginning of `view_` is `t_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return starts_with( std::basic_string_view( v_ ), t_ );		// To remove possible trailing '\0'.
		} else {
			using std::size;
			return size( v_ ) && ( v_[ 0 ] == t_ );
		}
	}

	/// Returns 1, if the end of `view_` is `t_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return ends_with( std::basic_string_view( v_ ), t_ );			// To remove possible trailing '\0'.
		} else {
			using std::size;
			return size( v_ ) && ( back( v_ ) == t_ );
		}
	}

	/// Returns the size of `v_` if the beginning of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V, Contiguous_elements U >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		U								 && u_
	) noexcept {
		if constexpr( Character_array< V > || Character_array< U > ) {
			return starts_with( std::basic_string_view( v_ ), std::basic_string_view( u_ ) );	// To remove possible trailing '\0'.
		} else {
			using std::size;
			return equal( first( v_, size( u_ ) ), u_ ) ? size( u_ ) : 0u;
		}
	}

	/// Returns the size of `v_`, if the end of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V, Contiguous_elements U >
	[[nodiscard]] constexpr std::size_t ends_with( 
		const V							  & v_, 
		U								 && u_
	) noexcept {
		if constexpr( Character_array< V > || Character_array< U > ) {
			return ends_with( std::basic_string_view( v_ ), std::basic_string_view( u_ ) );		// To remove possible trailing '\0'.
		} else {
			using std::size;
			return equal( last( v_, size( u_ ) ), u_ ) ? size( u_ ) : 0u;
		}
	}

	/// Returns 2 if `view_` starts with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( extent_v< V > > 1 ) {
			using std::data, std::size;
			return	( size( v_ ) > 1 )	? Newline::is_newline2( v_[ 0 ], v_[ 1 ] )
				:	  size( v_ )		? Newline::is_newline ( v_[ 0 ] )
				:						  0;
		} else if constexpr( extent_v< V > == 1 ) {
			return Newline::is_newline( v_[ 0 ] );
		} else {
			return 0;
		}
	}

	/// Returns 2 if `view_` ends with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							& v_, 
		Newline 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return ends_with( view_type_t< V, true >( v_ ), Newline{} );	// To remove trailing '\0'.
		} else if constexpr( extent_v< V > > 1 ) {
			using std::data, std::size;
			const auto last = data( v_ ) + size( v_ ) - ( size( v_ ) > 0 );
			return	( size( v_ ) > 1 )	? Newline::is_newline2( *last, *( last - 1 ) )
				:	  size( v_ )		? Newline::is_newline ( v_[ 0 ] )
				:						  0;
		} else if constexpr( extent_v< V > == 1 ) {
			return Newline::is_newline( v_[ 0 ] );
		} else {
			return 0;
		}
	}




	/// Returns `v_`, possibly excluding a leading `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_front( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_front( view_type_t< V, true >( v_ ), t_ );	// To remove trailing '\0'.
		} else {
			return not_first( v_, starts_with( v_, t_ ) );
		}
	}

	/// Returns `v_` possibly excluding a trailing `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_back( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_back( view_type_t< V, true >( v_ ), t_ );	// To remove trailing '\0'.
		} else {
			return not_last( v_, ends_with( v_, t_ ) );
		}
	}


	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_first( view_type_t< V, true >( v_ ), t_ );	// To remove trailing '\0'.
		} else {
			using std::begin, std::end;
			auto							itr = begin( v_ );
			while( ( itr != end( v_ ) ) && ( *itr == t_ ) )		++itr;
			return view_type_t< V, true >{ itr, end( v_ ) };
		}
	}

	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		Pred							 && p_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_first( view_type_t< V, true >( v_ ), p_ );	// To remove trailing '\0'.
		} else {
			using std::begin, std::end;
			auto							itr = begin( v_ );
			while( ( itr != end( v_ ) ) && p_( *itr ) )		++itr;
			return view_type_t< V, true >{ itr, end( v_ ) };
		}
	}

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		Newline 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_first( view_type_t< V, true >( v_ ), Newline{} );	// To remove trailing '\0'.
		} else {
			return not_first( v_, starts_with( v_, Newline{} ) );
		}
	}


	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_last( view_type_t< V, true >( v_ ), t_ );				// To remove trailing '\0'.
		} else {
			auto							itr  = end( v_ );
			const auto						end_ = begin( v_ );
			while( ( --itr != end_ ) && ( *itr == t_ ) );
			return view_type_t< V, true >{ begin( v_ ), itr + 1 };
		}
	}

	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		Pred							 && p_ 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_last( view_type_t< V, true >( v_ ), p_ );				// To remove trailing '\0'.
		} else {
			using std::begin, std::end;
			auto							itr  = end( v_ );
			const auto						end_ = begin( v_ );
			while( ( --itr != end_ ) && p_( *itr ) );
			return view_type_t< V, true >{ begin( v_ ), itr + 1 };
		}
	}

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		Newline 
	) noexcept {
		if constexpr( Character_array< V > ) {
			return trim_last( view_type_t< V, true >( v_ ), Newline{} );	// To remove trailing '\0'.
		} else {
			return not_last( v_, ends_with( v_, Newline{} ) );
		}
	}


	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V, typename T >
	[[nodiscard]] constexpr auto trim( 
		const V							  & v_, 
		T								 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
	}




	///	Sorts the elements in v in non-descending order.
	///	- The order of equal elements is not guaranteed to be preserved.
	/// - https://en.cppreference.com/w/cpp/algorithm/sort
	template< Contiguous_elements V >
	constexpr void sort( V & v_ ) {
		if constexpr( Character_array< V > ) {
			sort( std::basic_string_view( v_ ) );		// To remove possible trailing '\0'.
		} else {
			using std::begin, std::end;
			std::sort( begin( v_ ), end( v_ ) );
		}
	}




	/// A tool to apply binary_ on all pairs of items from v0_/v1_. 
	/// v0_ and v1_ must have the same same size.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_v< Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr void on_each_pair(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		if constexpr( Character_array< V0 > ) {			// To remove possible trailing '\0'.
			return on_each_pair( view_type_t< V0, true >( v0_ ), v1_, binary_ );
		} else if constexpr( Character_array< V1 > ) {	// To remove possible trailing '\0'.
			return on_each_pair( v0_, view_type_t< V1, true >( v1_ ), binary_ );
		} else {
			using std::begin;
			detail::assert_equal_extent( v0_, v1_ );
			auto							i1 = begin( v1_ );
			for( auto & item : v0_ ) 		binary_( item, *( i1++ ) );
		}
	}

	/// Applies the function f_ on all elements while f_ returns true.
	/** - Returns true iff all pairs are iterated. 
		- Terminates unless v0_.size() == v1_.size().
		- This could be used to mutate elements or aggregate/count in different ways.
	**/
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr bool on_each_pair_while(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		if constexpr( Character_array< V0 > ) {			// To remove possible trailing '\0'.
			return on_each_pair_while( view_type_t< V0, true >( v0_ ), v1_, binary_ );
		} else if constexpr( Character_array< V1 > ) {	// To remove possible trailing '\0'.
			return on_each_pair_while( v0_, view_type_t< V1, true >( v1_ ), binary_ );
		} else {
			using std::begin, std::end;
			detail::assert_equal_extent( v0_, v1_ );
			auto 							itr0 = begin( v0_ );
			const auto 						end0 = end( v0_ );
			auto 							itr1 = begin( v1_ );
			while( ( itr0 != end0 ) && binary_( *itr0, *itr1 ) )	{	++itr0; ++itr1;		}
			return itr0 == end0;
		}
	}

	///	Checks if unary predicate p returns true for all elements in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool all_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		if constexpr( Character_array< V > ) {	// To remove possible trailing '\0'.
			return std::ranges::all_of( view_type_t< V, true >( v_ ), p_ );
		} else {
			return std::ranges::all_of( v_, p_ );
		}
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
		if constexpr( Character_array< V > ) {	// To remove possible trailing '\0'.
			return std::ranges::any_of( view_type_t< V, true >( v_ ), p_ );
		} else {
			return std::ranges::any_of( v_, p_ );
		}
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
		if constexpr( Character_array< V > ) {	// To remove possible trailing '\0'.
			return std::ranges::none_of( view_type_t< V, true >( v_ ), p_ );
		} else {
			return std::ranges::none_of( v_, p_ );
		}
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

	/// Calculate the Luhn sum (a control sum).
	/// – UB if any character is outside ['0', '9'].
	/// - https://en.wikipedia.org/wiki/Luhn_algorithm
	template< String V >
	[[nodiscard]] constexpr std::size_t luhn_sum( const V & str_ ) noexcept {
		if constexpr( Character_array< V > ) {
			return luhn_sum( std::basic_string_view( str_ ) );	// To remove possible trailing '\0'.
		} else {
			static constexpr char		 	twice[] = { 0, 2, 4, 6, 8, 1, 3, 5, 7, 9 };
			std::size_t						sum{};
			bool							one{ true };
			for( const auto c : str_ )		sum += ( one = !one ) ? ( c - '0' ) : twice[ c - '0' ];
			return sum;
		}
	}




	/// Split v_ into two parts: before `at_` and after (but not including) `at_ + n_`.
	/// - If `at_ >= size( v_ )`, then `{ v_, last( v_, 0 ) }` is returned.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto split_at( 
		const V							  & v_, 
		const std::size_t 					at_, 
		const std::size_t 					n_ = 1 
	) noexcept {
		// first() and not_first() handle the case if at_ + n_ >= size( v_ ).
		return std::pair{ first( v_, at_ ), not_first( v_, at_ + n_ ) };
	}

	/// Split `view_` into two parts: before and after the first `x_`, not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto split_by( 
		const V							  & v_, 
		const Value_type_t< V >				item_ 
	) noexcept {
		return split_at( v_, find( v_, item_ ) );
	}

	/// Split the v_ into two parts: before and after (but not including) `by_`.
	/// - If `begin( by_ ) <= begin( v_ )`, the first string view returned is `first( v_, 0 )`.
	///	- If `end( by_ )   >= end( v_ )`,   the second string view returned is `last( v_, 0 )`.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V, Contiguous_elements By >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		By								 && by_
	) noexcept {
		if constexpr( Character_array< By > ) {	// To remove possible trailing '\0'.
			return split_by( v_, std::basic_string_view( by_ ) );
		} else {
			using std::size;
			return split_at( v_, find( v_, by_ ), size( by_ ) );
		}
	}

	/// Split into two parts: before and after the first newline (`'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`), but not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< String V >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		Newline 
	) noexcept {
		const std::size_t 					i  = find( v_, Newline::is_newline );
		return split_at( v_, i, starts_with( not_first( v_, i ), Newline{} ) );
	}



	/// A class to simplify iterating using ´split_by´. It uses views, so the original string must remain static.
	/// - Example usage: ´for( const auto item : String_view_splitter( "A\nNumber\nof\nRows", Newline{} ) ) { ... }´. 
	/// - The Divider type may be any that is accepted by ´split_by( ..., Divider )´. 
	/// - String_view_splitter is constexpr [and never throws]. 
	template< Character Char, typename Divider, typename Traits = std::char_traits< std::remove_const_t< Char > > >
	class String_view_splitter {
		class End						{};
		using Value					  = std::basic_string_view< std::remove_const_t< Char >, Traits >;
		Value							m_str;
		Divider							m_divider;
		

		class iterator {
			std::pair< Value, Value >	m_parts;
			Divider						m_divider;

		public:
			constexpr iterator( const Value str_, const Divider divider_ )	noexcept :
				m_parts{ split_by( str_, divider_ ) }, m_divider{ divider_ } {}

			/// Iterate to next item. 
			constexpr iterator & operator++()	noexcept		{
				m_parts = split_by( m_parts.second, m_divider );
				return *this;
			}

			/// Get the string_view of the present element. 
			constexpr Value operator*()			const noexcept	{	return m_parts.first;									}

			/// Does *not* check equality! Only checks if we are done iterating. 
			constexpr bool operator==( End )	const noexcept	{	return m_parts.first.data() == m_parts.second.data();	}
		};
		
	public:
		constexpr String_view_splitter( const Value str_, const Divider divider_ ) 	noexcept :
			m_str{ str_ }, m_divider{ divider_ } {}

		constexpr iterator begin()				const noexcept	{	return { m_str, m_divider };							}
		constexpr End end()						const noexcept	{	return {};												}
	};

	template< String S, typename D >
	String_view_splitter( S &&, D ) 
		-> String_view_splitter< Value_type_t< S >, D, typename std::remove_cvref_t< S >::traits_type >;

	template< Character Ch, typename D >
	String_view_splitter( Ch *, D ) -> String_view_splitter< std::remove_reference_t< Ch >, D >;

}	// namespace pax
