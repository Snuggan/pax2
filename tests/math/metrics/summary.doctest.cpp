//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "_metrics-key.hpp"
#include <pax/math/metrics/summary.hpp>

#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::metrics { 

	template< typename Sum >
	constexpr double the_weight( Sum::uses_weights ? 3 : 1 );

	template< typename Cntr >
	void push_filtered( 
		Cntr	  & cntr_, 
		float 		w_ 
	) {
		using Value = Weighted< double >;
		if constexpr( Cntr::uses_weights ) {
			cntr_.push( Value{ 1, w_ } );
			cntr_.push( Value{ 2, w_ } );
			cntr_.push( Value{ 3, w_ } );
			cntr_.push( Value{ 4, w_ } );
			cntr_.push( Value{ 5, w_ } );
		} else{
			cntr_.push( 1 );
			cntr_.push( 2 );
			cntr_.push( 3 );
			cntr_.push( 4 );
			cntr_.push( 5 );
		}
	}

	template< typename Sum, typename T0 >
	void push(
		Sum				  & s_,
		const T0			e_,
		const double 		w_
	) {
		if constexpr( Sum::uses_weights )	
			s_.push_back( Weighted< typename Sum::value_type >( T0( e_ ), w_ ) );
		else													
			s_.push_back( e_ );
	}


	DOCTEST_TEST_CASE_TEMPLATE( "summary statistics", Sum, 
		Summary_with_weight< long long, 5u >,
		Summary_with_weight< double,    6u >,
		Summary_with_weight< long long, 7u >,
		Summary< double,    8u >, 
		Summary< long long, 9u >
	) {
		DOCTEST_SUBCASE( "calculations" ) {
			const auto		w{ the_weight< Sum > };
			const auto		w2( w*w  );
			const auto		w3( w*w2 );
			const auto		w4( w*w3 );

			Sum				s{};
			for( auto e : Correct::dataset )	push( s, e, w );
		
		
			DOCTEST_FAST_CHECK_EQ( count( s ),						Correct::count );
			DOCTEST_FAST_CHECK_EQ( sum< 1 >( s ),					doctest::Approx( Correct::sum *w ) );
			DOCTEST_FAST_CHECK_EQ( sum< 2 >( s ),					doctest::Approx( Correct::sum2*w ) );
			DOCTEST_FAST_CHECK_EQ( sum< 3 >( s ),					doctest::Approx( Correct::sum3*w ) );
			DOCTEST_FAST_CHECK_EQ( sum< 4 >( s ),					doctest::Approx( Correct::sum4*w ) );

			DOCTEST_FAST_CHECK_EQ( weight< 1 >( s ),				count( s )*w );
			DOCTEST_FAST_CHECK_EQ( weight< 2 >( s ),				count( s )*w2 );
			DOCTEST_FAST_CHECK_EQ( weight< 3 >( s ),				count( s )*w3 );
			DOCTEST_FAST_CHECK_EQ( weight< 4 >( s ),				count( s )*w4 );

			DOCTEST_FAST_CHECK_EQ( moment< 0 >( s ),				doctest::Approx( Correct::moment0 ) );
			DOCTEST_FAST_CHECK_EQ( moment< 1 >( s ),				doctest::Approx( Correct::moment1 ) );
			DOCTEST_FAST_CHECK_EQ( moment< 2 >( s ),				doctest::Approx( Correct::moment2 ) );
			DOCTEST_FAST_CHECK_EQ( moment< 3 >( s ),				doctest::Approx( Correct::moment3 ) );
			DOCTEST_FAST_CHECK_EQ( moment< 4 >( s ),				doctest::Approx( Correct::moment4 ) );
			DOCTEST_FAST_CHECK_EQ( moment< 5 >( s ),				doctest::Approx( Correct::moment5 ) );

			DOCTEST_FAST_CHECK_EQ( cmoment< 0 >( s ),				doctest::Approx( Correct::cmoment0 ) );
			DOCTEST_FAST_CHECK_EQ( cmoment< 1 >( s ),				doctest::Approx( Correct::cmoment1 ) );
			DOCTEST_FAST_CHECK_EQ( cmoment< 2 >( s ),				doctest::Approx( Correct::cmoment2 ) );
			DOCTEST_FAST_CHECK_EQ( cmoment< 3 >( s ),				doctest::Approx( Correct::cmoment3 ) );
			DOCTEST_FAST_CHECK_EQ( cmoment< 4 >( s ),				doctest::Approx( Correct::cmoment4 ) );
			DOCTEST_FAST_CHECK_EQ( cmoment< 5 >( s ),				doctest::Approx( Correct::cmoment5 ) );

			DOCTEST_FAST_CHECK_EQ( mean( s ),						doctest::Approx( Correct::mean ) );
			DOCTEST_FAST_CHECK_EQ( population_variance( s ),		doctest::Approx( Correct::population_variance ) );
			DOCTEST_FAST_CHECK_EQ( sample_variance( s ),			doctest::Approx( Correct::sample_variance ) );

			DOCTEST_FAST_CHECK_EQ( population_skewness( s ),		doctest::Approx( Correct::population_skewness ) );
			DOCTEST_FAST_CHECK_EQ( sample_skewness( s ),			doctest::Approx( Correct::sample_skewness ) );
			DOCTEST_FAST_CHECK_EQ( skewness_g1( s ),				doctest::Approx( Correct::skewness_g1 ) );
			DOCTEST_FAST_CHECK_EQ( skewness_G1( s ),				doctest::Approx( Correct::skewness_G1 ) );
			DOCTEST_FAST_CHECK_EQ( skewness_b1( s ),				doctest::Approx( Correct::skewness_b1 ) );

			DOCTEST_FAST_CHECK_EQ( population_kurtosis( s ),		doctest::Approx( Correct::population_kurtosis_fisher ) );
			DOCTEST_FAST_CHECK_EQ( population_kurtosis_fisher( s ),	doctest::Approx( Correct::population_kurtosis_fisher ) );
			DOCTEST_FAST_CHECK_EQ( population_kurtosis_pearson( s ),doctest::Approx( Correct::population_kurtosis_pearson ) );
			DOCTEST_FAST_CHECK_EQ( sample_kurtosis( s ),			doctest::Approx( Correct::sample_kurtosis_fisherK ) );
			DOCTEST_FAST_CHECK_EQ( sample_kurtosis_fisher( s ),		doctest::Approx( Correct::sample_kurtosis_fisherK ) );
			DOCTEST_FAST_CHECK_EQ( sample_kurtosis_pearson( s ),	doctest::Approx( Correct::sample_kurtosis_pearson ) );
			DOCTEST_FAST_CHECK_EQ( kurtosis_G2( s ),				doctest::Approx( Correct::kurtosis_G2 ) );
			DOCTEST_FAST_CHECK_EQ( kurtosis_b2( s ),				doctest::Approx( Correct::kurtosis_b2 ) );
		}
		DOCTEST_SUBCASE( "nans" ) {
			const auto		w{ the_weight< Sum > };
			const auto		w2( w*w  );
			const auto		w3( w*w2 );
			const auto		w4( w*w3 );
			Sum				s;

			{
				DOCTEST_FAST_CHECK_EQ( count( s ),						0 );
				DOCTEST_FAST_CHECK_EQ( sum< 1 >( s ),					0 );
				DOCTEST_FAST_CHECK_EQ( sum< 2 >( s ),					0 );
				DOCTEST_FAST_CHECK_EQ( sum< 3 >( s ),					0 );
				DOCTEST_FAST_CHECK_EQ( sum< 4 >( s ),					0 );
				DOCTEST_FAST_CHECK_EQ( weight< 1 >( s ),				count( s )*w );
				DOCTEST_FAST_CHECK_EQ( weight< 2 >( s ),				count( s )*w2 );
				DOCTEST_FAST_CHECK_EQ( weight< 3 >( s ),				count( s )*w3 );
				DOCTEST_FAST_CHECK_EQ( weight< 4 >( s ),				count( s )*w4 );
				DOCTEST_FAST_CHECK_EQ( moment< 0 >( s ),				1 );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( moment< 1 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( moment< 2 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( moment< 3 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( moment< 4 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( moment< 5 >( s )	) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 0 >( s ),				1 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 1 >( s ),				0 );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( cmoment< 2 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( cmoment< 3 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( cmoment< 4 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( cmoment< 5 >( s )	) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( mean( s ) ) );
			
				DOCTEST_FAST_CHECK_UNARY( std::isnan( population_variance( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( sample_variance( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_g1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_G1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_b1( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_G2( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_b2( s ) ) );
			} {
				push( s, 1, w );

				DOCTEST_FAST_CHECK_EQ( count( s ),					1 );
				DOCTEST_FAST_CHECK_EQ( sum< 1 >( s ),				1*w );
				DOCTEST_FAST_CHECK_EQ( sum< 2 >( s ),				1*w );
				DOCTEST_FAST_CHECK_EQ( sum< 3 >( s ),				1*w );
				DOCTEST_FAST_CHECK_EQ( sum< 4 >( s ),				1*w );
				DOCTEST_FAST_CHECK_EQ( weight< 1 >( s ),			count( s )*w );
				DOCTEST_FAST_CHECK_EQ( weight< 2 >( s ),			count( s )*w2 );
				DOCTEST_FAST_CHECK_EQ( weight< 3 >( s ),			count( s )*w3 );
				DOCTEST_FAST_CHECK_EQ( weight< 4 >( s ),			count( s )*w4 );
				DOCTEST_FAST_CHECK_EQ( mean< 1 >( s ),				sum< 1 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 2 >( s ),				sum< 2 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 3 >( s ),				sum< 3 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 4 >( s ),				sum< 4 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 1 >( s ),			mean( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 2 >( s ),			mean< 2 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 3 >( s ),			mean< 3 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 4 >( s ),			mean< 4 >( s ) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 1 >( s ),			0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 2 >( s ),			0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 3 >( s ),			0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 4 >( s ),			0 );
			
				DOCTEST_FAST_CHECK_EQ( population_variance( s ),	0 );
				DOCTEST_FAST_CHECK_UNARY( std::isnan( sample_variance( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_g1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_G1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_b1( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_G2( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_b2( s ) ) );
			} {
				push( s, 3, w );

				DOCTEST_FAST_CHECK_EQ( count( s ),					 2 );
				DOCTEST_FAST_CHECK_EQ( sum< 1 >( s ),				 4*w );
				DOCTEST_FAST_CHECK_EQ( sum< 2 >( s ),				10*w );
				DOCTEST_FAST_CHECK_EQ( sum< 3 >( s ),				28*w );
				DOCTEST_FAST_CHECK_EQ( sum< 4 >( s ),				82*w );
				DOCTEST_FAST_CHECK_EQ( weight< 1 >( s ),			count( s )*w );
				DOCTEST_FAST_CHECK_EQ( weight< 2 >( s ),			count( s )*w2 );
				DOCTEST_FAST_CHECK_EQ( weight< 3 >( s ),			count( s )*w3 );
				DOCTEST_FAST_CHECK_EQ( weight< 4 >( s ),			count( s )*w4 );
				DOCTEST_FAST_CHECK_EQ( mean< 1 >( s ),				sum< 1 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 2 >( s ),				sum< 2 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 3 >( s ),				sum< 3 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 4 >( s ),				sum< 4 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 1 >( s ),			mean( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 2 >( s ),			mean< 2 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 3 >( s ),			mean< 3 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 4 >( s ),			mean< 4 >( s ) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 1 >( s ),			 0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 2 >( s ),			 1 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 3 >( s ),			 0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 4 >( s ),			 1 );
			
				DOCTEST_FAST_CHECK_EQ( population_variance( s ),	 1 );
				DOCTEST_FAST_CHECK_EQ( sample_variance( s ),		 2 );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_g1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_G1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( skewness_b1( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_G2( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_b2( s ) ) );
			} {
				push( s, 4, w );

				DOCTEST_FAST_CHECK_EQ( count( s ),					  3 );
				DOCTEST_FAST_CHECK_EQ( sum< 1 >( s ),				  8*w );
				DOCTEST_FAST_CHECK_EQ( sum< 2 >( s ),				 26*w );
				DOCTEST_FAST_CHECK_EQ( sum< 3 >( s ),				 92*w );
				DOCTEST_FAST_CHECK_EQ( sum< 4 >( s ),				338*w );
				DOCTEST_FAST_CHECK_EQ( weight< 1 >( s ),			count( s )*w );
				DOCTEST_FAST_CHECK_EQ( weight< 2 >( s ),			count( s )*w2 );
				DOCTEST_FAST_CHECK_EQ( weight< 3 >( s ),			count( s )*w3 );
				DOCTEST_FAST_CHECK_EQ( weight< 4 >( s ),			count( s )*w4 );
				DOCTEST_FAST_CHECK_EQ( mean< 1 >( s ),				sum< 1 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 2 >( s ),				sum< 2 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 3 >( s ),				sum< 3 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( mean< 4 >( s ),				sum< 4 >( s )/double( w )/count( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 1 >( s ),			mean( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 2 >( s ),			mean< 2 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 3 >( s ),			mean< 3 >( s ) );
				DOCTEST_FAST_CHECK_EQ( moment< 4 >( s ),			mean< 4 >( s ) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 1 >( s ),			 0 );
				DOCTEST_FAST_CHECK_EQ( cmoment< 2 >( s ),			doctest::Approx(  1.5555555556 ) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 3 >( s ),			doctest::Approx( -0.7407407407 ) );
				DOCTEST_FAST_CHECK_EQ( cmoment< 4 >( s ),			doctest::Approx(  3.6296296296 ) );
			
				DOCTEST_FAST_CHECK_EQ( population_variance( s ),	doctest::Approx(  2.3333333333*2/3 ) );
				DOCTEST_FAST_CHECK_EQ( sample_variance( s ),		doctest::Approx(  2.3333333333 ) );

				DOCTEST_FAST_CHECK_UNARY( !std::isnan( population_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( sample_skewness( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( skewness_g1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( skewness_G1( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( skewness_b1( s ) ) );

				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( population_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( sample_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_G2( s ) ) );
				DOCTEST_FAST_CHECK_UNARY(  std::isnan( kurtosis_b2( s ) ) );
			} {
				push( s, 4, w );

				DOCTEST_FAST_CHECK_EQ( count( s ),					  4 );

				DOCTEST_FAST_CHECK_UNARY( !std::isnan( population_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( population_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( population_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( sample_kurtosis( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( sample_kurtosis_fisher( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( sample_kurtosis_pearson( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( kurtosis_G2( s ) ) );
				DOCTEST_FAST_CHECK_UNARY( !std::isnan( kurtosis_b2( s ) ) );
			}
		}
	}

}	// namespace pax::metrics
