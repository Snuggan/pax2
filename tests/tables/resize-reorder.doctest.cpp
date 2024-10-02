//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/resize-reorder.hpp>

#include <pax/doctest.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 


namespace pax {

	DOCTEST_TEST_CASE( "resize_reorder_2d" ) { 
		using std::span;

		static constexpr std::size_t 	size{ 25u*4u };		// >= 99
		int arr[ size ] = {
			 0,  1,  2,  3,
			 4,  5,  6,  7,
			 8,  9, 10, 11,
			12, 13, 14, 15,
			16, 17, 18, 19,
			20, 21, 22, 23,
			24, 25, 26, 27,
			28, 29, 30, 31,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0,
			 0,  0,  0,  0
		};
		const span		sp{ arr };

		{	// Minimal actual case.
			static constexpr int res[] = { 1, 0 };
			{	// 1x2 -> 2x1
				int arr[] = { 1, 2 };
				resize_reorder_2d( span( arr ), 1, 2, 2, 1 );
				DOCTEST_FAST_CHECK_EQ( span( arr ), span( res ) );
			}
			{	// 2x1 -> 1x2
				int arr[] = { 1, 2 };
				resize_reorder_2d( span( arr ), 2, 1, 1, 2 );
				DOCTEST_FAST_CHECK_EQ( span( arr ), span( res ) );
			}
		}
		{	// Normal resize.
			// Add rows and columns, 8x4 -> 10*9
			resize_reorder_2d( sp.first( 10*9 ), 8, 4, 10, 9 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*9 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*9 + 3 ],		31 );

			// Add rows, remove columns, 10x9 -> 11x6
			resize_reorder_2d( sp.first( 10*9 ), 10, 9, 11, 6 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*6 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*6 + 3 ],		31 );

			// Remove rows, add columns, 11x6 -> 9x8
			resize_reorder_2d( sp.first( 9*8 ), 11, 6, 9, 8 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*8 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*8 + 3 ],		31 );

			// Remove rows and columns, 9x8 -> 8x4
			resize_reorder_2d( sp.first( 9*8 ), 9, 8, 8, 4 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*4 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*4 + 3 ],		31 );

			// Add rows, 8x4 -> 10x4
			resize_reorder_2d( sp.first( 10*4 ), 8, 4, 10, 4 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*4 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*4 + 3 ],		31 );

			// Remove rows, 10x4 -> 8x4
			resize_reorder_2d( sp.first( 10*4 ), 10, 4, 8, 4 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*4 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*4 + 3 ],		31 );

			// Add columns, 8x4 -> 8x6
			resize_reorder_2d( sp.first( 8*6 ), 8, 4, 8, 6 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*6 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*6 + 3 ],		31 );

			// Remove columns, 8x6 -> 8x4
			resize_reorder_2d( sp.first( 8*6 ), 8, 6, 8, 4 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*4 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*4 + 3 ],		31 );

			// No change, 8x4 -> 8x4
			resize_reorder_2d( sp.first( 8*6 ), 8, 6, 8, 6 );
			DOCTEST_FAST_CHECK_EQ( sp[ 3*4 + 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( sp[ 7*4 + 3 ],		31 );
		}
		{	// Check original
			static constexpr int res[] = {
				 0,  1,  2,  3,
				 4,  5,  6,  7,
				 8,  9, 10, 11,
				12, 13, 14, 15,
				16, 17, 18, 19,
				20, 21, 22, 23,
				24, 25, 26, 27,
				28, 29, 30, 31,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0,
				 0,  0,  0,  0
			};
			DOCTEST_FAST_CHECK_EQ( span( arr ), span( res ) );
		}
	}

}	// namespace pax
