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


#include <pax/math/comparison.hpp>
#include <pax/reporting/error_message.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {

	constexpr bool output{ false };
	using Report = std::runtime_error;
	
	DOCTEST_TEST_CASE( "Final_error_message" ) {
		auto err1 = Final_error_message();
		auto err2 = Final_error_message( "Hej!" );
		err1.deactivate();
		err2.deactivate();
	}
	DOCTEST_TEST_CASE( "debug THROW_UNLESS" ) {
		DOCTEST_CHECK_NOTHROW  ( PAX_THROW_IF( false  ) << "More..." );
		DOCTEST_CHECK_NOTHROW  ( PAX_WARN_IF ( false  ) << "More..." );

		DOCTEST_CHECK_NOTHROW  ( PAX_THROW_UNLESS( true  ) << "More..." );
		DOCTEST_CHECK_NOTHROW  ( PAX_WARN_UNLESS ( true  ) << "More..." );
	}
	DOCTEST_TEST_CASE( "debug" ) {
		try {
			PAX_THROW_IF( std::make_error_code( std::errc( 1 ) ) );
		} catch( Report & re_ ) {
			if( output )	std::cerr << re_;
		}
		try {
			PAX_THROW_IF( std::make_error_condition( std::errc( 1 ) ) );
		} catch( Report & re_ ) {
			if( output )	std::cerr << re_;
		}
		try {
			PAX_THROW_UNLESS( all( 0<1, 2==2, 2==3 ) );
		} catch( Report & re_ ) {
			if( output )	std::cerr << re_;
		}
		try {
			PAX_THROW_UNLESS( 2==3 );
		} catch( Report & re_ ) {
			if( output )	std::cerr << re_;
		}
	}

}	// namespace pax
