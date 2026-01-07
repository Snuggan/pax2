//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/shadow.hpp>
#include <pax/doctest.hpp>
#include <print>


namespace pax {

	template< auto V >	struct litteral_test	{	static constexpr decltype( V ) v = V;	};

	DOCTEST_TEST_CASE( "shadow text" ) {
		DOCTEST_FAST_CHECK_EQ ( std::format( "{}", shadow( "text" ) ), "text" );

		static_assert( shadow( "text" )						== "text" );
		static_assert( shadow( "text" ).size()				== 4u );
		// static_assert( shadow( "text" )						>  "test" );
		static_assert( shadow( "text" )[ 3 ]				== 't' );
		static_assert( *( shadow( "text" ).begin() + 1 ) 	== 'e' );
		static_assert( *( shadow( "text" ).end()   - 2 ) 	== 'x' );
		static_assert( shadow( "text" ).front() 			== 't' );
		static_assert( shadow( "text" ).back() 				== 't' );
		static_assert( shadow( "text" ).first( 2 )			== "te" );
		static_assert( shadow( "text" ).first( 9 )			== "text" );
		static_assert( shadow( "text" ).not_first( 2 )		== "xt" );
		static_assert( shadow( "text" ).not_first( 9 )		== "" );
		static_assert( shadow( "text" ).last( 2 )			== "xt" );
		static_assert( shadow( "text" ).last( 9 )			== "text" );
		static_assert( shadow( "text" ).not_last( 2 )		== "te" );
		static_assert( shadow( "text" ).not_last( 9 )		== "" );
		static_assert( shadow( "text" ).part( 1, 2 )		== "ex" );
		static_assert( shadow( "text" ).part( 9, 2 )		== "" );
		static_assert( shadow( "text" ).part( 1, 9 )		== "ext" );
		static_assert( shadow( "text" ).starts_with( "tex" ) );
		static_assert( shadow( "text" ).ends_with( "ext" ) );
#if defined( __clang__ )	// Not constexpr in gcc:
		static_assert( shadow( "text" ).contains( 'x' ) );
		static_assert( shadow( "text" ).contains( "xt" ) );
#endif
	}
	DOCTEST_TEST_CASE( "shadow numbers" ) {
		// static constexpr std::array		nums{ 0, 1, 2, 3, 4, 5, 0 };
		// DOCTEST_FAST_CHECK_EQ ( std::format( "{}", shadow( nums ) ), "text" );

// 		static_assert( shadow( nums )					== "text" );
// 		static_assert( shadow( nums ).size()			== 4u );
// 		// static_assert( shadow( nums )					>  "test" );
// 		static_assert( shadow( nums )[ 3 ]				== 't' );
// 		static_assert( *( shadow( nums ).begin() + 1 ) 	== 'e' );
// 		static_assert( *( shadow( nums ).end()   - 2 ) 	== 'x' );
// 		static_assert( shadow( nums ).front() 			== 't' );
// 		static_assert( shadow( nums ).back() 			== 't' );
// 		static_assert( shadow( nums ).first( 2 )		== "te" );
// 		static_assert( shadow( nums ).first( 9 )		== "text" );
// 		static_assert( shadow( nums ).not_first( 2 )	== "xt" );
// 		static_assert( shadow( nums ).not_first( 9 )	== "" );
// 		static_assert( shadow( nums ).last( 2 )			== "xt" );
// 		static_assert( shadow( nums ).last( 9 )			== "text" );
// 		static_assert( shadow( nums ).not_last( 2 )		== "te" );
// 		static_assert( shadow( nums ).not_last( 9 )		== "" );
// 		static_assert( shadow( nums ).part( 1, 2 )		== "ex" );
// 		static_assert( shadow( nums ).part( 9, 2 )		== "" );
// 		static_assert( shadow( nums ).part( 1, 9 )		== "ext" );
// 		static_assert( shadow( nums ).starts_with( "tex" ) );
// 		static_assert( shadow( nums ).ends_with( "ext" ) );
// #if defined( __clang__ )	// Not constexpr in gcc:
// 		static_assert( shadow( nums ).contains( 5 ) );
// 		static_assert( shadow( nums ).contains( {{ 3, 4 }} ) );
// #endif
	}
	DOCTEST_TEST_CASE( "litteral text" ) {
		DOCTEST_FAST_CHECK_EQ ( std::format( "{}", litt( "text" ) ), "text" );

		static_assert( litt( "text" )					== "text" );
		// static_assert( litt( "text" )					>  "test" );
		static_assert( litt( "text" )[ 4 ]				== 0 );
		static_assert( *( litt( "text" ).begin() + 1 ) 	== 'e' );
		static_assert( *( litt( "text" ).end()   - 2 ) 	== 'x' );
		static_assert( litt( "text" ).front() 			== 't' );
		static_assert( litt( "text" ).back() 			== 't' );
		static_assert( litt( "text" ).size()			== 4u );
		static_assert( litt( "text" ).first( 2 )		== "te" );
		static_assert( litt( "text" ).first( 9 )		== "text" );
		static_assert( litt( "text" ).last( 2 )			== "xt" );
		static_assert( litt( "text" ).last( 9 )			== "text" );
		static_assert( litt( "text" ).part( 1, 2 )		== "ex" );
		static_assert( litt( "text" ).part( 9, 2 )		== "" );
		static_assert( litt( "text" ).part( 1, 9 )		== "ext" );
		static_assert( litt( "text" ).starts_with( "tex" ) );
		static_assert( litt( "text" ).ends_with( "ext" ) );
#if defined( __clang__ )	// These are not constexpr in gcc...
		static_assert( litt( "text" ).contains( 'x' ) );
		static_assert( litt( "text" ).contains( "xt" ) );
#endif
		// Test template usage.
		static_assert( litteral_test< litt( "text" ) >::v	== "text" );
	}

}
