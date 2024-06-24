//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cmath>
#include <utility>		// std::forward


// For unit tests
#define PAX_FAST_CHECK_ABOUT_ZERO( __about_zero__, __digits__ )	\
	DOCTEST_FAST_CHECK_UNARY( pax::about_zero_test<  __digits__  >( std::cerr, ( __about_zero__ ) ) )

// For unit tests
#define PAX_FAST_CHECK_SIMILAR( __first__, __second__, __digits__ )	\
	DOCTEST_FAST_CHECK_UNARY( pax::similar_test<  __digits__  >( std::cerr, ( __first__ ), ( __second__ ) ) )


namespace pax {

	/// Return true, iff all t_... are true.
	template< typename ...Ts >
	[[nodiscard]] constexpr bool all( Ts && ...ts_ )					noexcept	{
		return ( true && ... && static_cast< bool >( ts_ ) );
	}

	template< typename Pred, typename ...Ts >
		requires(( true && ... && std::is_invocable_r_v< bool, Pred, Ts > ))
	[[nodiscard]] constexpr bool all( Pred && pred_, Ts && ...ts_  )	noexcept	{
		return ( true && ... && pred_( ts_ ) );
	}

	/// Return true, iff at least one t_... is true.
	template< typename ...Ts >
	[[nodiscard]] constexpr bool any( Ts && ...ts_ )					noexcept	{
		return ( false || ... || static_cast< bool >( ts_ ) );
	}

	template< typename Pred, typename ...Ts >
		requires(( true && ... && std::is_invocable_r_v< bool, Pred, Ts > ))
	[[nodiscard]] constexpr bool any( Pred && pred_, Ts && ...ts_  )	noexcept	{
		return ( true || ... || pred_( ts_ ) );
	}

	/// Return true, iff no t_... is true.
	template< typename ...Ts >
	[[nodiscard]] constexpr bool none( Ts && ...ts_ )					noexcept	{
		return !( false || ... || static_cast< bool >( ts_ ) );
	}



	/// Return t_ < T{}.
	constexpr auto is_negative		  = []< typename T >( const T t_ )	noexcept	{
		if constexpr( std::is_unsigned_v< T > )					return false;
		else													return t_ < T{};
	};

	/// Return t_ >= T{}.
	constexpr auto is_non_negative  = []< typename T >( const T t_ )	noexcept	{
		if constexpr( std::is_unsigned_v< T > )					return true;
		else													return t_ >= T{};
	};

	/// Return t_ == T{}.
	constexpr auto is_zero			  = []< typename T >( const T t_ )	noexcept	{	return t_ == T{};	};

	/// Return t_ != T{}.
	constexpr auto is_non_zero		  = []< typename T >( const T t_ )	noexcept	{	return t_ != T{};	};

	/// Return t_ > T{}.
	constexpr auto is_positive		  = []< typename T >( const T t_ )	noexcept	{	return t_ >  T{};	};

	/// Return t_ <= T{}.
	constexpr auto is_non_positive	  = []< typename T >( const T t_ )	noexcept	{	return t_ <= T{};	};

	/// Return true iff t_ is finite.
	constexpr auto is_finite	  = []< typename T >( const T t_ )	noexcept	{
		if constexpr( std::numeric_limits< T >::has_infinity )	return std::isfinite( t_ );
		else													return true;
	};

	/// Return true iff t_ is a NaN.
	constexpr auto is_nan		  = []< typename T >( const T t_ )	noexcept	{
		if constexpr( std::numeric_limits< T >::has_quiet_NaN )	return t_ != t_;
		else													return false;
	};



	struct Compare_impl {
		template< typename Cmp, typename A, typename B, typename ...Ts >
		static constexpr bool cmp0( A a_, B b_, Ts ...ts_ )			noexcept	{
			if constexpr( sizeof...( Ts ) == 0 )	return Cmp::cmp( a_, b_ );
			else									return Cmp::cmp( a_, b_ ) && cmp0< Cmp >( b_, std::forward< Ts >( ts_ )... );
		}

		template< typename A, typename B >
		static constexpr bool integral = std::is_integral_v< A > && std::is_integral_v< B >;

		struct eq {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_equal( a_, b_ );
				else								return a_ == b_;
			}
		};
		struct ne {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_not_equal( a_, b_ );
				else								return a_ != b_;
			}
			template< typename A, typename B, typename ...Ts >
			static constexpr bool cmp0( A a_, B b_, Ts ...ts_ )	noexcept	{
				if constexpr( sizeof...( Ts ) == 0 )	return cmp( a_, b_ );
				else									return cmp( a_, b_ ) || cmp0( b_, std::forward< Ts >( ts_ )... );
			}
		};
		struct lt {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_less( a_, b_ );
				else								return a_ <  b_;
			}
		};
		struct le {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_less_equal( a_, b_ );
				else								return a_ <= b_;
			}
		};
		struct ge {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_greater_equal( a_, b_ );
				else								return a_ >= b_;
			}
		};
		struct gt {
			template< typename A, typename B >
			static constexpr bool cmp( A a_, B b_ )	noexcept	{
				if constexpr( integral< A, B > )	return std::cmp_greater( a_, b_ );
				else								return a_ >  b_;
			}
		};
	};

	/// Return true iff all arguments are sorted strictly ascending.
	constexpr auto lt = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::cmp0< Compare_impl::lt >( std::forward< Ts >( ts_ )... );
	};

	/// Return true iff all arguments are sorted weakly ascending.
	constexpr auto le = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::cmp0< Compare_impl::le >( std::forward< Ts >( ts_ )... );
	};

	/// Return true iff all arguments have equal value.
	constexpr auto eq = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::cmp0< Compare_impl::eq >( std::forward< Ts >( ts_ )... );
	};

	/// Return true iff any two arguments are unequal.
	constexpr auto ne = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::ne::cmp0( std::forward< Ts >( ts_ )... );
	};

	/// Return true iff all arguments are sorted weakly descending.
	constexpr auto ge = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::cmp0< Compare_impl::ge >( std::forward< Ts >( ts_ )... );
	};

	/// Return true iff all arguments are sorted strictly descending.
	constexpr auto gt = []< typename ...Ts >( Ts ...ts_ )	noexcept	{
		return Compare_impl::cmp0< Compare_impl::gt >( std::forward< Ts >( ts_ )... );
	};



	/// Always return a non-negative number.
	template< typename T >
	constexpr auto abs( const T t_ )						noexcept	{
		if constexpr( std::is_unsigned_v< T > )	return t_;
		else									return is_negative( t_ ) ? -t_ : t_;
	}

	/// Return the absolute difference of the arguments.
	template< typename A, typename B >
	constexpr auto abs_diff( A a_, B b_ )					noexcept	{
		return lt( a_, b_ ) ? ( b_ - a_ ) : ( a_ - b_ );
	}



	/// Return 0 <= a_ < b_, also with unsigned integers.
	template< typename A, typename B >
	constexpr bool in_range( A a_, B b_ )					noexcept	{
		return is_non_negative( a_ ) && lt( a_, b_ );
	}

	/// Return a_ <= b_ < c_, also with unsigned integers.
	template< typename A, typename B, typename C >
	constexpr bool in_range( A a_, B b_, C c_ )				noexcept	{
		return le( a_, b_ ) && lt( b_, c_ );
	}

	/// Can type To represent value from_?
	template< typename To, typename From >
	constexpr bool in_range( const From from_ ) 			noexcept	{
		return le( std::numeric_limits< To >::lowest(), from_, std::numeric_limits< To >::max() );
	}

	/// Can type From represent all values of type To?
	template< typename To, typename From >
	constexpr bool in_range() 								noexcept	{
		return le(	std::numeric_limits< From >::lowest(),	std::numeric_limits< To   >::lowest(),
					std::numeric_limits< To   >::max(),		std::numeric_limits< From >::max() 		);
	}



	/// Return the smallest of all t_.
	template< typename A, typename ...Ts >
	constexpr auto min( const A a_, Ts && ...ts_ ) 			noexcept	{
		if constexpr( sizeof...( Ts ) == 0u )				return a_;
		else {
			if constexpr( std::numeric_limits< A >::quiet_NaN() ) 
				if( is_nan( a_ ) )							return a_;
			const auto b = min( std::forward< Ts >( ts_ )... );
			return lt( a_, b ) ? a_ : b;
		}
	}

	/// Return the largest of all t_.
	template< typename A, typename ...Ts >
	constexpr auto max( const A a_, Ts && ...ts_ )			noexcept	{
		if constexpr( sizeof...( Ts ) == 0u )				return a_;
		else {
			if constexpr( std::numeric_limits< A >::quiet_NaN() ) 
				if( is_nan( a_ ) )							return a_;
			const auto b = max( std::forward< Ts >( ts_ )... );
			return gt( a_, b ) ? a_ : b;
		}
	}

	/// Returns the "middle" value of three arguments.
	template< typename T >
	constexpr T mid( T t0_, T t1_, T t2_ ) 					noexcept	{
		if constexpr( std::numeric_limits< T >::quiet_NaN() ) 
			if( is_nan( t0_ ) || is_nan( t1_ ) )			return std::numeric_limits< T >::quiet_NaN();

		return lt( t0_, t1_ )	? ( lt( t1_, t2_ ) ? t1_ : lt( t2_, t0_ ) ? t0_ : t2_ )
								: ( lt( t0_, t2_ ) ? t0_ : lt( t2_, t1_ ) ? t1_ : t2_ );
	}



	/// Keeps track of min/max values. Given prior values, one could be constructed by the function minmax( ... ).
	template< typename T >
	class Minmax {
		T	m_values[ 2 ];

	public:
		using value_type	  = T;

		constexpr Minmax()	noexcept
			: m_values{ std::numeric_limits< value_type >::max(), std::numeric_limits< value_type >::lowest() } {};

		constexpr Minmax( const T t_ )						noexcept	:	m_values{ t_, t_ } {}
		constexpr Minmax( const Minmax & ) 					noexcept	=	default;
		constexpr Minmax( Minmax && ) 						noexcept	=	default;
		constexpr Minmax & operator=( const Minmax & )		noexcept	=	default;
		constexpr Minmax & operator=( Minmax && )			noexcept	=	default;

		template< typename ...Ts >
		constexpr Minmax( const T a_, Ts && ...ts_ )		noexcept	:	Minmax( a_ ) {	( *this << ... << ts_ );	}


		/// Accumulate a value.
		template< typename U >
		constexpr Minmax & operator<<( const U u_ )			noexcept	{
			m_values[ 0 ] = pax::min( u_, min() );
			m_values[ 1 ] = pax::max( u_, max() );
			return *this;
		}

		/// Accumulate a value.
		constexpr Minmax & operator<<( const Minmax & u_ )	noexcept	{
			m_values[ 0 ] = pax::min( u_.min(), min() );
			m_values[ 1 ] = pax::max( u_.max(), max() );
			return *this;
		}

		/// Are we still in the default state?
		constexpr bool empty()							const noexcept	{	return min() > max();					}

		/// What is the smallest value?
		constexpr value_type min()						const noexcept	{	return m_values[ 0 ];					}

		/// What is the largest value?
		constexpr value_type max()						const noexcept	{	return m_values[ 1 ];					}
		
		template< std::size_t I >
		constexpr friend T get( const Minmax & mm_ )	noexcept		{
			static_assert( I < 2 );
			return mm_.m_values[ I ];
		}
	};



	template< typename T > struct Serialize;

	template< typename T >
	struct Serialize< Minmax< T > > {
		template< typename J >
		static void assign(
			J								  & j_,
			const Minmax< T >				  & minmax_
		) {
			j_[ "min" ] = minmax_.min();
			j_[ "max" ] = minmax_.max();
		}
	};




	template< typename T0, typename T1=T0 >
	static constexpr const unsigned default_digits = ( default_digits< T0 > < default_digits< T1 > )
		? ( std::is_integral_v< T0 > ? default_digits< T1 > : default_digits< T0 > )
		: ( std::is_integral_v< T1 > ? default_digits< T0 > : default_digits< T1 > );

	template< typename T >
	static constexpr const unsigned default_digits< T, T > 
		= std::is_integral_v< T > ? 0 : std::numeric_limits< T >::digits10 - 2;


	template< unsigned Digits >
	class Similar {
		static constexpr double m_delta() noexcept {
			double										result = 1;
			if constexpr( Digits > 0 )
				for( unsigned i=0; i<Digits; ++i )		result*= 10;
			return 1/result;
		}

		static constexpr const double delta{ m_delta() };

		template< typename T >
		static constexpr bool m_comp( const T t0_, const T t1_ )noexcept {
			return abs_diff( t0_, t1_ ) < ( abs( t0_ ) + abs( t1_ ) )*delta;
		}

	public:
		template< typename T >
		static constexpr bool comp( const T t_ )				noexcept {
			if constexpr( std::is_integral_v< T > )		return !t_;
			else										return abs( t_ ) < delta;
		}

		template< typename T0, typename T1 >
		static constexpr bool comp( const T0 t0_, const T1 t1_ ) noexcept {
			using T = std::common_type_t< T0, T1 >;
			if constexpr( std::is_integral_v< T > )		return eq( t0_, t1_ );
			else										return eq( t0_, t1_ ) || ( t0_	
				? ( t1_ ? m_comp< T >( t0_, t1_ ) : comp( t0_ ) )
				: ( t1_ ? comp( t1_ )			  : true      ) );
		}
	};


	/// Check if the argument is close to zero. Specify the number of zero digits with Digits.
	/**	There are two cases:
		- The argument is a non-floating point type: returns ( v ).
		- Otherwise: returns ( abs( v ) <= 10^-Digits ).

		Default value of Digits is N-2, where N is the number of digits the floating point type can represent.
		@{
	**/
	template< unsigned Digits, typename T >
	constexpr bool about_zero( const T v ) 				noexcept	{
		return Similar< Digits >::comp( v );
	}

	template< typename T >
	constexpr bool about_zero( const T v ) 				noexcept	{
		return about_zero< default_digits< T > >( v );
	}
	// @}


	/// Check if the arguments are similar. Specify the number of same digits with Digits.
	/**	There are four cases:
		- Both argument are non-floating point type: returns ( t0_ == t1_ ).
		- Both arguments are zero: returns true.
		- One argument is zero: returns similar( the-other-argument ).
		- Neither argument is zero: returns ( abs( t0_-t1_ ) <= 10^-Digits*( abs(t0_) + abs(t1_) )/2 ).

		Default value of Digits is N-2, where N is the smallest number of digits the either floating
		point type can represent.
		@{
	**/
	template< unsigned Digits, typename T0, typename T1 >
	constexpr bool similar( const T0 t0_, const T1 t1_ ) noexcept	{
		return Similar< Digits >::comp( t0_, t1_ );
	}

	template< typename T0, typename T1 >
	constexpr bool similar( const T0 t0_, const T1 t1_ ) noexcept	{
		return similar< default_digits< T0, T1 > >( t0_, t1_ );
	}
	// @}



	///////////////////////////////////////////////////////////////////////////////////////////////////////
	// For unit tests:
	// The stuff below, PAX_FAST_CHECK_ABOUT_ZERO, and PAX_FAST_CHECK_SIMILAR.
	///////////////////////////////////////////////////////////////////////////////////////////////////////

	template< unsigned Digits, typename Out, typename T0 > 
	constexpr bool about_zero_test( Out & out_, const T0 & t0_ ) {
		const bool result = about_zero< Digits >( t0_ );
		if( !result ) out_ << "FAILED: similar< " << Digits << " >( " << t0_ << " )\n";
		return result;
	}

	template< unsigned Digits, typename Out, typename T0, typename T1 > 
	constexpr bool similar_test( Out & out_, const T0 & t0_, const T1 & t1_ ) {
		const bool result = similar< Digits >( t0_, t1_ );
		if( !result ) out_ << "FAILED: similar< " << Digits << " >( " << t0_ << ", " << t1_ << " )\n";
		return result;
	}

}		// namespace pax
