//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "../../math/metrics/_metrics-key.hpp"
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>

#include <pax/textual/json.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::metrics { 
	constexpr auto	nilsson		= 1.82;

	class My_pt {
		metrics_value_type			m_z;
		
	public:
		constexpr My_pt( metrics_value_type z_ ) : m_z{ z_ } {}
		friend constexpr metrics_value_type height( const My_pt & pt_ )	noexcept	{	return pt_.m_z;		}
		friend constexpr bool is_first_return( const My_pt & )			noexcept	{	return false;		}
	};


	DOCTEST_TEST_CASE( "Function_filter" ) {
		{	// comparison
			const auto		a			= Function_filter( "count_1ret", nilsson );
			const auto		b			= Function_filter( "count_1ret_ge182cm" );
			const auto		c			= Function_filter( "count_1ret_ge{}cm_lt190cm", nilsson );
			const auto		d			= Function_filter( "count_all" );

			DOCTEST_FAST_CHECK_EQ( to_string( a ), "count_1ret" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( b ), "count_1ret_ge182cm" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( c ), "count_1ret_ge182cm_lt190cm" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( d ), "count_all" ); 
			
			DOCTEST_FAST_CHECK_EQ( a, a ); 
			DOCTEST_FAST_CHECK_NE( a, b ); 
			DOCTEST_FAST_CHECK_NE( b, c ); 
			DOCTEST_FAST_CHECK_NE( d, a ); 
			
			DOCTEST_FAST_CHECK_LT( a, b ); 
			DOCTEST_FAST_CHECK_LT( c, b ); 
			DOCTEST_FAST_CHECK_LT( d, a ); 
		}
		{	// help string
			// Function_filter::help( std::cerr );
		}
		{	// checking the algorithms
			Point_aggregator	data{ std::span( Correct::dataset ) };
			const auto 			f = [ &data ]( std::string_view id ) { return Function_filter( id ).calculate( data ); };
			
			DOCTEST_FAST_CHECK_EQ( f( "count_all"		),	Correct::count );
			DOCTEST_FAST_CHECK_EQ( f( "mean_all"		),	doctest::Approx( Correct::moment1 ) );
			DOCTEST_FAST_CHECK_EQ( f( "mean2_all"		),	doctest::Approx( Correct::moment2 ) );
			DOCTEST_FAST_CHECK_EQ( f( "variance_all"	),	doctest::Approx( Correct::sample_variance ) );
			DOCTEST_FAST_CHECK_EQ( f( "skewness_all"	),	doctest::Approx( Correct::sample_skewness ).epsilon( 0.0001 ) );
			DOCTEST_FAST_CHECK_EQ( f( "kurtosis_all"	),	doctest::Approx( Correct::sample_kurtosis_fisherK ).epsilon( 0.001 ) );
			DOCTEST_FAST_CHECK_EQ( f( "L2_all"			),	doctest::Approx( Correct::L2 ) );
			DOCTEST_FAST_CHECK_EQ( f( "L3_all"			),	doctest::Approx( Correct::L3 ) );
			DOCTEST_FAST_CHECK_EQ( f( "L4_all"			),	doctest::Approx( Correct::L4 ) );
			DOCTEST_FAST_CHECK_EQ( f( "mad_all"			),	Correct::mad );
			DOCTEST_FAST_CHECK_EQ( f( "p75_all"			),	Correct::quartile3 );
		}
	}
	DOCTEST_TEST_CASE( "metric_id_set" ) {
		{	// generate_filter_algorithm_wrappers
			constexpr const char * collection[] = {
				"skewness_all_ge125cm_lt250cm",
				"count_1ret_ge172cm",
				"p80_all_ge182cm",				// copy (is in basic-linear)
				"basic-linear",
				"count_1ret"					// copy (is in basic-linear)
			};

			const auto result		  = metric_set( std::span{ collection }, nilsson );
			std::size_t 				i{};
			DOCTEST_FAST_CHECK_EQ( result.size(),				8 );
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"skewness_all_ge125cm_lt250cm"	);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"variance_all_ge182cm"			);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"p30_all_ge182cm"				);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"p80_all_ge182cm"				);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"p95_all_ge182cm"				);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"count_1ret"					);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"count_1ret_ge172cm"			);		++i;
			DOCTEST_FAST_CHECK_EQ( to_string( result[ i ] ),	"count_1ret_ge182cm"			);		++i;
			DOCTEST_FAST_CHECK_EQ( result.size(),				i );
		}
	}
	DOCTEST_TEST_CASE( "Textual" ) {
		const auto				id		 = "p93_all_ge34cm_lt500cm";
		const auto				json_str = "{\n  \"filter\": {\n    \"max_level\": 5.0,\n    \"min_level\": 0.3400000035762787,\n    \"points\": \"all points\",\n    \"value\": \"all_ge34cm_lt500cm\"\n  },\n  \"function\": {\n    \"description\": \"Percentile {}, where {} is in [0, 100].\",\n    \"value\": \"p93\"\n  },\n  \"value\": \"p93_all_ge34cm_lt500cm\"\n}";
		const Function_filter	ff( id );
		
		DOCTEST_FAST_CHECK_EQ( to_string( ff ), id );
		DOCTEST_FAST_CHECK_EQ( pax::to_string( to_json( ff ) ), json_str );
	}

}	// namespace pax::metrics
