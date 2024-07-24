//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "_metrics-key.hpp"
#include <pax/math/metrics/ordered.hpp>

#include <pax/doctest.hpp>
#include <vector>


namespace pax::metrics { 

	double slow_mad( const double * begin_, const double * end_ ) {
		auto			ord   = std::vector< double >( begin_, end_ );
		auto			ordered{ order( std::span( ord ) ) };
		auto			diffs = std::vector< double >{};
		const auto		med   = ordered::median( ordered );
		for( auto item : ordered )
			diffs.push_back( ( item > med ) ? ( item - med ) : ( med - item ) );
		return ordered::median( order( std::span( diffs ) ) );
	}


	DOCTEST_TEST_CASE( "binom" ) {
		using TL = ordered::TL_moment_array< 1, double >;
	
		DOCTEST_FAST_CHECK_EQ( TL::binom( 0, 0 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 1, 0 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 2, 0 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 3, 0 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 4, 0 ),		1 );

		DOCTEST_FAST_CHECK_EQ( TL::binom( 1, 1 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 2, 1 ),		2 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 3, 1 ),		3 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 4, 1 ),		4 );

		DOCTEST_FAST_CHECK_EQ( TL::binom( 2, 2 ),		1 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 3, 2 ),		3 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 4, 2 ),		6 );
		DOCTEST_FAST_CHECK_EQ( TL::binom( 5, 2 ),		10 );
	}

	DOCTEST_TEST_CASE( "ordered statistics" ) {
		const auto ord{ std::vector< double >{ std::begin( Correct::dataset ), std::end( Correct::dataset ) } };
	
		DOCTEST_SUBCASE( "empty" ) {
			std::vector< double >		s0;
			const auto 					s{ order( std::span( s0 ) ) };

			DOCTEST_FAST_CHECK_EQ( s.size(),						0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, -9.0 ),	0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s,  9.0 ),	0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, -9.0 ),	0 );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::min( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quantile( s, 1.0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quantile( s, 2.0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quartile( s, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quartile( s, 1 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::median( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quartile( s, 2 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quartile( s, 3 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::quartile( s, 4 ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::max( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 1 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 2 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 4 >( s ) ) );
		}
		DOCTEST_SUBCASE( "many elements" ) {
			auto s0					  = ord;
			const auto 					s{ order( std::span( s0 ) ) };
		
			DOCTEST_FAST_CHECK_EQ( s.size(),							Correct::count );

			DOCTEST_FAST_CHECK_EQ( ordered::min( s ),					Correct::minimum );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 0.0 ),			Correct::minimum );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 1.0 ),			Correct::quartile1 );
			DOCTEST_FAST_CHECK_EQ( ordered::median( s ),				Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).median(),	Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),		slow_mad( Correct::dataset, Correct::dataset+Correct::count ) );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 2 ),			Correct::median );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 3 ),			Correct::quartile3 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 4 ),			Correct::maximum );
			DOCTEST_FAST_CHECK_EQ( ordered::max( s ),					Correct::maximum );

			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),		doctest::Approx( Correct::mad ) );

			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 1 >( s ),			doctest::Approx( Correct::L1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 2 >( s ),			doctest::Approx( Correct::L2 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 3 >( s ),			doctest::Approx( Correct::L3 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 4 >( s ),			doctest::Approx( Correct::L4 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 5 >( s ),			doctest::Approx( Correct::L5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 9 >( s ),			doctest::Approx( Correct::L9 ) );

			{
				const ordered::L_moment_array< 9, double >				Larr( s );
				DOCTEST_FAST_CHECK_EQ( Larr[ 1 ],						doctest::Approx( Correct::L1 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 2 ],						doctest::Approx( Correct::L2 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 3 ],						doctest::Approx( Correct::L3 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 4 ],						doctest::Approx( Correct::L4 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 5 ],						doctest::Approx( Correct::L5 ) );
				DOCTEST_FAST_CHECK_EQ( Larr[ 9 ],						doctest::Approx( Correct::L9 ) );
			}

			DOCTEST_FAST_CHECK_EQ( ordered::L_moment_ratio< 3 >( s ),	doctest::Approx( Correct::L3_ratio ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment_ratio< 4 >( s ),	doctest::Approx( Correct::L4_ratio ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 0 ),		doctest::Approx( Correct::TL1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 0 ),		doctest::Approx( Correct::TL2 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 0 ),		doctest::Approx( Correct::TL3 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 0 ),		doctest::Approx( Correct::TL4 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 0 ),		doctest::Approx( Correct::TL5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 9 >( s, 0 ),		doctest::Approx( Correct::TL9 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 1 ),		doctest::Approx( Correct::TL1_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 1 ),		doctest::Approx( Correct::TL2_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 1 ),		doctest::Approx( Correct::TL3_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 1 ),		doctest::Approx( Correct::TL4_1 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 1 ),		doctest::Approx( Correct::TL5_1 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 1 >( s, 3, 5 ),	doctest::Approx( Correct::TL1_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 2 >( s, 3, 5 ),	doctest::Approx( Correct::TL2_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 3 >( s, 3, 5 ),	doctest::Approx( Correct::TL3_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 4 >( s, 3, 5 ),	doctest::Approx( Correct::TL4_3_5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment< 5 >( s, 3, 5 ),	doctest::Approx( Correct::TL5_3_5 ) );

			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment_ratio< 3 >( s, 3, 5 ),	doctest::Approx( Correct::TL3_3_5_ratio ) );
			DOCTEST_FAST_CHECK_EQ( ordered::TL_moment_ratio< 4 >( s, 3, 5 ),	doctest::Approx( Correct::TL4_3_5_ratio ) );

			DOCTEST_FAST_CHECK_EQ( ordered::quantile( s, 0.25 ),		Correct::quartile1 );
			DOCTEST_FAST_CHECK_EQ( ordered::percentile( s, 25 ),		Correct::quartile1 );

			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::min( s ) - 1 ),		 0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::min( s ) ),	   		 0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, 1800.0 ),	 	   				 3 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::max( s ) ),	   		14 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, ordered::max( s ) + 1 ),		15 );

			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::min( s ) - 1 ),		15 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::min( s ) ),			15 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, 1800.0 ),						12 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::max( s ) ),	 		 1 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, ordered::max( s ) + 1 ), 		 0 );
		}
		DOCTEST_SUBCASE( "few elements" ) {
			std::vector< double >		s0;

			s0.push_back( 7 );
			auto s = order( std::span( s0 ) );
			DOCTEST_FAST_CHECK_EQ( s.size(),							1 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s, -9.0 ),		0 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_lt( s,  9.0 ),		1 );
			DOCTEST_FAST_CHECK_EQ( ordered::count_ge( s, -9.0 ),		1 );
			DOCTEST_FAST_CHECK_EQ( ordered::min( s ),					7 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 0.0 ),			ordered::min( s ) );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 1.0 ),			7 );
			DOCTEST_FAST_CHECK_EQ( ordered::median( s ),				7 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 2 ),			ordered::median( s ) );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),		0.0 );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).median(),	7.0 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 3 ),			7 );
			DOCTEST_FAST_CHECK_EQ( ordered::quartile( s, 4 ),			ordered::max( s ) );
			DOCTEST_FAST_CHECK_EQ( ordered::max( s ),					7 );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 1 >( s ),		doctest::Approx(  7.0 ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 2 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 4 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 5 >( s ) ) );

			s0.push_back( 8 );
			s = order( std::span( s0 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 1 >( s ),		doctest::Approx(  7.5 ) );
			DOCTEST_FAST_CHECK_EQ( ordered::L_moment< 2 >( s ),		doctest::Approx(  0.5 ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 4 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( ordered::L_moment< 5 >( s ) ) );
			DOCTEST_FAST_CHECK_EQ( ordered::median_mad( s ).mad(),		0.5 );

			s0.push_back( 16 );
			s = order( std::span( s0 ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 1 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 2 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY(  std::isnan( ordered::L_moment< 4 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY(  std::isnan( ordered::L_moment< 5 >( s ) ) );

			s0.push_back( 16 );
			s = order( std::span( s0 ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 1 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 2 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 4 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY(  std::isnan( ordered::L_moment< 5 >( s ) ) );

			s0.push_back( 16 );
			s = order( std::span( s0 ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 1 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 2 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 3 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 4 >( s ) ) );
			DOCTEST_FAST_CHECK_UNARY( !std::isnan( ordered::L_moment< 5 >( s ) ) );

			// s0.push_back( 8 );
			// auto s2 = order( std::span( s0 ).as_const() );
		}
	}

}	// namespace pax::metrics
