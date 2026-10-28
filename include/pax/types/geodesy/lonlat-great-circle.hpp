//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once
#include "lonlat.hpp"
#include "../polynomial-extras.hpp"
#include <numbers>		// std::numbers::pi_v


namespace pax {
	template< floating F >
	constexpr F deg2rad( const F deg )	noexcept	{	return deg*( std::numbers::pi_v< F >/F( 180 ) );		}

	template< floating F >
	constexpr F rad2deg( const F rad )	noexcept	{	return rad*( F( 180 )*std::numbers::inv_pi_v< F > );	}



	/// Implementing great circle distances.
	template< typename Algo, Spheroid Sph, floating F, std::size_t N >
	class Great_circle_distance {
		F				m_alpha0{}, m_sigma1{}, m_sigma2{}, m_epsilon{};
		
		template< std::size_t M >	// chebyshev2_x2 is more efficient than chebyshev2.
		static constexpr auto cheby2 = chebyshev2_x2< M, F, Algo >;	

		static constexpr F sqr ( const F x ) 				noexcept	{	return x*x;							}

		// Eq.  8 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F lambda( 
			const F omega,
			const F sigma
		)	const noexcept	{	return omega - Sph.f()*std::sin( alpha0() )*I3( sigma, epsilon() );	}
		
	protected:		// Debugging purposes...

		// Eq. 10 in "Algorithms for geodesics" by Charles F. F. Karney. 
		// Cached as it is used in many places.
		constexpr F alpha0()								const noexcept	{	return m_alpha0;				}

		// Eq. 11 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F sigma1()								const noexcept	{	return m_sigma1;				}

		// Eq. 20 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F sigma2()								const noexcept	{	return m_sigma2;				}

		// Eq. 16 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F epsilon()								const noexcept	{	return m_epsilon;				}

		// Eq. 17 in "Algorithms for geodesics" by Charles F. F. Karney. 
		static constexpr F A1( const F epsilon_ )			noexcept		{
			static constexpr auto A1poly = polynomial< Algo >( 1.0, 0.25, 1.0/64, 1.0/256 );
			return A1poly( epsilon_*epsilon_ )/( 1 - epsilon_ );
		}

		// Eq. 15, 17, 18 in "Algorithms for geodesics" by Charles F. F. Karney. 
		static constexpr F I1(
			const F sigma_,
			const F eps_	// epsilon
		) noexcept {
			// return 0.76831538886412;
			static constexpr auto C11 = ( polynomial< Algo >(  -16.0,  6.0, -1.0 )/  32 );
			static constexpr auto C12 = ( polynomial< Algo >( -128.0, 64.0, -9.0 )/2048 );
			static constexpr auto C13 = ( polynomial< Algo >(  -16.0,  9.0 		 )/ 768 );
			static constexpr auto C14 = ( polynomial< Algo >(   -5.0,  3.0		 )/ 512 );
			static constexpr auto C15 = ( polynomial< Algo >(   -7.0			 )/1280 );
			static constexpr auto C16 = ( polynomial< Algo >(   -7.0			 )/2048 );

			const F eps2			  = eps_*eps_;
			const F cos_sigma		  = std::cos( sigma_ );

			// sin(nx) = chebyshev2< n-1 >( cos(x) )*sin(x): https://en.wikipedia.org/wiki/Chebyshev_polynomials
			return	fma( std::sin( sigma_ ),
							eps_ * fma( C11( eps2 ), cheby2<  1 >( cos_sigma ),
							eps_ * fma( C12( eps2 ), cheby2<  3 >( cos_sigma ),
							eps_ * fma( C13( eps2 ), cheby2<  5 >( cos_sigma ),
							eps_ * fma( C14( eps2 ), cheby2<  7 >( cos_sigma ),
							eps_ * fma( C15( eps2 ), cheby2<  9 >( cos_sigma ),
							eps_ * 	    C16( eps2 )* cheby2< 11 >( cos_sigma ) ) ) ) ) ),
						sigma_ ) * A1( eps_ );
		}

		// Eq. 23 in "Algorithms for geodesics" by Charles F. F. Karney. 
		static constexpr F I3(
			const F sigma_,
			const F eps_	// epsilon
		) noexcept {
			static constexpr F n	 = Sph.ff();
			static constexpr auto A3 = polynomial< Algo >(
				( polynomial< Algo >( 128.0 			 )/128 ) ( n ),
				( polynomial< Algo >( -64.0,  64.0		 )/128 ) ( n ),
				( polynomial< Algo >( -32.0, -16.0, 48.0 )/128 ) ( n ),
				( polynomial< Algo >(  -8.0, -24.0, -8.0 )/128 ) ( n ),
				( polynomial< Algo >(  -6.0,  -4.0 		 )/128 ) ( n ),
				( polynomial< Algo >(  -3.0 			 )/128 ) ( n )
			);
			static constexpr auto C31 = polynomial< Algo >(
				( polynomial< Algo >( 32.0, -32.0	  	 )/128 ) ( n ),
				( polynomial< Algo >( 16.0,   0.0, -16.0 )/128 ) ( n ),
				( polynomial< Algo >(  6.0,   6.0,  -2.0 )/128 ) ( n ),
				( polynomial< Algo >(  5.0,   2.0		 )/128 ) ( n ),
				( polynomial< Algo >(  3.0			 	 )/128 ) ( n )
			);
			static constexpr auto C32 = polynomial< Algo >(
				( polynomial< Algo >( 16.0, -24.0,   8.0 )/256 ) ( n ),
				( polynomial< Algo >( 12.0,  -8.0, -12.0 )/256 ) ( n ),
				( polynomial< Algo >(  6.0,   2.0 		 )/256 ) ( n ),
				( polynomial< Algo >(  5.0 				 )/256 ) ( n )
			);
			static constexpr auto C33 = polynomial< Algo >(
				( polynomial< Algo >(  5.0,  -9.0,   5.0 )/192 ) ( n ),
				( polynomial< Algo >(  9.0, -10.0 		 )/384 ) ( n ),
				( polynomial< Algo >(  7.0 				 )/512 ) ( n )
			);
			static constexpr auto C34 = polynomial< Algo >(
				( polynomial< Algo >(  7.0, -14.0 		 )/512 ) ( n ),
				( polynomial< Algo >(  7.0 				 )/512 ) ( n )
			);
			static constexpr auto C35 = polynomial< Algo >(
				( polynomial< Algo >( 21.0/2560 		 )	   ) ( n )
			);

			const F eps2			  = eps_*eps_;
			const F cos_sigma 		  = std::cos( sigma_ );

			// sin(nx) = chebyshev2< n-1 >( cos(x) )*sin(x): https://en.wikipedia.org/wiki/Chebyshev_polynomials
			return	fma( std::sin( sigma_ ), 
							eps_ * fma( C31( eps2 ), cheby2< 1 >( cos_sigma ),
							eps_ * fma( C32( eps2 ), cheby2< 3 >( cos_sigma ),
							eps_ * fma( C33( eps2 ), cheby2< 5 >( cos_sigma ),
							eps_ * fma( C34( eps2 ), cheby2< 7 >( cos_sigma ),
							eps_ *      C35( eps2 )* cheby2< 9 >( cos_sigma ) ) ) ) ),
						sigma_
					) * A3( eps_ );	// Eq. 24 in "Algorithms for geodesics" by Charles F. F. Karney. 
		}

		constexpr F lambda2()	const noexcept	{
			const F s2				  = sigma2();
			// Eq. 12 in "Algorithms for geodesics" by Charles F. F. Karney. 
			const F omega2			  = std::atan2(
				std::sin( alpha0() )*std::sin( s2 ),	
				std::cos( s2 )
			);
			return lambda( omega2, s2 );
		}

		// Eq.  6 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F phi2()		const noexcept	{
			const F a0				  = alpha0();
			const F s2				  = sigma2();
			const F cosa0			  = std::cos( a0 );
			// Eq. 13 in "Algorithms for geodesics" by Charles F. F. Karney. 
			const F beta2			  = std::atan2(
				cosa0*std::sin( s2 ),	
				std::hypot( cosa0*std::cos( s2 ), std::sin( a0 ) )
			);
			return std::atan2( std::tan( beta2 ), 1 - Sph.f() );
		}

		// // Eq. 14 in "Algorithms for geodesics" by Charles F. F. Karney.
		// constexpr F alpha2()	const noexcept	{
		// 	const F a0		  = alpha0();
		// 	return std::atan2(
		// 		std::sin( a0 ),
		// 		std::cos( a0 )*std::cos( sigma2() )
		// 	);
		// }

		// constexpr F lambda12()	const noexcept	{
		// 	const F sigma1	  = sigma1();
		// 	// Eq. 12 in "Algorithms for geodesics" by Charles F. F. Karney.
		// 	const F omega1	  = std::atan2( // Eq 12.
		// 		std::sin( alpha0() )*std::sin( sigma1 ),
		// 		std::cos( sigma1 )
		// 	);
		// 	return lambda2() - lambda( omega1, sigma1 );
		// }

	public:
		constexpr Great_circle_distance(
			const Lonlat< Sph, F, N >	pos1_, 
			const F						alpha_radians_,
			const F						distance_
		) noexcept {
			{
				// Calculate alpha0 and sigma1.
				const F beta1		  = Sph.parametric_latitude( deg2rad( lat( pos1_ ) ) );		// Eq 6.
				const F sina		  = std::sin( alpha_radians_ );
				const F cosa		  = std::cos( alpha_radians_ );
				const F sinb		  = std::sin( beta1 );
				const F cosb		  = std::cos( beta1 );
				m_alpha0			  = std::atan2( sina*cosb, std::hypot( cosa, sina*sinb ) ); // Eq 10.
				m_sigma1			  = std::atan2( sinb, cosa*cosb );							// Eq 11.
			} {
				// Calculate epsilon.
				// Eq.  9 in "Algorithms for geodesics" by Charles F. F. Karney. 
				const F k2			  = Sph.ee2()*sqr( std::cos( alpha0() ) );
				const F sqrtk		  = std::sqrt( 1.0 + k2 );
				m_epsilon			  = ( sqrtk - 1.0 )/( sqrtk + 1.0 );
			} {
				// Calculate sigma2.
				// Eq. 21 in "Algorithms for geodesics" by Charles F. F. Karney. 
				static constexpr auto C11 = ( polynomial< Algo >( 30720.0, -17280.0,  8200.0 )/61440 );
				static constexpr auto C12 = ( polynomial< Algo >( 19200.0, -23680.0, 20025.0 )/61440 );
				static constexpr auto C13 = ( polynomial< Algo >( 18560.0, -36000.0			 )/61440 );
				static constexpr auto C14 = ( polynomial< Algo >( 21560.0, -57384.0			 )/61440 );
				static constexpr auto C15 = ( polynomial< Algo >( 27736.0					 )/61440 );
				static constexpr auto C16 = ( polynomial< Algo >( 38081.0					 )/61440 );

				const F eps_		  = epsilon();
				const F eps2		  = eps_*eps_;
		
				// Eq.  7 in "Algorithms for geodesics" by Charles F. F. Karney. 
				const F s1			  = I1( sigma1(), eps_ )*Sph.b();
				m_sigma2			  = ( s1 + distance_ )/( Sph.b()*A1( eps_ ) );
				const F cos_sigma2	  = std::cos( m_sigma2 );

				// sin(nx) = chebyshev2< n-1 >( cos(x) )*sin(x): https://en.wikipedia.org/wiki/Chebyshev_polynomials
				m_sigma2			  = fma( std::sin( m_sigma2 ),
												eps_ * fma( C11( eps2 ), cheby2<  1 >( cos_sigma2 ),
												eps_ * fma( C12( eps2 ), cheby2<  3 >( cos_sigma2 ),
												eps_ * fma( C13( eps2 ), cheby2<  5 >( cos_sigma2 ),
												eps_ * fma( C14( eps2 ), cheby2<  7 >( cos_sigma2 ),
												eps_ * fma( C15( eps2 ), cheby2<  9 >( cos_sigma2 ),
												eps_ *      C16( eps2 )* cheby2< 11 >( cos_sigma2 ) ) ) ) ) ),
											m_sigma2
										);
			}
		}

		constexpr Lonlat< Sph, F, N > great_circle_destination() const noexcept {
			return { rad2deg( lambda2() ), rad2deg( phi2() ) };
		}		
	};


	template< typename Algo, Spheroid Sph, floating F, std::size_t N >
	static constexpr Lonlat< Sph, F, N > great_circle_destination(
		const Lonlat< Sph, F, N >		pos1_, 
		const F							alpha_radians_,
		const F							distance_
	) noexcept {
		return Destination< Algo, Sph, F, N >( pos1_, alpha_radians_, distance_ ).great_circle_destination();
	}

}	// namespace pax
