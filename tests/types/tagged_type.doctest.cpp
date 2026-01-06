//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/litteral.hpp>
#include <pax/types/tagged_type.hpp>
#include <pax/doctest.hpp>


namespace pax {

	template< auto N >
	using Statique2				  = Statique< N, struct specific >;
	
	template< auto N, typename Tag >
	constexpr int test( Statique< N, Tag > )	{	return 1;	}

	template< auto N >					requires( std::is_unsigned_v< decltype( N ) > )
	constexpr int test( Statique2< N > )		{	return 3;	}

	template< auto N, typename Tag >	requires( std::is_unsigned_v< decltype( N ) > )
	constexpr int test( Statique< N, Tag > )	{	return 5;	}
	
	DOCTEST_TEST_CASE( "Statique" ) {
		static_assert( Stat< true > );
		static_assert( Stat< 3 >		== 3 );
		static_assert( Stat< 3 >()		== 3 );
		static_assert( Stat< 3 >.value	== 3 );
		static_assert( Stat< 2 >   < Stat< 3 > );
		static_assert( Stat< 2.5 > + Stat< 4 > == 6.5 );

		static_assert( "Hej" == Stat< Litt{ "Hej" } > );
		static_assert( "Hej" == Stat< Litt{ "Hej" }, struct litt > );
	
		DOCTEST_FAST_CHECK_EQ( test( Stat< 1 >			),	1 );	// Not an unsigned value.
		DOCTEST_FAST_CHECK_EQ( test( Statique2< 1u >{}	),	3 );	// Not the general tag.
		DOCTEST_FAST_CHECK_EQ( test( Stat< 1u >			),	5 );
	}
	
	

	template< typename Tag >
	constexpr int test( Tagged< Tag, bool > )				{	return 1;	}

	constexpr int test( Tagged< struct specific, bool > )	{	return 3;	}
	
	DOCTEST_TEST_CASE( "Tagged" ) {
		// static_assert( Tagged< struct general >( true ) );
		static_assert( tagged( true ) );
		static_assert( tagged( 42ul ) == tagged( 42u ) );
		static_assert( tagged< struct specific >( 42ul ) == tagged( 42u ) );

		static_assert( "Hej" == tagged( "Hej" ) );
		static_assert( "Hej" == tagged< struct hej >( "Hej" ) );

		DOCTEST_FAST_CHECK_EQ( test( tagged< struct general  >( false ) ),	1 );
		DOCTEST_FAST_CHECK_EQ( test( tagged< struct specific >( false ) ),	3 );
	}
	
}
