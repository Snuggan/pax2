//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "../../math/metrics/_metrics-key.hpp"
#include <pax/pdal/metrics-infrastructure/_general.hpp>
#include <pax/pdal/metrics-infrastructure/function.hpp>

#include <pax/external/doctest/doctest-pax.hpp>
#include <vector>


namespace pax::metrics { 
	
	DOCTEST_TEST_CASE( "Function" ) {
		DOCTEST_SUBCASE( "parsing id" ) {
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "count"		) ), "count" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "mean"		) ), "mean" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "mean2"		) ), "mean2" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "variance"	) ), "variance" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "skewness"	) ), "skewness" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "kurtosis"	) ), "kurtosis" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "L2"		) ), "L2" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "L3"		) ), "L3" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "L4"		) ), "L4" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "mad"		) ), "mad" ); 
			DOCTEST_FAST_CHECK_EQ( to_string( Function( "p95"		) ), "p95" ); 

			// Badly formed
			DOCTEST_CHECK_THROWS ( Function( "<nonsense>"	) ); 
			DOCTEST_CHECK_THROWS ( Function( "mean3"		) ); 
			DOCTEST_CHECK_THROWS ( Function( "p"			) ); 
			DOCTEST_CHECK_THROWS ( Function( "p-5"			) ); 
			DOCTEST_CHECK_THROWS ( Function( "p195"			) ); 
			DOCTEST_CHECK_THROWS ( Function( "L0"			) ); 
			DOCTEST_CHECK_THROWS ( Function( "L5"			) ); 
			DOCTEST_CHECK_THROWS ( Function( "L2_ratio"		) ); 
		}
		DOCTEST_SUBCASE( "checking the algorithms" ) {
			std::vector< metrics_value_type >	data0{ std::begin( Correct::dataset ), std::end( Correct::dataset ) };
			const std::span 					data ( data0 );
			std::sort( data.begin(), data.end() );
			
			DOCTEST_FAST_CHECK_EQ( Function( "count"		)( data ), Correct::count ); 
			DOCTEST_FAST_CHECK_EQ( Function( "mean"			)( data ), doctest::Approx( Correct::moment1 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "mean2"		)( data ), doctest::Approx( Correct::moment2 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "variance"		)( data ), doctest::Approx( Correct::sample_variance ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "skewness"		)( data ), doctest::Approx( Correct::sample_skewness ).epsilon( 0.0001 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "kurtosis"		)( data ), doctest::Approx( Correct::sample_kurtosis_fisherK ).epsilon( 0.001 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "L2"			)( data ), doctest::Approx( Correct::L2 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "L3"			)( data ), doctest::Approx( Correct::L3 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "L4"			)( data ), doctest::Approx( Correct::L4 ) ); 
			DOCTEST_FAST_CHECK_EQ( Function( "mad"			)( data ), Correct::mad ); 
			DOCTEST_FAST_CHECK_EQ( Function( "p75"			)( data ), Correct::quartile3 ); 
		}
		DOCTEST_SUBCASE( "comparison" ) {
			DOCTEST_FAST_CHECK_EQ( Function( "p95"  ), Function( "p95" ) ); 
			DOCTEST_FAST_CHECK_NE( Function( "p94"  ), Function( "p95" ) ); 
			
			DOCTEST_FAST_CHECK_LT( Function( "p94"  ), Function( "p95" ) ); 
			DOCTEST_FAST_CHECK_LT( Function( "mean" ), Function( "L2" ) ); 
		}
	}
	
}	// namespace pax::metrics
