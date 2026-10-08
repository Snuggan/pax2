//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../base.hpp"
#include <array>
#include <cmath>		// std::fma
#include <format>		// std::format


namespace pax {

	class Spheroid {
		using F = double;

		static constexpr F sqr( const F x_ )						noexcept		{	return x_*x_;						}

		constexpr F part( const F beta_ )							const noexcept	{
			return a()*std::fma( f()*( f() - 2.0 ), sqr( std::cos( beta_ ) ), 1.0 );
		}

	public:
		// DON'T USE!Must be "published" if Sphereoid is to be a template argument...
		F															m_a{}, m_f{};

		using value_type = F;

		constexpr Spheroid()									  = default;
		constexpr Spheroid( const Spheroid & )					  = default;
		constexpr Spheroid & operator=( const Spheroid & )		  = default;
		constexpr Spheroid( const value_type a_, const value_type x_ )	  noexcept
			:	m_a{ a_ }, 
				m_f{ ( x_ > 500. ) ? ( a_ - x_ )/a_ : ( x_ > 1. ) ? 1./x_ : x_ } {}

		/// Equatorial radius.
		constexpr value_type a()									const noexcept	{	return m_a;							}

		/// Polar radius.
		constexpr value_type b()									const noexcept	{	return std::fma( -f(), a(), a() );	}

		/// Aspect ratio (flattening),
		constexpr value_type f()									const noexcept	{	return m_f;							}

		/// Inverse aspect ratio (inverse flattening),
		constexpr value_type f_inv()								const noexcept	{	return 1.0/f();						}

		/// Eccentricity squqred.
		constexpr value_type e2()									const noexcept	{	return f()*( 2.0 - f() );			}

		/// Eccentricity.
		constexpr value_type e()									const noexcept	{	return std::sqrt( e2() );			}
		
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

		/// Check if the instantiation has valid values.
		constexpr value_type valid()								const noexcept	{
			return	( a() > 6377000. ) && ( a() < 6379000. ) && ( f() > 0.0033 ) && ( f() < 0.00341 );
		}

		/// Spheroid contents to std::string.
		explicit constexpr operator std::string() 					const			{
			return std::format( "{{a: {}, b: {}}}", a(), b() );
		}

		/// Stream a Spheroid's contents.
		template< typename Out >
		friend constexpr Out & operator<<(
			Out				  & out_,
			const Spheroid	  & c_
		) {
			return out_ << std::string( c_ );
		}
	};

	namespace earth {
		// From https://en.wikipedia.org/wiki/Earth_ellipsoid
		constexpr Spheroid airy1830				{	6377563.396,	299.3249646		};
		constexpr Spheroid australian1966		{	6378160.0,		298.25			};
		constexpr Spheroid bessel1841			{	6377397.155,	299.1528128		};
		constexpr Spheroid clarke1866			{	6378206.4,		294.9786982		};
		constexpr Spheroid clarke1878			{	6378190.0,		293.4659980		};
		constexpr Spheroid clarke1880			{	6378249.145,	293.465			};
		constexpr Spheroid everest1830			{	6377299.365,	300.80172554	};
		constexpr Spheroid everest1830def1967	{	6377298.556,	300.8017		};
		constexpr Spheroid everest1830mod1967	{	6377304.063,	300.8017		};
		constexpr Spheroid grs1967				{	6378160.0,		298.247167427	};
		constexpr Spheroid grs1980				{	6378137.0,		298.257222101	};
		constexpr Spheroid hayford1910			{	6378388.0,		297.0			};
		constexpr Spheroid helmert1906			{	6378200.0,		298.3			};
		constexpr Spheroid iers1989				{	6378136.0,		298.257			};
		constexpr Spheroid iers2003				{	6378136.6,		298.25642		};
		constexpr Spheroid international1924	{	hayford1910						};
		constexpr Spheroid International1967	{	6378157.5,		298.24961539	};
		constexpr Spheroid krassovsky1940		{	6378245.0,		298.3			};
		constexpr Spheroid maupertuis1738		{	6397300.0,		191.0			};
		constexpr Spheroid plessis1817			{	6376523.0,		308.64			};
		constexpr Spheroid southamerican1969	{	6378160.0,		298.25			};
		constexpr Spheroid wgs1966				{	6378145.0,		298.25			};
		constexpr Spheroid wgs1972				{	6378135.0,		298.26			};
		constexpr Spheroid wgs1984				{	6378137.0,		298.257223563	};
	}

}	// namespace pax
