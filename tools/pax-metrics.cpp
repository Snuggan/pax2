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


#define DOCTEST_CONFIG_DISABLE			// "remove" everything pertaining to doctest.

#include <pax/pdal/metrics-infrastructure/function-filter.hpp>
#include <pax/meta/meta.hpp>
#include <pax/meta/cmd-arguments.hpp>


namespace pax { 
	const Meta2			meta{
		"pax-metrics",
        "pax-metrics <metrics>",
        "List specified metrics.",
        "Given a set of metric and metric set ids, it returns a sorted list of metrics."
	};

	std::string function_filter_help() {
		std::ostringstream			stream;
		metrics::Function_filter::help( stream, "" );
		return stream.str();
	}
	
	int main_plots( int argc, const char **argv ) {
		try {
			const auto parameters = cmd_args::Parameters{ meta.info(), meta.description(), meta.usage() }
				( 	'm', "metrics",		function_filter_help(), cmd_args::Parameter_type::one_or_more_values()	)
				(	'j', "json", 		"Output result + metadata info.", cmd_args::Parameter_type::off_flag()	)
				( 	"nilsson_level",	"For some metrics in the sets, ignore z-values below this value.", cmd_args::Default_value( "1.49" )	)
				;

			const auto args			  = parameters.parse( argc, argv );
			const auto metric_set	  = metrics::metric_set( std::span{ args( "metrics" ) }, args.cast< double >( "nilsson_level" ) );
		
			if( args.flag( "json" ) ) {
				std::vector< std::string >		metrics;
				for( const auto metric : metric_set )
					metrics.push_back( to_string( metric ) );
				
				Json_value json = to_json( meta );
				json[ "execution" ][ "arguments" ]	= args;
				json[ "execution" ][ "result" ]		= metrics;
				std::cout << std::setw( 2 ) << json << '\n';
			} else {
				for( const auto metric : metric_set )
					std::cout << to_string( metric ) << ' ';
			}
			return EXIT_SUCCESS;

		} 
		catch( const std::exception & e_ )	{	std::cerr << e_.what() << '\n';				}
		catch( ... ) 						{	std::cerr << "<Unknown_exception>\n";		}

		const auto failure = std20::format( ANSI_BOLD"{} failed.\n" ANSI_RESET, meta.name() );
		fprintf( stderr, "%s", failure.c_str() );
		return EXIT_FAILURE;
	}
}	// namespace pax

int main( int argc, const char **argv )		{	return pax::main_plots( argc, argv );		}

