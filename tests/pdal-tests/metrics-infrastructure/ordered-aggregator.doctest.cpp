//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "../../math/metrics/_metrics-key.hpp"
#include <pax/pdal/metrics-infrastructure/ordered-aggregator.hpp>

#include <pax/math/metrics/ordered.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::metrics { 

	template< typename Dest, typename V >
	constexpr void push_back_many_( Dest & dest_, const std::initializer_list< V > & c_ ) {
		dest_.reserve( dest_.size() + c_.size() );
		for( auto e : c_ )		dest_.push_back( e );
	}

	template< typename T, std::size_t N >
	auto slow_mad( const std::span< T, N > data_ ) {
		auto			ord   = Ordered_vector< double >( data_ );
		auto			ordrd( ord.ordered_span() );
		auto			diffs = Ordered_vector< double >{};
		const auto		med   = ordered::median( ordrd );
		for( auto item : ordrd )
			diffs.push_back( ( item > med ) ? ( item - med ) : ( med - item ) );
		return ordered::median( diffs.ordered_span() );
	}


	DOCTEST_TEST_CASE( "ordered statistics" ) {
		const std::span		dataset( Correct::dataset );
		const Ordered_vector< double > ord( dataset );
	
		DOCTEST_SUBCASE( "many elements" ) {
			auto s0						  = ord;
			const auto s 				  = s0.ordered_span();

			DOCTEST_FAST_CHECK_EQ( s.size(),								Correct::count );
			DOCTEST_FAST_CHECK_EQ( ordered::min( s ),						Correct::minimum );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 0 ),				Correct::minimum );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 1 ),				Correct::quartile1 );
			DOCTEST_FAST_CHECK_EQ( ordered::median( s ),					Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).median(),		Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),			slow_mad( std::span{ Correct::dataset } ) );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 2 ),				Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 3 ),				Correct::quartile3 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 4 ),				Correct::maximum );
			DOCTEST_FAST_CHECK_EQ( ordered::max( s ),						Correct::maximum );

			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),			doctest::Approx( Correct::mad ) );

			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 1 >( s ),				doctest::Approx( Correct::L1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 2 >( s ),				doctest::Approx( Correct::L2 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 3 >( s ),				doctest::Approx( Correct::L3 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 4 >( s ),				doctest::Approx( Correct::L4 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 5 >( s ),				doctest::Approx( Correct::L5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 9 >( s ),				doctest::Approx( Correct::L9 ) );
			{
				const ordered::L_moment_array< 9, const double >			Larr( s );
				DOCTEST_FAST_CHECK_EQ( Larr[ 1 ],							doctest::Approx( Correct::L1 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 2 ],							doctest::Approx( Correct::L2 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 3 ],							doctest::Approx( Correct::L3 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 4 ],							doctest::Approx( Correct::L4 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 5 ],							doctest::Approx( Correct::L5 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 9 ],							doctest::Approx( Correct::L9 ) );
			}

			DOCTEST_FAST_CHECK_EQ( ordered::L_moment_ratio< 3 >( s ),		doctest::Approx( Correct::L3_ratio ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment_ratio< 4 >( s ),		doctest::Approx( Correct::L4_ratio ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 0 ),			doctest::Approx( Correct::TL1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 0 ),			doctest::Approx( Correct::TL2 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 0 ),			doctest::Approx( Correct::TL3 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 0 ),			doctest::Approx( Correct::TL4 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 0 ),			doctest::Approx( Correct::TL5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 9 >( s, 0 ),			doctest::Approx( Correct::TL9 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 1 ),			doctest::Approx( Correct::TL1_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 1 ),			doctest::Approx( Correct::TL2_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 1 ),			doctest::Approx( Correct::TL3_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 1 ),			doctest::Approx( Correct::TL4_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 1 ),			doctest::Approx( Correct::TL5_1 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 3, 5 ),		doctest::Approx( Correct::TL1_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 3, 5 ),		doctest::Approx( Correct::TL2_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 3, 5 ),		doctest::Approx( Correct::TL3_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 3, 5 ),		doctest::Approx( Correct::TL4_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 3, 5 ),		doctest::Approx( Correct::TL5_3_5 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment_ratio< 3 >( s, 3, 5 ),	doctest::Approx( Correct::TL3_3_5_ratio ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment_ratio< 4 >( s, 3, 5 ),	doctest::Approx( Correct::TL4_3_5_ratio ) );

			DOCTEST_FAST_CHECK_EQ( ordered::quantile( s, 0.25 ),			Correct::quartile1 );
			DOCTEST_FAST_CHECK_EQ( ordered::percentile( s, 25 ),			Correct::quartile1 );

			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::min( s ) - 1 ),		 0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::min( s ) ),	   		 0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, 1800 ),	 	   				 3 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::max( s ) ),	   		14 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::max( s ) + 1 ),		15 );

			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::min( s ) - 1 ),		15 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::min( s ) ),			15 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, 1800 ),						12 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::max( s ) ),	 		 1 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::max( s ) + 1 ), 	 0 );
		}
		DOCTEST_SUBCASE( "partial" ) {
			{
				auto s0{ ord };
				const auto original_size{ size( s0 ) };
				push_back_many_( s0, { 3000, 3100, 3200, 3300 } );
				auto s = s0.ordered_span();
				DOCTEST_FAST_CHECK_EQ( s.size(),			original_size + 4 );
				DOCTEST_FAST_CHECK_EQ( ordered::max( s ),	3300 );
			} {
				auto s0{ ord };
				const auto original_size{ size( s0 ) };
				push_back_many_( s0, { 10, 11, 12, 13 } );
				auto s = s0.ordered_span();
				DOCTEST_FAST_CHECK_EQ( s.size(),			original_size + 4 );
				DOCTEST_FAST_CHECK_EQ( ordered::min( s ),	10 );
			} {
				auto s0{ ord };
				const auto original_size{ size( s0 ) };
				push_back_many_( s0, { 10, 11, 12, 13, 3000, 3100, 3200, 3300 } );
				auto s = s0.ordered_span();
				DOCTEST_FAST_CHECK_EQ( s.size(),			original_size + 8 );
				DOCTEST_FAST_CHECK_EQ( ordered::min( s ),	10 );
				DOCTEST_FAST_CHECK_EQ( ordered::max( s ),	3300 );
			}
		}
		DOCTEST_SUBCASE( "all" ) {
			Ordered_vector< double >	cntr{};
			cntr.push_back( 1 );
			cntr.push_back( 2 );
			cntr.push_back( 3 );
			cntr.push_back( 4 );
			cntr.push_back( 5 );
			DOCTEST_FAST_CHECK_EQ( size( cntr ),		 5 );
		}
	}

}	// namespace pax::metrics
