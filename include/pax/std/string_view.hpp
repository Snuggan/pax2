//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/types/point-stuff/contiguous.hpp>
#include <string_view>


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


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
	using std::data, std::size, std::begin, std::end;
	
	
	template< traits::character Char, std::size_t N >
	constexpr std::basic_string_view< std::remove_cv_t< Char > > make_view( const std::span< Char, N > sp_ ) {
		return { sp_.data(), sp_.size() };
	}

	template< typename T >
	[[nodiscard]] constexpr std::size_t shave_zero_suffix( const T &, const std::size_t sz_ )		{	return sz_;		}

	template< traits::string Str >			  requires( traits::character_array< Str > )
	[[nodiscard]] constexpr std::size_t shave_zero_suffix( const Str & str_, const std::size_t sz_ )
	{	return sz_ - ( sz_ && !str_[ sz_ - 1 ] );																		}


	/// Returns true iff c is any ao the linebreak characters LF or CR.
	static constexpr auto is_newline  = []( const unsigned c )	noexcept {
		// The first part of the test is redundant, but is thought to quicken up the test in most cases.
		return ( c == '\n' ) || ( c == '\r' );
	};
	
	/// Returns 2 if { LF, CR } or { CR, LF }, returns 1 if c is LF or CR, and returns 0 otherwise.
	static constexpr auto newlines( const unsigned c, const unsigned c2 )			noexcept {
		// ( c^c2 ) == 0x7 signifies either { LF, CR } or { CR, LF }:
		return is_newline( c ) ? 1u + ( ( c^c2 ) == 0x7 ) : 0u;
	}
	

	/// Returns 2 if `view_` starts with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< traits::string V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							& v_, 
		linebreak 
	) noexcept {
		if constexpr( traits::extent_v< V > > 1 ) {
			return	( size( v_ ) > 1 )	? newlines  ( v_[ 0 ], v_[ 1 ] )
				:	  size( v_ )		? is_newline( v_[ 0 ] )
				:						  0;
		} else if constexpr( traits::extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0;
		}
	}

	/// Returns 2 if `view_` ends with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< traits::string V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							& v_, 
		linebreak 
	) noexcept {
		if constexpr( traits::extent_v< V > > 1 ) {
			const auto		s = shave_zero_suffix( v_, size( v_ ) );
			const auto last = data( v_ ) + s - ( s > 0 );
			return	( s > 1 )	? newlines  ( *last, *( last - 1 ) )
				:	bool( s )	? is_newline( v_[ 0 ] )
				:	0u;
		} else if constexpr( traits::extent_v< V > == 1 ) {
			return is_newline( v_[ 0 ] );
		} else {
			return 0u;
		}
	}




	template< traits::string V >
	[[nodiscard]] constexpr std::size_t length( V && v_ )		noexcept	{
		if constexpr( traits::character_array< V > )	return std::basic_string_view( v_ ).size();
		else											return size( v_ );
	}


	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< traits::string V >
	[[nodiscard]] constexpr auto & back( const V & v_ )			noexcept	{
		const auto			sz = length( v_ );
		assert( sz && "back( strv ) requires size( strv ) > 0" );
		return *( data( v_ ) + sz - 1 );
	}


	/// Returns a basic_string_view of the first i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), std::min( i_, sz ) );
	}

	/// Returns a basic_string_view of the last i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + sz - i_, i_ )
							: std::basic_string_view( v_ );
	}

	/// Returns a basic_string_view of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( end( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto not_first( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		const auto			sz = length( v_ );
		return ( i_ < sz )	? std::basic_string_view( data( v_ ) + i_, sz - i_ )
							: std::basic_string_view( data( v_ ) + sz, 0 );
	}

	/// Returns a basic_string_view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( begin( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto not_last( 
		V				 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		const auto			sz = length( v_ );
		return std::basic_string_view( data( v_ ), ( i_ < sz ) ? sz - i_ : 0u );
	}

	/// Returns a basic_string_view of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< traits::string V >
	[[nodiscard]] constexpr auto subview( 
		V					 && v_, 
		const std::ptrdiff_t 	offset_, 
		const std::size_t 		size_ 
	) noexcept {
		const auto				sz = size( v_ );
		const std::size_t 		offset	=	( offset_ >= 0 )					? std::min( std::size_t( offset_ ), sz ) 
										:	( std::size_t( -offset_ ) < sz )	? sz - std::size_t( -offset_ )
										:										  std::size_t{};

		return std::basic_string_view( data( v_ ) + offset, std::min( length( v_ ) - offset, size_ ) );
	}



	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< traits::string V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}



	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		auto			itr = begin( v_ );
		auto			e   = itr + shave_zero_suffix( v_, size( v_ ) );

		while( ( itr != e ) && p_( *itr ) )				++itr;
		return std::basic_string_view{ itr, e };
	}

	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		const traits::value_type_t< V >   & t_ 
	) noexcept {
		return trim_first( v_, [ & t_ ]( const traits::value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		const V		  & v_, 
		linebreak 
	) noexcept {
		return not_first( v_, starts_with( v_, linebreak{} ) );
	}

	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		Pred		 && p_ 
	) noexcept {
		const auto		b   = begin( v_ );
		auto			itr = b + shave_zero_suffix( v_, size( v_ ) );

		while( ( --itr != b ) && p_( *itr ) );
		return std::basic_string_view{ b, itr + 1 };
	}

	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return trim_last( v_, [ & t_ ]( const traits::value_type_t< V > & t ){ return t == t_; } );
	}

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		const V		  & v_, 
		linebreak 
	) noexcept {
		return not_last( v_, ends_with( v_, linebreak{} ) );
	}

	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V, typename T >
	[[nodiscard]] constexpr auto trim( 
		const V		  & v_, 
		T			 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
	}



	/// A class to simplify iterating using ´split_by´. It uses views, so the original string must remain static.
	/// - Example usage: ´for( const auto item : String_view_splitter( "A\nNumber\nof\nRows", linebreak{} ) ) { ... }´. 
	/// - The Divider type may be any that is accepted by ´split_by( ..., Divider )´. 
	/// - String_view_splitter is constexpr [and never throws]. 
	template< traits::character Char, typename Divider, typename Traits = std::char_traits< std::remove_const_t< Char > > >
	class String_view_splitter {
		class End						{};
		using Value					  = std::basic_string_view< std::remove_const_t< Char > >;
		using Span					  = std::span< std::remove_const_t< Char > >;
		Value							m_str;
		Divider							m_divider;
		

		class iterator {
			split_result< Char >		m_parts;
			Divider						m_divider;

		public:
			constexpr iterator( const Value str_, const Divider divider_ )	noexcept :
				m_parts{ split( str_, divider_ ) }, m_divider{ divider_ } {}

			/// Iterate to next item. 
			constexpr iterator & operator++()	noexcept		{
				m_parts = split( m_parts.rest, m_divider );
				return *this;
			}

			/// Get the string_view of the present element. 
			constexpr Value operator*()		const noexcept	{	return make_view( m_parts.first );	}

			/// Does *not* check equality! Only checks if we are done iterating. 
			constexpr bool operator==( End )	const noexcept	{	return m_parts.first.data() == m_parts.rest.data();	}
		};
		
	public:
		constexpr String_view_splitter( const Value str_, const Divider divider_ ) 	noexcept :
			m_str{ str_ }, m_divider{ divider_ } {}

			template< std::size_t N >
		constexpr String_view_splitter( const std::span< Char, N > str_, const Divider divider_ ) 	noexcept :
			m_str{ make_view( str_ ) }, m_divider{ divider_ } {}

		constexpr iterator begin()				const noexcept	{	return { m_str, m_divider };							}
		constexpr End end()						const noexcept	{	return {};												}
	};

	template< traits::string S, typename D >
	String_view_splitter( S &&, D ) 
		-> String_view_splitter< traits::element_type_t< S >, D, typename std::remove_cvref_t< S >::traits_type >;

	template< traits::character Ch, typename D >
	String_view_splitter( Ch *, D ) -> String_view_splitter< std::remove_reference_t< Ch >, D >;



	/// Return the first newline used in view_ (`"\n"`, `"\r"`, `"\n\r"`, or `"\r\n"`).
	/// - If none is found, `"\n"` is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto identify_newline( V && str_ ) noexcept {
		using my_view = std::basic_string_view< traits::value_type_t< V > >;
		static constexpr const my_view			 	res = { "\n\r\n" };

		// A bit roundabout, but we don't want to risk losing the view's reference.
		const auto here = make_view( find_span( str_, linebreak{} ) );
		return  here.empty()	? res.subview( 0, 1 ) 	// Default value, if no line break was found.
								: res.subview( ( here.front() == '\r' ), here.size() );
	}
}