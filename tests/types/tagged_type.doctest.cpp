//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/shadow.hpp>
#include <pax/types/tagged_type.hpp>
#include <pax/doctest.hpp>
#include <print>


namespace pax {

	template< typename Tag >
	constexpr int test( Tagged< Tag, bool > )				{	return 1;	}

	constexpr int test( Tagged< struct specific, bool > )	{	return 3;	}
	
	DOCTEST_TEST_CASE( "Tagged" ) {
		// static_assert( Tagged< struct general >( true ) );
		// static_assert( tagged( true ) );
		// static_assert( tagged( 42ul ) == tagged( 42u ) );
		// static_assert( 42ul == tagged( 42u )() );
		//
		// DOCTEST_FAST_CHECK_EQ( tagged( 5.0 ), tagged( 5 ) );
		// DOCTEST_FAST_CHECK_EQ( tagged< struct hej >( 5.0 ), tagged< struct hej >( 5 ) );
		//
		// DOCTEST_FAST_CHECK_LT( tagged( 4.0 ), tagged( 5 ) );
		// DOCTEST_FAST_CHECK_LT( tagged< struct hej >( 4.0 ), tagged< struct hej >( 5 ) );
		//
		// DOCTEST_FAST_CHECK_EQ( test( tagged< struct general  >( false ) ),	1 );
		// DOCTEST_FAST_CHECK_EQ( test( tagged< struct specific >( false ) ),	3 );
	}
}
