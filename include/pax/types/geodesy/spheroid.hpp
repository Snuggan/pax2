//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>		// std::sin, std::sqrt etc.
#include <format>		// std::format


namespace pax {

	template< typename F = double >
	struct Spheroid {
		using value_type = F;

		// DON'T USE! Must be "published" if Sphereoid is to be a template argument...
		value_type													m_a{}, m_f{};

	private:
		static constexpr value_type sqr( const value_type x_ )		noexcept		{	return x_*x_;						}

		constexpr value_type part( const value_type beta_ )			const noexcept	{
			return a()*std::fma( f()*( f() - 2.0 ), sqr( std::cos( beta_ ) ), 1.0 );
		}

	public:
		constexpr Spheroid()									  = default;
		constexpr Spheroid( const Spheroid & )					  = default;
		constexpr Spheroid & operator=( const Spheroid & )		  = default;
		constexpr Spheroid( const value_type a_, const value_type x_ )	  noexcept
			:	m_a{ a_ }, 
				m_f{ ( x_ > 500. ) ? ( a_ - x_ )/a_ : ( x_ > 1. ) ? 1./x_ : x_ } {}

		/// Equatorial radius.
		constexpr value_type a()									const noexcept	{	return m_a;							}

		/// Polar radius, a(1 - f).
		constexpr value_type b()									const noexcept	{
			return a() - a()*f();		// return std::fma( -f(), a(), a() );
		}

		/// Flattening, (a − b)/a.
		constexpr value_type f()									const noexcept	{	return m_f;							}

		/// Inverse aspect ratio (inverse flattening), 1/f,
		constexpr value_type f_inv()								const noexcept	{	return 1.0/f();						}

		/// Third flattening, (a − b)/(a + b).
		constexpr value_type ff()									const noexcept	{	return f()/( 2.0 - f() );			}

		/// First eccentricity, squqred,  (a^2 − b^2)/a^2 = f(2 − f).
		constexpr value_type e2()									const noexcept	{	return f()*( 2.0 - f() );			}

		/// First eccentricity, sqrt( e2 ).
		constexpr value_type e()									const noexcept	{	return std::sqrt( e2() );			}

		/// Second eccentricity, squqred,  (a^2 − b^2)/b^2 = e^2/(1 − e^2).
		constexpr value_type ee2()									const noexcept	{
			const value_type sqrf = sqr( 1.0 - f() );
			return ( 1.0 - sqrf )/sqrf;
		}

		/// Second eccentricity, sqrt( ee2 ).
		constexpr value_type ee()									const noexcept	{	return std::sqrt( ee2() );			}
		
		/// Radius at lat_. -pi/2 <= lat_ <= pi/2.
		constexpr value_type radius( const value_type lat_ )		const noexcept	{
			const value_type sin2 = sqr( std::sin( lat_ ) );
			const value_type cos2 = sqr( std::cos( lat_ ) );
			const value_type temp = sqr( 1.0 - f() );
			return a()*std::sqrt( ( cos2 + temp*temp*sin2 )/( cos2 + temp*sin2 ) );
		}
		
		/// Gaussian curvature. -pi/2 <= lat_ <= pi/2.
		constexpr value_type K( const value_type lat_ )				const noexcept	{
			return sqr( ( 1.0 - f() )/part( lat_ ) );
		}

		/// Mean curvature. -pi/2 <= lat_ <= pi/2.
		constexpr value_type H( const value_type lat_ )				const noexcept	{
			const value_type	p = a()*part( lat_ );
			return ( 1.0 - f() )*( a()*a() + p )/( 2.0*p*std::sqrt( p ) );
		}

		/// Meridional radius of curvature. -pi/2 <= lat_ <= pi/2.
		constexpr value_type M( const value_type lat_ )				const noexcept	{
			const value_type	t = 1.0 - e2()*sqr( std::sin( lat_ ) );
			return a()*( 1.0 - e2() )/( t*std::sqrt( t ) );
		}

		/// Prime-vertival radius of curvature. -pi/2 <= lat_ <= pi/2.
		constexpr value_type N( const value_type lat_ )				const noexcept	{
			return a()/std::sqrt( 1.0 - e2()*sqr( std::sin( lat_ ) ) );
		}

		/// Geocentric latitude. -pi/2 <= lat_ <= pi/2.
		constexpr value_type geocentric_latitude( value_type lat_ )	const noexcept	{
			return std::atan( sqr( 1.0 - f() )*std::tan( lat_ ) );
		}

		/// Parametric (reduced) latitude. -pi/2 <= lat_ <= pi/2.
		constexpr value_type parametric_latitude( value_type lat_ )	const noexcept	{
			return std::atan( ( 1.0 - f() )*std::tan( lat_ ) );
		}

		/// Check if the instantiation has valid values.
		constexpr value_type valid()								const noexcept	{
			return	( a() > 6377000. ) && ( a() < 6379000. ) && ( f() > 0.0033 ) && ( f() < 0.00341 );
		}

		/// Spheroid contents to std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{{a: {}, f: {}}}", a(), f() );
		}

		/// Stream a Spheroid's contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out				  & out_,
			const Spheroid		c_
		) {	return out_ << std::string( c_ );										}

		/// Stream a Spheroid's contents.
		friend bool operator==(
			const Spheroid		sph0_,
			const Spheroid		sph1_
		) {	return ( sph0_.a() == sph1_.a() ) && ( sph0_.f() == sph1_.f() );										}
	};

	namespace earth {
		constexpr Spheroid airy1830				{	6377563.396,	6356256.910		};
		constexpr Spheroid airymod				{	6377340.189,	6356034.446		};
		constexpr Spheroid andrae1876			{	6377104.43,		300.0			};
		constexpr Spheroid appliedphysics1965	{	6378137.0,		298.25			};
		constexpr Spheroid australian1966		{	6378160.0,		298.25			};
		constexpr Spheroid bessel1841			{	6377397.155,	299.1528128		};
		constexpr Spheroid bessel1841namibia	{	6377483.865,	299.1528128		};
		constexpr Spheroid clarke1866			{	6378206.4,		6356583.8		};
		constexpr Spheroid clarke1878			{	6378190.0,		293.4659980		};
		constexpr Spheroid clarke1880			{	6378249.145,	293.465			};
		constexpr Spheroid clarke1880mod		{	6378249.145,	293.4663		};
		constexpr Spheroid cpm1799				{	6375738.7,		334.29			};	// Comm. des Poids et Mesures 1799
		constexpr Spheroid delambre1800			{	6375653.0,		334.0			};
		constexpr Spheroid delambre1810			{	6376428.0,		311.5			};
		constexpr Spheroid engelis1985			{	6378136.05,		298.2566		};
		constexpr Spheroid everest1830			{	6377276.345,	300.8017		};
		constexpr Spheroid everest1830def1967	{	6377298.556,	300.8017		};
		constexpr Spheroid everest1948			{	6377304.063,	300.8017		};
		constexpr Spheroid everest1956			{	6377301.243,	300.8017		};
		constexpr Spheroid everest1969			{	6377295.664,	300.8017		};
		constexpr Spheroid fischer1960			{	6378166.0,		298.3			};
		constexpr Spheroid fischer1960mod		{	6378155.0,		298.3			};
		constexpr Spheroid fischer1968			{	6378150.0,		298.3			};
		constexpr Spheroid grs1967				{	6378160.0,		298.247167427	};
		constexpr Spheroid grs1980				{	6378137.0,		298.257222101	};
		constexpr Spheroid hayford1910			{	6378388.0,		297.0			};
		constexpr Spheroid helmert1906			{	6378200.0,		298.3			};
		constexpr Spheroid hough				{	6378270.0,		297.0			};
		constexpr Spheroid iag1975				{	6378140.0,		298.257			};
		constexpr Spheroid iers1989				{	6378136.0,		298.257			};
		constexpr Spheroid iers2003				{	6378136.6,		298.25642		};
		constexpr Spheroid international1909	{	hayford1910						};
		constexpr Spheroid international1967	{	6378157.5,		6356772.2		};
		constexpr Spheroid iugg1980				{	grs1980							};
		constexpr Spheroid kaula1961			{	6378163.0,		298.24			};
		constexpr Spheroid krassovsky1940		{	6378245.0,		298.3			};
		constexpr Spheroid lerch1979			{	6378139.0,		298.257			};
		constexpr Spheroid maupertius1738		{	6397300.0,		191.0			};
		constexpr Spheroid merit1983			{	6378137.0,		298.257			};
		constexpr Spheroid plessis1817			{	6376523.0,		6355863.0		};
		constexpr Spheroid southamerican1969	{	australian1966					};
		constexpr Spheroid walbeck				{	6376896.0,		6355834.8467	};
		constexpr Spheroid wgs1960				{	6378165.0,		298.3			};
		constexpr Spheroid wgs1966				{	6378145.0,		298.25			};
		constexpr Spheroid wgs1972				{	6378135.0,		298.26			};
		constexpr Spheroid wgs1984				{	6378137.0,		298.257223563	};
	}

}	// namespace pax
