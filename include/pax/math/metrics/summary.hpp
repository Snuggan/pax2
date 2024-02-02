//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <span>
#include <cmath>		// std::root, std::isnan, etc.
#include <array>
#include <type_traits>


namespace pax {

	/// Use this class if you want aggregators with weighted values, i.e. Summary.
	template< typename T >
	struct Weighted {
		using value_type  = T;
		using weight_type = value_type;
		
		value_type			x{};
		weight_type			w{};

		constexpr Weighted()									noexcept = default;
		constexpr Weighted( const Weighted & )					noexcept = default;
		constexpr Weighted( Weighted && )						noexcept = default;
		constexpr Weighted & operator=( const Weighted & )		noexcept = default;
		constexpr Weighted & operator=( Weighted && )			noexcept = default;

		constexpr Weighted( value_type x_, weight_type w_ )		noexcept : x{ x_ }, w{ w_ } {}
		constexpr Weighted( value_type x_ )						noexcept : Weighted{ x_, 1 } {}
		
		constexpr Weighted & operator+=( const Weighted & w_ )	noexcept {
			x += w_.x;		w += w_.w;
			return *this;
		}
		
		constexpr Weighted & operator*=( const Weighted & w_ )	noexcept {
			x *= w_.x;		w *= w_.w;
			return *this;
		}
	};
	
	template< typename T >
	constexpr T get_value( const Weighted< T > & wt_ )			noexcept {	return wt_.x;	}




	namespace level {
		enum : std::size_t	{	count, mean, variance, std_dev=variance, skewness, kurtosis		};
	}


	/// A class to calculate summary statistics.
	/** Use Summary_with_weight< T, P > for weighted statistics (same as Summary< Weighted< T >, P >).		**/
	template< typename T, std::size_t P = 4 >
	class Summary {
		template< typename U > struct Meta {
			static constexpr bool	uses_weights{ false };
			using value_type	  = std::decay_t< U >;
		};
		template< typename U > struct Meta< Weighted< U > > {
			static constexpr bool	uses_weights{ true };
			using value_type	  = std::decay_t< U >;
		};

		std::size_t 				m_count{};
		std::array< T, P >			m_sum{};
		
		template< std::size_t N, typename U >
		constexpr U eat( const U u_ ) noexcept {
			if constexpr( N ) {
				auto p{ eat< N-1 >( u_ ) };
				std::get< N-1 >( m_sum ) += p;
				return p *= u_;
			} else {
				++m_count;
				if constexpr( Meta< T >::uses_weights )		return U( u_.x*u_.w, u_.w );
				else										return u_;
			}
		}

	public:
		using value_type								  = typename Meta< T >::value_type;
		using summary_type								  = T;
		static constexpr std::size_t						level{ P };
		static constexpr bool								uses_weights{ Meta< T >::uses_weights };
	
		constexpr Summary()									noexcept = default;
		constexpr Summary( const Summary & )				noexcept = default;
		constexpr Summary( Summary && )						noexcept = default;
		constexpr Summary & operator=( const Summary & )	noexcept = default;
		constexpr Summary & operator=( Summary && )			noexcept = default;


		/// Add a value with weight to the sum. 
		constexpr void push_back( const summary_type v_ )	noexcept		{
			eat< P >( v_ );
		}			

		/// Get an std::span to the sums: { sum(x), sum(x^2), ... }.
		constexpr auto span()								const noexcept	{
			return std::span< const summary_type, P >( m_sum.data(), P );
		}

		/// Access the count.
		constexpr std::size_t count()						const noexcept	{
			return m_count;
		}

		/// Access the sum of items to the power of P: sum_i w_i*x_i^P.
		template< std::size_t p = 1 >	requires( P >= p )
		constexpr auto sum()								const noexcept	{
			if constexpr( !p )								return m_count;
			else if constexpr( !uses_weights )				return std::get< p-1 >( m_sum );
			else											return std::get< p-1 >( m_sum ).x;
		}

		/// Access the sum of weights to the power of P: sum_i w_i^P.
		template< std::size_t p = 1 >	requires( P >= p )
		constexpr auto weight()								const noexcept	{
			if constexpr( !p || !uses_weights )				return m_count;
			else											return std::get< p-1 >( m_sum ).w;
		}
	};

	namespace detail {
		template< typename Needed_aggregator_, typename Given_aggregator_ > 
		struct Sufficient_aggregator;

		template<
			typename Needed_T_, std::size_t Needed_P_, 
			typename Given_T_,  std::size_t Given_P_
		> struct Sufficient_aggregator< 
			Summary< Needed_T_, Needed_P_ >, 
			Summary< Given_T_,  Given_P_ > 
		> : std::bool_constant< Needed_P_ <= Given_P_ > {};
	}

	template< typename T, std::size_t P = 4 >
	using Summary_with_weight	  = Summary< Weighted< T >, P >;




	/// Create a Summary from two iterators.
	template< std::size_t P = 4, typename Iterator >
	constexpr auto summary( Iterator iter_, const Iterator end_ ) noexcept {
		using T = std::decay_t< decltype( *iter_ ) >;
		Summary< T, P >	sum;
		while( iter_ != end_ ) {
			sum.push_back( *iter_ );
			++iter_;
		}
		return sum;
	}

	/// Create a Summary from some container.
	template< std::size_t P = 4, typename Container >
	constexpr auto summary( const Container & cont_ ) noexcept {
		using std::begin, std::end;
		return summary( begin( cont_ ), end( cont_ ) );
	}

	/// Create a Summary from a C array.
	template< std::size_t P = 4, typename T, std::size_t Size >
	constexpr auto summary( const T ( &c_ )[ Size ] ) noexcept {
		return summary< P >( c_, c_+Size );
	}


	static_assert( sizeof( Summary< float, 0 > ) == sizeof( std::size_t ) + sizeof( std::size_t ) );
	static_assert( sizeof( Summary< float, 4 > ) == sizeof( std::size_t ) + 4*sizeof( float ) );
	static_assert( sizeof( Summary_with_weight< float, 0  > ) == sizeof( std::size_t ) + sizeof( std::size_t ) );
	static_assert( sizeof( Summary_with_weight< float, 4  > ) == sizeof( std::size_t ) + 8*sizeof( float ) );

	static_assert(!std::has_virtual_destructor_v< Summary< float > > );
	static_assert( std::is_nothrow_destructible_v< Summary< float > > );
	static_assert( std::is_trivially_destructible_v< Summary< float > > );

	static_assert( std::is_default_constructible_v< Summary< float > > );
	static_assert( std::is_nothrow_default_constructible_v< Summary< float > > );
	static_assert(!std::is_trivially_default_constructible_v< Summary< float > > );

	static_assert( std::is_copy_constructible_v< Summary< float > > );
	static_assert( std::is_nothrow_copy_constructible_v< Summary< float > > );
	static_assert( std::is_trivially_copy_constructible_v< Summary< float > > );

	static_assert( std::is_copy_assignable_v< Summary< float > > );
	static_assert( std::is_nothrow_copy_assignable_v< Summary< float > > );
	static_assert( std::is_trivially_copy_assignable_v< Summary< float > > );

	static_assert( std::is_move_constructible_v< Summary< float > > );
	static_assert( std::is_nothrow_move_constructible_v< Summary< float > > );
	static_assert( std::is_trivially_move_constructible_v< Summary< float > > );

	static_assert( std::is_move_assignable_v< Summary< float > > );
	static_assert( std::is_nothrow_move_assignable_v< Summary< float > > );
	static_assert( std::is_trivially_move_assignable_v< Summary< float > > );

	static_assert( std::is_swappable_v< Summary< float > > );
	static_assert( std::is_nothrow_swappable_v< Summary< float > > );

	static_assert( summary( { 2, 0, 3 } ).sum< 0 >() ==  3 );
	static_assert( summary( { 2, 0, 3 } ).sum< 4 >() == 97 );





	template< typename ...T >
	struct floating_point_type {
		using type0 = typename std::common_type< T... >::type;
		using type  = typename std::conditional< std::is_floating_point_v< type0 >, type0, double >::type;
	};

	template< typename ...T >
	using floating_point_type_t = typename floating_point_type< T... >::type;

	template< typename ...T >
	constexpr auto sum_nan{ std::numeric_limits< floating_point_type_t< T... > >::quiet_NaN() };


	/// The sum of the items to the power of P (default is P = 1).
	/** \sum_i w_i * x_i^p							**/
	template< std::size_t p = 1, typename T, std::size_t P >
	constexpr auto sum( const Summary< T, P > & s_ )			noexcept	{
		return s_.template sum< p >();
	}

	/// The sum of the weights to the power of P (default is P = 1).
	/** \sum_i w_i^p							**/
	template< std::size_t p = 1, typename T, std::size_t P >
	constexpr auto weight( const Summary< T, P > & s_ )			noexcept	{
		return s_.template weight< p >();
	}

	/// The number of items summarized.
	template< typename T, std::size_t P >
	constexpr std::size_t count( const Summary< T, P > & s_ )	noexcept	{	return s_.count();		}




	/// sum< p >/count() (default is p = 1).
	/** \frac{1}{N}\sum_i x_i^p						**/
	template< std::size_t p = 1, typename T, std::size_t P >	requires( P >= p )
	constexpr auto mean( const Summary< T, P > & s_ )			noexcept	{
		using V		  = floating_point_type_t< T >;
		if constexpr( p == 0 )		return V( 1 );
		else						return V( sum< p >( s_ ) ) / weight( s_ );
	}

	/// Mean square error is the same as mean< 2 >().
	/** \frac{\sum_i x_i^2}{\sum_i w_i}						**/
	template< typename T, std::size_t P >						requires( P >= 2 )
	constexpr auto mse( const Summary< T, P > & s_ )			noexcept	{
		return mean< 2 >( s_ );
	}

	/// Root mean square error.
	/** \sqrt{\frac{\sum_i x_i^2}{\sum_i w_i}}				**/
	template< typename T, std::size_t P >						requires( P >= 2 )
	constexpr auto rmse( const Summary< T, P > & s_ )			noexcept	{
		return std::sqrt( mse( s_ ) );
	}

	/// Moment p of the items (same as mean< p >).
	/** \frac{\sum_i w_i x_i^p}{\sum_i w_i}					**/
	template< std::size_t p = 1, typename T, std::size_t P >	requires( P >= p )
	constexpr auto moment( const Summary< T, P > & s_ )			noexcept	{
		return mean< p >( s_ );
	}

	/// Central moment p of the items, using pascal triangle.
	/** \sum_i (x_i-\bar{x})^p						**/
	template< std::size_t p, std::size_t p0 = p, typename T, std::size_t P, typename V = floating_point_type_t< T > >
	constexpr auto pascal_cmoment_raw( 
		const Summary< T, P > & s_,
		V								mean_	= V{}, 
		const V		 					pascal_	= V( 1 )
	) noexcept {
		static_assert( p <= P, "Summary level out of bounds" );
		if constexpr( p == 0 ) {
			return weight( s_ )*pascal_;
		} else {
			if constexpr( p == p0 )		mean_	= -mean( s_ );
			constexpr auto				factor{ p/V( p0+1-p ) };
			return std::fma( sum< p >( s_ ), pascal_, 
										pascal_cmoment_raw< p-1, p0 >( s_, mean_, pascal_*mean_*factor ) );
		}
	}

	/// Central moment p of the items, somewhat optimized.
	/** \sum_i (x_i-\bar{x})^p						**/
	template< std::size_t p, typename T, std::size_t P >			requires( P >= p )
	constexpr auto cmoment_raw( const Summary< T, P > & s_ )		noexcept	{
		using V = floating_point_type_t< T >;
		if      constexpr( p == 0 )		return V( count( s_ ) );
		else if constexpr( p == 1 )		return V{ 0 };
		else if constexpr( p >= 4 )		return pascal_cmoment_raw< p >( s_ );
		else {
			const auto					m{ mean( s_ ) };
			if      constexpr( p == 2 )	return std::fma( -sum( s_ ), m, sum< 2 >( s_ ) );
			else if constexpr( p == 3 )	
				return std::fma( -std::fma( -2*sum( s_ ), m, 3*sum< 2 >( s_ ) ), m, sum< 3 >( s_ ) );
		}
	}

	/// Central moment p of the items, somewhat optimized.
	/** \frac{1}{N}\sum_i (x_i-\bar{x})^p			**/
	template< std::size_t p, typename T, std::size_t P >			requires( P >= p )
	constexpr auto cmoment( const Summary< T, P > & s_ )			noexcept	{
		using V = floating_point_type_t< T >;
		if      constexpr( p == 0 )		return V( 1 );
		else if constexpr( p == 1 )		return V{ 0 };
		else 							return cmoment_raw< p >( s_ )/weight( s_ );
	}


	/// The population variance of the items (factor 1/n).
	/** \frac{1}{N}\sum_i (x_i-\bar{x})^2
		See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 2 )
	constexpr auto population_variance( const Summary< T, P > & s_ ) noexcept	{
		return cmoment< 2 >( s_ );
	}

	/// The sample variance of the items (factor 1/(n-1)).
	/** \frac{1}{N-1}\sum_i (x_i-\bar{x})^2
		See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 2 )
	constexpr auto sample_variance( const Summary< T, P > & s_ )	noexcept	{
		return cmoment_raw< 2 >( s_ ) * weight( s_ )/( weight( s_ )*weight( s_ ) - weight< 2 >( s_ ) );
	}

	/// The standard deviation of the items.
	/** \sqrt{\frac{1}{N}\sum_i (x_i-\bar{x})^2}
		See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 2 )
	constexpr auto population_std_dev( const Summary< T, P > & s_ ) noexcept	{
		return std::sqrt( population_variance( s_ ) );
	}

	/// The standard deviation of the items.
	/** \sqrt{\frac{1}{N-1}\sum_i (x_i-\bar{x})^2}
		See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 2 )
	constexpr auto sample_std_dev( const Summary< T, P > & s_ )		noexcept	{
		return std::sqrt( sample_variance( s_ ) );
	}

	/// The Fisher-Pearson standardized moment coefficient.
	template< typename T, std::size_t P >
	constexpr auto fisher_pearson_coeff( const Summary< T, P > & s_ ) noexcept	{
		const floating_point_type_t< T >		n( count( s_ ) );
		return std::sqrt( n*(n-1) )/( n-2 );
	}

	/// Same as population_skewness. The skewness of the items (using the second cmoment, unbiased under normality).
	template< typename T, std::size_t P >							requires( P >= 3 )
	constexpr auto skewness_g1( const Summary< T, P > & s_ )		noexcept	{
		const auto		m{ cmoment_raw< 2 >( s_ ) };
		return	( count( s_ ) >= level::skewness )
			?	cmoment_raw< 3 >( s_ )/( m*std::sqrt( m/weight( s_ ) ) )
			:	sum_nan< T >;
	}

	/// Same as sample_skewness. The skewness of the items (unbiased under normality).
	template< typename T, std::size_t P >							requires( P >= 3 )
	constexpr auto skewness_G1( const Summary< T, P > & s_ )		noexcept	{
		return skewness_g1( s_ )*fisher_pearson_coeff( s_ );
	}

	/// The skewness of the items (using the standard deviation, unbiased under normality).
	template< typename T, std::size_t P >							requires( P >= 3 )
	constexpr auto skewness_b1( const Summary< T, P > & s_ )		noexcept	{
		const auto		s{ sample_std_dev( s_ ) };
		return	( count( s_ ) >= level::skewness )
			?	cmoment_raw< 3 >( s_ )/( weight( s_ )*s*s*s )
			:	sum_nan< T >;
	}

	/// The skewness of the items (using the second cmoment, unbiased under normality).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 3 )
	constexpr auto population_skewness( const Summary< T, P > & s_ ) noexcept	{
		return skewness_g1( s_ );
	}

	/// The skewness of the items (unbiased under normality).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 3 )
	constexpr auto sample_skewness( const Summary< T, P > & s_ )	noexcept	{
		return skewness_G1( s_ );
	}


	/// Same as population_kurtosis. The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto population_kurtosis_fisher( const Summary< T, P > & s_ ) noexcept	{
		const auto		m2{ cmoment_raw< 2 >( s_ ) };
		return	( count( s_ ) >= level::kurtosis )
			?	cmoment_raw< 4 >( s_ )*weight( s_ )/( m2*m2 ) - 3
			:	sum_nan< T >;
	}

	/// Same as population_kurtosis. The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto population_kurtosis_pearson( const Summary< T, P > & s_ ) noexcept {
		const auto var{ population_variance( s_ ) };
		return	( count( s_ ) >= level::kurtosis )
			?	cmoment< 4 >( s_ ) / ( var*var )
			:	sum_nan< T >;
	}

	enum class Kurtosis {	fisherK, pearson	};

	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto sample_kurtosis_detail( 
		const Summary< T, P >	  & s_,
		const Kurtosis							kurtosis_
	) noexcept {
		// Sources:
		// [ 1 ] Formal C++ suggestion, with errors in formulas: http://wg21.link/P1708
		// [ 2 ] Formulas used here: https://arxiv.org/pdf/1304.6564.pdf

		if( count( s_ ) < level::kurtosis )	return sum_nan< T >;

		using FP = floating_point_type_t< T >;
		constexpr auto sq = []( auto x ){ return x*x; };

		const FP	sm2_w( sq( weight< 1 >( s_ ) ) );

		const FP	w(
			( ( sm2_w - weight< 2 >( s_ ) ) * ( 
				  sm2_w             *sm2_w 
				- 6*sm2_w           *weight< 2 >( s_ ) 
				+ 8*weight< 1 >( s_ )*weight< 3 >( s_ ) 
				+ 3*weight< 2 >( s_ )*weight< 2 >( s_ )
				- 6*weight< 4 >( s_ )
			) )
		);

		const FP	temp( 
				- 4*weight< 1 >( s_ )*weight< 3 >( s_ ) 
				+ 3*weight< 2 >( s_ )*weight< 2 >( s_ )
		);

		const FP	p( cmoment< 4 >( s_ )*sm2_w*( sq( sm2_w ) + temp ) / sq( sample_variance( s_ ) ) );
	
		switch( kurtosis_ ) {
			case Kurtosis::pearson:
				return p / w;
			case Kurtosis::fisherK:
				return ( p
					-3*sm2_w*(
						  sq( sm2_w )
		  				- 2*sm2_w*weight< 2 >( s_ ) 
						- temp
					) ) / w;
			default:
				return sum_nan< T >;
		}
	}

	/// The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto sample_kurtosis_pearson( const Summary< T, P > & s_ ) noexcept	{
		if constexpr( Summary< T, P >::uses_weights ) {
			return sample_kurtosis_detail( s_, Kurtosis::pearson );
		} else {
			using FP = floating_point_type_t< T >;
			const auto		n{ count( s_ ) };
			const FP		v( sample_variance( s_ ) );
			return ( count( s_ ) >= level::kurtosis )
				?	cmoment_raw< 4 >( s_ )*n*( n+1 )/( ( n-1 )*( n-2 )*( n-3 )*v*v )
				:	sum_nan< T >;
		}
	}

	/// The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto sample_kurtosis_fisher( const Summary< T, P > & s_ )	noexcept	{
		if constexpr( Summary< T, P >::uses_weights ) {
			return sample_kurtosis_detail( s_, Kurtosis::fisherK );
		} else {
			const auto		n{ count( s_ ) };
			return ( n >= level::kurtosis )
				?	sample_kurtosis_pearson( s_ ) - 3.0f*n*n/( (n-2)*(n-3) )
				:	sum_nan< T >;
		}
	}

	/// The kurtosis of the items (unbiased under normality).
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto kurtosis_G2( const Summary< T, P > & s_ )		noexcept	{
		const auto		n{ count( s_ ) };
		return	( n >= level::kurtosis )
			?	std::fma( population_kurtosis_fisher( s_ ), n+1, 6 )*(n-1)/( (n-2)*(n-3) )
			:	sum_nan< T >;
	}

	/// The kurtosis of the items (using standard deviation).
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto kurtosis_b2( const Summary< T, P > & s_ )		noexcept	{
		const auto		v{ sample_variance( s_ ) };
		return	( count( s_ ) >= level::kurtosis )
			?	cmoment_raw< 4 >( s_ )/( weight( s_ )*v*v ) - 3
			:	sum_nan< T >;
	}

	/// The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto population_kurtosis( const Summary< T, P > & s_ ) noexcept	{
		return population_kurtosis_fisher( s_ );
	}

	/// The kurtosis of the items (using the second cmoment).
	/** See http://wg21.link/P1708					**/
	template< typename T, std::size_t P >							requires( P >= 4 )
	constexpr auto sample_kurtosis( const Summary< T, P > & s_ )	noexcept	{
		return sample_kurtosis_fisher( s_ );
	}


	template< typename Ostream, typename T, std::size_t P >
	auto & operator<<(
		Ostream								  & out_,
		const Summary< T, P >				  & stats_
	) {
		out_ 										 << "{\"items\":            " << count( stats_ );
		if constexpr ( P >= level::mean )		out_ << ", \"mean\":            " << mean( stats_ );
		if constexpr ( P >= level::std_dev ) 	out_ << ", \"sample-std-dev\":  " << sample_std_dev( stats_ );
		if constexpr ( P >= level::skewness )	out_ << ", \"sample-skewness\": " << sample_skewness( stats_ );
		if constexpr ( P >= level::kurtosis )	out_ << ", \"sample-kurtosis\": " << sample_kurtosis( stats_ );
		return out_	<< '}';
	}	

}	// namespace pax