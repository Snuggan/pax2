#pragma once

#include "../concepts.hpp"
#include "../std/format.hpp"

#include <iostream>
#include <string>
#include <string_view>


namespace pax {

	template< Character Ch >
	struct Quote_it {
		using string_view	  = std::basic_string_view< Ch >;
		using size_type		  = string_view::size_type;

		constexpr Quote_it( const string_view str_ ) : m_sv( str_ ) {}
		string_view				m_sv;
	};

}	// namespace pax


namespace std20 {

	template< pax::Character Ch >
	struct formatter< pax::Quote_it< Ch > > {
	private:
		using size_type = pax::Quote_it< Ch >::size_type;

		static constexpr Ch specials[ 67 ] = {
			"\\0^A^B^C^D^E^F\\a\\b\\t\\n\\v\\f\\r^N^O" 
			"^P^Q^R^S^T^U^V^W^X^Y^Z\\e^\\^]^^^_"
		};

		char			m_quote	  = '"';
		char			m_esc	  = '\\';

	public:
		template< typename Dest >
		friend constexpr Dest & operator<<(
			Dest			  & dest_,
			const pax::Quote_it< Ch >		v_
		) {
			for( auto c : v_.m_sv ) switch( c ) {
				case '"' :
				case '\\':		dest_ << '\\' << c;			break;
				case 0x7f:		dest_ << '^'  << '?';		break;
				case 0xa0:		dest_ << '^'  << '!';		break;
				default: 		
					( c <  0x20 )	? ( dest_ << specials[ 2*(int)c ] << specials[ 2*(int)c + 1 ] )
									: ( dest_ << c );
			}
			return dest_;
		}

		constexpr auto parse( format_parse_context & parse_ctx_ ) {
			auto		iter	  = parse_ctx_.begin();
			auto		get_char  = [&]() { return iter != parse_ctx_.end() ? *iter : 0; };
			char		c		  = get_char();
			if( c == 0 || c == '}' )
				return iter;

			m_quote = c;
			++iter;

			if( ( c = get_char() ) != 0 && c != '}' ) {
				m_esc = c;
				++iter;
			}

			if( (c = get_char() ) != 0 && c != '}' )
				throw format_error( "Invalid Quote_it format specification" );
			return iter;
		}

		auto format( const pax::Quote_it< Ch > & qi_, format_context & format_ctx_ ) const {
			size_type		pos	  = 0;
			const size_type	end	  = qi_.m_sv.length();
			auto			out	  = format_ctx_.out();
			*out++ = m_quote;
			while( pos < end ) {
				auto c = qi_.m_sv[ pos++ ];
				if( c == m_quote || c == m_esc )
					*out++ = m_esc;
				*out++ = c;
			}
			*out++ = m_quote;
			return out;
		}

	};
	
}	// namespace std20
