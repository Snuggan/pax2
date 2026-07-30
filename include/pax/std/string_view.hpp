//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/types/point-stuff/contiguous.hpp>
#include <string_view>


#if defined( NDEBUG )
#	define TEST( ... )		
#else
#	define TEST( ... )		static_assert( __VA_ARGS__ );
#endif


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
	using std::data, std::begin;

	template< traits::character Char, std::size_t N >
	constexpr std::basic_string_view< std::remove_cv_t< Char > > make_view( const std::span< Char, N > sp_ ) {
		return { sp_.data(), sp_.size() };
	}


	/// Returns a basic_string_view of the first i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto view_first( 
		V					 && v_, 
		const std::size_t 		i_ = 1 
	) noexcept {
		const auto				sz = no_nullchar_size( v_ );
		return std::basic_string_view( data( v_ ), std::min( i_, sz ) );
	}
	TEST( view_first( "abcdefghi",  3 ) == "abc" );
	TEST( view_first( "abcdefghi", 12 ) == "abcdefghi" );

	/// Returns a basic_string_view of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( end( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto view_not_first( 
		V					 && v_, 
		const std::size_t 		i_ = 1 
	) noexcept {
		const auto				sz = no_nullchar_size( v_ );
		return ( i_ < sz )	?	std::basic_string_view( data( v_ ) + i_, sz - i_ )
							: 	std::basic_string_view( data( v_ ) + sz, 0 );
	}
	TEST( view_not_first( "abcdefghi",  3 ) == "defghi" );
	TEST( view_not_first( "abcdefghi", 12 ) == "" );

	/// Returns a basic_string_view of the last i_ elements of v_.
	///	- If i_ > size( v_ ), basic_string_view( v_ ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto view_last( 
		V					 && v_, 
		const std::size_t 		i_ = 1 
	) noexcept {
		const auto				sz = no_nullchar_size( v_ );
		return ( i_ < sz )	?	std::basic_string_view( data( v_ ) + sz - i_, i_ )
							: 	std::basic_string_view( v_ );
	}
	TEST( view_last( "abcdefghi",  3 ) == "ghi" );
	TEST( view_last( "abcdefghi", 12 ) == "abcdefghi" );

	/// Returns a basic_string_view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty basic_string_view( begin( v_ ) ) is returned.
	template< traits::string V >
	[[nodiscard]] constexpr auto view_not_last( 
		V					 && v_, 
		const std::size_t 	i_ = 1 
	) noexcept {
		const auto				sz = no_nullchar_size( v_ );
		return std::basic_string_view( data( v_ ), ( i_ < sz ) ? sz - i_ : 0u );
	}
	TEST( view_not_last( "abcdefghi",  3 ) == "abcdef" );
	TEST( view_not_last( "abcdefghi", 12 ) == "" );

	/// Returns a basic_string_view of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< traits::string V >
	[[nodiscard]] constexpr auto view_mid( 
		V					 && v_, 
		std::ptrdiff_t 			offs_, 
		const std::size_t 		n_ 
	) noexcept {
		const auto sz = no_nullchar_size( v_ );
		offs_ =	( offs_ >= 0 )	?		std::min( std::size_t(  offs_ ), sz )
								: sz -	std::min( std::size_t( -offs_ ), sz );
		return std::basic_string_view{ data( v_ ) + offs_, std::min( sz - offs_, n_ ) };
	}
	TEST( view_mid( "abcdefghi",  2,  3 ) == "cde" );
	TEST( view_mid( "abcdefghi", -5,  3 ) == "efg" );
	TEST( view_mid( "abcdefghi",  2, 12 ) == "cdefghi" );
	TEST( view_mid( "abcdefghi", -5, 12 ) == "efghi" );


	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		V			 && v_, 
		Pred		 && p_ 
	) noexcept {
		auto			itr = begin( v_ );
		const auto		end = no_nullchar_end( v_ );
		while( ( itr != end ) && p_( *itr ) )		++itr;
		return std::basic_string_view{ itr, end };
	}

	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		V								 && v_, 
		const traits::value_type_t< V >   & t_ 
	) noexcept {
		return trim_first( v_, [ t_ ]( auto c ){ return c == t_; } );
	}
	TEST( trim_first( "", '+' ) 			==	"" );
	TEST( trim_first( "++++abcdef++", '+' ) ==	"abcdef++" );

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_first( 
		V			 && v_, 
		linebreak 
	) noexcept {
		return trim_first( v_, linebreak::check );
	}
	TEST( trim_first( "abcdefgh",			linebreak{} )	==	"abcdefgh" );
	TEST( trim_first( "\n\r",				linebreak{} )	==	"" );
	TEST( trim_first( "\n\rabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );

	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, traits::string V >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		V			 && v_, 
		Pred		 && p_ 
	) noexcept {
		const auto		b   = begin( v_ );
		auto			itr = no_nullchar_end( v_ );
		if( itr != b )	while( ( --itr != b ) && p_( *itr ) );
		return std::basic_string_view{ b, itr + 1 - p_( *itr ) };
	}

	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		V								 && v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return trim_last( v_, [ t_ ]( auto c ){ return c == t_; } );
	}
	TEST( trim_last( "", '+' ) 			   ==	"" );
	TEST( trim_last( "++++abcdef++", '+' ) ==	"++++abcdef" );

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< traits::string V >
	[[nodiscard]] constexpr auto trim_last( 
		V			 && v_, 
		linebreak 
	) noexcept {
		return trim_last( v_, linebreak::check );
	}
	TEST( trim_last( "abcdefgh",			linebreak{} )	==	"abcdefgh" );
	TEST( trim_last( "\n\r",				linebreak{} )	==	"" );
	TEST( trim_last( "\n\rabcdefgh\n\r",	linebreak{} )	==	"\n\rabcdefgh" );

	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< traits::string V, typename T >
	[[nodiscard]] constexpr auto trim( 
		V			 && v_, 
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
}	// namespace pax
#undef TEST
