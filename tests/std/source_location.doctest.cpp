//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/reporting/error_message.hpp>
#include <pax/reporting/debug.hpp>
#include <pax/external/doctest/doctest-pax.hpp>
#include <pax/reporting/as_ascii.hpp>

namespace pax {

	constexpr std::source_location this_source_location( 
		const std::source_location & this_ = std::source_location::current() 
	) noexcept	{	return this_;	}

	DOCTEST_TEST_CASE( "debug std::source_location" ) {
		const auto loc		  = this_source_location();
		const auto good		  = std::string( "pax2/tests/std/source_location.doctest.cpp:19" );
		const auto path		  = to_string( loc );
		DOCTEST_ASCII_CHECK_EQ( path.substr( path.size() - good.size(), good.size() ),	good );
		DOCTEST_FAST_CHECK_EQ ( loc.line(),				19 );
		DOCTEST_FAST_CHECK_EQ ( sizeof( loc ),			 8 );

		DOCTEST_FAST_CHECK_EQ ( to_string( std::source_location{} ), "<unspecified source location>:0" );
	}
	
}	// namespace pax
