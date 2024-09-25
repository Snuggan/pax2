#pragma once

#include "../concepts.hpp"
#include "../std/format.hpp"

#include <string_view>

#define DOCTEST_QUOTED_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )
#define DOCTEST_QUOTED_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )
#define DOCTEST_QUOTED_WARN_EQ ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ(  pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )
#define DOCTEST_QUOTED_WARN_NE ( __1__, __2__ )	DOCTEST_FAST_WARN_NE(  pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )


namespace pax {

	template< Character Ch, typename Traits = std::char_traits< std::remove_cvref_t< Ch > > >
	struct Quote_it : public std::basic_string_view< std::remove_cvref_t< Ch >, Traits > {
		using string_view  = std::basic_string_view< std::remove_cvref_t< Ch >, Traits >;
		using string_view::string_view;

		constexpr Quote_it( const string_view sv_ ) : string_view{ sv_ } {};

		template< typename Out >
		friend constexpr Out & operator<<( Out & out_, const Quote_it qi_ ) {
			return out_ << std20::format( "{}", qi_ );
		}
	};

	template< typename It, typename EndOrSize >
	Quote_it( It, EndOrSize )	-> Quote_it< std::remove_reference_t< std::iter_reference_t< It > > >;

	template< String S >
	Quote_it( S && )			-> Quote_it< Value_type_t< S >, typename std::remove_cvref_t< S >::traits_type >;

	template< Character Ch, std::size_t N >
	Quote_it( Ch( & c_ )[ N ] )	-> Quote_it< std::remove_reference_t< Ch > >;

	template< Character Ch >
	Quote_it( Ch * const & c_ )	-> Quote_it< std::remove_reference_t< Ch > >;

}	// namespace pax


namespace std20 {

	template< pax::Character Ch, typename Traits >
	struct formatter< pax::Quote_it< Ch, Traits > > {
	private:
		using 				Quote_it		  = pax::Quote_it< Ch, Traits >;
		static constexpr	const Ch *specials[ 32 ] = { 
			"\\0",   "<SOH>", "<STX>", "<ETX>", "<EOT>", "<ENQ>", "<ACK>", "\\a",
			"\\b",   "\\t",   "\\n",   "\\v",   "\\f",   "\\r",   "<SO>",  "<SI>",
			"<DLE>", "<DC1>", "<DC2>", "<DC3>", "<DC4>", "<NAK>", "<SYN>", "<ETB>", 
			"<CAN>", "<EM>",  "<SUB>", "\\e",   "<FS>",  "<GS>",  "<RS>",  "<US>" 
		};
		
		static constexpr const Ch * substitute( const Ch c_ ) {
			switch( c_ ) {
				case '"' : 		return "\\\"";
				case '\\': 		return "\\\\";
				case 0x7f:		return "<DEL>";
				default:		return ( c_ >= ' ' ) ? "" : specials[ std::size_t( c_ ) ];
			}
		}

	public:
		constexpr auto parse( format_parse_context & parse_ctx_ ) {
			const auto	iter	  = parse_ctx_.begin();
			const Ch	c		  = iter != parse_ctx_.end() ? *iter : 0;
			if( ( c != 0 ) && ( c != '}' ) )
				throw format_error( "Invalid Quote_it format specification" );
			return iter;
		}

		constexpr auto format( const Quote_it qi_, format_context & format_ctx_ ) const {
			auto				out	  = format_ctx_.out();
			auto				itr	  = qi_.begin();
			const auto			end	  = qi_.end();
			*out++					  = '"';
			while( itr != end ) {
				auto			itr2  = itr;
				const Ch *		sub{};
				while( ( *( sub = substitute( *itr2 ) ) == 0 ) && ( ++itr2 != end ) );
				if( itr2 != itr ) {
					out = format_to( out, "{}", typename Quote_it::string_view{ itr, itr2 } );
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
