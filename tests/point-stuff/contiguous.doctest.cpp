//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/contiguous.hpp>
#include <pax/doctest.hpp>
#include <pax/debug.hpp>


namespace pax {
	
	DOCTEST_TEST_CASE( "contiguous" ) {
		DOCTEST_ASCII_CHECK_EQ( make_span( "" ), "" );
		DOCTEST_ASCII_CHECK_EQ( trim_last( "", '+' ), "" );
	}

}	// namespace pax
