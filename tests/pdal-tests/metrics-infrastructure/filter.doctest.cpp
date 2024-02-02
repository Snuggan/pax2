//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/pdal/metrics-infrastructure/filter.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::metrics { 

	DOCTEST_TEST_CASE( "Filter values" ) {
		static constexpr metrics_value_type	Min = -std::numeric_limits< metrics_value_type >::infinity();
		static constexpr metrics_value_type	Max = +std::numeric_limits< metrics_value_type >::infinity();
		{
			const auto filter = Filter( "all" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"all" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		false );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		Min );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		Max );
		} {
			const auto filter = Filter( "all_ge125cm" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"all_ge125cm" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		false );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		1.25 );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		Max );
		} {
			const auto filter = Filter( "all_ge125cm_lt250cm" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"all_ge125cm_lt250cm" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		false );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		1.25 );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		2.5 );
		} {
			const auto filter = Filter( "1ret" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"1ret" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		true );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		Min );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		Max );
		} {
			const auto filter = Filter( "1ret_ge125cm" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"1ret_ge125cm" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		true );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		1.25 );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		Max );
		} {
			const auto filter = Filter( "1ret_ge125cm_lt250cm" );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"1ret_ge125cm_lt250cm" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		true );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		1.25 );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		2.5 );
		} {
			const auto filter = Filter( "all_ge50cm_lt{}cm", 2.5 );
			DOCTEST_FAST_CHECK_EQ( to_string( filter ),		"all_ge50cm_lt250cm" );
			DOCTEST_FAST_CHECK_EQ( filter.first_only(),		false );
			DOCTEST_FAST_CHECK_EQ( filter.min_level(),		0.5 );
			DOCTEST_FAST_CHECK_EQ( filter.max_level(),		2.5 );
		} {
			DOCTEST_FAST_CHECK_EQ( Filter( "all_lt65536cm" ), Filter( "all" ) );
		} {
			DOCTEST_CHECK_THROWS ( Filter( "" ) );
			DOCTEST_CHECK_THROWS ( Filter( "dummy" ) );
			DOCTEST_CHECK_THROWS ( Filter( "all_ge65536cm" ) );
			DOCTEST_CHECK_THROWS ( Filter( "all_ge-22cm" ) );
			DOCTEST_CHECK_THROWS ( Filter( "all_lt-22cm" ) );
		}
	}
	DOCTEST_TEST_CASE( "Filter comparison" ) {
		DOCTEST_FAST_CHECK_EQ( Filter( "all" ), Filter( "all" ) ); 
		DOCTEST_FAST_CHECK_NE( Filter( "all" ), Filter( "1ret" ) ); 
		DOCTEST_FAST_CHECK_NE( Filter( "all" ), Filter( "all_ge125cm" ) ); 
		
		DOCTEST_FAST_CHECK_LT( Filter( "all" ), Filter( "all_ge125cm" ) ); 
		DOCTEST_FAST_CHECK_LT( Filter( "all" ), Filter( "1ret" ) ); 
	}
	
}	// namespace pax::metrics
