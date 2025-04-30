//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/table.hpp>

#include <pax/doctest.hpp>
#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 

#include <sstream>


namespace pax {

	constexpr auto		 		cols = 8u;
	constexpr auto		 		rows = 4u;
	constexpr std::array		extents{ cols, rows };
	constexpr std::array		arr{
		 0,  1,  2,  3,  4,  5,  6,  7,		// 0
		 8,  9, 10, 11, 12, 13, 14, 15,		// 1
		16, 17, 18, 19, 20, 21, 22, 23,		// 2
		24, 25, 26, 27, 28, 29, 30, 31,		// 3
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
	};
	constexpr std::span			sp0{ arr };
	constexpr std::span			sp { sp0.first( cols*rows ) };


	DOCTEST_TEST_CASE( "layout" ) { 
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 2 >, std::layout_right >;
			const MD			md( sp.data(), extents );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		rows );
			DOCTEST_FAST_CHECK_EQ( md[ 7, 3 ],		3*cols + 7 );
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 2 >, std::layout_left >;
			const MD			md( sp.data(), extents );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	cols );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	rows );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 0 ],		 0 );
			DOCTEST_FAST_CHECK_EQ( md[ 0, 1 ],		cols );
			DOCTEST_FAST_CHECK_EQ( md[ 1, 0 ],		 1 );
			DOCTEST_FAST_CHECK_EQ( md[ 7, 3 ],		3 + rows*7 );
		}
	}
	DOCTEST_TEST_CASE( "begin/end dim 2" ) { 
		constexpr std::array	extents2{ 3, 4 };
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 2 >, std::layout_right >;
			const MD			md( sp.data(), extents2 );

			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	extents2[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	extents2[ 1 ] );
			{
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			} {
				auto			b = begin< 1 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	4 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 2 >, std::layout_left >;
			const MD			md( sp.data(), extents2 );

			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	extents2[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	extents2[ 1 ] );
			{
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	3 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			} {
				auto			b = begin< 1 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
		}
	}
	DOCTEST_TEST_CASE( "begin/end dim 3" ) { 
		constexpr std::array	extents3{ 3, 4, 2 };
		{	// layout_right
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 3 >, std::layout_right >;
			const MD			md( sp.data(), extents3 );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	extents3[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	extents3[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 2 ),	extents3[ 2 ] );
			{	// Dim 0
				auto				b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				// print_meta( md );
				// auto				b = begin< 1 >( md, 1 );
				// DOCTEST_FAST_CHECK_EQ( &md[ 1, 1, 0 ] - &md[ 0, 1, 0 ], 0 );
				// DOCTEST_FAST_CHECK_EQ( &md[ 0, 1, 1 ] - &md[ 0, 1, 0 ], 0 );
				// DOCTEST_FAST_CHECK_EQ( b.stride(),	8 );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
			{	// Dim 2
				auto				b = begin< 2 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	2 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 2 >( md, 1 ) - b,	0 );
			}
		}
		{	// layout_left
			using MD		  = std::mdspan< const int, std::dextents< std::size_t, 3 >, std::layout_left >;
			const MD			md( sp.data(), extents3 );
			DOCTEST_FAST_CHECK_EQ( md.extent( 0 ),	extents3[ 0 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 1 ),	extents3[ 1 ] );
			DOCTEST_FAST_CHECK_EQ( md.extent( 2 ),	extents3[ 2 ] );
			{	// Dim 0
				auto				b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	3 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
				// print_meta( md );
				// auto				b = begin< 1 >( md, 1 );
				// DOCTEST_FAST_CHECK_EQ( &md[ 1, 1, 0 ] - &md[ 0, 1, 0 ], b.stride() );
				// DOCTEST_FAST_CHECK_EQ( &md[ 0, 1, 1 ] - &md[ 0, 1, 0 ], b.stride() );
				// DOCTEST_FAST_CHECK_EQ( b.stride(),	8 );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 0 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				// DOCTEST_FAST_CHECK_EQ( end< 1 >( md, 1 ) - b,	0 );
			}
			{	// Dim 2
				auto				b = begin< 2 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 0, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 0, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 2, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 0, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 2, 3, 1 ] );
				DOCTEST_FAST_CHECK_EQ( end< 2 >( md, 1 ) - b,	0 );
			}
		}
	}
	DOCTEST_TEST_CASE( "resize" ) { 
		constexpr std::array	downsize { cols - 2, rows - 1 };
		constexpr std::array	upsize_r { cols,     rows + 2 };
		constexpr std::array	upsize_c { cols + 2, rows     };
		constexpr std::array	upsize_rc{ cols + 2, rows + 2 };

		{	// layout_right
			using MDspan	  = std::mdspan< int, std::dextents< std::size_t, 2 >, std::layout_right >;
			constexpr int		in01{ sp[ 1 ] }, in10{ sp[ rows ] }, in25{ sp[ 2 + rows*5 ] };

			{	// Downsize
				constexpr auto						new_extent = downsize;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
			}
			{	// Upsize rows
				constexpr auto						new_extent = upsize_r;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
			}
			{	// Upsize cols
				constexpr auto						new_extent = upsize_c;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
			}
			{	// Upsize rows and cols
				constexpr auto						new_extent = upsize_rc;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, rows ],	 0 );
			}
		}
		{	// layout_left
			using MDspan	  = std::mdspan< int, std::dextents< std::size_t, 2 >, std::layout_left >;
			constexpr int		in01{ sp[ cols ] }, in10{ sp[ 1 ] }, in25{ sp[ cols*2 + 5 ] };

			{	// Downsize
				constexpr auto						new_extent = downsize;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
			}
			{	// Upsize rows
				constexpr auto						new_extent = upsize_r;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
			}
			{	// Upsize cols
				constexpr auto						new_extent = upsize_c;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
			}
			{	// Upsize rows and cols
				constexpr auto						new_extent = upsize_rc;
				std::vector< int >					data( sp0.begin(),  sp0.end() );
				DOCTEST_FAST_CHECK_LE( new_extent[ 0 ]*new_extent[ 1 ], data.size() );

				MDspan								srce{ data.data(), extents };
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( srce[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( srce[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( srce[ 5, 2 ],		in25 );

				MDspan								dest{ data.data(), new_extent };
				resize( std::span( data ), srce, dest );
				DOCTEST_FAST_CHECK_EQ( dest.data_handle(),	data.data() );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 0 ),	new_extent[ 0 ] );
				DOCTEST_FAST_CHECK_EQ( dest.extent( 1 ),	new_extent[ 1 ] );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, 1 ],		in01 );
				DOCTEST_FAST_CHECK_EQ( dest[ 1, 0 ],		in10 );
				DOCTEST_FAST_CHECK_EQ( dest[ 5, 2 ],		in25 );
				DOCTEST_FAST_CHECK_EQ( dest[ 0, rows ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, 0 ],		 0 );
				DOCTEST_FAST_CHECK_EQ( dest[ cols, rows ],	 0 );
			}
		}
	}

}	// namespace pax
