//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/pdal/metrics-infrastructure/point-aggregator.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::metrics { 

	class My_pt {
		metrics_value_type			m_z;
		bool						m_first;
		
	public:
		constexpr My_pt( metrics_value_type z_, bool first_ ) : m_z{ z_ }, m_first{ first_ } {}
		friend constexpr metrics_value_type height( const My_pt & pt_ )	noexcept	{	return pt_.m_z;			}
		friend constexpr bool is_first_return( const My_pt & pt_ )		noexcept	{	return pt_.m_first;		}
	};


	DOCTEST_TEST_CASE( "Point_aggregator" ) {
		constexpr My_pt				pts3[ 3 ] = { { 5.0, false }, { 2.0, false }, { 4.0, true } };
		Point_aggregator			acc;
		for( auto item : pts3 )		acc.push_back( item );

		{
			const auto filter	  = Filter( "all" );
			const auto v		  = acc.ordered_span( filter );
			DOCTEST_FAST_CHECK_EQ( v.size(),		3 );
			DOCTEST_FAST_CHECK_EQ( v.front(),		2 );
			DOCTEST_FAST_CHECK_EQ( v.back(),		5 );
		} {
			const auto filter	  = Filter( "all_ge225cm" );
			const auto v		  = acc.ordered_span( filter );
			DOCTEST_FAST_CHECK_EQ( v.size(),		2 );
			DOCTEST_FAST_CHECK_EQ( v.front(),		4 );
			DOCTEST_FAST_CHECK_EQ( v.back(),		5 );
		} {
			const auto filter	  = Filter( "all_ge225cm_lt350cm" );
			const auto v		  = acc.ordered_span( filter );
			DOCTEST_FAST_CHECK_EQ( v.size(),		0 );
		} {
			const auto filter	  = Filter( "1ret" );
			const auto v		  = acc.ordered_span( filter );
			DOCTEST_FAST_CHECK_EQ( v.size(),		1 );
			DOCTEST_FAST_CHECK_EQ( v.front(),		4 );
		} {
			const auto filter	  = Filter( "1ret_ge525cm" );
			const auto v		  = acc.ordered_span( filter );
			DOCTEST_FAST_CHECK_EQ( v.size(),		0 );
		}
	}
	
}	// namespace pax::metrics
