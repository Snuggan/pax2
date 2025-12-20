//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/constant_type.hpp>
#include <pax/doctest.hpp>


namespace pax {

	template< auto N >
	using Const_type2				  = Const_type_base< N, struct specific >;
	
	template< auto N, typename Tag >
	constexpr int test( Const_type_base< N, Tag > )	{	return 1;	}

	template< auto N >					requires( std::is_unsigned_v< decltype( N ) > )
	constexpr int test( Const_type2< N > )			{	return 3;	}

	template< auto N, typename Tag >	requires( std::is_unsigned_v< decltype( N ) > )
	constexpr int test( Const_type_base< N, Tag > )	{	return 5;	}
	
	DOCTEST_TEST_CASE( "Constant_base" ) {
		static_assert( True );
		static_assert( Const< 3 >		== 3 );
		static_assert( Const< 3 >()		== 3 );
		static_assert( Const< 3 >.value	== 3 );
		static_assert( Const< 2 >   < Const< 3 > );
		static_assert( Const< 2.5 > + Const< 4 > == 6.5 );
	
		DOCTEST_FAST_CHECK_EQ( test( Const< 1 >          ),	1 );	// Not an unsigned value.
		DOCTEST_FAST_CHECK_EQ( test( Const_type2< 1u >{} ),	3 );	// Not the general tag.
		DOCTEST_FAST_CHECK_EQ( test( Const< 1u >         ),	5 );
	}
	
	

	template< typename Tag >
	constexpr int test( Bool< Tag > )				{	return 1;	}

	constexpr int test( Bool< struct specific > )	{	return 3;	}
	
	DOCTEST_TEST_CASE( "Constant_base" ) {
		static_assert( Bool  < struct general >( true ) );
		static_assert( Signed< struct general >( 42 ) == Unsigned< struct specific >( 42u ) );

		DOCTEST_FAST_CHECK_EQ( test( Bool< struct general  >( false ) ),	1 );
		DOCTEST_FAST_CHECK_EQ( test( Bool< struct specific >( false ) ),	3 );
	}
	
}
