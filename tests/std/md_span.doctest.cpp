//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/table.hpp>

#include <pax/doctest.hpp>
#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 

#include <sstream>


namespace pax {

	constexpr std::size_t 		sz = 32;
	constexpr std::size_t 		cols = 4;
	constexpr std::size_t 		rows = sz/cols;
	static  int arr[ 64 ] = {
		 0,  1,  2,  3,  
		 4,  5,  6,  7,  
		 8,  9, 10, 11, 
		12, 13, 14, 15, 
		16, 17, 18, 19, 
		20, 21, 22, 23, 
		24, 25, 26, 27, 
		28, 29, 30, 31, 
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
	};
	constexpr std::span			sp{ std::span{ arr }.first( sz ) };


	DOCTEST_TEST_CASE( "layout" ) { 
		{	// layout_right
			using Right		  = std::mdspan< int, std::dextents< std::size_t, 2 >, std::layout_right >;
			const Right			md( sp.data(), std::array{ cols, rows } );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		rows );
			DOCTEST_FAST_CHECK_EQ( md[ 3, 7 ],		rows*cols-1 );
		}
		{	// layout_left
			using Right		  = std::mdspan< int, std::dextents< std::size_t, 2 >, std::layout_left >;
			const Right			md( sp.data(), std::array{ cols, rows } );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		cols );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 3, 7 ],		rows*cols-1 );
		}
	}

}	// namespace pax
