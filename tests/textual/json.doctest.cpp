//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include "json.doctest.hpp"

#include <pax/textual/json.hpp>
#include <pax/external/doctest/doctest-pax.hpp>

#include <tuple>


namespace pax::json_test { 
	
	DOCTEST_TEST_CASE( "json special values" ) {
		DOCTEST_SUBCASE( "in constructor" ) {
			Json_value	json( {
				{ "-inf",		-1.0/0.0 },
				{ "nan",		 0.0/0.0 },
				{ "inf",		 1.0/0.0 }
			} );

			DOCTEST_FAST_CHECK_UNARY( std::isinf( json.at( "-inf" ).get< double >() ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( json.at(  "nan" ).get< double >() ) );
			DOCTEST_FAST_CHECK_UNARY( std::isinf( json.at(  "inf" ).get< double >() ) );
		}
		DOCTEST_SUBCASE( "in asignment" ) {
			Json_value	json{};
			json[ "-inf" ]	  = -1.0/0.0;
			json[  "nan" ]	  =  0.0/0.0;
			json[  "inf" ]	  =  1.0/0.0;

			DOCTEST_FAST_CHECK_UNARY( std::isinf( json.at( "-inf" ).get< double >() ) );
			DOCTEST_FAST_CHECK_UNARY( std::isnan( json.at(  "nan" ).get< double >() ) );
			DOCTEST_FAST_CHECK_UNARY( std::isinf( json.at(  "inf" ).get< double >() ) );
		}
	}


	DOCTEST_TEST_CASE( "various json" ) {
		
		Json_value	json1( {
			{ "bool",		false },
			{ "int",		-42 },
			{ "unsigned",	142 },
			{ "string",		"string-42" },
			{ "vector",		Vector{{ 1, 2, 3, 4 }} }
		} );
		json1[ "unsigned" ]	  = 42;
		json1[ "double" ]	  = 42.42;
		
		Test		test = from_json< Test >( json1 );

		DOCTEST_FAST_CHECK_EQ( test.m_bool,			false );
		DOCTEST_FAST_CHECK_EQ( test.m_int,			-42 );
		DOCTEST_FAST_CHECK_EQ( test.m_unsigned,		 42 );
		DOCTEST_FAST_CHECK_EQ( test.m_double,		 42.42 );
		DOCTEST_FAST_CHECK_EQ( test.m_string,		"string-42" );
		DOCTEST_FAST_CHECK_EQ( test.m_vector[ 1 ],	2 );

		test.m_bool		  = true;
		test.m_int		  = -2;
		test.m_unsigned	  =  2;
		test.m_double	  =  2.25;
		test.m_string	  = "new";
		test.m_vector	  = {{ 4, 3 }};
		Json_value json2  = to_json( test );

		DOCTEST_FAST_CHECK_EQ( from_json< bool        >( json2.at( "bool"    	) ),		true );
		DOCTEST_FAST_CHECK_EQ( from_json< int         >( json2.at( "int" 		) ),		-2 );
		DOCTEST_FAST_CHECK_EQ( from_json< unsigned    >( json2.at( "unsigned"	) ),		 2 );
		DOCTEST_FAST_CHECK_EQ( from_json< double      >( json2.at( "double" 	) ),		 2.25 );
		DOCTEST_FAST_CHECK_EQ( from_json< std::string >( json2.at( "string" 	) ),		"new" );
		DOCTEST_FAST_CHECK_EQ( from_json< Vector      >( json2.at( "vector" 	) )[ 1 ],	3 );
	}



	using Tuple	  = std::tuple< double, const char *, int >;
	constexpr auto name( const Tuple & , std::size_t i ) {
		constexpr const char *	names[ 3 ] = { "ett", "två", "tre" };
		return names[ i ];
	}

}	// namespace pax
