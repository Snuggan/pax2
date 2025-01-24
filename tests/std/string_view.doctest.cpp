//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/string_view.hpp>
#include <pax/doctest.hpp>
#include <sstream>


#if !defined( PAX_ASCII_TEST_UNUSABLE )
namespace pax {
	
	constexpr std::size_t sz = 37;
	constexpr char	strange_string[ sz + 1 ] = { 
		'a', 1,  2,  3,  4,  5,  6,  7,  8,  9, 
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
		30, 31, 32, '\'', '"', '\\', 0x7f
	};

	DOCTEST_TEST_CASE( "quote_it" ) { 
		DOCTEST_FAST_CHECK_EQ( as_ascii( "" ), "" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( "With \"double\" quotes" ), "With \\\"double\\\" quotes" );

		DOCTEST_ASCII_CHECK_EQ( 
			as_ascii( strange_string ), 
				"a<SOH><STX><ETX><EOT><ENQ><ACK>\\a\\b\\t\\n\\v\\f\\r<SO><SI>"
				"<DLE><DC1><DC2><DC3><DC4><NAK><SYN><ETB><CAN><EM><SUB>\\e<FS><GS><RS><US> '\\\"\\\\<DEL>"
		);
	}
	DOCTEST_TEST_CASE( "as_ascii" ) { 
		DOCTEST_FAST_CHECK_EQ( as_ascii( std::string_view{ strange_string, sz } ), 
			"a<SOH><STX><ETX><EOT><ENQ><ACK>\\a\\b\\t\\n\\v\\f\\r<SO><SI>"
			"<DLE><DC1><DC2><DC3><DC4><NAK><SYN><ETB><CAN><EM><SUB>\\e<FS><GS><RS><US> '\\\"\\\\<DEL>"
		);
		{	// with text technical
			DOCTEST_FAST_CHECK_EQ( invisible_character_name( 'a' ),			"" );
			DOCTEST_FAST_CHECK_EQ( invisible_character_name( char( 1 ) ),	"<SOH>" );
			DOCTEST_FAST_CHECK_EQ( invisible_character_name( char( 7 ) ),	"\\a" );

			DOCTEST_FAST_CHECK_EQ( as_ascii( char( 1 ) ),		"<SOH>" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( char( 7 ) ),		"\\a" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( 'a' ),				"a" );

			std::ostringstream		os;
			constexpr auto			v0 = std::string_view( ">\0\a\b\t\n\v\f\r\"'\x18\x7f <", 15 );
			constexpr auto			vr = std::string_view( ">\\0\\a\\b\\t\\n\\v\\f\\r\\\"'<CAN><DEL> <" );
			os << as_ascii( v0 );
			const auto 				res0{ os.str() };
			DOCTEST_FAST_CHECK_EQ( res0,		vr );
			DOCTEST_FAST_CHECK_EQ( res0.size(),	vr.size() );
		} { 
			DOCTEST_FAST_CHECK_EQ( as_ascii( '&' ), 			"&" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( 'A' ), 			"A" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( char( 0 ) ), 		"\\0" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( '\n' ),			"\\n" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( char( 4 ) ), 		"<EOT>" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( '\\' ), 			"\\\\" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( '"' ), 			"\\\"" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( char( 0x7f ) ), 	"<DEL>" );

			DOCTEST_FAST_CHECK_EQ( as_ascii( "a" ),				"a" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( "abc" ),			"abc" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( "\a" ),			"\\a" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( "abc\a" ),			"abc\\a" );
			DOCTEST_FAST_CHECK_EQ( as_ascii( "\aabc" ),			"\\aabc" );

			DOCTEST_FAST_CHECK_EQ( as_ascii( "abcd\t" ),		"abcd\\t" );
		}
	}
}	// namespace pax
#endif
