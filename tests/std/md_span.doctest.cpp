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

	template< typename Layout, std::size_t Rank = 4 >
	using mdspan = std::mdspan< const int, std::dextents< std::size_t, Rank >, Layout >;


	DOCTEST_TEST_CASE( "Table print" ) { 
		{	// Not csv
			{	// Rank 1
				constexpr std::string_view					result{ "[0, 1, 2, 3, 4]\n" };
				constexpr mdspan< std::layout_right, 1 >	md( sp0.data(), 5 );
				std::stringstream							temp{};
				pax::print( temp, md );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 right
				constexpr std::string_view		result{ "[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 2 >( sp0.data(), 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 left
				constexpr std::string_view		result{ "[0, 1, 2]\n[3, 4, 5]\n[6, 7, 8]\n[9, 10, 11]\n[12, 13, 14]\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_left, 2 >( sp0.data(), 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "\n[0, c, r]:\n[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n\n[1, c, r]:\n[15, 16, 17, 18, 19]\n[20, 21, 22, 23, 24]\n[25, 26, 27, 28, 29]\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 3 >( sp0.data(), 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 right
				constexpr std::string_view		result{ "\n[0, 0, c, r]:\n[0, 1, 2, 3, 4]\n[5, 6, 7, 8, 9]\n[10, 11, 12, 13, 14]\n\n[0, 1, c, r]:\n[15, 16, 17, 18, 19]\n[20, 21, 22, 23, 24]\n[25, 26, 27, 28, 29]\n\n[1, 0, c, r]:\n[30, 31, 32, 33, 34]\n[35, 36, 37, 38, 39]\n[40, 41, 42, 43, 44]\n\n[1, 1, c, r]:\n[45, 46, 47, 48, 49]\n[50, 51, 52, 53, 54]\n[55, 56, 57, 58, 59]\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 4 >( sp0.data(), 2, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 left
				constexpr std::string_view		result{ "\n[r, c, 0, 0]:\n[0, 1]\n[2, 3]\n\n[r, c, 1, 0]:\n[4, 5]\n[6, 7]\n\n[r, c, 2, 0]:\n[8, 9]\n[10, 11]\n\n[r, c, 0, 1]:\n[12, 13]\n[14, 15]\n\n[r, c, 1, 1]:\n[16, 17]\n[18, 19]\n\n[r, c, 2, 1]:\n[20, 21]\n[22, 23]\n\n[r, c, 0, 2]:\n[24, 25]\n[26, 27]\n\n[r, c, 1, 2]:\n[28, 29]\n[30, 31]\n\n[r, c, 2, 2]:\n[32, 33]\n[34, 35]\n\n[r, c, 0, 3]:\n[36, 37]\n[38, 39]\n\n[r, c, 1, 3]:\n[40, 41]\n[42, 43]\n\n[r, c, 2, 3]:\n[44, 45]\n[46, 47]\n\n[r, c, 0, 4]:\n[48, 49]\n[50, 51]\n\n[r, c, 1, 4]:\n[52, 53]\n[54, 55]\n\n[r, c, 2, 4]:\n[56, 57]\n[58, 59]\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_left, 4 >( sp0.data(), 2, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
		}
		{	// csv
			{	// Rank 1
				constexpr std::string_view		result{ "0;1;2;3;4\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 1 >( sp0.data(), 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 right
				constexpr std::string_view		result{ "0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 2 >( sp0.data(), 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2 left
				constexpr std::string_view		result{ "0;1;2\n3;4;5\n6;7;8\n9;10;11\n12;13;14\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_left, 2 >( sp0.data(), 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "\n[0, c, r]:\n0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n\n[1, c, r]:\n15;16;17;18;19\n20;21;22;23;24\n25;26;27;28;29\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 3 >( sp0.data(), 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 right
				constexpr std::string_view		result{ "\n[0, 0, c, r]:\n0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n\n[0, 1, c, r]:\n15;16;17;18;19\n20;21;22;23;24\n25;26;27;28;29\n\n[1, 0, c, r]:\n30;31;32;33;34\n35;36;37;38;39\n40;41;42;43;44\n\n[1, 1, c, r]:\n45;46;47;48;49\n50;51;52;53;54\n55;56;57;58;59\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 4 >( sp0.data(), 2, 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4 left
				constexpr std::string_view		result{ "\n[r, c, 0, 0]:\n0;1\n2;3\n\n[r, c, 1, 0]:\n4;5\n6;7\n\n[r, c, 2, 0]:\n8;9\n10;11\n\n[r, c, 0, 1]:\n12;13\n14;15\n\n[r, c, 1, 1]:\n16;17\n18;19\n\n[r, c, 2, 1]:\n20;21\n22;23\n\n[r, c, 0, 2]:\n24;25\n26;27\n\n[r, c, 1, 2]:\n28;29\n30;31\n\n[r, c, 2, 2]:\n32;33\n34;35\n\n[r, c, 0, 3]:\n36;37\n38;39\n\n[r, c, 1, 3]:\n40;41\n42;43\n\n[r, c, 2, 3]:\n44;45\n46;47\n\n[r, c, 0, 4]:\n48;49\n50;51\n\n[r, c, 1, 4]:\n52;53\n54;55\n\n[r, c, 2, 4]:\n56;57\n58;59\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_left, 4 >( sp0.data(), 2, 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
		}
	}

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
			{	// Dim 0
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	1 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
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
			{	// Dim 0
				auto			b = begin< 0 >( md, 1 );
				DOCTEST_FAST_CHECK_EQ( b.stride(),	3 );
				DOCTEST_FAST_CHECK_EQ( *( b++ ), 	md[ 1, 0 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 1 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 2 ] );
				DOCTEST_FAST_CHECK_EQ( *( b++ ),	md[ 1, 3 ] );
				DOCTEST_FAST_CHECK_EQ( end< 0 >( md, 1 ) - b,	0 );
			}
			{	// Dim 1
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
				// // Items are not sequencial
				// std::vector 	test{	&md[ 0, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 0, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 1 ] - &md[ 0, 1, 0 ] };
				// std::sort( test.begin(), test.end() );
				// std::println( "Items are not sequencial: {}", test );
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
				// // Items are not sequencial
				// std::vector 	test{	&md[ 0, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 0 ] - &md[ 0, 1, 0 ],
				// 						&md[ 0, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 1, 1, 1 ] - &md[ 0, 1, 0 ],
				// 						&md[ 2, 1, 1 ] - &md[ 0, 1, 0 ] };
				// std::sort( test.begin(), test.end() );
				// std::println( "Items are not sequencial: {}", test );
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
