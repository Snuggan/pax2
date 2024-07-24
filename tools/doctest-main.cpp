//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#define DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_SUPER_FAST_ASSERTS


#include <pax/doctest.hpp>


int main( const int argc, const char** argv ) {
	
	// Setup.
	doctest::Context context;
	context.setOption( "no-version", true );
	context.applyCommandLine( argc, argv );

	// Run the tests.
	const auto test_result = context.run();
	if( context.shouldExit() )	return test_result;

	if( pax::skip_slow_tests ) {
		std::cerr <<
			"\n"
			"*****************************************************************************************\n"
			"*******************        NOT DOING THE SLOW STUFF IN doctest        *******************\n"
			"*****************************************************************************************\n"
			"*******************   (Switch in doctests-src/_config-slow.hpp:36.)   *******************\n"
			"*****************************************************************************************\n\n"
			;
	}
	
	// Done.
	return test_result;
}
