#pragma once

#include "../concepts.hpp"
#include "../std/format.hpp"

#include <string_view>


#define DOCTEST_QUOTED_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )
#define DOCTEST_QUOTED_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )

#define DOCTEST_QUOTED_WARN_EQ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ(  pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )
#define DOCTEST_QUOTED_WARN_NE( __1__, __2__ )	DOCTEST_FAST_WARN_NE(  pax::Quote_it( __1__ ), pax::Quote_it( __2__ ) )


namespace pax {

	template< Character Ch, typename Traits = std::char_traits< std::remove_cvref_t< Ch > > >
	struct Quote_it : public std::basic_string_view< std::remove_cvref_t< Ch >, Traits > {
		using string_view  = std::basic_string_view< std::remove_cvref_t< Ch >, Traits >;

		using string_view::string_view;
		constexpr Quote_it( const string_view sv_ ) : string_view{ sv_ } {};

		template< typename Dest >
		friend constexpr Dest & operator<<(
			Dest			  & dest_,
			const Quote_it		qi_
		) {
			return dest_ << std20::format( "{}", qi_ );
		}
	};


	template< typename It, typename EndOrSize >
	Quote_it( It, EndOrSize )
		-> Quote_it< std::remove_reference_t< std::iter_reference_t< It > > >;

	template< String S >
	Quote_it( S && )
		-> Quote_it< Value_type_t< S >, typename std::remove_cvref_t< S >::traits_type >;

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
		static constexpr	Ch specials[ 67 ] = R"(\0^A^B^C^D^E^F\a\b\t\n\v\f\r^N^O^P^Q^R^S^T^U^V^W^X^Y^Z\e^\^]^^^_)";

	public:
		constexpr auto parse( format_parse_context & parse_ctx_ ) {
			const auto	iter	  = parse_ctx_.begin();
			const char	c		  = iter != parse_ctx_.end() ? *iter : 0;
			if( ( c != 0 ) && ( c != '}' ) )
				throw format_error( "Invalid Quote_it format specification" );
			return iter;
		}

		constexpr auto format( const Quote_it qi_, format_context & format_ctx_ ) const {
			auto				out	  = format_ctx_.out();
			*out					  = '"';
			for( const auto c : qi_ ) {
				switch( c ) {
					case '"' :
					case '\\':			*++out = '\\';	*++out = c;		break;
					case 0x7f:			*++out = '^';	*++out = '?';	break;
					case Ch( 0xa0 ):	*++out = '^';	*++out = '!';	break;
					default:
						if( c >= 0x20 )	*++out = c;
						else {
							*++out = specials[ 2*c ];
							*++out = specials[ 2*c + 1 ];
						}
						break;
				}
			}
			*++out					  = '"';
			return out;
		}

	};
	
}	// namespace std20
