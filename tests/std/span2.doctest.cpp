//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/span2.hpp>
#include <pax/reporting/as_ascii.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {
	
	DOCTEST_TEST_CASE( "parts" ) {
		constexpr const span2			first_( "abcde" );
		constexpr const span2			last_( "jkl" );
		constexpr const span2< const char >	null_;
		constexpr const span2< const char, dynamic_extent >			str( "abcdefghijkl" );
		constexpr const span2 			strN( "abcdefghijkl" );

		{	// front, back
			char			str[ 10 ] = { '1','2','3','4','5','6','7','8','9','0' };
			const span2		v( str );
			
			v[ 0 ]			= 'a';		DOCTEST_FAST_CHECK_EQ( v.front(), 'a' );
			v.front()		= 'c';		DOCTEST_FAST_CHECK_EQ( v.front(), 'c' );
			v.back ()		= 'd';		DOCTEST_FAST_CHECK_EQ( v.back (), 'd' );
			*v.begin()		= 'e';		DOCTEST_FAST_CHECK_EQ( v.front(), 'e' );
			*v.rbegin()		= 'f';		DOCTEST_FAST_CHECK_EQ( v.back (), 'f' );
		}
		{	// first
			DOCTEST_ASCII_CHECK_EQ( str.first(  0 ), 				null_ );
			DOCTEST_ASCII_CHECK_EQ( str.first(  5 ), 				first_ );
			DOCTEST_ASCII_CHECK_EQ( str.first( 22 ),  				str );
			DOCTEST_FAST_CHECK_EQ ( str.first( 22 ).data(),  		str.data() );
			DOCTEST_ASCII_CHECK_EQ( null_.first( 22 ),  			null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.first( 22 ).data(),  		nullptr );
			DOCTEST_ASCII_CHECK_EQ( str .first<  5 >(), 			first_ );
			DOCTEST_ASCII_CHECK_EQ( strN.first<  5 >(), 			first_ );
			DOCTEST_ASCII_CHECK_EQ( strN.first< 22 >(),				str );
			DOCTEST_FAST_CHECK_EQ ( strN.first< 22 >().data(),		strN.data() );
			DOCTEST_ASCII_CHECK_EQ( null_.first<  0 >(), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.first<  0 >().data(), 	nullptr );
			// Should assert:	    str.first< 13 >()
		}
		{	// last
			DOCTEST_ASCII_CHECK_EQ( str.last(  0 ), 				null_ );
			DOCTEST_ASCII_CHECK_EQ( str.last(  3 ), 				last_ );
			DOCTEST_ASCII_CHECK_EQ( str.last( 22 ), 				str );
			DOCTEST_FAST_CHECK_EQ ( str.last( 22 ).data(), 			str.data() );
			DOCTEST_ASCII_CHECK_EQ( null_.last( 22 ),  				null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.last( 22 ).data(),  		nullptr );
			DOCTEST_ASCII_CHECK_EQ( str.last < 3 >(), 				last_ );
			DOCTEST_ASCII_CHECK_EQ( strN.last < 3 >(), 				last_ );
			DOCTEST_ASCII_CHECK_EQ( strN.last< 22 >(), 				str );
			DOCTEST_FAST_CHECK_EQ ( strN.last< 22 >().data(), 		strN.data() );
			DOCTEST_ASCII_CHECK_EQ( null_.last<  0 >(), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.last<  0 >().data(), 		nullptr );
			// Should assert:	    null_.last< 22 >()
		}
		{	// not_first
			DOCTEST_ASCII_CHECK_EQ( str.not_first(  0 ), 			str );
			DOCTEST_ASCII_CHECK_EQ( str.not_first(  9 ), 			last_ );
			DOCTEST_ASCII_CHECK_EQ( str.not_first( 22 ), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( str.not_first( 22 ).data(), 	str.data() + str.size() );
			DOCTEST_ASCII_CHECK_EQ( null_.not_first( 22 ),  		null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.not_first( 22 ).data(),  	nullptr );
			DOCTEST_ASCII_CHECK_EQ( strN.not_first<  0 >(),			str );
			DOCTEST_ASCII_CHECK_EQ( strN.not_first<  9 >(), 		last_ );
			DOCTEST_ASCII_CHECK_EQ( strN.not_first< 22 >(), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( strN.not_first< 22 >().data(), 	strN.data() + strN.size() );
		}
		{	// not_last
			DOCTEST_ASCII_CHECK_EQ( str.not_last(  0 ), 			str );
			DOCTEST_ASCII_CHECK_EQ( str.not_last(  7 ), 			first_ );
			DOCTEST_ASCII_CHECK_EQ( str.not_last( 22 ), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( str.not_last( 22 ).data(), 		str.data() );
			DOCTEST_ASCII_CHECK_EQ( null_.not_last( 22 ),  			null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.not_last( 22 ).data(),  	nullptr );
			DOCTEST_ASCII_CHECK_EQ( strN.not_last <  0 >(),			str );
			DOCTEST_ASCII_CHECK_EQ( strN.not_last <  7 >(), 		first_ );
			DOCTEST_ASCII_CHECK_EQ( strN.not_last < 22 >(), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( strN.not_last < 22 >().data(), 	strN.data() );
		}
		{	// subspan
			DOCTEST_ASCII_CHECK_EQ( null_.subspan( 0, 0 ),				null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.subspan( 0, 0 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ( null_.subspan( 0, 2 ),				null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.subspan( 0, 2 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ( null_.subspan( 2, 2 ),				null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.subspan( 2, 2 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ( null_.subspan( 2, 0 ),				null_ );
			DOCTEST_FAST_CHECK_EQ ( null_.subspan( 2, 0 ).data(),		nullptr );

			DOCTEST_ASCII_CHECK_EQ( str.subspan(   0, 22 ),				str );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(   0,  5 ), 			str.first( 5 ) );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(   3,  2 ), 			"de" );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(   7, 22 ), 			str.not_first( 7 ) );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(  22,  0 ), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( str.subspan(  22,  0 ).data(), 		str.data() + str.size() );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(  -7, 22 ), 			str.last( 7 ) );
			DOCTEST_ASCII_CHECK_EQ( str.subspan(  -9,  2 ), 			"de" );
			DOCTEST_ASCII_CHECK_EQ( str.subspan( -22,  0 ), 			null_ );
			DOCTEST_FAST_CHECK_EQ ( str.subspan( -22,  0 ).data(), 		str.data() );
			DOCTEST_ASCII_CHECK_EQ( str.subspan( -22, 22 ), 			str );

			DOCTEST_ASCII_CHECK_EQ( str .subspan<   2 >(   3 ), 		"de" );
			DOCTEST_ASCII_CHECK_EQ( strN.subspan<   2 >(  3 ), 			"de" );
			DOCTEST_ASCII_CHECK_EQ( str .subspan<   2 >(  -9 ), 		"de" );
			DOCTEST_ASCII_CHECK_EQ( strN.subspan<   2 >( -9 ), 			"de" );
			DOCTEST_ASCII_CHECK_EQ( strN.subspan<  12 >(  0 ),			str );

			DOCTEST_ASCII_CHECK_EQ((str .subspan<   0,  5 >( )), 		str.first( 5 ) );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<   0,  5 >()), 		str.first( 5 ) );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<   0, 12 >()),			str );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<   0, 22 >()),			str );
			DOCTEST_ASCII_CHECK_EQ((str .subspan<   3,  2 >( )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<   3,  2 >()), 		"de" );
			// Should assert:	    str.subspan<   7, 22 >( )
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<   7, 22 >()), 		str.not_first( 7 ) );
			DOCTEST_ASCII_CHECK_EQ((str .subspan<  22,  0 >( )), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( str .subspan<  22,  0 >( ).data(), 	str.data() + strN.size() );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<  22,  0 >()), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( strN.subspan<  22,  0 >().data(), 	strN.data() + strN.size() );
			// Should assert:	    str .subspan<  22, 22 >( )
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<  22, 22 >()), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( strN.subspan<  22, 22 >().data(), 	strN.data() + strN.size() );
			// Should assert:	    str .subspan<  -7, 22 >( )
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<  -7, 22 >()), 		str.last( 7 ) );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan<  -9,  2 >()), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((str .subspan<  -9,  2 >( )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((str .subspan< -22,  0 >( )),		null_ );
			DOCTEST_FAST_CHECK_EQ ( str .subspan< -22,  0 >( ).data(), 	str.data() );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan< -22,  0 >()), 		null_ );
			DOCTEST_FAST_CHECK_EQ ( strN.subspan< -22,  0 >().data(), 	strN.data() );
			DOCTEST_ASCII_CHECK_EQ((strN.subspan< -22, 22 >()), 		str );
		}
	}

}	// namespace pax
