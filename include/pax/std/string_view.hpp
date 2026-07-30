//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/concepts.hpp>
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

	template< traits::string Str >
	constexpr auto make_view( Str && str_ ) {
		using std::data, std::size;
		return std::basic_string_view< traits::value_type_t< Str > >{ data( str_ ), size( str_ ) };
	}

}	// namespace pax
