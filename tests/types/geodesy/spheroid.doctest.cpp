//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/geodesy/spheroid.hpp>
#include <pax/doctest.hpp>

#include <numbers>


namespace pax {
	using namespace earth;
	constexpr auto a90 	= std::numbers::pi_v< double >/2.0;
	constexpr auto sqr( const auto x_ )	noexcept			{	return x_*x_;				}

	DOCTEST_TEST_CASE( "spheroid functions" ) {
		constexpr auto sph	  = wgs1984;
		const auto a		  = sph.a();
		const auto b		  = sph.b();

		DOCTEST_CHECK_EQ( sph,					sph );
		DOCTEST_CHECK_NE( grs1980,				sph );

		DOCTEST_CHECK_EQ( sph.a(),				6378137.0 );
		DOCTEST_CHECK_EQ( sph.b(),				doctest::Approx( 6356752.314245 ) );
		DOCTEST_CHECK_EQ( sph.f(),				doctest::Approx( 1./298.257222101 ) );
		DOCTEST_CHECK_EQ( sph.f_inv(),			doctest::Approx( 298.257222101 ) );
		DOCTEST_CHECK_EQ( sph.ff(),				doctest::Approx( 0.00167922038638370 ) );
		DOCTEST_CHECK_EQ( sph.e2(),				doctest::Approx( ( a*a - b*b )/( a*a ) ) );
		DOCTEST_CHECK_EQ( sph.e2(),				doctest::Approx( 0.00669437999014132  ) );
		DOCTEST_CHECK_EQ( sph.ee2(),			doctest::Approx( ( a*a - b*b )/( b*b ) ) );
		DOCTEST_CHECK_EQ( sph.ee2(),			doctest::Approx( sph.e2()/( 1.0 - sph.e2() ) ) );
		DOCTEST_CHECK_EQ( sph.ee2(),			doctest::Approx( 0.00673949674227643 ) );
		DOCTEST_CHECK_EQ( sph.radius( 0.0 ),	a );
		DOCTEST_CHECK_EQ( sph.radius( a90 ),	b );
		DOCTEST_CHECK_EQ( sph.K( 0.0 ),			doctest::Approx( 1.0/sqr( b ) ) );
		DOCTEST_CHECK_EQ( sph.K( a90 ),			sqr( b/sqr( a ) ) );
		DOCTEST_CHECK_EQ( sph.H( 0.0 ),			doctest::Approx( ( a*a + b*b )/( 2*a*b*b ) ) );
		DOCTEST_CHECK_EQ( sph.H( a90 ),			b/( a*a ) );
		DOCTEST_CHECK_EQ( sph.M( 0.0 ),			doctest::Approx( b*b/a ) );
		DOCTEST_CHECK_EQ( sph.M( a90 ),			doctest::Approx( a*a/b ) );
		DOCTEST_CHECK_EQ( sph.N( 0.0 ),			a );
		DOCTEST_CHECK_EQ( sph.N( a90 ),			a/std::sqrt( 1.0 - sph.e2() ));
	}
	DOCTEST_TEST_CASE( "spheroid instances" ) {
		static_assert( airy1830				.valid() );
		static_assert( airymod				.valid() );
		static_assert( andrae1876			.valid() );
		static_assert( appliedphysics1965	.valid() );
		static_assert( australian1966		.valid() );
		static_assert( bessel1841			.valid() );
		static_assert( bessel1841namibia	.valid() );
		static_assert( clarke1866			.valid() );
		static_assert( clarke1878			.valid() );
		static_assert( clarke1880			.valid() );
		static_assert( clarke1880mod		.valid() );
		// static_assert( cpm1799			.valid() );
		// static_assert( delambre1800		.valid() );
		// static_assert( delambre1810		.valid() );
		static_assert( engelis1985			.valid() );
		static_assert( everest1830			.valid() );
		static_assert( everest1830def1967	.valid() );
		static_assert( everest1948			.valid() );
		static_assert( everest1956			.valid() );
		static_assert( everest1969			.valid() );
		static_assert( fischer1960			.valid() );
		static_assert( fischer1960mod		.valid() );
		static_assert( fischer1968			.valid() );
		static_assert( grs1967				.valid() );
		static_assert( grs1980				.valid() );
		static_assert( hayford1910			.valid() );
		static_assert( helmert1906			.valid() );
		static_assert( hough				.valid() );
		static_assert( iag1975				.valid() );
		static_assert( iers1989				.valid() );
		static_assert( iers2003				.valid() );
		static_assert( international1909	.valid() );
		static_assert( international1967	.valid() );
		static_assert( iugg1980				.valid() );
		static_assert( kaula1961			.valid() );
		static_assert( krassovsky1940		.valid() );
		static_assert( lerch1979			.valid() );
		// static_assert( maupertius1738	.valid() );
		static_assert( merit1983			.valid() );
		// static_assert( plessis1817		.valid() );
		static_assert( southamerican1969	.valid() );
		// static_assert( walbeck			.valid() );
		static_assert( wgs1960				.valid() );
		static_assert( wgs1966				.valid() );
		static_assert( wgs1972				.valid() );
		static_assert( wgs1984				.valid() );
	}

}	// namespace pax
