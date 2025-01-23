#pragma once

#include "../std/string_view.hpp"
#include "../std/format.hpp"


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


#if defined( FMT_HEADER_ONLY )
#	define PAX_ASCII_TEST_UNUSABLE
#endif

#if PAX_USE_STD_FORMAT
#	define PAX_FORMATTER_RESOLVER
#else
#	define PAX_FORMATTER_RESOLVER , Ch, std::true_type
#endif


namespace std20 {

	template< pax::Character Ch, typename Traits >
	struct formatter< pax::Ascii< Ch, Traits > PAX_FORMATTER_RESOLVER > {
		constexpr auto parse( format_parse_context & parse_ctx_ ) {
			auto	iter	  = parse_ctx_.begin();
			auto	get_char  = [ & ]() { return iter != parse_ctx_.end() ? *iter : '}'; };
			Ch		c		  = get_char();

			// if( c != '}' ) {
			// 	// Process c ...
			// 	// ++iter;			// ... and increments iter ...
			// 	// c = get_char();	// ... and read the next character
			// }

			if( c != '}' )		throw format_error( "Invalid Ascii format specification" );
			return iter;
		}

		constexpr auto format( const pax::Ascii< Ch, Traits > qi_, format_context & format_ctx_ ) const {
			auto				out	  = format_ctx_.out();
			auto				itr	  = qi_.begin();
			const auto			end	  = qi_.end();
			*out++					  = '"';
			while( itr != end ) {
				auto			itr2  = itr;
				string_view		sub{};
				while( ( ( sub = pax::Ascii< Ch, Traits >::substitute_view( *itr2 ) ).size() == 0u ) && ( ++itr2 != end ) );
				if( itr2 != itr ) {
					out = format_to( out, "{}", basic_string_view{ itr, itr2 } );
					itr = itr2;
				} else if( itr2 != end ) {
					out = format_to( out, "{}", sub );
					++itr;
				}
			}
			*out++					  = '"';
			return out;
		}
	};
	
}	// namespace std20

namespace pax {

	template< typename Out, typename Ch, typename Traits >
	constexpr Out & operator<<( Out & out_, const Ascii< Ch, Traits > qi_ ) {
		return out_ << std20::format( "{}", qi_ );
	}

	template< Character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch c_ )	noexcept	{
		const auto view = pax::Ascii< Ch >::substitute_view( c_ );
		return view.size()	? std20::format( "\"{}\"", view )
							: std20::format( "\"{}\"", c_ );
	}

	template< String S >
	constexpr auto as_ascii( const S & str_ )					noexcept	{
		return std20::format( "{}", Ascii{ str_ } );
	}

}	// namespace pax
