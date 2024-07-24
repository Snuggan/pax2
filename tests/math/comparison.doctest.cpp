//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/math/comparison.hpp>
#include <pax/doctest.hpp>


namespace pax::doctester { 
	constexpr const float		Inf		= std::numeric_limits< float  >::infinity();
	constexpr const double		NaN		= std::numeric_limits< double >::quiet_NaN();
	
	DOCTEST_TEST_CASE( "comparison chained" ) {
		{	// none
			static_assert(  none() );
			static_assert(  none( false, false, false ) );
			static_assert( !none( true,  false, false ) );
			static_assert( !none( false, true,  false ) );
			static_assert( !none( false, false, true  ) );
			static_assert( !none( true,  true,  false ) );
			static_assert( !none( false, true,  true  ) );
			static_assert( !none( true,  false, true  ) );
			static_assert( !none( true,  true,  true  )  );

			static_assert( !none( 1u, 0<3,  true,  'e' ) );
			static_assert(  none( 0u, 0>3,  false, '\0' ) );
		}
		{	// all
			static_assert(  all() );
			static_assert( !all(  false, false, false ) );
			static_assert( !all(  true,  false, false ) );
			static_assert( !all(  false, true,  false ) );
			static_assert( !all(  false, false, true  ) );
			static_assert( !all(  true,  true,  false ) );
			static_assert( !all(  false, true,  true  ) );
			static_assert( !all(  true,  false, true  ) );
			static_assert(  all(  true,  true,  true  ) );

			static_assert(  all( 1u, 0<3,  true,  'e' ) );
			static_assert( !all( 1u, 0>3,  true,  'e' ) );
		}
		{	// any
			static_assert( !any() );
			static_assert( !any(  false, false, false ) );
			static_assert(  any(  true,  false, false ) );
			static_assert(  any(  false, true,  false ) );
			static_assert(  any(  false, false, true  ) );
			static_assert(  any(  true,  true,  false ) );
			static_assert(  any(  false, true,  true  ) );
			static_assert(  any(  true,  false, true  ) );
			static_assert(  any(  true,  true,  true  ) );

			static_assert(  any( 1u, 0<3,  true,  'e' ) );
			static_assert(  any( 1u, 0>3,  true,  'e' ) );
			static_assert(  any( 0u, 0<3, false, '\0' ) );
			static_assert( !any( 0u, 0>3, false, '\0' ) );
		}
	}

	DOCTEST_TEST_CASE( "comparison is_" ) {
		{ // is_finite
			static_assert(  is_finite( -1u  ) );
			static_assert(  is_finite(  1u  ) );
			static_assert(  is_finite(  0u  ) );
			static_assert(  is_finite(  0.0 ) );
			static_assert(  is_finite( -0.0 ) );
			static_assert(  is_finite( +0.0 ) );
			static_assert(  is_finite( -1   ) );
			static_assert(  is_finite( -1.0 ) );

			static_assert( !is_finite(  Inf ) );
			static_assert( !is_finite( -Inf ) );
			static_assert( !is_finite( -NaN ) );
			static_assert( !is_finite(  NaN ) );
		}
		{ // is_nan
			static_assert( !is_nan( -1u  ) );
			static_assert( !is_nan(  1u  ) );
			static_assert( !is_nan(  0u  ) );
			static_assert( !is_nan(  0.0 ) );
			static_assert( !is_nan( -0.0 ) );
			static_assert( !is_nan( +0.0 ) );
			static_assert( !is_nan( -1   ) );
			static_assert( !is_nan( -1.0 ) );

			static_assert( !is_nan(  Inf ) );
			static_assert( !is_nan( -Inf ) );
			static_assert(  is_nan( -NaN ) );
			static_assert(  is_nan(  NaN ) );
		}
		{ // is_negative
			static_assert( !is_negative( -1u  ) );
			static_assert( !is_negative(  1u  ) );
			static_assert( !is_negative(  0u  ) );
			static_assert( !is_negative(  0.0 ) );
			static_assert( !is_negative( -0.0 ) );
			static_assert( !is_negative( +0.0 ) );
			static_assert(  is_negative( -1   ) );
			static_assert(  is_negative( -1.0 ) );

			static_assert( !is_negative(  Inf ) );
			static_assert(  is_negative( -Inf ) );
			static_assert( !is_negative( -NaN ) );
			static_assert( !is_negative(  NaN ) );
		}
		{ // is_non_negative
			static_assert(  is_non_negative( -1u  ) );
			static_assert(  is_non_negative(  1u  ) );
			static_assert(  is_non_negative(  0u  ) );
			static_assert(  is_non_negative(  0.0 ) );
			static_assert(  is_non_negative( -0.0 ) );
			static_assert(  is_non_negative( +0.0 ) );
			static_assert( !is_non_negative( -1   ) );
			static_assert( !is_non_negative( -1.0 ) );

			static_assert(  is_non_negative(  Inf ) );
			static_assert( !is_non_negative( -Inf ) );
			static_assert( !is_non_negative( -NaN ) );
			static_assert( !is_non_negative(  NaN ) );
		}
		{ // is_zero
			static_assert( !is_zero( -1u  ) );
			static_assert( !is_zero(  1u  ) );
			static_assert(  is_zero(  0u  ) );
			static_assert(  is_zero(  0.0 ) );
			static_assert(  is_zero( -0.0 ) );
			static_assert(  is_zero( +0.0 ) );
			static_assert( !is_zero( -1   ) );
			static_assert( !is_zero( -1.0 ) );

			static_assert( !is_zero(  Inf ) );
			static_assert( !is_zero( -Inf ) );
			static_assert( !is_zero( -NaN ) );
			static_assert( !is_zero(  NaN ) );
		}
		{ // is_non_zero
			static_assert(  is_non_zero( -1u  ) );
			static_assert(  is_non_zero(  1u  ) );
			static_assert( !is_non_zero(  0u  ) );
			static_assert( !is_non_zero(  0.0 ) );
			static_assert( !is_non_zero( -0.0 ) );
			static_assert( !is_non_zero( +0.0 ) );
			static_assert(  is_non_zero( -1   ) );
			static_assert(  is_non_zero( -1.0 ) );

			static_assert(  is_non_zero(  Inf ) );
			static_assert(  is_non_zero( -Inf ) );
			static_assert(  is_non_zero( -NaN ) );
			static_assert(  is_non_zero(  NaN ) );
			static_assert(  NaN != 32.0 );
		}
		{ // is_positive
			static_assert(  is_positive( -1u  ) );
			static_assert(  is_positive(  1u  ) );
			static_assert( !is_positive(  0u  ) );
			static_assert( !is_positive(  0.0 ) );
			static_assert( !is_positive( -0.0 ) );
			static_assert( !is_positive( +0.0 ) );
			static_assert( !is_positive( -1   ) );
			static_assert( !is_positive( -1.0 ) );

			static_assert(  is_positive(  Inf ) );
			static_assert( !is_positive( -Inf ) );
			static_assert( !is_positive( -NaN ) );
			static_assert( !is_positive(  NaN ) );
		}
		{ // is_non_positive
			static_assert( !is_non_positive( -1u  ) );
			static_assert( !is_non_positive(  1u  ) );
			static_assert(  is_non_positive(  0u  ) );
			static_assert(  is_non_positive(  0.0 ) );
			static_assert(  is_non_positive( -0.0 ) );
			static_assert(  is_non_positive( +0.0 ) );
			static_assert(  is_non_positive( -1   ) );
			static_assert(  is_non_positive( -1.0 ) );

			static_assert( !is_non_positive(  Inf ) );
			static_assert(  is_non_positive( -Inf ) );
			static_assert( !is_non_positive( -NaN ) );
			static_assert( !is_non_positive(  NaN ) );
		}
	}

	DOCTEST_TEST_CASE( "binary comparison" ) {
		{	// eq
			static_assert(  eq(  0,    0   ) );
			static_assert(  eq(  0u,   0   ) );
			static_assert(  eq(  0,    0u  ) );
			static_assert(  eq(  0u,   0u  ) );
			static_assert(  eq( -0.0,  0   ) );
			static_assert(  eq(  0,   -0.0 ) );
			static_assert(  eq( -0.0,  0.0 ) );
			static_assert(  eq(  0.0, -0.0 ) );

			static_assert(  eq( -4,  -4  ) );
			static_assert(  eq(  4,   4u ) );
			static_assert(  eq(  4u,  4  ) );
			static_assert( !eq( -4,  -4u ) );
			static_assert( !eq( -4u, -4  ) );

			static_assert( !eq( -4,   5u  ) );
			static_assert( !eq(  5u, -4   ) );
			static_assert(  eq(  3,   3.0 ) );
			static_assert( !eq(  3,   3.1 ) );
			static_assert( !eq(  3.1, 3   ) );

			static_assert(  eq( -Inf, -Inf ) );
			static_assert( !eq( -Inf,  Inf ) );
			static_assert( !eq(  Inf, -Inf ) );
			static_assert( !eq(  NaN,  NaN ) );
			static_assert( !eq(  3,    NaN ) );
		}
		{	// ne 
			static_assert( !ne(  0,    0   ) );
			static_assert( !ne(  0u,   0   ) );
			static_assert( !ne(  0,    0u  ) );
			static_assert( !ne(  0u,   0u  ) );
			static_assert( !ne( -0.0,  0   ) );
			static_assert( !ne(  0,   -0.0 ) );
			static_assert( !ne( -0.0,  0.0 ) );
			static_assert( !ne(  0.0, -0.0 ) );

			static_assert( !ne( -4,  -4  ) );
			static_assert( !ne(  4,   4u ) );
			static_assert( !ne(  4u,  4  ) );
			static_assert(  ne( -4,  -4u ) );
			static_assert(  ne( -4u, -4  ) );

			static_assert(  ne( -4,   5u  ) );
			static_assert(  ne(  5u, -4   ) );
			static_assert( !ne(  3,   3.0 ) );
			static_assert(  ne(  3,   3.1 ) );
			static_assert(  ne(  3.1, 3   ) );

			static_assert( !ne( -Inf, -Inf ) );
			static_assert(  ne( -Inf,  Inf ) );
			static_assert(  ne(  Inf, -Inf ) );
			static_assert(  ne(  NaN,  NaN ) );
			static_assert(  ne(  3,    NaN ) );
		}
		{	// lt
			static_assert( !lt(  0,    0   ) );
			static_assert( !lt(  0u,   0   ) );
			static_assert( !lt(  0,    0u  ) );
			static_assert( !lt(  0u,   0u  ) );
			static_assert( !lt( -0.0,  0   ) );
			static_assert( !lt(  0,   -0.0 ) );
			static_assert( !lt( -0.0,  0.0 ) );
			static_assert( !lt(  0.0, -0.0 ) );

			static_assert( !lt( -4,  -4  ) );
			static_assert( !lt(  4,   4u ) );
			static_assert( !lt(  4u,  4  ) );
			static_assert(  lt( -4,  -4u ) );
			static_assert( !lt( -4u, -4  ) );

			static_assert(  lt( -4,   5u  ) );
			static_assert( !lt(  5u, -4   ) );
			static_assert( !lt(  3,   3.0 ) );
			static_assert(  lt(  3,   3.1 ) );
			static_assert( !lt(  3.1, 3   ) );

			static_assert( !lt( -Inf, -Inf ) );
			static_assert(  lt( -Inf,  Inf ) );
			static_assert( !lt(  Inf, -Inf ) );
			static_assert( !lt(  NaN,  NaN ) );
			static_assert( !lt(  3,    NaN ) );
		}
		{	// le
			static_assert(  le(  0,    0   ) );
			static_assert(  le(  0u,   0   ) );
			static_assert(  le(  0,    0u  ) );
			static_assert(  le(  0u,   0u  ) );
			static_assert(  le( -0.0,  0   ) );
			static_assert(  le(  0,   -0.0 ) );
			static_assert(  le( -0.0,  0.0 ) );
			static_assert(  le(  0.0, -0.0 ) );

			static_assert(  le( -4,  -4  ) );
			static_assert(  le(  4,   4u ) );
			static_assert(  le(  4u,  4  ) );
			static_assert(  le( -4,  -4u ) );
			static_assert( !le( -4u, -4  ) );

			static_assert(  le( -4,   5u  ) );
			static_assert( !le(  5u, -4   ) );
			static_assert(  le(  3,   3.0 ) );
			static_assert(  le(  3,   3.1 ) );
			static_assert( !le(  3.1, 3   ) );

			static_assert(  le( -Inf, -Inf ) );
			static_assert(  le( -Inf,  Inf ) );
			static_assert( !le(  Inf, -Inf ) );
			static_assert( !le(  NaN,  NaN ) );
			static_assert( !le(  3,    NaN ) );
		}
		{	// ge
			static_assert(  ge(  0,    0   ) );
			static_assert(  ge(  0u,   0   ) );
			static_assert(  ge(  0,    0u  ) );
			static_assert(  ge(  0u,   0u  ) );
			static_assert(  ge( -0.0,  0   ) );
			static_assert(  ge(  0,   -0.0 ) );
			static_assert(  ge( -0.0,  0.0 ) );
			static_assert(  ge(  0.0, -0.0 ) );

			static_assert(  ge( -4,  -4  ) );
			static_assert(  ge(  4,   4u ) );
			static_assert(  ge(  4u,  4  ) );
			static_assert( !ge( -4,  -4u ) );
			static_assert(  ge( -4u, -4  ) );

			static_assert( !ge( -4,   5u  ) );
			static_assert(  ge(  5u, -4   ) );
			static_assert(  ge(  3,   3.0 ) );
			static_assert( !ge(  3,   3.1 ) );
			static_assert(  ge(  3.1, 3   ) );

			static_assert(  ge( -Inf, -Inf ) );
			static_assert( !ge( -Inf,  Inf ) );
			static_assert(  ge(  Inf, -Inf ) );
			static_assert( !ge(  NaN,  NaN ) );
			static_assert( !ge(  3,    NaN ) );
		}
		{	// gt
			static_assert( !gt(  0,    0   ) );
			static_assert( !gt(  0u,   0   ) );
			static_assert( !gt(  0,    0u  ) );
			static_assert( !gt(  0u,   0u  ) );
			static_assert( !gt( -0.0,  0   ) );
			static_assert( !gt(  0,   -0.0 ) );
			static_assert( !gt( -0.0,  0.0 ) );
			static_assert( !gt(  0.0, -0.0 ) );

			static_assert( !gt( -4,  -4  ) );
			static_assert( !gt(  4,   4u ) );
			static_assert( !gt(  4u,  4  ) );
			static_assert( !gt( -4,  -4u ) );
			static_assert(  gt( -4u, -4  ) );

			static_assert( !gt( -4,   5u  ) );
			static_assert(  gt(  5u, -4   ) );
			static_assert( !gt(  3,   3.0 ) );
			static_assert( !gt(  3,   3.1 ) );
			static_assert(  gt(  3.1, 3   ) );

			static_assert( !gt( -Inf, -Inf ) );
			static_assert( !gt( -Inf,  Inf ) );
			static_assert(  gt(  Inf, -Inf ) );
			static_assert( !gt(  NaN,  NaN ) );
			static_assert( !gt(  3,    NaN ) );
		}
	}
	DOCTEST_TEST_CASE( "multiple comparison" ) {
		{	// eq
			static_assert( !eq(  4u,  0, -4.0 ) );
			static_assert( !eq(  4u,  4, -4.0 ) );
			static_assert(  eq(  4u,  4,  4.0 ) );
			static_assert( !eq( -4.0, 4,  4u  ) );
			static_assert( !eq( -4.0, 0,  4u  ) );
		}
		{	// ne
			static_assert(  ne(  4u,  0, -4.0 ) );
			static_assert(  ne(  4u,  4, -4.0 ) );
			static_assert( !ne(  4u,  4,  4.0 ) );
			static_assert(  ne( -4.0, 4,  4u  ) );
			static_assert(  ne( -4.0, 0,  4u  ) );
		}
		{	// lt
			static_assert( !lt(  4u,  0, -4.0 ) );
			static_assert( !lt(  4u,  4, -4.0 ) );
			static_assert( !lt(  4u,  4,  4.0 ) );
			static_assert( !lt( -4.0, 4,  4u  ) );
			static_assert(  lt( -4.0, 0,  4u  ) );
		}
		{	// le
			static_assert( !le(  4u,  0, -4.0 ) );
			static_assert( !le(  4u,  4, -4.0 ) );
			static_assert(  le(  4u,  4,  4.0 ) );
			static_assert(  le( -4.0, 4,  4u  ) );
			static_assert(  le( -4.0, 0,  4u  ) );
		}
		{	// ge
			static_assert(  ge(  4u,  0, -4.0 ) );
			static_assert(  ge(  4u,  4, -4.0 ) );
			static_assert(  ge(  4u,  4,  4.0 ) );
			static_assert( !ge( -4.0, 4,  4u  ) );
			static_assert( !ge( -4.0, 0,  4u  ) );
		}
		{	// gt
			static_assert(  gt(  4u,  0, -4.0 ) );
			static_assert( !gt(  4u,  4, -4.0 ) );
			static_assert( !gt(  4u,  4,  4.0 ) );
			static_assert( !gt( -4.0, 4,  4u  ) );
			static_assert( !gt( -4.0, 0,  4u  ) );
		}
	}

	DOCTEST_TEST_CASE( "comparison abs" ) {
		{	// abs
			static_assert( abs( -Inf ) ==  Inf 	);
			static_assert( abs( +Inf ) ==  Inf 	);
			static_assert( abs( -1   ) ==  1 	);
			static_assert( abs(  0   ) ==  0	);
			static_assert( abs(  1   ) ==  1	);
			static_assert( abs( -1u  ) == -1u	);
			static_assert( abs(  0u  ) ==  0u	);
			static_assert( abs(  1u  ) ==  1u	);
			static_assert( abs(  0.0 ) ==  0.0	);
			static_assert( abs( -0.0 ) ==  0.0	);
			static_assert( abs( +0.0 ) ==  0.0	);
			static_assert( abs( -1.0 ) ==  1.0	);
			static_assert( abs(  1.0 ) ==  1.0	);
			static_assert( is_nan( abs( NaN ) ) );
		}
		{	// abs_diff
			using std::isnan;
			static_assert( abs_diff(  -1,  2u )    == 3   );
			static_assert( abs_diff(  2u, -1  )    == 3   );
			static_assert( abs_diff(  1,   2  )    == 1   );
			static_assert( abs_diff(  2,   2  )    == 0   );
			static_assert( abs_diff(  3,   2  )    == 1   );
			static_assert( abs_diff(  1u,  2u )    == 1u  );
			static_assert( abs_diff(  2u,  2u )    == 0u  );
			static_assert( abs_diff(  3u,  2u )    == 1u  );
			static_assert( abs_diff(  +0.0, -0.0 ) == 0.0 );
			static_assert( abs_diff(  Inf, -Inf )  == Inf );

			DOCTEST_FAST_CHECK_UNARY( is_nan( abs_diff( Inf,   Inf ) ) );
			DOCTEST_FAST_CHECK_UNARY( is_nan( abs_diff( 1.0f, -NaN ) ) );
			DOCTEST_FAST_CHECK_UNARY( is_nan( abs_diff( 1.0f,  NaN ) ) );
		}
	}

	DOCTEST_TEST_CASE( "comparison in_range" ) {
		static_assert( !in_range( 0, 0 ) );
		static_assert(  in_range( 0, 1 ) );
		static_assert( !in_range( 1, 0 ) );

		static_assert( !in_range( 0, 0, 0 ) );
		static_assert(  in_range( 0, 0, 1 ) );
		static_assert( !in_range( 0, 1, 0 ) );
		static_assert( !in_range( 0, 1, 1 ) );
		static_assert( !in_range( 1, 0, 0 ) );
		static_assert( !in_range( 1, 0, 1 ) );
		static_assert( !in_range( 1, 1, 0 ) );
		static_assert( !in_range( 1, 1, 1 ) );

		static_assert(  in_range< unsigned >(  1  ) );
		static_assert( !in_range< unsigned >( -1  ) );
		static_assert(  in_range<   signed >(  1u ) );
		static_assert( !in_range<   signed >( -1u ) );

		static_assert( !in_range< signed short,   unsigned short >() );
		static_assert( !in_range< signed short,   unsigned  long >() );
		static_assert(  in_range< unsigned short,   signed  long >() );
	}

	DOCTEST_TEST_CASE( "comparison min, max, mid" ) {
		{	// min
			static_assert( min( 7.5,       1, 2, 5 ) ==  1 );
			static_assert( min( 7.5, -Inf, 1, 2, 5 ) == -Inf );
			static_assert( min( 7.5, +Inf, 1, 2, 5 ) ==  1 );
			static_assert( min( 7.5, 1, 2, 5, -Inf ) == -Inf );
			static_assert( min( 7.5, 1, 2, 5, +Inf ) ==  1 );
			static_assert( is_nan( min( NaN, 2.0 ) ) );
			static_assert( is_nan( min( 2.0, NaN ) ) );
			static_assert( is_nan( min( NaN, 2,   5 ) ) );
			static_assert( is_nan( min( 7.5, NaN, 5 ) ) );
			static_assert( is_nan( min( 7.5, 5, NaN ) ) );
			static_assert( is_nan( min( 5, NaN ) ) );
		}
		{	// max
			static_assert( max( 7.5,       1, 2, 5 ) ==  7.5 );
			static_assert( max( 7.5, -Inf, 1, 2, 5 ) ==  7.5 );
			static_assert( max( 7.5, +Inf, 1, 2, 5 ) == +Inf );
			static_assert( max( 7.5, 1, 2, 5, -Inf ) ==  7.5 );
			static_assert( max( 7.5, 1, 2, 5, +Inf ) == +Inf );
			static_assert( is_nan( max( NaN, 2.0 ) ) );
			static_assert( is_nan( max( 2.0, NaN ) ) );
			static_assert( is_nan( max( NaN, 2,   5 ) ) );
			static_assert( is_nan( max( 7.5, NaN, 5 ) ) );
			static_assert( is_nan( max( 7.5, 5, NaN ) ) );
		}
		{	// mid
			static_assert( mid( 0, 0, 0 ) == 0 );
			static_assert( mid( 0, 0, 1 ) == 0 );
			static_assert( mid( 0, 1, 0 ) == 0 );
			static_assert( mid( 1, 0, 0 ) == 0 );
			static_assert( mid( 1, 1, 0 ) == 1 );
			static_assert( mid( 1, 0, 1 ) == 1 );
			static_assert( mid( 0, 1, 1 ) == 1 );
			static_assert( mid( 0, 1, 2 ) == 1 );
			static_assert( mid( 2, 0, 1 ) == 1 );
			static_assert( mid( 1, 2, 0 ) == 1 );

			static_assert( is_nan( mid( NaN, 1.0, 2.0 ) ) );
			static_assert( is_nan( mid( 2.0, NaN, 1.0 ) ) );
			static_assert( is_nan( mid( 1.0, 2.0, NaN ) ) );
		}
		{	// minmax
			{
				constexpr auto mm{ Minmax< int >{} };
				static_assert(  mm.empty() );
			} {
				constexpr auto mm{ Minmax( -1.0, -5.0 ) };
				static_assert( !mm.empty() );
			} {
				constexpr auto mm{ Minmax< double >( 4, 0.5, 2, 1 ) };
				static_assert( !mm.empty() );
			}

			static_assert( Minmax< double >( 0.5 ).min() == 0.5 );
			static_assert( Minmax< double >( 0.5 ).max() == 0.5 );

			static_assert( Minmax< double >( 0.5, 4 ).min() == 0.5 );
			static_assert( Minmax< double >( 4, 0.5 ).max() == 4.0 );

			static_assert( Minmax< double >( 0.5, 4, 2, 1 ).min() == 0.5 );
			static_assert( Minmax< double >( 4, 0.5, 2, 1 ).min() == 0.5 );
			static_assert( Minmax< double >( 4, 2, 0.5, 1 ).min() == 0.5 );
			static_assert( Minmax< double >( 4, 2, 1, 0.5 ).min() == 0.5 );
			static_assert( Minmax< double >( 0.5, 4, 2, 1 ).max() == 4 );
			static_assert( Minmax< double >( 4, 0.5, 2, 1 ).max() == 4 );
			static_assert( Minmax< double >( 4, 2, 0.5, 1 ).max() == 4 );
			static_assert( Minmax< double >( 4, 2, 1, 0.5 ).max() == 4 );

			static_assert( is_nan( Minmax< double >( 0.5, NaN ).min() ) );
			static_assert( is_nan( Minmax< double >( NaN, 0.5 ).min() ) );
			static_assert( is_nan( Minmax< double >( 0.5, NaN ).max() ) );
			static_assert( is_nan( Minmax< double >( NaN, 0.5 ).max() ) );

			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).min() ) );
			static_assert( is_nan( Minmax< double >( 0.5, 4,   NaN, 2, 1   ).min() ) );
			static_assert( is_nan( Minmax< double >( 0.5, 4,   2,   1, NaN ).min() ) );
			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).max() ) );
			static_assert( is_nan( Minmax< double >( 0.5, 4,   NaN, 2, 1   ).max() ) );
			static_assert( is_nan( Minmax< double >( 0.5, 4,   2,   1, NaN ).max() ) );
		
			auto mm = Minmax< double >();
			mm << NaN;
			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).min() ) );
			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).max() ) );
			mm << 4.2;
			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).min() ) );
			static_assert( is_nan( Minmax< double >( NaN, 0.5, 4,   2, 1   ).max() ) );
		}
	}

	DOCTEST_TEST_CASE( "comparison about_zero" ) {
		{	// about_zero
			static_assert(  about_zero( 0 ) );
			static_assert( !about_zero( 1 ) );
			static_assert(  about_zero< 2 >( 0 ) );
			static_assert( !about_zero< 2 >( 1 ) );

			static_assert(  about_zero( 0.0   ) );
			static_assert(  about_zero( 1e-14 ) );
			static_assert( !about_zero( 1e-13 ) );
			static_assert( !about_zero( NaN   ) );
			static_assert(  about_zero< 2 >( 0.0  ) );
			static_assert(  about_zero< 2 >( 1e-3 ) );
			static_assert( !about_zero< 2 >( 1e-2 ) );
			static_assert( !about_zero< 2 >( NaN  ) );
		}
		{	// similar integers
			static_assert(  similar< 2 >(  32u,  32  ) );
			static_assert( !similar< 2 >( -32u, -32  ) );
			static_assert( !similar< 2 >( -32,  -32u ) );
		}
		{	// similar floating point
			enum{ pass = 5, nopass };
			constexpr double	small = 9.999E-6;
			constexpr double	base  = 1.0/small;
			constexpr auto		min	  = std::numeric_limits< double >::min();
			constexpr auto		max	  = std::numeric_limits< double >::max();

			// Ordinary case
			static_assert( !similar          (     -1.0,        1.0 ) );
			static_assert( !similar          (     -1.0,        1   ) );
			static_assert( !similar          (     -1,          1.0 ) );
			static_assert( !similar          (     -1,          1   ) );
			static_assert( !similar          (     -1,         -1u  ) );
			static_assert(  similar          ( 123456.0f,  123450.0 ) );
			static_assert( !similar          (  12345.0f,   12340.0 ) );
	
			static_assert(  similar<   pass >(  123456.0,  123457.0 ) );
			static_assert(  similar<   pass >(  123456  ,  123457.0 ) );
			static_assert( !similar< nopass >(  123456.0,  123457.0 ) );
			static_assert( !similar< nopass >(  123456  ,  123457.0 ) );
			static_assert(  similar<   pass >( -123456.0, -123457.0 ) );
			static_assert(  similar<   pass >( -123456  , -123457.0 ) );
			static_assert( !similar< nopass >( -123456.0, -123457.0 ) );
			static_assert( !similar< nopass >( -123456  , -123457.0 ) );

			// Opposit signs
			static_assert( !similar<   pass >(  base+1, -base-1 ) );
			static_assert( !similar<   pass >( -base-1,  base+1 ) );
	
			// Large numbers
			static_assert(  similar<   pass >(  base+2,  base+1 ) );
			static_assert(  similar<   pass >(  base+1,  base+2 ) );
			static_assert( !similar< nopass >(  base+2,  base+1 ) );
			static_assert( !similar< nopass >(  base+1,  base+2 ) );

			// Negative large numbers
			static_assert(  similar<   pass >( -base-2, -base-1 ) );
			static_assert(  similar<   pass >( -base-1, -base-2 ) );
			static_assert( !similar< nopass >( -base-2, -base-1 ) );
			static_assert( !similar< nopass >( -base-1, -base-2 ) );

			// Numbers between +-1 and 0
			constexpr double	small1  = ( 1 +   small ) * 1e-20;
			constexpr double	small2  = ( 1 + 2*small ) * 1e-20;
			static_assert(  similar<   pass >(  small1,  small2 ) );
			static_assert(  similar<   pass >(  small2,  small1 ) );
			static_assert( !similar< nopass >(  small1,  small2 ) );
			static_assert( !similar< nopass >(  small2,  small1 ) );
			static_assert(  similar<   pass >( -small1, -small2 ) );
			static_assert(  similar<   pass >( -small2, -small1 ) );
			static_assert( !similar< nopass >( -small1, -small2 ) );
			static_assert( !similar< nopass >( -small2, -small1 ) );

			// Comparisons involving zero
			static_assert(  similar          (    +0.0,  +0.0   ) );
			static_assert(  similar          (    +0.0,  -0.0   ) );
			static_assert(  similar          (    -0.0,  -0.0   ) );
			static_assert(  similar          (     0.0,   min   ) );
			static_assert(  similar          (     min,   0.0   ) );
			static_assert(  similar          (     0.0,  -min   ) );
			static_assert(  similar          (    -min,   0.0   ) );
			static_assert( !similar          (    min,   -min   ) );
			static_assert( !similar          (   -min,    min   ) );

			static_assert(  similar<   pass >(  small,   +0.0   ) );
			static_assert(  similar<   pass >(   +0.0,  small   ) );
			static_assert(  similar<   pass >( -small,   +0.0   ) );
			static_assert(  similar<   pass >(   +0.0, -small   ) );
			static_assert(  similar<   pass >(  small,   -0.0   ) );
			static_assert(  similar<   pass >(   -0.0,  small   ) );
			static_assert(  similar<   pass >( -small,   -0.0   ) );
			static_assert(  similar<   pass >(   -0.0, -small   ) );

			static_assert( !similar< nopass >(  small,   +0.0   ) );
			static_assert( !similar< nopass >(   +0.0,  small   ) );
			static_assert( !similar< nopass >( -small,   +0.0   ) );
			static_assert( !similar< nopass >(   +0.0, -small   ) );
			static_assert( !similar< nopass >(  small,   -0.0   ) );
			static_assert( !similar< nopass >(   -0.0,  small   ) );
			static_assert( !similar< nopass >( -small,   -0.0   ) );
			static_assert( !similar< nopass >(   -0.0, -small   ) );

			// Comparisons involving extremes
			static_assert(  similar          (     Inf,     Inf ) );
			static_assert(  similar          (    -Inf,    -Inf ) );
			static_assert( !similar          (    -Inf,     Inf ) );
			static_assert( !similar          (     Inf,     max ) );
			static_assert( !similar          (    -Inf,    -max ) );
			static_assert( !similar          (     max,     Inf ) );
			static_assert( !similar          (    -max,    -Inf ) );
			static_assert(  similar< 2 >     (    -Inf,    -Inf ) );
			static_assert( !similar< 2 >     (    -Inf,     Inf ) );

			// Comparisons involving NaN values
			static_assert           ( !similar< 2 >(  NaN,  0.0 ) );
			static_assert           ( !similar< 2 >(  0.0,  NaN ) );
			DOCTEST_FAST_CHECK_UNARY( !similar< 2 >(  NaN,  NaN ) );
		}
	}
	
}	// namespace pax::doctester
