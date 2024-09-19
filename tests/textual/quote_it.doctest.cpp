//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/quote_it.hpp>

#include <pax/doctest.hpp>


namespace pax {
	
	DOCTEST_TEST_CASE( "quote_it" ) { 
		DOCTEST_FAST_CHECK_EQ( std20::format( "{}", Quote_it< char >( R"(With "double" quotes)" ) ), R"("With \"double\" quotes")" );
	}

}	// namespace pax
