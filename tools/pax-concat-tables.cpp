//	Copyright (c) 2014, Peder Axensten
//	All rights reserved.
//
//	Redistribution and use in source and binary forms, with or without
//	modification, are permitted provided that the following conditions are met:
//	    * Redistributions of source code must retain the above copyright
//	      notice, this list of conditions and the following disclaimer.
//	    * Redistributions in binary form must reproduce the above copyright
//	      notice, this list of conditions and the following disclaimer in the
//	      documentation and/or other materials provided with the distribution.
//	    * Neither the name of the Swedish University of Agricultural Sciences nor the
//	      names of its contributors may be used to endorse or promote products
//	      derived from this software without specific prior written permission.
//
//	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
//	ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//	DISCLAIMED. IN NO EVENT SHALL PEDER AXENSTEN BE LIABLE FOR ANY
//	DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
//	(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//	LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
//	ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//	(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

/** \file **/


#define DOCTEST_CONFIG_DISABLE		// "remove" everything pertaining to doctest.

#include <pax/tables/concat-tables.hpp>
#include <pax/meta/meta.hpp>
#include <pax/meta/cmd-arguments.hpp>


namespace pax { 
	const Meta2			meta2 {
		"pax-concat-tables",
		"pax-concat-tables --source=<directory path> --dest=<file path>", 
		"Concatinates text table files in a directory, i.e. csv files.", 

		"1. Ignore empty files.\n"
		"2. Get Header from first non-empty file.\n"
		"3. Check that headers in Other non-empty files equal Header.\n"
		"	- Unequal: throw an exception.\n"
		"	- Equal:   append all rows in Other except the header.\n"
		"4. (Sort all rows except header.)\n"
		"5. Save concatinateded files."
	};

	int main_concat_tables( int argc, const char **argv ) {
		std::filesystem::path		dest, source;

		try {
			const auto parameters = cmd_args::Parameters{ meta2.info(), meta2.description(), meta2.usage() }
				( 	's', "source",	ANSI_BOLD"Source path." ANSI_RESET" The files to concatenate."							)
				( 	'd', "dest",	ANSI_BOLD"Destination path." ANSI_RESET" Path of the resulting file."					)
				(	'm', "meta", 	"Save metadata files with execution info.",		cmd_args::Parameter_type::off_flag()	)
				( 	"verbose",		"Display execution progress.", 					cmd_args::Parameter_type::off_flag()	)
				( 	"count",		"Use progress report based on <count> items.", 	cmd_args::Default_value( "0" )			)
				;

			const auto args		  = parameters.parse( argc, argv );
			source				  = args.cast< std::filesystem::path >( "source" );
			dest				  = args.cast< std::filesystem::path >( "dest" );		

			const auto				result_meta = concat_tables( source, dest, args.flag( "verbose" ), args.cast< std::size_t >( "count" ) );

			// Save json file.
			if( args.flag( "meta" ) ) {
				Json_value json = to_json( meta2 );
				json[ "execution" ][ "arguments" ]	= args;
				json[ "execution" ][ "result" ]		= result_meta;
				save_json( dest, json );
			}
			return EXIT_SUCCESS;
		} 
		catch( const std::exception & e_ )	{	std::cerr << e_.what() << '\n';				}
		catch( ... ) 						{	std::cerr << "<Unknown_exception>\n";		}

		const auto failure = std20::format( ANSI_BOLD"{} failed to concat {}\n" ANSI_RESET, meta2.name(), to_string( source ) );
		fprintf( stderr, "%s", failure.c_str() );
		return EXIT_FAILURE;
	}
}	// namespace pax

int main( int argc, const char **argv )	{	return pax::main_concat_tables( argc, argv );			}

