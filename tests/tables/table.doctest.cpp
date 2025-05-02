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
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
	};
	constexpr std::span			sp{ std::span{ arr }.first( sz ) };

	template< typename LayoutPolicy, std::size_t Rank = 4 >
	using mdspan = std::mdspan< int, std::dextents< std::size_t, Rank >, std::layout_right >;
	
	
	template< typename T, std::size_t N >
	constexpr T sum( std::span< T, N > sp_ ) {
		std::remove_cv_t< T >	result{};
		for( auto item : sp_ )	result += item;
		return result;
	}
	
	
	template< typename T >
	void resize_check(
		Table< T >			  & table,
		const std::size_t 		rows_, 
		const std::size_t 		cols_
	) {
		table.resize( rows_, cols_ );
		DOCTEST_FAST_CHECK_EQ( table.rows(),	rows_ );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 	cols_ );
		DOCTEST_FAST_CHECK_EQ( table[ 3, 1 ],	13 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 3 ],	31 );
	}


	DOCTEST_TEST_CASE( "Test consistency of ::data() and ::data_handle" ) { 
		Table< int >			table{ 10, 12 };
		{	// Preliminaries.
			table[ 2, 3 ] = 23;
			DOCTEST_FAST_CHECK_EQ( table.data(),	table.data_handle() );
			DOCTEST_FAST_CHECK_EQ( table[ 2, 3 ],	23 );
		}
		{	// Resize.
			table.resize( 8, 9 );
			DOCTEST_FAST_CHECK_EQ( table.data(),	table.data_handle() );
			DOCTEST_FAST_CHECK_EQ( table[ 2, 3 ],	23 );
		}
		{	// Copy constructor.
			Table< int >		table2{ table };
			table2[ 2, 3 ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table .data(),	table .data_handle() );
			DOCTEST_FAST_CHECK_EQ( table [ 2, 3 ],	23 );
			DOCTEST_FAST_CHECK_EQ( table2.data(),	table2.data_handle() );
			DOCTEST_FAST_CHECK_EQ( table2[ 2, 3 ],	-3 );
			table[ 2, 3 ]	  = 23;
		}
		{	// Copy operator.
			Table< int >		table2{};
			table2			  = table;
			table2[ 2, 3 ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table .data(),	table .data_handle() );
			DOCTEST_FAST_CHECK_EQ( table [ 2, 3 ],	23 );
			DOCTEST_FAST_CHECK_EQ( table2.data(),	table2.data_handle() );
			DOCTEST_FAST_CHECK_EQ( table2[ 2, 3 ],	-3 );
			table[ 2, 3 ]	  = 23;
		}
		{	// Move operator.
			Table< int >		table2{ table };
			table2[ 2, 3 ]	  = -3;
			table			  = std::move( table2 );
			
			DOCTEST_FAST_CHECK_EQ( table .data(),	table .data_handle() );
			DOCTEST_FAST_CHECK_EQ( table [ 2, 3 ],	-3 );
			table[ 2, 3 ]	  = 23;
		}
		{	// Move constructor.
			Table< int >		table2{ std::move( table ) };
			table2[ 2, 3 ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table2.data(),	table2.data_handle() );
			DOCTEST_FAST_CHECK_EQ( table2[ 2, 3 ],	-3 );
			table			  = table2;
		}
	}
	DOCTEST_TEST_CASE( "Table basics, constructed with rows and cols" ) { 
		{
			Table< int >		   table{ 0, 0 };
			DOCTEST_FAST_CHECK_EQ( table.rows(),			0 );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			0 );
			DOCTEST_FAST_CHECK_EQ( table.size(),			0 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		0 );
		}
		{
			Table< int >		   table{ 10, 12 };
			DOCTEST_FAST_CHECK_EQ( table.rows(),			 10 );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			 12 );
			DOCTEST_FAST_CHECK_EQ( table.size(),			120 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		  0 );
		}
	}
	DOCTEST_TEST_CASE( "Table basics, constructed with contents" ) { 
		Table					table{ sp, cols };
		{	// Construction.
			DOCTEST_FAST_CHECK_EQ( table.rows(),			rows );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			cols );
			DOCTEST_FAST_CHECK_EQ( table.size(),			32 );
			DOCTEST_FAST_CHECK_EQ( table.span(),			sp );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 );
		}
		{	// row( i ).
			const auto r	  = table.row( 2 );
			DOCTEST_FAST_CHECK_EQ( sum( r ),			38 );
			DOCTEST_FAST_CHECK_EQ( r.front(),			 8 );
			DOCTEST_FAST_CHECK_EQ( r.back(),			11 );
		}
		{	// Change a row.
			auto itr		  = table.begin_row( 3 );
			const auto end	  = table.end_row( 3 );
			while( itr < end ) {
				*itr = -*itr;
				++itr;
			}
			DOCTEST_FAST_CHECK_EQ( table[ 3, 1 ],			-13 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 - 2*54 );
		}
		{	// Change a column.
			auto itr		  = table.begin_col( 1 );
			const auto end	  = table.end_col( 1 );
			while( itr < end ) {
				*itr = -*itr;
				++itr;
			}
			DOCTEST_FAST_CHECK_EQ( table[ 3, 1 ],			+13 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 - 2*54 - 2*( 107 - 13 ) );
		}
	}
	DOCTEST_TEST_CASE( "Table resize" ) { 
		{	// Minimal actual case.
			static constexpr int data0[] = { 1, 2, 3, 4, 5 };
			static constexpr int data1[] = { 1, 0, 0, 0, 0 };
			{	// 1x5 -> 5x1 -> 0x0
				Table table( std::span{ data0 }, 5 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data0 ) );
				table.resize( 5, 1 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data1 ) );
				table.resize( 0, 2 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span< int >{} );
			}
			{	// 5x1 -> 1x5 -> 0x0
				Table table( std::span{ data0 }, 1 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data0 ) );
				table.resize( 1, 5 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data1 ) );
				table.resize( 1, 0 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span< int >{} );
			}
			{	// 0x0 -> 4x5
				Table< int > table;
				table.resize( 4, 5 );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 4 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
			}
		}
		{	// Normal resize.
			Table table( sp, 4 );
			DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
			DOCTEST_FAST_CHECK_EQ( table.cols(), 		 4 );
			DOCTEST_FAST_CHECK_EQ( table[ 3, 1 ],		13 );
			DOCTEST_FAST_CHECK_EQ( table[ 7, 3 ],		31 );

			resize_check( table, 10, 9 );		// Add rows and columns, 8x4 -> 10*9
			resize_check( table, 11, 6 );		// Add rows, remove columns, 10x9 -> 11x6
			resize_check( table, 9, 8 );		// Remove rows, add columns, 11x6 -> 9x8
			resize_check( table, 8, 4 );		// Remove rows and columns, 9x8 -> 8x4
			resize_check( table, 10, 4 );		// Add rows, 8x4 -> 10x4
			resize_check( table, 8, 4 );		// Remove rows, 10x4 -> 8x4
			resize_check( table, 8, 6 );		// Add columns, 8x4 -> 8x6
			resize_check( table, 8, 4 );		// Remove columns, 8x6 -> 8x4
			resize_check( table, 8, 6 );		// No change, 8x4 -> 8x4

			// Really small, 8x4 -> 1x1
			table[ 0, 0 ] = 42;
			table.resize( 1, 1 );
			DOCTEST_FAST_CHECK_EQ( table[ 0, 0 ],		42 );

			// Make big again, 1x1 -> 3x3
			table.resize( 3, 3 );
			DOCTEST_FAST_CHECK_EQ( table[ 0, 0 ],		42 );
		}
	}
	DOCTEST_TEST_CASE( "Table remove_column" ) { 
		Table table( sp, 4 );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 4 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 1 ],		29 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 2 ],		30 );
		
		table.remove_column( 2 );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 3 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 1 ],		29 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 2 ],		31 );
		
		table.remove_column( 2 );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 2 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 0 ],		28 );
		
		table.remove_column( 1 );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 1 );
		DOCTEST_FAST_CHECK_EQ( table[ 7, 0 ],		28 );
		
		table.remove_column( 0 );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 0 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 0 );
	}
	DOCTEST_TEST_CASE( "Table print" ) { 
		{	// Not csv
			{	// Rank 1
				constexpr std::string_view		result{ "rank 1, size 5, layout_right, extents [ 5 ], strides [ 1 ]\n" };
				std::stringstream				temp{};
				pax::print_meta( temp, mdspan< std::layout_right, 1 >( arr, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2
				constexpr std::string_view		result{ "rank 2, size 15, layout_right, extents [ 3, 5 ], strides [ 5, 1 ]\n" };
				std::stringstream				temp{};
				pax::print_meta( temp, mdspan< std::layout_right, 2 >( arr, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "rank 3, size 30, layout_right, extents [ 2, 3, 5 ], strides [ 15, 5, 1 ]\n" };
				std::stringstream				temp{};
				pax::print_meta( temp, mdspan< std::layout_right, 3 >( arr, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4
				constexpr std::string_view		result{ "rank 4, size 60, layout_right, extents [ 2, 2, 3, 5 ], strides [ 30, 15, 5, 1 ]\n" };
				std::stringstream				temp{};
				pax::print_meta( temp, mdspan< std::layout_right, 4 >( arr, 2, 2, 3, 5 ) );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
		}
		{	// csv
			{	// Rank 1
				constexpr std::string_view		result{ "0;1;2;3;4\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 1 >( arr, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 2
				constexpr std::string_view		result{ "0;1;2;3;4\n5;6;7;8;9\n10;11;12;13;14\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 2 >( arr, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 3
				constexpr std::string_view		result{ "0;1;2;3;4\t15;16;17;18;19\n5;6;7;8;9\t20;21;22;23;24\n10;11;12;13;14\t25;26;27;28;29\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 3 >( arr, 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
			{	// Rank 4
				constexpr std::string_view		result{ "0;1;2;3;4\t15;16;17;18;19\n5;6;7;8;9\t20;21;22;23;24\n10;11;12;13;14\t25;26;27;28;29\n\n30;31;32;33;34\t45;46;47;48;49\n35;36;37;38;39\t50;51;52;53;54\n40;41;42;43;44\t55;56;57;58;59\n" };
				std::stringstream				temp{};
				pax::print( temp, mdspan< std::layout_right, 4 >( arr, 2, 2, 3, 5 ), ';' );
				DOCTEST_FAST_CHECK_EQ( temp.str(), result );
			}
		}
	}

	namespace Text_ns {
		using T = Table< char >;
		
		static_assert(!std::has_virtual_destructor< T >{} );
		static_assert( std::is_nothrow_destructible< T >{} );
		static_assert(!std::is_trivially_destructible< T >{} );

		static_assert( std::is_default_constructible< T >{} );
		static_assert( std::is_nothrow_default_constructible< T >{} );
		static_assert(!std::is_trivially_default_constructible< T >{} );

		static_assert( std::is_copy_constructible< T >{} );
		static_assert(!std::is_nothrow_copy_constructible< T >{} );
		static_assert(!std::is_trivially_copy_constructible< T >{} );

		static_assert( std::is_copy_assignable< T >{} );
		static_assert(!std::is_nothrow_copy_assignable< T >{} );
		static_assert(!std::is_trivially_copy_assignable< T >{} );

		static_assert( std::is_move_constructible< T >{} );
		static_assert( std::is_nothrow_move_constructible< T >{} );
		static_assert(!std::is_trivially_move_constructible< T >{} );

		static_assert( std::is_move_assignable< T >{} );
		static_assert( std::is_nothrow_move_assignable< T >{} );
		static_assert(!std::is_trivially_move_assignable< T >{} );

		static_assert( std::is_swappable< T >{} );
		static_assert( std::is_nothrow_swappable< T >{} );
	}

}	// namespace pax
