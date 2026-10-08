//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/types/point-stuff/geodesy/spheroid.hpp>
#include <pax/doctest.hpp>

#include <numbers>


namespace pax {
	using namespace earth;
	constexpr auto pi 	= std::numbers::pi_v< double >;
	constexpr auto sqr( const auto x_ )	noexcept			{	return x_*x_;				}

	DOCTEST_TEST_CASE( "spheroid functions" ) {
		const auto a		  = grs1980.a();
		const auto b		  = grs1980.b();

		DOCTEST_CHECK_EQ( grs1980.a(),				6378137.0 );
		DOCTEST_CHECK_EQ( grs1980.b(),				doctest::Approx( 6356752.3142 ) );
		DOCTEST_CHECK_EQ( grs1980.f(),				doctest::Approx( 1./298.257222101 ) );
		DOCTEST_CHECK_EQ( grs1980.f_inv(),			doctest::Approx( 298.257222101 ) );
		DOCTEST_CHECK_EQ( grs1980.e2(),				doctest::Approx( 0.00669437999014 ) );
		DOCTEST_CHECK_EQ( grs1980.radius( 0.0 ),		a );
		DOCTEST_CHECK_EQ( grs1980.radius( pi/2.0 ),	b );
		DOCTEST_CHECK_EQ( grs1980.K( 0.0 ),			1.0/sqr( b ) );
		DOCTEST_CHECK_EQ( grs1980.K( pi/2.0 ),		sqr( b/sqr( a ) ) );
		DOCTEST_CHECK_EQ( grs1980.H( 0.0 ),			doctest::Approx( ( a*a + b*b )/( 2*a*b*b ) ) );
		DOCTEST_CHECK_EQ( grs1980.H( pi/2.0 ),		b/( a*a ) );
		DOCTEST_CHECK_EQ( grs1980.M( 0.0 ),			doctest::Approx( b*b/a ) );
		DOCTEST_CHECK_EQ( grs1980.M( pi/2.0 ),		a*a/b );
		DOCTEST_CHECK_EQ( grs1980.N( 0.0 ),			a );
		DOCTEST_CHECK_EQ( grs1980.N( pi/2.0 ),		a/std::sqrt( 1.0 - grs1980.e2() ));
	}
	DOCTEST_TEST_CASE( "spheroids" ) {
		static_assert( airy1830.valid() );
		static_assert( australian1966.valid() );	
		static_assert( bessel1841.valid() );
		static_assert( clarke1866.valid() );
		static_assert( clarke1878.valid() );
		static_assert( clarke1880.valid() );
		static_assert( everest1830.valid() );
		static_assert( everest1830def1967.valid() );
		static_assert( everest1830mod1967.valid() );
		static_assert( grs1967.valid() );
		static_assert( grs1980.valid() );
		static_assert( hayford1910.valid() );
		static_assert( helmert1906.valid() );
		static_assert( iers1989.valid() );
		static_assert( iers2003.valid() );
		static_assert( international1924.valid() );
		static_assert( International1967.valid() );
		static_assert( krassovsky1940.valid() );
		// static_assert( maupertuis1738.valid() );
		// static_assert( plessis1817.valid() );
		static_assert( southamerican1969.valid() );
		static_assert( wgs1966.valid() );
		static_assert( wgs1972.valid() );
		static_assert( wgs1984.valid() );

		DOCTEST_CHECK_EQ( airy1830.a(),				6377563.396	);
		DOCTEST_CHECK_EQ( australian1966.a(),		6378160.0	);	
		DOCTEST_CHECK_EQ( bessel1841.a(),			6377397.155	);
		DOCTEST_CHECK_EQ( clarke1866.a(),			6378206.4	);
		DOCTEST_CHECK_EQ( clarke1878.a(),			6378190.0	);
		DOCTEST_CHECK_EQ( clarke1880.a(),			6378249.145	);
		DOCTEST_CHECK_EQ( everest1830.a(),			6377299.365	);
		DOCTEST_CHECK_EQ( everest1830def1967.a(),	6377298.556	);
		DOCTEST_CHECK_EQ( everest1830mod1967.a(),	6377304.063	);
		DOCTEST_CHECK_EQ( grs1967.a(),				6378160.0	);
		DOCTEST_CHECK_EQ( grs1980.a(),				6378137.0	);
		DOCTEST_CHECK_EQ( hayford1910.a(),			6378388.0	);
		DOCTEST_CHECK_EQ( helmert1906.a(),			6378200.0	);
		DOCTEST_CHECK_EQ( iers1989.a(),				6378136.0	);
		DOCTEST_CHECK_EQ( iers2003.a(),				6378136.6	);
		DOCTEST_CHECK_EQ( international1924.a(),		6378388.0	);
		DOCTEST_CHECK_EQ( International1967.a(),		6378157.5	);
		DOCTEST_CHECK_EQ( krassovsky1940.a(),		6378245.0	);
		DOCTEST_CHECK_EQ( maupertuis1738.a(),		6397300.0	);
		DOCTEST_CHECK_EQ( plessis1817.a(),			6376523.0	);
		DOCTEST_CHECK_EQ( southamerican1969.a(),		6378160.0	);
		DOCTEST_CHECK_EQ( wgs1966.a(),				6378145.0	);
		DOCTEST_CHECK_EQ( wgs1972.a(),				6378135.0	);
		DOCTEST_CHECK_EQ( wgs1984.a(),				6378137.0	);
	}

}	// namespace pax
