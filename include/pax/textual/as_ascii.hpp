#pragma once

#include "../concepts.hpp"
#include "../std/format.hpp"

#include <string_view>

#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE(  pax::as_ascii( __1__ ), pax::as_ascii( __2__ ) )


#if defined( FMT_HEADER_ONLY )
#	define PAX_ASCII_TEST_UNUSABLE
#endif

namespace pax {

	template< Character Ch, typename Traits = std::char_traits< std::remove_cvref_t< Ch > > >
	struct Ascii : public std::basic_string_view< std::remove_cvref_t< Ch >, Traits > {
		using string_view  = std::basic_string_view< std::remove_cvref_t< Ch >, Traits >;

		using string_view::string_view;

		constexpr Ascii( const string_view sv_ ) : string_view{ sv_ } {};

		template< typename Out >
		friend constexpr Out & operator<<( Out & out_, const Ascii qi_ ) {
			return out_ << std20::format( "{}", qi_ );
		}
	};

	template< typename It, typename EndOrSize >
	Ascii( It, EndOrSize )		-> Ascii< std::remove_cvref_t< std::iter_reference_t< It > > >;

	template< String S >
	Ascii( S && )				-> Ascii< Value_type_t< S >, typename std::remove_cvref_t< S >::traits_type >;

	template< Contiguous_elements Cont >
	Ascii( Cont && )			-> Ascii< Value_type_t< Cont > >;

	template< Character Ch, std::size_t N >
	Ascii( Ch( & c_ )[ N ] )	-> Ascii< std::remove_cvref_t< Ch > >;

	template< Character Ch >
	Ascii( Ch * const & c_ )	-> Ascii< std::remove_cvref_t< Ch > >;

}	// namespace pax


namespace std20 {

	template< pax::Character Ch, typename Traits >
	struct formatter   < pax::Ascii< Ch, Traits > > {
	private:
		using Ascii = pax::Ascii< Ch, Traits >;
		using string_view = typename Ascii::string_view;
		static constexpr	string_view specials[ 32 ] = { 
			"\\0",   "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "\\a",
			"\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "<SO>",  "<SI>",
			"<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>", 
			"<CAN>", "<EM>",  "<SUB>", "\\e",   "<FS>",  "<GS>",  "<RS>",  "<US>" 
		};
		
		static constexpr string_view substitute0( const Ch c_ ) {
			switch( c_ ) {
				case '"' : 		return "\\\"";
				case '\\': 		return "\\\\";
				case 0x7f:		return "<DEL>";
				default:		return ( c_ >= ' ' ) ? string_view{} : specials[ std::size_t( c_ ) ];
			}
		}

	public:
		static constexpr std::basic_string< Ch, Traits > substitute( const Ch c_ ) {
			const string_view	result{ substitute0( c_ ) };
			return result.empty() ? std::basic_string< Ch, Traits >{ c_ } : std::basic_string< Ch, Traits >{ result };
		}

		constexpr auto parse( format_parse_context & parse_ctx_ ) {
			auto	iter	  = parse_ctx_.begin();
			auto get_char	  = [ & ]() { return iter != parse_ctx_.end() ? *iter : '}'; };
			Ch c			  = get_char();

			if( c != '}' ) {
				// Process c ...
				// ++iter;			// ... and increments iter ...
				// c = get_char();	// ... and read the next character
			}

			if( c != '}' )		throw format_error( "Invalid Ascii format specification" );
			return iter;
		}

		constexpr auto format( const Ascii qi_, format_context & format_ctx_ ) const {
			auto				out	  = format_ctx_.out();
			auto				itr	  = qi_.begin();
			const auto			end	  = qi_.end();
			*out++					  = '"';
			while( itr != end ) {
				auto			itr2  = itr;
				string_view		sub{};
				while( ( ( sub = substitute0( *itr2 ) ).size() == 0u ) && ( ++itr2 != end ) );
				if( itr2 != itr ) {
					out = format_to( out, "{}", string_view{ itr, itr2 } );
					itr = itr2;
				}
				if( itr2 != end ) {
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

	template< Character Ch >
	constexpr std::basic_string< Ch > as_ascii( const Ch c_ )	noexcept	{
		using Traits = std::char_traits< std::remove_cvref_t< Ch > >;
		return std20::formatter< pax::Ascii< Ch, Traits > >::substitute( c_ );
	}

	template< String Str >
	constexpr auto as_ascii( const Str & str_ )					noexcept	{
		return std20::format( "{}", Ascii{ str_ } );
	}

}	// namespace pax
