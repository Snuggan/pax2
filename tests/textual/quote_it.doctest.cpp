//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/quote_it.hpp>

#include <pax/doctest.hpp>


namespace pax {
	
	constexpr char	strange_string[ 39 ] = { 0x7f, 
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 
		10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
		20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
		30, 31, 32, '\'', '"', '\\', 60
	};

	DOCTEST_TEST_CASE( "quote_it" ) { 
		DOCTEST_FAST_CHECK_EQ( std20::format( "{}", Quote_it< char >{ "With \"double\" quotes" } ), "\"With \\\"double\\\" quotes\"" );

		DOCTEST_QUOTED_CHECK_EQ( 
			std20::format( "{}", Quote_it{ strange_string, 38 } ), 
			R"("^?\0^A^B^C^D^E^F\a\b\t\n\v\f\r^N^O^P^Q^R^S^T^U^V^W^X^Y^Z\e^\^]^^^_ '\"\\<")"
		);
	}

}	// namespace pax
