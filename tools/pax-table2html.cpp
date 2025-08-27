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

#include <pax/textual/string_meta.hpp>	// html_table
#include <pax/std/file.hpp>
#include <pax/meta/meta.hpp>
#include <pax/meta/cmd-arguments.hpp>


namespace pax { 
	const Meta2			meta2 {
		"pax-table2html",
		"pax-table2html <csv text file (or similar) path>", 
		"Convert a csv-like file into a fully conformant html table."
		"\n- If 'title' is not specified the filename without file suffix is used."
	};

	int main_concat_tables( int argc, const char **argv ) {
		std::filesystem::path		source_path, dest_path;
		std::string					trouble;

		try {
			const auto parameters = cmd_args::Parameters{ meta2.info(), meta2.description(), meta2.usage() }
				( 	'd', "dest",		ANSI_BOLD"Destination path." ANSI_RESET" Path of the resulting html file.", 
																			cmd_args::Default_value( "" )	)
				( 	"title",			"Title of html page.",				cmd_args::Default_value( "" )	)
				(	'm', "meta", 		"Output some metadata.", 			cmd_args::Parameter_type::on_flag()	)
				;

			const auto args			  = parameters.parse( argc, argv );
			if( args().empty() )
				throw std::runtime_error( 
					"You must supply a path to a csv text file (or similar).\n"
					"See 'pax-table2html --help'.\n" 
				);
			source_path				  = args().front();
			dest_path				  = args.cast< std::filesystem::path >( "dest" );
			auto title				  = args.cast< std::string           >( "title" );
			trouble					  = std20::format( "Tool\t{}\nSource\t{}\nDestinatrion\t{}\n", 
											meta2.name(), to_string( source_path ), to_string( dest_path ) );

			const std::string html	  = table2html( 
				read_string( source_path ), 
				title.empty()		  ? source_path.stem().native() : title,
				args( "meta" )[ 0 ] == "true"
			);

			if( dest_path.empty() )	dest_path = source_path.parent_path() / ( source_path.stem().native() + ".html" );
			std::ofstream( dest_path ) << html;

			return EXIT_SUCCESS;
		} 
		catch( Runtime_exception    & e_ )	{	std::cerr << ( e_ << trouble ).what();										} 
		catch( const std::exception & e_ )	{	std::cerr << ( error_message( e_.what() ) << trouble ).what();				} 
		catch( ... ) 						{	std::cerr << ( error_message( "<Unknown_exception>" ) << trouble ).what();	}

		const auto failure = std20::format( ANSI_BOLD"{} failed to to produce a html table {}\n" ANSI_RESET, meta2.name(), to_string( source_path ) );
		fprintf( stderr, "%s", failure.c_str() );
		return EXIT_FAILURE;
	}
}	// namespace pax

int main( int argc, const char **argv )	{	return pax::main_concat_tables( argc, argv );			}

