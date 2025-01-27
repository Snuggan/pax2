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


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< Contiguous_elements V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}




	/// Returns `v_`, possibly excluding a leading `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_front( 
		const V				  & v_, 
		const Value_type_t< V >	t_ 
	) noexcept {
		return not_first( v_, starts_with( v_, t_ ) );
	}

	/// Returns `v_` possibly excluding a trailing `t_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_back( 
		const V				  & v_, 
		const Value_type_t< V >	t_ 
	) noexcept {
		return not_last( v_, ends_with( v_, t_ ) );
	}


	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto			itr = begin( v_ );
		auto			e   = end  ( v_ );
		if constexpr( Character_array< V > ) 
			e -= ( itr != e ) && !*( e - 1 );			// To remove possible trailing '\0'.

		while( ( itr != e ) && p_( *itr ) )				++itr;
		return view_type_t< V, true >{ itr, e };
	}

	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_first( 
		const V					  & v_, 
		const Value_type_t< V >   & t_ 
	) noexcept {
		return trim_first( v_, [ & t_ ]( const Value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		Newline 
	) noexcept {
		return not_first( v_, starts_with( v_, Newline{} ) );
	}


	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto			itr = end  ( v_ );
		const auto		b   = begin( v_ );
		if constexpr( Character_array< V > ) 
			itr -= ( itr != b ) && !*( itr - 1 );		// To remove possible trailing '\0'.

		while( ( --itr != b ) && p_( *itr ) );
		return view_type_t< V, true >{ b, itr + 1 };
	}

	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_last( 
		const V				  & v_, 
		const Value_type_t< V >	t_ 
	) noexcept {
		return trim_last( v_, [ & t_ ]( const Value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		Newline 
	) noexcept {
		return not_last( v_, ends_with( v_, Newline{} ) );
	}


	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V, typename T >
	[[nodiscard]] constexpr auto trim( 
		const V		  & v_, 
		T			 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
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
		if constexpr( Character_array< V > ) {	// To remove possible trailing '\0'.
			return std::ranges::all_of( std::basic_string_view( v_ ), p_ );
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
			return std::ranges::any_of( std::basic_string_view( v_ ), p_ );
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
			return std::ranges::none_of( std::basic_string_view( v_ ), p_ );
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
		using std::size;
		auto				s = size( by_ );
		if constexpr( Character_array< V > ) 
			s -= bool( s ) && !by_[ s - 1 ];		// To remove possible trailing '\0'.

		return split_at( v_, find( v_, by_ ), s );
	}

	/// Split into two parts: before and after the first newline (`'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`), but not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< String V >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		Newline 
	) noexcept {
		const std::size_t 	i = find( v_, is_newline );
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
