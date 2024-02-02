//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#include <pax/std/file.hpp>
#include <pax/meta/cmd-arguments.hpp>

#include <pax/textual/json.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax::cmd_args {
	
	using Group_type		  = Parameter_set< char >::Group;

	const auto parameters = Parameter_set< char >(
		"Version string.",
		"Descriptive text.",
		"Formal usage."
	)	<<	Group_type( "These are all flags." )
			( 'o',	"off",		"This flag is off by default.", Parameter_type::off_flag() )
			( 'O',	"on",		"This flag is on by default.",  Parameter_type::on_flag() )
		<<	Group_type(	"Ordinary string values" )
			( 'i',	"invisible", "An invisible argument.", Invisible{}, Parameter_type::off_flag() )
			( '1',	"one",		"At least one value.", Parameter_type::one_or_more_values() )
			( '0',	"zero",		"Zero or more values.", Parameter_type::zero_or_more_values() )
			( 's',	"scalar", 	"A scalar value. Long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long long." )
			( 'm',	"multi",	"Zero or more values.", Parameter_type::zero_or_more_values() )
		;

	DOCTEST_TEST_CASE( "cmd_args via span" ) {
		{
			{
				const char *		argv[] = 
					{ "Test name", "--one", "ONE", "--scalar=SCALAR", "SCALAR3", "-oO", "--no-on", "--no-off" };
		
				DOCTEST_CHECK_THROWS_WITH( parameters.parse( 0, argv ),
					"Error: Parameter '--one' requires a value, but none was given. Use argument --help for info." );
				DOCTEST_CHECK_THROWS_WITH( parameters.parse( 3, argv ), 
					"Error: Parameter '--scalar' requires a value, but none was given. Use argument --help for info." );
				DOCTEST_CHECK_NOTHROW( parameters.parse( 4, argv ) );
			} {
				const char *		argv[] = 
					{ "Test name", "--scalar=SCALAR", "--one", "ONE", "--scalar", "SCALAR2" };
				DOCTEST_CHECK_THROWS_WITH( parameters.parse( 6, argv ), 
					"Error: Parameter '--scalar' is a scalar, but was given multiple values 'SCALAR' and 'SCALAR2'. Use argument --help for info." );
			} {
				const char *		argv[] = 
					{ "Test name", "--unknown=irrelevant" };
				DOCTEST_CHECK_THROWS_WITH( parameters.parse( 2, argv ), 
					"Error: Unknown parameter '--unknown'. Use argument --help for info." );
			}
		}

		{
			const file_path		path( doctest_data_root() / "arguments.json" );
			const char *		argv[] = 
				{ "CMD", "--args-file", path.c_str(), "--one", "TWO", "SCALAR3", "-oO", "--no-on", "--no-off", "-i", "-m=abc,def", "-m=ghi, jkl" };
			{
				const auto values{ parameters.parse( 6, argv ) };
				const char										  * one_of[ 4 ] = { "aa", "bbbb", "SCALAR", "dd" };
				DOCTEST_CHECK_EQ( values().size(),					1 );
				DOCTEST_CHECK_EQ( values().front(),					"SCALAR3" );
				DOCTEST_CHECK_EQ( values( "scalar" ).size(),		1 );
				DOCTEST_CHECK_EQ( values( "scalar" ).front(),		"SCALAR" );
				DOCTEST_CHECK_EQ( values.one_of( "scalar", std::span( one_of ) ), "SCALAR" );
				DOCTEST_CHECK_EQ( values( "one" ).size(),			2 );
				DOCTEST_CHECK_EQ( values( "one" ).front(),			"ONE" );
				DOCTEST_CHECK_EQ( values( "one" ).back(),			"TWO" );
				DOCTEST_CHECK_EQ( values.flag( "off" ),				false );
				DOCTEST_CHECK_EQ( values.flag( "on" ),				false );

				DOCTEST_CHECK_THROWS_WITH( values.flag( "scalar" ), "Parameter '--scalar' is not a flag." );
				DOCTEST_CHECK_THROWS_WITH( values.one_of( "scalar", std::span( one_of ).first( 2 ) ), 
					"'SCALAR' (value of '--scalar') is not one of the required values." );
				DOCTEST_CHECK_THROWS_WITH( values( "no-such" ), 	"Unknown parameter '--no-such'." );
			} {
				const auto values{ parameters.parse( 7, argv ) };
				DOCTEST_CHECK_EQ( values.flag( "invisible" ),		false );
				DOCTEST_CHECK_EQ( values.flag( "off" ),				true );
				DOCTEST_CHECK_EQ( values.flag( "on" ),				true );
			} {
				const auto values{ parameters.parse( 12, argv ) };
				DOCTEST_CHECK_EQ( values.flag( "invisible" ),		true );
				DOCTEST_CHECK_EQ( values.flag( "off" ),				false );
				DOCTEST_CHECK_EQ( values.flag( "on" ),				false );

				DOCTEST_CHECK_EQ( values( "multi" ).size(),			4 );
				DOCTEST_CHECK_EQ( values( "multi" ).front(),		"abc" );
				DOCTEST_CHECK_EQ( values( "multi" )[ 1 ],			"def" );
				DOCTEST_CHECK_EQ( values( "multi" )[ 2 ],			"ghi" );
				DOCTEST_CHECK_EQ( values( "multi" ).back(),			" jkl" );
			}
		}
	}
	
	DOCTEST_TEST_CASE( "cmd_args read json" ) {
		const std::string		json = 
			"{ \n"
			"	\"parameter-name\": \"argument1\", \n"
			"	\"group-name\": {\n"
			"		\"parameter-name\": \"argument2\", \n"
			"		\"subname\": { \n"
			"			\"name-id\": \"name\" \n"
			"		} \n"
			"	} \n"
			"} \n"
			;
		const auto correct = std::vector< std::string >{ "from json", "parameter-name", "argument2", "name-id", "name", "parameter-name", "argument1" };
		const auto result = Parse_json::string( json );

		for( std::size_t i=0; i<result.size(); ++i ) {
			DOCTEST_CHECK_EQ( result.size(), correct.size() );
			DOCTEST_CHECK_EQ( result[ i ], correct[ i ] );
		}
			
	}

}	// namespace pax::cmd_args
