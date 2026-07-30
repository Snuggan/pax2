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
