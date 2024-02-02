//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/from_string.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax { 

	DOCTEST_TEST_CASE( "tuplery check" ) { 
		using Tuple	  = std::tuple< double, const char *, int >;
		const Tuple		t{ 3.2, "Hej!", -4 };

		constexpr const char * strs[ 3 ] = { "3.2", "Hej!", "-4" };
		DOCTEST_FAST_CHECK_EQ( from_string< Tuple >( std::span( strs ) ), t );
	}

}	// namespace pax
