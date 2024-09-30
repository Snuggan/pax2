//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/as_ascii.hpp>

#include <pax/doctest.hpp>


namespace pax {
	
	constexpr std::size_t sz = 37;
	constexpr char	strange_string[ sz + 1 ] = { 
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
		30, 31, 32, '\'', '"', '\\', 0x7f
	};

	DOCTEST_TEST_CASE( "quote_it" ) { 
		DOCTEST_FAST_CHECK_EQ( std20::format( "{}", Ascii< char >{} ), "\"\"" );
		DOCTEST_FAST_CHECK_EQ( std20::format( "{}", Ascii{ "With \"double\" quotes" } ), "\"With \\\"double\\\" quotes\"" );

		DOCTEST_ASCII_CHECK_EQ( 
			std20::format( "{}", Ascii{ strange_string, sz } ), 
				"\"\\0<SOH><STX><ETX><EOT><ENQ><ACK>\\a\\b\\t\\n\\v\\f\\r<SO><SI>"
				"<DLE><DC1><DC2><DC3><DC4><NAK><SYN><ETB><CAN><EM><SUB>\\e<FS><GS><RS><US> '\\\"\\\\<DEL>\""
		);
	}

	DOCTEST_TEST_CASE( "as_ascii" ) { 
		DOCTEST_FAST_CHECK_EQ( as_ascii( char( 0 ) ), 		"\\0" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( '\n' ),			"\\n" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( char( 4 ) ), 		"<EOT>" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( '\\' ), 			"\\\\" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( '"' ), 			"\\\"" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( char( 0x7f ) ), 	"<DEL>" );
		DOCTEST_FAST_CHECK_EQ( as_ascii( '&' ), 			"&" );

		DOCTEST_FAST_CHECK_EQ( as_ascii( std::string_view{ strange_string, sz } ), 
			"\"\\0<SOH><STX><ETX><EOT><ENQ><ACK>\\a\\b\\t\\n\\v\\f\\r<SO><SI>"
			"<DLE><DC1><DC2><DC3><DC4><NAK><SYN><ETB><CAN><EM><SUB>\\e<FS><GS><RS><US> '\\\"\\\\<DEL>\""
		);
	}

}	// namespace pax
