//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <filesystem>
#include <source_location>
#include <iostream>			// std::cerr
#include <cstdlib>			// std::abort


namespace pax {


	inline static std::filesystem::path doctest_data_root( 
		const std::source_location loc_ = std::source_location::current() 
	) {
		static constexpr auto dir = "doctests-data";

		static constexpr const char * alternatives[] = {
			"", 
			"..", 
			"../..", 
			"~/develop/pax"
		};


		for( const std::filesystem::path p : alternatives )
			if( std::filesystem::exists( p / dir ) )	return p / dir;

		std::cerr << "*** No test data directory '" << dir << "' found... ***\nTried the following:\n";
		std::error_code				ec;
		for( const std::filesystem::path p : alternatives ) {
			const auto can = std::filesystem::canonical( p, ec );
			std::cerr << '\t' << ( ( ec ? p : can ) / dir ).native() << '\n';
		}
		std::cerr << "(called from '" << loc_.file_name() << ':' << loc_.line() << "')\n";
		std::abort();
		return {};
	}


	constexpr const bool skip_slow_tests	  = false;// || true;
	constexpr const bool detailed_tests		  = false;
	constexpr const bool output_debug_info	  = detailed_tests && false; 		// Outputs extra information to std::cerr.
}	// namespace pax


#define PAX_ACTIVATE_DOCTEST

// Info on options: https://github.com/onqtam/doctest/blob/master/doc/markdown/configuration.md

// This will remove all macros from doctest that don't have the DOCTEST_ prefix - like CHECK, TEST_CASE and SUBCASE.
// Then only the full macro names will be available - DOCTEST_CHECK, DOCTEST_TEST_CASE and DOCTEST_SUBCASE. The user
// is free to make his own short versions of these macros - example.
//
// This can be defined both globally and in specific source files only.
#define DOCTEST_CONFIG_NO_SHORT_MACRO_NAMES

// By default char* is being treated as a pointer. With this option comparing char* pointers will switch to using strcmp()
// for comparisons and when stringified the string will be printed instead of the pointer value.
//
// This should be defined globally.
#define DOCTEST_CONFIG_TREAT_CHAR_STAR_AS_STRING

// This makes the fast assert macros (FAST_CHECK_EQ(a,b) - with a FAST_ prefix) compile even faster! (50-80%)
//
// Each fast assert is turned into a single function call - the only downside of this is: if an assert fails and a
// debugger is attached - when it breaks it will be in an internal function - the user will have to go 1 level up in
// the callstack to see the actual assert.
//
// This can be defined both globally and in specific source files only.
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS

// This will remove support for colors in the console output of the framework.
// This should be defined only in the source file where the library is implemented (it's relevant only there).
#define DOCTEST_CONFIG_COLORS_NONE


#include <doctest/doctest.h>
