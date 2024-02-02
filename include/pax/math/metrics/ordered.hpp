//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <span>
#include <array>
#include <cmath>		// std::fma, std::lerp
#include <algorithm>	// std::sort, std::clamp


namespace pax { 

	/// Sort the items.
	template< typename T, std::size_t N >
	constexpr auto order( std::span< T, N > sp_ ) 							noexcept	{
		std::sort( sp_.begin(), sp_.end() );
		return sp_;
	}

	/// Items are const and can not be reordered. If they are not already ordered, an exception is thrown. 
	template< typename T, std::size_t N >
	constexpr auto order( const std::span< const T, N > sp_ ) 				noexcept	{
		assert( is_sorted( sp_ ) && "Const values cannot be ordered." );
		return sp_;
	}



namespace ordered {

	template< std::floating_point F >
	constexpr F		NaN{ std::numeric_limits< F >::quiet_NaN() };


	/// Return the smallest value. An empty std::span returns NaN. 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N >
	constexpr F min( const std::span< F, N > sp_ )							noexcept	{
		return sp_.empty() ? NaN< F > : sp_.front();
	}

	/// Return the largest value. An empty std::span returns NaN. 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N >
	constexpr F max( const std::span< F, N > sp_ )							noexcept	{
		return sp_.empty() ? NaN< F > : sp_.back();
	}


	/// Counts the number of items < v_.
	/** This is also the index of the first value >= v_.
		If sp_ is not ordered, the result is undefined.										**/
	template< std::floating_point F, std::size_t N, typename T0 >
	constexpr F count_lt( const std::span< F, N > sp_, const T0 v_ )		noexcept	{
		return std::distance( sp_.begin(), std::lower_bound( sp_.begin(), sp_.end(), v_ ) );
	}

	/// Counts the number of items >= v_.
	/** If sp_ is not ordered, the result is undefined.										**/
	template< std::floating_point F, std::size_t N, typename T0 >
	constexpr F count_ge( const std::span< F, N > sp_, const T0 v_ )		noexcept	{
		return sp_.size() - count_lt( sp_, v_ );
	}



	/// Return the quantile, calculated by linear interpolating of the nearest samples. 
	/** q_ is clamped to [0, 1]. 
		If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N >
	constexpr F quantile( const std::span< F, N > sp_, const double q_ ) 	noexcept	{
		if( sp_.size() ) {
			const F				f{ std::clamp( F( q_ ), F( 0 ), F( 1 ) ) * ( sp_.size() - 1 ) };
			const std::size_t	i( f );
			return	( sp_.size() > i + 1 )	? std::lerp( sp_[ i ], sp_[ i+1 ], f - i )
											: sp_.back();
		}
		return NaN< F >;
	}

	/// Return the percentile, calculated by linear interpolating of the nearest samples. 
	/** percentile_ is clamped to [0, 100]. 
		If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N > 
	constexpr F percentile( const std::span< F, N > sp_, std::size_t p_ )	noexcept	{
		return quantile( sp_, 0.01*p_ );
	}

	/// Return the quartile, calculated by linear interpolating of the nearest samples. 
	/** quartile_ is clamped to [0, 4].
		If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N > 
	constexpr F quartile( const std::span< F, N > sp_, std::size_t q_ ) 	noexcept	{
		return quantile( sp_, 0.25*q_ );
	}

	/// Return median, calculated by linear interpolating of the nearest samples. 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::floating_point F, std::size_t N > 
	constexpr F median( const std::span< F, N > sp_ ) 						noexcept	{
		return quantile( sp_, 0.5 );
	}



	/// Result object of median_mad.
	template< std::floating_point F >
	class MAD {
		std::remove_cv_t< F >		m_median, m_mad;

	public:
		template< std::size_t N > 
		constexpr MAD( const std::span< F, N > sp_ ) 						noexcept	{

			m_median			  = ordered::median( sp_ );
			if( sp_.size() == 0 )	{
				m_mad = -1;
				return;
			} else if( sp_.size() == 1 )	{
				m_mad = 0;
				return;
			}

			auto					d_itr{ sp_. begin() };
			auto					u_itr( sp_.rbegin() );
			std::remove_cv_t< F >	d_val{ m_median - *d_itr }, 
									u_val{ *u_itr - m_median }, 
									val{}, val2{};
	
			// If sp_.size() is even we want to take the average of the two middle values...
			std::ptrdiff_t 			count( sp_.size()/2 + 1 );

			// We use the fact that sp_ is ordered and so is sp_ - median( sp_ ).
			// However, abs( * ) means that it is ordered "from both ends towards median( sp_ )".
			// So at each iteration, we check what end is largest. 
			while( count > 0 ) {
				val2		  = val;
				if( d_val > u_val ) {
					val		  = d_val;
					d_val	  = m_median - *( ++d_itr );	// From the top, m_median is greater.
				} else {
					val		  = u_val;
					u_val	  = *( ++u_itr ) - m_median;	// From the bottom, m_median is smaller.
				}
				--count;
			}

			m_mad = sp_.size()%2 ? val : ( val + val2 )*0.5;
		}

		constexpr bool valid()				const noexcept	{	return mad() >= 0;	}
		constexpr F median()				const noexcept	{	return m_median;	}
		constexpr F mad()					const noexcept	{	return m_mad;		}
	};

	template< std::floating_point F, std::size_t N > 
	constexpr MAD< F > median_mad( const std::span< F, N > sp_ )		noexcept	{
		return MAD< F >{ sp_ };
	}




	template< std::size_t r > 							struct L_moment_detail;
	template< std::size_t r, std::floating_point F > 	class L_moment_array;
	template< std::size_t r >							struct TL_moment_detail;
	template< std::size_t r, std::floating_point F > 	class TL_moment_array;


	/// Return the L-moment of order r. 
	/** There is the general implementation and also specializations for r = 1, 2, 3, and 4.
		If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::size_t r, std::floating_point F, std::size_t N > 
	constexpr F L_moment( const std::span< F, N > sp_ )					noexcept	{
		return L_moment_detail< r >()( sp_ );
	}

	/// Return L-moment ratio, the same as L_moment< r >()/L_moment< 2 >(). 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::size_t r, std::floating_point F, std::size_t N > 
	constexpr F L_moment_ratio( const std::span< F, N > sp_ )			noexcept	{
		static_assert( r > 2 );
		if( sp_.size() < r )				return NaN< F >;
		const L_moment_array< r, F >		l( sp_ );
		return l[ r ]/l[ 2 ];
	}


	/// Return the TL-moment of order r, t, s. 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::size_t r, std::floating_point F, std::size_t N > 
	constexpr F TL_moment( 
		std::span< F, N >			sp_,
		std::size_t 				s_, 
		std::size_t 				t_ 
	) noexcept	{	return ( s_ + t_ == 0 ) ? L_moment< r >( sp_ ) : TL_moment_detail< r >()( sp_, s_, t_ );	}

	/// The same as TL_moment< r >( s_, s_ ). 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::size_t r, std::floating_point F, std::size_t N > 
	constexpr F TL_moment( 
		std::span< F, N > 			sp_,
		std::size_t 				s_ 
	) noexcept	{	return TL_moment< r >( sp_, s_, s_ );											}

	/// Return TL-moment ratio, the same as TL_moment< r >( s_, t_ )/TL_moment< 2 >( s_, t_ ). 
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< std::size_t r, std::floating_point F, std::size_t N > 
	constexpr F TL_moment_ratio( 
		std::span< F, N >			sp_,
		std::size_t 				s_, 
		std::size_t 				t_ 
	) noexcept	{
		static_assert( r > 2 );
		if( s_ + t_ == 0 )					return L_moment_ratio< r >( sp_ );
		const TL_moment_array< r, F >		l( sp_, s_, t_ );
		return l[ r ]/l[ 2 ];
	}


	/// Output the quartiles of sp_.
	/** If sp_ is not ordered, the result is undefined. If sp_ is empty, NaN is returned.	**/
	template< typename Out, std::floating_point F, std::size_t N >
	Out & quartiles( Out & out_, std::span< F, N > sp_ ) {
		return out_	<< '['  << sp_.min()
					<< ", " << quartile( sp_, 1 )
					<< ", " << median( sp_ )
					<< ", "	<< quartile( sp_, 3 )
					<< ", "	<< sp_.max()
					<< ']';
	}




	/// Algorith to efficiently calculate the L-moment for multiple 'r'.
	template< std::size_t r, std::floating_point F > 
	class L_moment_array : public std::array< std::remove_cv_t< F >, r+1 > {
		using V						  = std::remove_cv_t< F >;
		using Base					  = std::array< std::remove_cv_t< F >, r+1 >;
		struct L_moment_recursive;
		
	public:
		template< std::floating_point Fsp, std::size_t N >
		constexpr L_moment_array( const std::span< Fsp, N > sp_ ) noexcept : Base{} {
			L_moment_recursive			W( sp_.size() );
			( *this )[ 0 ] 			  = sp_.size();
			auto itr	   			  = sp_.begin();
			const auto itrE			  = sp_.end();
			while( itr < itrE ) {
				for( std::size_t R=1; R<=r; ++R )
					( *this )[ R ]	 += *itr * W[ R ];
				++W;
				++itr;
			}
			for( std::size_t R=1; R<=r; ++R )
				( *this )[ R ]		 /= ( *this )[ 0 ];
		}
	};

	template< std::size_t r, std::floating_point F > 
	struct L_moment_array< r, F >::L_moment_recursive : Base {
		constexpr L_moment_recursive( const int64_t n_ ) noexcept : Base{}, kn2( 1 - n_ ) {
			static_assert( r > 0 );
			( *this )[ 0 ]	  =  n_;
			( *this )[ 1 ]	  =  1;
			if constexpr( r > 1 )		( *this )[ 2 ]	  = -1;
			if constexpr( r > 2 )		pp< 3 >();
		}
	
		constexpr L_moment_recursive & operator++()	noexcept {
			kn2				 += 2;
			if constexpr( r > 1 )		( *this )[ 2 ]	 += 2/( ( *this )[ 0 ] - 1 );
			if constexpr( r > 2 )		pp< 3 >();
			return *this;
		}

	private: 
		int64_t 		kn2;
	
		template< std::size_t R >
		constexpr void pp() noexcept {
			( *this )[ R ]	  =	(	( *this )[ R-1 ]*( 2*R - 3 )*kn2 
								  - ( *this )[ R-2 ]*( R - 2 )*( ( *this )[ 0 ] + R - 2 ) 
								) / ( ( R-1 )*( ( *this )[ 0 ] - R+1 ) );
			if constexpr( R < r )		pp< R+1 >();
		}
	};

	/// Algorith to efficiently calculate the L-moment for one 'r'.
	template< std::size_t r > 
	struct L_moment_detail {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()( const std::span< F, N > sp_ )	const noexcept	{
			static_assert( r > 0 );
			if( sp_.size() < r )				return NaN< F >;

			using V			  = std::remove_cv_t< F >;
			std::array< V, r >	P{};			// The terms, the values are iteratively updated.
			auto x			  = sp_.begin();	// A pointer to the first x value.
		
			// Set up the "point" value (P_{r,0,0}).
			P[ 0 ]			  = ( ( r % 2 ) ? +1 : -1 )/V( sp_.size() );
			V sum			  = *x * P[ 0 ];

			// The first r iterations, there are only j terms. Thereafter there are r items. 
			for( std::size_t j=1; j<r; ++j ) {
				V sum_k		  = ( P[ j ] = -( ( r + j - 1 )*( r - j )*P[ j - 1 ]/( j*( sp_.size() - j ) ) ) );
				for( std::size_t k=0; k<j; ++k )
					sum_k	 += ( P[ k ] *= j/V( j - k ) );
				sum			 += sum_k * *( ++x );
			}

			// Do the rest of the values (r terms for each x).
			for( std::size_t j=r; j<sp_.size(); ++j ) {
				V sum_k		  = 0;
				for( std::size_t k=0; k<r; ++k )
					sum_k	 += ( P[ k ] *= j/V( j - k ) );
				sum			 += sum_k * *( ++x );
			}

			return sum;
		}
	};

	template<> struct L_moment_detail< 1u > {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()( const std::span< F, N > sp_ )	const noexcept	{
			if( sp_.size() < 1 )		return NaN< F >;
			std::remove_cv_t< F >		sum{};
			for( auto v : sp_ )			sum += v;
			return sum/sp_.size();
		}
	};
	template<> struct L_moment_detail< 2u > {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()( const std::span< F, N > sp_ )	const noexcept	{
			if( sp_.size() < 2 )		return NaN< F >;
			std::remove_cv_t< F >		sum{};
			int64_t		 				K	  = -( sp_.size() - 1 );
			for( auto v : sp_ ) {
				sum += K*v;
				K	+= 2;
			}
			return sum/( sp_.size()*( sp_.size() - 1 ) );
		}
	};
	template<> struct L_moment_detail< 3u > {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()( const std::span< F, N > sp_ )	const noexcept	{
			if( sp_.size() < 3 )		return NaN< F >;
			std::remove_cv_t< F >		sum{};
			int64_t						i	  = 0;
			int64_t						K	  = ( sp_.size() - 1 )*( sp_.size() - 2 );
			for( auto v : sp_ ) {
				sum += K*v;
				i	+= 12;
				K	+= i - 6*sp_.size();
			}
			return sum/( sp_.size()*( sp_.size() - 1 )*( sp_.size() - 2 ) );
		}
	};
	template<> struct L_moment_detail< 4u > {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()( const std::span< F, N > sp_ )	const noexcept	{
			if( sp_.size() < 4 )		return NaN< F >;
			std::remove_cv_t< F >		sum{};
			const int64_t				K0 	  = 12*(sp_.size()*sp_.size() + 1);
			int64_t						i	  = 0;
			int64_t						K 	  = -( sp_.size() - 1 )*( sp_.size() - 2 )*( sp_.size() - 3 );
			for( auto v : sp_ ) {
				sum += K*v;
				++i;
				K	+= 60*i*( i - sp_.size() ) + K0;
			}
			return sum/( sp_.size()*( sp_.size() - 1 )*( sp_.size() - 2 )*( sp_.size() - 3 ) );
		}
	};



	/// Algorith to efficiently calculate the TL-moment for multiple 'r'.
	template< std::size_t r, std::floating_point F > 
	class TL_moment_array : public std::array< std::remove_cv_t< F >, r+1 > {
		using V						  = std::remove_cv_t< F >;
		using Base					  = std::array< std::remove_cv_t< F >, r+1 >;
		struct TL_moment_recursive;

	public:
		// This is public, so that it may be tested...
		static constexpr auto binom( const int64_t a, const int64_t b ) noexcept {
//			PAX_THROW_UNLESS_GE( a, b, 0 );
			std::size_t 		a0	  = ( a-b <= b ) ? ( b + 1 ) : ( a - b + 1 );
			std::size_t 		p	  = 1;
			for( std::size_t i=1; a0<=std::size_t( a ); ++i, ++a0 )
								p	  = ( p*a0 )/i;
			return p;
		}

		template< std::size_t N >
		constexpr TL_moment_array(
			const std::span< F, N >		sp_,
			const std::size_t 			s_
		) noexcept : TL_moment_array( sp_, s_, s_ ) {}
	
		template< std::size_t N >
		constexpr TL_moment_array(
			const std::span< F, N >		sp_,
			const std::size_t 			s_,
			const std::size_t 			t_
		) noexcept : Base{} {
			TL_moment_recursive			W( sp_.size(), s_, t_ );
			( *this )[ 0 ]			  = sp_.size();
			auto itr				  = sp_.begin() + s_;
			const auto itrE			  = sp_.end()   - t_;
			while( itr < itrE ) {
				for( std::size_t R=1; R<=r; ++R )
					( *this )[ R ]	 += *itr * W[ R ];
				++W;
				++itr;
			}
			
			for( std::size_t R=( sp_.size() - s_ - t_ ); R<=r; ++R )
				( *this )[ R ]		  = NaN< F >;
		}
	};

	template< std::size_t r, std::floating_point F > 
	struct TL_moment_array< r, F >::TL_moment_recursive : Base {

		constexpr TL_moment_recursive(
			const std::size_t 			n_,
			const std::size_t 			s_,
			const std::size_t 			t_
		) noexcept : Base{}, n( n_ ), j( s_ ), s( s_ ), t( t_ ) {
			static_assert( r > 0 );
			( *this )[ 0 ]			  = n;

			( *this )[ 1 ]			  = binom( n - j - 1, t )/F( binom( n, 1 + s + t ) );
			if constexpr( 1<r )			iterate< 2 >();
		}
	
		constexpr void operator++() noexcept {
			++j;
			( *this )[ 1 ]			 *= j*( n - j - t )/F( ( j - s )*( n - j ) );
			if constexpr( 1<r )			iterate< 2 >();
		}
	
	private:
		int64_t n, j, s, t;

		template< int64_t R0 >
		constexpr void iterate() noexcept {
			if constexpr( R0 == 2 ) {
				int64_t k_2st			  = 2 + s + t;
				( *this )[ 2 ]			  = k_2st*( k_2st*( j+1 ) - ( s+1 )*( n+1 ) )*( *this )[ 1 ]
											/F( 2*( s+1 )*( t+1 )*( n-s-t-1 ) );
				if constexpr( 2<r )			iterate< 3 >();
			} else {
				enum {	R = R0-1, Rm1 = R-1, Rp1 = R+1		};
				const int64_t k_rst		  = R + s + t;
				const int64_t k_2rst	  = k_rst + R;
				const auto A			  = k_rst*( R+s )*( n-k_rst )/F( k_2rst*( k_2rst-1 ) );
				const auto C			  = Rm1*( Rm1+t )*( n+Rm1 )/F( ( k_2rst-1 )*( k_2rst-2 ) );
				const auto a			  = Rp1*( R+t )/F( R*( k_rst+1 ) );
				const auto c			  = Rm1*k_rst/F( R*( Rm1+t ) );
				( *this )[ R+1 ]		  = ( ( j-s-A-C )*( *this )[ R ] - c*C*( *this )[ Rm1 ] )/F( a*A );
				if constexpr( R0<r )		iterate< R0+1 >();
			}
		}
	};

	/// Algorith to efficiently calculate the TL-moment for one 'r'.
	template< std::size_t r > 
	struct TL_moment_detail {
		template< std::floating_point F, std::size_t N >
		constexpr auto operator()(
			const std::span< F, N >		sp_,
			const std::size_t 			s_
		) const noexcept	{	return operator()( sp_, s_, s_ );						}

		template< std::floating_point F, std::size_t N >
		constexpr auto operator()(
			const std::span< F, N >		sp_,
			const std::size_t 			s_,
			const std::size_t 			t_
		) const noexcept	{	return TL_moment_array< r, F >( sp_, s_, t_ )[ r ];		}
	};

}}	// namespace pax::ordered
