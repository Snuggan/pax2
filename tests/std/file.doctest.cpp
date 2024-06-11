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


#include <pax/std/file.hpp>
#include <pax/std/span2.hpp>

#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {
	
	const auto no_err				  = std::error_code{};
	const auto no_such_file			  = std::make_error_code( std::errc::no_such_file_or_directory );


	inline auto message_str( 
		const std::string_view			message_,
		const std::source_location	  & sl_ = std::source_location::current()
	) {
		return std20::format( "{}: {}\n\tPath:               'www'\n", to_string( sl_ ), message_ );
	}
	
	
	DOCTEST_TEST_CASE( "filesystem checked_path" ) {
		DOCTEST_CHECK_THROWS_WITH_AS( file_path  ( "www" ), message_str( "Not a regular file" ).c_str(),		Runtime_exception );
		DOCTEST_CHECK_THROWS_WITH_AS( dir_path   ( "www" ), message_str( "No such directory" ).c_str(),			Runtime_exception );
		DOCTEST_CHECK_THROWS_WITH_AS( source_path( "www" ), message_str( "No such file or directory" ).c_str(),	Runtime_exception );
		DOCTEST_CHECK_THROWS_WITH_AS( dest_path  ( "www/vvv" ), 
							message_str( "Can't save file here as there is no parent directory" ).c_str(),		Runtime_exception );
		DOCTEST_CHECK_NOTHROW       ( dest_path  ( "www" ) );
	}
	DOCTEST_TEST_CASE( "filesystem file_path" ) {
		const auto	temp	  = std::filesystem::temp_directory_path() / "www";
		{
			std::ofstream f{ temp };
		}
		DOCTEST_FAST_CHECK_EQ( file_path  ( temp ), temp );
		DOCTEST_FAST_CHECK_EQ( source_path( temp ), temp );
		DOCTEST_FAST_CHECK_EQ( dest_path  ( temp ), temp );
		
		std::error_code		ec;
		std::filesystem::remove( temp, ec );
	}
	DOCTEST_TEST_CASE( "filesystem dir_path" ) {
		std::filesystem::path			tempdir( std::filesystem::temp_directory_path() );
		DOCTEST_FAST_CHECK_EQ( dir_path   ( tempdir ), tempdir );
		DOCTEST_FAST_CHECK_EQ( source_path( tempdir ), tempdir );
		DOCTEST_FAST_CHECK_EQ( dest_path  ( tempdir ), tempdir );
	}

	DOCTEST_TEST_CASE( "filesystem Temppath destination" ) {
		const std::filesystem::path		path{ "/hej/du/glade.txt" };
		const Temppath					temp{ path };
		DOCTEST_FAST_CHECK_NE( path, temp.temporary() );
		DOCTEST_FAST_CHECK_EQ( path, temp.destination() );
	}
	DOCTEST_TEST_CASE( "filesystem Temppath" ) {
		const std::filesystem::path		p{ std::filesystem::temp_directory_path() / "Temppath.ext" };
		std::filesystem::path			t{};

		{	// With finalize
			auto temp				  = Temppath( p );
			t 						  = temp.temporary();
			DOCTEST_FAST_CHECK_NE( p, t );
			
			DOCTEST_FAST_CHECK_NE( temp.temporary(), p );
			DOCTEST_FAST_CHECK_EQ( temp.temporary(), t );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( t ) );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );

			std::ofstream				output( temp.temporary() );
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( t ) );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );
			output << "Hejsan!\n";

			DOCTEST_FAST_CHECK_EQ( temp.done_with_assert(),     no_err );
			DOCTEST_FAST_CHECK_EQ( temp.done(), 				no_such_file );
			DOCTEST_FAST_CHECK_EQ( temp.done(), 				no_such_file );

			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( t ) );
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( p ) );
			std::filesystem::remove( p );
		}
		DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( t ) );
		DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );

		{	// Without finalize
			auto		temp		  = Temppath( p );
			t 						  = temp.temporary();
			DOCTEST_FAST_CHECK_NE( p, t );
			
			DOCTEST_FAST_CHECK_EQ( temp.temporary(), t );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( t ) );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );
			{	std::ofstream out( temp.temporary() );		}
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( t ) );
			DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );
		}
		DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( t ) );
		DOCTEST_FAST_CHECK_UNARY(!std::filesystem::exists( p ) );
	}
	DOCTEST_TEST_CASE( "filesystem Temppath function" ) {
		const auto			old_file = std::filesystem::temp_directory_path() / "filename.txt";
		{	// Create old_file with contents.
			std::ofstream		out( old_file.native() );
			out << "old_file";
			out.close();
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( old_file ) );
		}
		auto				new_file = Temppath( old_file );
		{	// Create new_file with contents.
			std::ofstream		out( new_file.temporary() );
			out << "new_file";
			out.close();
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( new_file.temporary() ) );
		}

		DOCTEST_FAST_CHECK_EQ( new_file.done_with_assert(),     no_err );
		DOCTEST_FAST_CHECK_EQ( new_file.done(), 				no_such_file );
		DOCTEST_FAST_CHECK_EQ( new_file.done(), 				no_such_file );

		DOCTEST_FAST_CHECK_UNARY(  std::filesystem::exists( old_file ) );
		DOCTEST_FAST_CHECK_UNARY( !std::filesystem::exists( new_file.temporary() ) );
		{	// Check if old_file was replaced by new_file.
			std::string			target;
			std::ifstream		in( old_file.native() );
			in >> target;
			in.close();
			DOCTEST_FAST_CHECK_EQ( target, "new_file" );
		}
	
	
		std::filesystem::path 		temp;
		{
			const auto			target = Temppath( std::filesystem::temp_directory_path() / "file.txt" );
			temp = target.temporary();
			std::ofstream		out( target.temporary() );
			out << "target_file";
			out.close();
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( target.temporary() ) );
			DOCTEST_FAST_CHECK_UNARY( std::filesystem::exists( temp ) );
			DOCTEST_FAST_CHECK_EQ( target.temporary(), temp );
		}
		// Automatically erased?
		DOCTEST_FAST_CHECK_UNARY( !std::filesystem::exists( temp ) );
	}

}	//	namespace pax