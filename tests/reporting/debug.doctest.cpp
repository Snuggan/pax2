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


#include <pax/reporting/debug.hpp>
#include <pax/std/file.hpp>
#include <pax/doctest.hpp>


namespace pax {
	using string = struct string : std::string {};

	DOCTEST_TEST_CASE( "Debug::Object_info" ) {
		if( false ) {
			// Debug< float const & >{};
			Debug{ 123.0f };
			Debug{ "One line" };
			Crash{} << "Another line";
		}
	}
	DOCTEST_TEST_CASE( "debug pax_Debug::Object_info_t" ) {
		{
			{
				const std::string	base{ "double" };
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double >::as_string(),			base + "" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double const >::as_string(),		base + " const" );
				// DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double volatile >::as_string(),		base + " volatile" );
				// DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double const volatile >::as_string(),	base + " const volatile" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double & >::as_string(),			base + "&" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double const & >::as_string(),	base + " const&" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< double && >::as_string(),		base + "&&" );
			}
			{
				const std::string	base{ "pax::string" };
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string >::as_string(),			base + "" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string const >::as_string(),		base + " const" );
				// DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string volatile >::as_string(),		base + " volatile" );
				// DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string const volatile >::as_string(),	base + " const volatile" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string & >::as_string(),			base + "&" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string const & >::as_string(),	base + " const&" );
				DOCTEST_FAST_CHECK_EQ( Debug::Object_info< string && >::as_string(),		base + "&&" );
			}

			double double_v = 45.5;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype(  double_v ) >::as_string(), 	"double" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( &double_v ) >::as_string(), 	"double*" );
		
			double & double_ref_v = double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype(  double_ref_v ) >::as_string(), "double&" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( &double_ref_v ) >::as_string(), "double*" );				// Loses reference!
		
			const double const_double_v = double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype(  const_double_v ) >::as_string(), "double const" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( &const_double_v ) >::as_string(), "double const*" );
		
			const double & const_double_ref_v = const_double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype(  const_double_ref_v ) >::as_string(), "double const&" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( &const_double_ref_v ) >::as_string(), "double const*" );	// Loses reference!
		}
		{
			double double_v = 45.5;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype(  double_v ) >(  double_v ) ) >::as_string(), 
				"double&&" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype( &double_v ) >( &double_v ) ) >::as_string(), 
				"double*&&" );
		
			double & double_ref_v = double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype(  double_ref_v ) >(  double_ref_v ) ) >::as_string(), 
				"double&" );			// & && becomes &
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype( &double_ref_v ) >( &double_ref_v ) ) >::as_string(), 
				"double*&&" );			// Loses reference!
		
			const double const_double_v = double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype(  const_double_v ) >(  const_double_v ) ) >::as_string(), 
				"double const&&" );
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype( &const_double_v ) >( &const_double_v ) ) >::as_string(), 
				"double const*&&" );
		
			const double & const_double_ref_v = const_double_v;
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype(  const_double_ref_v ) >(  const_double_ref_v ) ) >::as_string(), 
				"double const&" );		// & && becomes &
			DOCTEST_FAST_CHECK_EQ( Debug::Object_info< decltype( std::forward< decltype( &const_double_ref_v ) >( &const_double_ref_v ) ) >::as_string(), "double const*&&" );	// Loses reference!
		}
	}

	DOCTEST_TEST_CASE( "error_message" ) {
		// std::cerr << error_message( "This is the message!", std::filesystem::path( "the/path" ), make_error_code( 42 ) );
	}

}	// namespace pax