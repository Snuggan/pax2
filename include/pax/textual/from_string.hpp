//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../reporting/error_message.hpp"
#include "../concepts.hpp"

#include <span>

// #include <charconv>
// Since clang has not yet implemented std::from_chars for floating point values, I use fast_float instead. 
#include "../external/fast_float/fast_float.h"


namespace pax {
	
	template< typename T >
	struct From_string;


	/// Convenience function, calls From_string< T >::from_string( attrs_ ).
	///
	template< typename T, typename ...U >
	constexpr T from_string(
		U					 && ...u_
	) {
		return From_string< std::remove_cv_t< T > >::from_string( std::forward< U >( u_ )... );
	}


	/// Handles string-like objects. Just returns them. 
	///
	template< String Str >
	struct From_string< Str > {
		template< String Str2 >
		static constexpr Str from_string( const Str2 & str_ )	noexcept	{	return Str( str_ );		}
	};



	/// Handles numerical values.
	///
	template< typename Num >
		requires( std::integral< Num > || std::floating_point< Num > )
	struct From_string< Num > {
	private:
		static auto report( const std::string_view text_ ) {
			return error_message( std20::format( "Could not convert '{}' to numerical.", text_ ) );
		}

	public:
		template< String Str >
		static constexpr Num from_string( const Str & str_ ) {
			std::string_view				text{ str_ };

			if( text.empty() )				throw report( "<empty string>" );
			if( text.front() == '+' )		text = text.substr( 1 );
			
			// Since clang has not yet implemented std::from_chars for floating point values, I use fast_float instead. 
			Num								num{};
			const auto [ p, ec ]		  = fast_float::from_chars( text.data(), text.data() + text.size(), num );
			if( ec != std::errc{} )			throw report( text );
			return num;
		}
	};


	
	
	/// Handles tuple-like objects. 
	///
	template< Has_static_size Tuple >
	struct From_string< Tuple > {
	private:
		static constexpr std::size_t rank	  = std::tuple_size_v< Tuple >;
		using Indices						  = std::make_index_sequence< rank >;

		template< String Str, std::size_t ...I >
		static constexpr Tuple from_string(
			const std::span< Str, rank >	strs_,
			std::integer_sequence< std::size_t, I... >
		) {
			static_assert( ( ... && ( I < rank ) ) );
			using std::get;
			return Tuple( From_string< std::tuple_element_t< I, Tuple > >::from_string( strs_[ I ] )... );
		}

	public:
		/// Convert from a string formed like "{ value1, value2, ... }". 
		///
		template< Character Ch, std::size_t N >
		static constexpr Tuple from_string( 
			const std::span< Ch, N >		str_ 
		);

		/// Convert from a set of strings "value1", "value2", ...
		///
		template< String Str, std::size_t N >
		static constexpr Tuple from_string( 
			const std::span< Str, N >		strs_ 
		) {
			static_assert( N >= rank );
			return from_string( strs_.template first< N >(), Indices{} );
		}
	};

}	// namespace pax
