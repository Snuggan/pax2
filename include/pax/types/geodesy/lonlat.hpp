//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../base.hpp"
#include "../polynomial.hpp"
#include "spheroid.hpp"
#include <array>
#include <utility>		// std::forward
#include <numbers>		// std::numbers::pi_v


namespace pax {

	/// This is the type used for arithmetic vectors with a fixed size.
	template< Spheroid Sph, floating F, std::size_t N >
	struct Lonlat : std::array< F, N > {};

	using Lonlat2d		  = Lonlat< earth::wgs1984, double, 2 >;
	using Lonlat3d		  = Lonlat< earth::wgs1984, double, 3 >;

	/// Create a Lonlat with element type F out of a bunch of elements.
	template< Spheroid Sph, floating F, arithmetic ... Fs >
	constexpr Lonlat< Sph, F, sizeof...( Fs ) > point_t( Fs && ... as_ )						noexcept	{
		return { static_cast< F >( std::forward< Fs >( as_ ) ) ... };
	}

	/// Create a Lonlat out of a bunch of elements.
	template< Spheroid Sph, arithmetic ... Fs >
	constexpr auto lonlat( Fs && ... as_ )	noexcept	{
		using F  = std::common_type_t< Fs ... >;
		return Lonlat< Sph, F, sizeof...( Fs ) >{ static_cast< F >( std::forward< Fs >( as_ ) ) ... };
	}

	/// Create a Lonlat with specified size N and all elements set to a_.
	template< Spheroid Sph, std::size_t N, floating F >
	constexpr auto lonlat( const F a_ )		noexcept	{
		Lonlat< Sph, F, N >		temp;
		temp.fill( a_ );
		return temp;
	}


	enum {	lon_idx  = 0,	lat_idx, 	};
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lon_idx )
	constexpr F   lon( const Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lon_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lon_idx )
	constexpr F & lon(       Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lon_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lat_idx )
	constexpr F   lat( const Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lat_idx >( ll_ );		}
	template< Spheroid Sph, floating F, std::size_t N >		requires( N > lat_idx )
	constexpr F & lat(       Lonlat< Sph, F, N > & ll_ )	noexcept	{	return std::get< lat_idx >( ll_ );		}

	template< Spheroid Sph, floating F, std::size_t N >
	constexpr auto spheroid( const Lonlat< Sph, F, N > & )	noexcept	{	return Sph;							}



	template< floating F >
	constexpr F deg2rad( const F deg )	noexcept	{	return deg*( std::numbers::pi_v< F >/F( 180 ) );		}

	template< floating F >
	constexpr F rad2deg( const F rad )	noexcept	{	return rad*( F( 180 )*std::numbers::inv_pi_v< F > );	}


	template< typename Algo, Spheroid Sph, floating F, std::size_t N >
	class Destination_implementation {
		F				m_alpha0{}, m_sigma1{}, m_sigma2{}, m_epsilon{};
		
		template< std::size_t M >
		using Poly	  = Polynomial< Algo, F, M >;

		static constexpr F sqr ( const F x ) 			noexcept	{	return x*x;			}

		// Eq.  8 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F lambda( 
			const F omega,
			const F sigma
		)	const noexcept	{	return omega - Sph.f()*std::sin( alpha0() )*I3( sigma );	}
		
	protected:		// Debugging purposes...

		// Eq. 10 in "Algorithms for geodesics" by Charles F. F. Karney. 
		// Cached as it is used in many places.
		constexpr F alpha0()	const noexcept	{	return m_alpha0;						}

		// Eq. 11 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F sigma1()	const noexcept	{	return m_sigma1;						}

		// Eq. 20 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F sigma2()	const noexcept	{	return m_sigma2;						}

		// Eq. 16 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F epsilon()	const noexcept	{	return m_epsilon;						}

		// Eq. 17 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F A1()		const noexcept	{
			static constexpr Poly< 4 >	A1poly({ 1.0, 0.25, 1.0/64, 1.0/256 });
			const F 					eps_  = epsilon();
			return A1poly( eps_*eps_ )/( 1 - eps_ );
		}

		// Eq. 24 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F A3()		const noexcept	{
			static constexpr F			n = Sph.ff();
			static constexpr Poly< 6 >	A3poly({
				1.0,
				fma( n, 0.5, -0.5 ),
				Poly< 3 >({ -0.25,   -0.125,   3.0/ 8 })( n ),
				Poly< 3 >({ -1.0/16, -3.0/16, -1.0/16 })( n ),
				fma( n, -1.0/32, -3.0/64 ),
				-3.0/128
			});
			return A3poly( epsilon() );
		}

		// Eq. 23 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F I1( const F sigma )	const noexcept	{
			// return 0.76831538886412;
			constexpr Poly< 3 >	C11poly({ -0.5,    3.0/16, -1.0/32 });
			constexpr Poly< 3 >	C12poly({ -1.0/16, 1.0/32, -9.0/2048 });
			const F eps_ = epsilon();
			const F eps2 = eps_*eps_;
			const F eps4 = eps2*eps2;
			const std::array< F, 6 > C({
				 eps_     *C11poly( eps2 ),
				 eps2     *C12poly( eps2 ),
				 eps2*eps_*fma( eps2, 3.0/256, -1.0/ 48 ),
				 eps4     *fma( eps2, 3.0/512, -5.0/512 ),
				-eps4*eps_*7.0/1280,
				-eps4*eps2*7.0/2048
			});
		
			F sum			  = sigma;
			F sigmasum		  = 0.0;
			const F sigma2l	  = 2*sigma;
			for( const F c : C ) 
				sum			  = fma( c, std::sin( sigmasum += sigma2l ), sum );

			return sum*A1();
		}

		// Eq. 23 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F I3( const F sigma )	const noexcept	{
			static constexpr F			n = Sph.ff();
			static constexpr Poly< 5 >	C31poly({
				fma( n, -0.25, 0.25 ),
				Poly< 3 >({ 0.125,  0.0,    -0.125  })( n ),
				Poly< 3 >({ 3.0/64, 3.0/64, -1.0/64 })( n ),
				fma( n, 1.0/64, 5.0/128 ),
				3.0/128
			});
			static constexpr Poly< 4 >	C32poly({
				Poly< 3 >({ 1.0/16, -3.0/32,  1.0/32 })( n ),
				Poly< 3 >({ 3.0/64, -1.0/32, -3.0/64 })( n ),
				fma( n, 1.0/128, 3.0/128 ),
				5.0/256
			});
			static constexpr Poly< 3 >	C33poly({
				Poly< 3 >({ 5.0/192, -3.0/64, 5.0/192 })( n ),
				fma( n, -5.0/192, 3.0/128 ),
				7.0/512
			});
			const 		F eps_ = epsilon();
			const 		F eps2 = eps_*eps_;
			const 		F eps4 = eps2*eps2;
			const std::array< F, 5 > C({
				eps_     *C31poly( eps2 ),
				eps2     *C32poly( eps2 ),
				eps2*eps_*C33poly( eps2 ),
				eps4     *fma( eps2, 7.0/512, fma( n, -7.0/256, 7.0/512 ) ),
				eps4*eps_*21.0/2560
			});

			F sum			  = sigma;
			F sigmasum		  = 0.0;
			const F sigma2l	  = 2*sigma;
			for( const F c : C )
				sum			  = fma( c, std::sin( sigmasum += sigma2l ), sum );

			return sum*A3();
		}

		constexpr F lambda2()	const noexcept	{
			const F s2		  = sigma2();
			// Eq. 12 in "Algorithms for geodesics" by Charles F. F. Karney. 
			const F omega2	  = std::atan2(
				std::sin( alpha0() )*std::sin( s2 ),	
				std::cos( s2 )
			);
			return lambda( omega2, s2 );
		}

		// Eq.  6 in "Algorithms for geodesics" by Charles F. F. Karney. 
		constexpr F phi2()		const noexcept	{
			const F a0		  = alpha0();
			const F s2		  = sigma2();
			const F cosa0	  = std::cos( a0 );
			// Eq. 13 in "Algorithms for geodesics" by Charles F. F. Karney. 
			const F beta2	  = std::atan2(
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
		constexpr Destination_implementation(
			const Lonlat< Sph, F, N >	pos1_, 
			const F						alpha_radians_,
			const F						distance_
		) noexcept {
			{
				// Calculate alpha0 and sigma1.
				const F beta1	  = Sph.parametric_latitude( deg2rad( lat( pos1_ ) ) );	// Eq 6.
				const F sina	  = std::sin( alpha_radians_ );
				const F cosa	  = std::cos( alpha_radians_ );
				const F sinb	  = std::sin( beta1 );
				const F cosb	  = std::cos( beta1 );
				m_alpha0		  = std::atan2( // Eq 10.
					sina*cosb,	
					std::hypot( cosa, sina*sinb ) 
				);
				m_sigma1		  = std::atan2(	// Eq 11.
					sinb,
					cosa*cosb 
				);
			} {
				// Calculate epsilon.
				// Eq.  9 in "Algorithms for geodesics" by Charles F. F. Karney. 
				const F k2		  = Sph.ee2()*sqr( std::cos( alpha0() ) );
				const F sqrtk	  = std::sqrt( 1.0 + k2 );
				m_epsilon		  = ( sqrtk - 1.0 )/( sqrtk + 1.0 );
			} {
				// Calculate sigma2.
				constexpr Poly< 3 >	C11poly({ 0.5,    - 9.0/32,  205.0/1536 });
				constexpr Poly< 3 >	C12poly({ 5.0/16, -37.0/96, 1335.0/4096 });
				const F eps_ = epsilon();
				const F eps2 = eps_*eps_;
				const F eps4 = eps2*eps2;
				const std::array< F, 6 > C({
					eps_     *C11poly( eps2 ),
					eps2     *C12poly( eps2 ),
					eps2*eps_*fma( eps2, -  75.0/ 128,  29.0/  96 ),
					eps4     *fma( eps2, -2391.0/2560, 539.0/1536 ),
					eps4*eps_* 3467.0/ 7680, 
					eps4*eps2*38081.0/61440
				});
		
				// Eq.  7 in "Algorithms for geodesics" by Charles F. F. Karney. 
				const F s1		  = I1( sigma1() )*Sph.b();
				m_sigma2		  = ( s1 + distance_ )/( Sph.b()*A1() );

				F tausum		  = 0.0;
				const F tau2l	  = 2*m_sigma2;
				for( const F c : C ) 
					m_sigma2	  = fma( c, std::sin( tausum += tau2l ), m_sigma2 );
			}
		}

		constexpr Lonlat< Sph, F, N > destination() const noexcept {
			return { rad2deg( lambda2() ), rad2deg( phi2() ) };
		}
		
	};


	template< typename Algo, Spheroid Sph, floating F, std::size_t N >
	static constexpr Lonlat< Sph, F, N > destination(
		const Lonlat< Sph, F, N >	pos1_, 
		const F						alpha_radians_,
		const F						distance_
	) noexcept {
		return Destination< Algo, Sph, F, N >( pos1_, alpha_radians_, distance_ ).destination();
	}

}	// namespace pax
