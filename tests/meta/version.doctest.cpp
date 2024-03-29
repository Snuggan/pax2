//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/meta/version.hpp>
#include <pax/std/format.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


DOCTEST_TEST_CASE( "versions" ) {
#	if defined( _LIBCPP_STD_VER )
		DOCTEST_FAST_CHECK_EQ( _LIBCPP_STD_VER,			PAX_EXTERNAL_USE_FMT ? 23 : 26 );
#	endif
}
