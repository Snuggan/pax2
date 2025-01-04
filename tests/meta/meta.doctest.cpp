//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/textual/json.hpp>
#include <pax/meta/meta.hpp>
#include <pax/doctest.hpp>
#include <pax/reporting/debug.hpp>


namespace pax::meta {
	
	const Meta2		meta{ "Name", "Usage", "Title", "Description" };

	DOCTEST_TEST_CASE( "meta" ) {
		DOCTEST_CHECK_EQ( meta.name(), 			"Name" );
		DOCTEST_CHECK_EQ( meta.usage(), 		"Usage" );
		DOCTEST_CHECK_EQ( meta.title(), 		"Title" );
		DOCTEST_CHECK_EQ( meta.description(), 	"Description" );

		DOCTEST_CHECK_EQ( metadata::license, 	"The 2-Clause BSD License" );
		DOCTEST_CHECK_EQ( metadata::copyright, 	"©2014–2025, Peder Axensten" );
		DOCTEST_CHECK_EQ( metadata::author, 	"Peder Axensten, peder#axensten.se" );
		DOCTEST_CHECK_EQ( metadata::maintainer, "Peder Axensten, peder#axensten.se" );

		// Debug{} << meta.info();
	}

	DOCTEST_TEST_CASE( "meta-json" ) {
		Json_value		json = to_json( meta );
		json[ "execution" ][ "arguments" ]	  = "Arguments";
		json[ "execution" ][ "result" ]		  = "Result";

		DOCTEST_CHECK_EQ( json[ "execution" ][ "result" ], 				"Result" );
		DOCTEST_CHECK_EQ( json[ "execution" ][ "tool" ][ "author" ],	metadata::author );

		// PAX_AS_JSON( json );
	}

}	// namespace pax
