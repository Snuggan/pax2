//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/std/string_view2.hpp>
#include <pax/textual/as_ascii.hpp>
#include <pax/doctest.hpp>

#include <sstream>

	
namespace pax {

	constexpr string_view2		abc( "abcdefghijkl" );

	DOCTEST_TEST_CASE( "string_view2" ) {
		{
			const char						  * arr = "abc";
			const string_view2					sp = arr;
			DOCTEST_FAST_CHECK_EQ( size( sp ),	3 );
		} 
		{
			const char						  * arr = " abc";
			const string_view2					sp = arr+1;
			DOCTEST_FAST_CHECK_EQ( size( sp ),	3 );
		} 
		{
			const char							arr[ 4 ] = "abc";
			const string_view2					sp = arr;
			DOCTEST_FAST_CHECK_EQ( sp.size(),	3 );
		} 
		{
			const string_view2					sp = "abc";
			DOCTEST_FAST_CHECK_EQ( size( sp ),	3 );
		}
		{
			const std::string					sp = "abc";
			DOCTEST_FAST_CHECK_EQ( size( sp ),	3 );
		}
	}
	DOCTEST_TEST_CASE( "nullptr handling" ) {
		DOCTEST_FAST_CHECK_UNARY( !string_view2{}.valid() );
		DOCTEST_FAST_CHECK_UNARY( !string_view2( nullptr, 4 ).valid() );
		DOCTEST_FAST_CHECK_EQ(  string_view2( nullptr, 4 ).size(),			0 );
	}
	DOCTEST_TEST_CASE( "string_view2 in general" ) {
		{	// copy
			std::string				str( abc );
			DOCTEST_FAST_CHECK_EQ( abc.last( 3 ).copy( str.data() ), 	3 );
			DOCTEST_FAST_CHECK_EQ( str,							"jkldefghijkl" );
		}
		{	// partition
			DOCTEST_FAST_CHECK_EQ( abc.first(), 				"a" );
			DOCTEST_FAST_CHECK_EQ( abc.first( 5 ), 				"abcde" );
			DOCTEST_FAST_CHECK_EQ( abc.first( 0 ), 				string_view2{} );
			DOCTEST_FAST_CHECK_EQ( abc,  						abc.first( 22 ) );
			DOCTEST_FAST_CHECK_EQ( abc.not_first(), 			"bcdefghijkl" );
			DOCTEST_FAST_CHECK_EQ( abc.not_first( 9 ), 			"jkl" );
			DOCTEST_FAST_CHECK_EQ( abc, 						abc.not_first( 0 ) );
			DOCTEST_FAST_CHECK_EQ( abc.not_first( 22 ), 		string_view2{} );
			DOCTEST_FAST_CHECK_EQ( abc.last(), 					"l" );
			DOCTEST_FAST_CHECK_EQ( abc.last( 0 ), 				string_view2{} );
			DOCTEST_FAST_CHECK_EQ( abc, 						abc.last( 22 ) );
			DOCTEST_FAST_CHECK_EQ( abc.not_last(), 				"abcdefghijk" );
			DOCTEST_FAST_CHECK_EQ( abc.not_last( 7 ), 			"abcde" );
			DOCTEST_FAST_CHECK_EQ( abc, 						abc.not_last( 0 ) );
			DOCTEST_FAST_CHECK_EQ( abc.not_last( 22 ), 			string_view2{} );
			DOCTEST_CHECK_EQ( abc.substr(  3,  2 ), 			"de" );
			DOCTEST_CHECK_EQ( abc.substr( -9,  2 ), 			"de" );
			DOCTEST_CHECK_EQ( abc, 								abc.substr( 0, 22 ) );
			DOCTEST_CHECK_EQ( abc.substr(  22,  0 ).size(), 	0 );
			DOCTEST_CHECK_EQ( abc.substr(  22,  0 ), 			string_view2{} );
			DOCTEST_CHECK_EQ( abc.substr( -22,  0 ).data(), 	abc.data() );
			DOCTEST_CHECK_EQ( abc.substr( -22,  0 ), 			string_view2{} );
			DOCTEST_CHECK_EQ( abc, 								abc.substr( -22, 22 ) );
			DOCTEST_CHECK_EQ( abc.substr(  0,  5 ), 			abc.first( 5 ) );
			DOCTEST_CHECK_EQ( abc.substr(  7, 22 ), 			abc.not_first( 7 ) );
			DOCTEST_CHECK_EQ( abc.substr(  9, 18446744073709551615u ).size(), 3 );
			DOCTEST_CHECK_EQ( abc.substr( -7, 22 ),				abc.last( 7 ) );
		}
		{	// comparisons
			const auto					abc2{ "abcdffghijkl" };

			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::less,    abc.not_last() <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::less,    abc  <=> abc2 );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::equal,   abc  <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::greater, abc2 <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::greater, abc  <=> abc.not_last() );

			DOCTEST_FAST_CHECK_LT( abc, abc2 );
			DOCTEST_FAST_CHECK_LE( abc, abc2 );
			DOCTEST_FAST_CHECK_GT( abc2, abc );
			DOCTEST_FAST_CHECK_GE( abc2, abc );
			DOCTEST_FAST_CHECK_NE( abc2, abc );

            DOCTEST_FAST_CHECK_LT( abc.first(  9 ), abc );
            DOCTEST_FAST_CHECK_LE( abc.first( 10 ), abc );
            DOCTEST_FAST_CHECK_LE( abc.first(  9 ), abc );
            DOCTEST_FAST_CHECK_EQ( abc.first( 12 ), abc );

			DOCTEST_FAST_CHECK_NE( abc, abc.first(  9 ) );
			DOCTEST_FAST_CHECK_GT( abc, abc.first(  9 ) );
			DOCTEST_FAST_CHECK_GE( abc, abc.first( 10 ) );
			DOCTEST_FAST_CHECK_GE( abc, abc.first(  8 ) );
		}
		{	// operator<<
			{	// with text
				std::ostringstream		os;
				os << string_view2( "abcd" ).first( 3 );
				DOCTEST_FAST_CHECK_EQ( os.str().size(), 		3 );
				DOCTEST_FAST_CHECK_EQ( os.str(), 				"abc" );
			} 
			{	// with text technical
#if !defined( PAX_ASCII_TEST_UNUSABLE )
				std::ostringstream		os;
				constexpr auto			v0 = string_view2( ">\a\b\t\n\v\f\r\"'\x18\x7f <" );
				constexpr auto			vr = string_view2( "\">\\a\\b\\t\\n\\v\\f\\r\\\"'<CAN><DEL> <\"" );

				os << as_ascii( v0 );
				const auto 				res0{ os.str() };
				const string_view2	 	res{ res0.data(), res0.size() };
		
				DOCTEST_FAST_CHECK_EQ( v0.size(),   14 );
				DOCTEST_FAST_CHECK_EQ( res0.size(), vr.size() );
				DOCTEST_FAST_CHECK_EQ( res.size(),  vr.size() );
				DOCTEST_FAST_CHECK_EQ( res, vr );
#endif
			}
		}
		{	// operator+
			std::string					s ( "string" );
			std::string					vs( "span" );
			constexpr string_view2		v ( "span" );

			DOCTEST_ASCII_CHECK_EQ( s +  s,										"stringstring" );
			DOCTEST_ASCII_CHECK_EQ( s +  v,										"stringspan" );
			DOCTEST_ASCII_CHECK_EQ( v +  s,										"spanstring" );
			DOCTEST_ASCII_CHECK_EQ( s += v,										"stringspan" );

			DOCTEST_ASCII_CHECK_EQ( std::string( "string" ) + string_view2( "span" ),	"stringspan" );
			DOCTEST_ASCII_CHECK_EQ( string_view2( "span" ) + std::string( "string" ),	"spanstring" );
			DOCTEST_ASCII_CHECK_EQ( std::string( "string" )	+= string_view2( "span" ),	"stringspan" );

			DOCTEST_ASCII_CHECK_EQ( std::string( "string" ) +  string_view2( vs ),		"stringspan" );
			DOCTEST_ASCII_CHECK_EQ( string_view2( vs ) +  std::string( "string" ),		"spanstring" );
			DOCTEST_ASCII_CHECK_EQ( std::string( "string" )	+= string_view2( vs ),		"stringspan" );
		}
	}

}	// namespace pax
