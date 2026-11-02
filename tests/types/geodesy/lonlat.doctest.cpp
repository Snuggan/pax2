//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/doctest.hpp>
#include <pax/types/geodesy/lonlat-great-circle.hpp>

namespace pax {

	constexpr Spheroid 			sph{	earth::wgs1984	};
	constexpr auto	origin	  = lonlat< sph >( 0.0, 40.0 );
	
	DOCTEST_TEST_CASE( "lonlat" ) {
		DOCTEST_FAST_CHECK_EQ( lon( origin ),					0.0 );
		DOCTEST_FAST_CHECK_EQ( lat( origin ),					40.0 );
		DOCTEST_FAST_CHECK_EQ( spheroid( origin ),				sph );
		DOCTEST_FAST_CHECK_EQ( rad2deg( deg2rad( 45.0 ) ),		doctest::Approx( 45.0 ) );
	}


	struct Destination_test : Great_circle_distance< horner_fma, earth::wgs1984, double, 2 > {
		using base = Great_circle_distance< horner_fma, earth::wgs1984, double, 2 >;
		using base::Great_circle_distance;
	public:
		using base::A1, base::alpha0, base::epsilon, base::I1, base::I3, 
			  base::lambda2, base::phi2, base::sigma1, base::sigma2;
	};
	DOCTEST_TEST_CASE( "lonlat destination" ) {
		const Destination_test	dest( origin, deg2rad( 30.0 ), 10000000.0 );

		DOCTEST_FAST_CHECK_EQ( rad2deg( dest.alpha0() ),	doctest::Approx( 22.55394020262 ) );
		DOCTEST_FAST_CHECK_EQ( rad2deg( dest.sigma1() ),	doctest::Approx( 43.99915364500 ) );

		DOCTEST_FAST_CHECK_EQ( dest.epsilon(),				doctest::Approx( 0.00143289220416 ) );
		DOCTEST_FAST_CHECK_EQ( dest.A1( dest.epsilon() ),	doctest::Approx( 1.00143546236207 ) );
		DOCTEST_FAST_CHECK_EQ( dest.I1( dest.sigma1(), dest.epsilon() ),	doctest::Approx( 0.76831538886412 ) );
		DOCTEST_FAST_CHECK_EQ( rad2deg( dest.sigma2() ),	doctest::Approx( 133.92164083038 ) );

		DOCTEST_FAST_CHECK_EQ( dest.I3( dest.sigma1(), dest.epsilon() ),	doctest::Approx( 0.76773786069 ) );
		DOCTEST_FAST_CHECK_EQ( dest.I3( dest.sigma2(), dest.epsilon() ),	doctest::Approx( 2.33534322170 ) );
		DOCTEST_FAST_CHECK_EQ( rad2deg( dest.lambda2() ),	doctest::Approx( 158.11205042393 ) );

		DOCTEST_FAST_CHECK_EQ( rad2deg( dest.phi2() ),		doctest::Approx(  41.79331020506 ) );

		DOCTEST_FAST_CHECK_EQ( lon( dest.great_circle_destination() ),	doctest::Approx( 158.11205042393 ) );
		DOCTEST_FAST_CHECK_EQ( lat( dest.great_circle_destination() ),	doctest::Approx(  41.79331020506 ) );
	}
	
}	// namespace pax
