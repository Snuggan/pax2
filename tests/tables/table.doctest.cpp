//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/table.hpp>

#include <pax/doctest.hpp>
#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 


namespace pax {

	constexpr std::size_t 		sz = 32;
	constexpr Col		 		cols{ 4u };
	constexpr Row		 		rows{ sz/cols() };
	static  int arr[ 64 ] = {
		 0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15, 
		16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 
		32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 
		48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63
	};
	constexpr std::span			sp{ std::span{ arr }.first( sz ) };
	
	
	template< typename T, std::size_t N >
	constexpr T sum( std::span< T, N > sp_ ) {
		std::remove_cv_t< T >	result{};
		for( auto item : sp_ )	result += item;
		return result;
	}
	
	template< typename T >
	constexpr T sum( Strider< T > str_ ) {
		std::remove_cv_t< T >	result{};
		for( auto item : str_ )	result += item;
		return result;
	}
	
	
	template< typename T >
	void resize_check(
		Table< T >			  & table,
		const Col		 		cols_, 
		const Row		 		rows_
	) {
		table.resize( cols_, rows_ );
		DOCTEST_FAST_CHECK_EQ( table.rows(),	rows_ );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 	cols_ );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 3 ) ],	13 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 3 ), Row( 7 ) ],	31 );
	}


	DOCTEST_TEST_CASE( "Test consistency of ::data() and ::data_handle" ) { 
		Table< int >			table{ Col( 10 ), Row( 12 ) };
		{	// Preliminaries.
			table[ Col( 2 ), Row( 3 ) ]		  = 23;
			DOCTEST_FAST_CHECK_EQ( table[ Col( 2 ), Row( 3 ) ],	23 );
			// DOCTEST_FAST_CHECK_EQ( table[ Row( 2 ), Col( 3 ) ],	23 );
			// DOCTEST_FAST_CHECK_EQ( table[ 2u, 3u ],				23 );
		}
		{	// Resize.
			table.resize( Col( 8 ), Row( 9 ) );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 2 ), Row( 3 ) ],	23 );
		}
		{	// Copy constructor.
			Table< int >		table2{ table };
			table2[ Col( 2 ), Row( 3 ) ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table [ Col( 2 ), Row( 3 ) ],	23 );
			DOCTEST_FAST_CHECK_EQ( table2[ Col( 2 ), Row( 3 ) ],	-3 );
			table[ Col( 2 ), Row( 3 ) ]	  = 23;
		}
		{	// Copy operator.
			Table< int >		table2{};
			table2							  = table;
			table2[ Col( 2 ), Row( 3 ) ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table [ Col( 2 ), Row( 3 ) ],	23 );
			DOCTEST_FAST_CHECK_EQ( table2[ Col( 2 ), Row( 3 ) ],	-3 );
			table[ Col( 2 ), Row( 3 ) ]		  = 23;
		}
		{	// Move operator.
			Table< int >		table2{ table };
			table2[ Col( 2 ), Row( 3 ) ]	  = -3;
			table							  = std::move( table2 );
			
			DOCTEST_FAST_CHECK_EQ( table [ Col( 2 ), Row( 3 ) ],	-3 );
			table[ Col( 2 ), Row( 3 ) ]		  = 23;
		}
		{	// Move constructor.
			Table< int >		table2{ std::move( table ) };
			table2[ Col( 2 ), Row( 3 ) ]	  = -3;
			
			DOCTEST_FAST_CHECK_EQ( table2[ Col( 2 ), Row( 3 ) ],	-3 );
			table							  = table2;
		}
	}
	DOCTEST_TEST_CASE( "Table basics, constructed with rows and cols" ) { 
		{
			Table< int >		   table{ Col( 0 ), Row( 0 ) };
			DOCTEST_FAST_CHECK_EQ( table.cols(),			0 );
			DOCTEST_FAST_CHECK_EQ( table.rows(),			0 );
			DOCTEST_FAST_CHECK_EQ( table.elements(),		0 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		0 );
		}
		{
			Table< int >		   table{ Col( 10 ), Row( 12 ) };
			DOCTEST_FAST_CHECK_EQ( table.cols(),			 10 );
			DOCTEST_FAST_CHECK_EQ( table.rows(),			 12 );
			DOCTEST_FAST_CHECK_EQ( table.elements(),		120 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		  0 );
		}
	}
	DOCTEST_TEST_CASE( "Table basics, constructed with contents" ) { 
		Table					table{ sp, cols };
		{	// Construction.
			DOCTEST_FAST_CHECK_EQ( table.rows(),			rows );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			cols );
			DOCTEST_FAST_CHECK_EQ( table.elements(),		32 );
			DOCTEST_FAST_CHECK_EQ( table.span(),			sp );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 );
		}
		{	// row( i ).
			const auto r	  = table.row( Row( 2 ) );
			DOCTEST_FAST_CHECK_EQ( sum( r ),			38 );
			DOCTEST_FAST_CHECK_EQ( r[ 0 ],				 8 );
			DOCTEST_FAST_CHECK_EQ( r[ cols() - 1 ],		11 );
		}
		{	// Change a row.
			auto itr		  = table.begin_row( Row( 3 ) );
			const auto end	  = table.end_row( Row( 3 ) );
			while( itr < end ) {
				*itr = -*itr;
				++itr;
			}
			DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 3 ) ],	-13 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),			31*16 - 2*54 );
		}
		{	// Change a column.
			auto itr		  = table.begin_col( Col( 1 ) );
			const auto end	  = table.end_col( Col( 1 ) );
			while( itr < end ) {
				*itr = -*itr;
				++itr;
			}
			DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 3 ) ],			+13 );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 - 2*54 - 2*( 107 - 13 ) );
		}
	}
	DOCTEST_TEST_CASE( "Table resize" ) { 
		{	// Minimal actual case.
			static constexpr int data0[] = { 1, 2, 3, 4, 5 };
			static constexpr int data1[] = { 1, 0, 0, 0, 0 };
			{	// 1x5 -> 5x1 -> 0x0
				Table table( std::span{ data0 }, Col( 5 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data0 ) );
				table.resize( Col( 1 ), Row( 5 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data1 ) );
				table.resize( Col( 2 ), Row( 0 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span< int >{} );
			}
			{	// 5x1 -> 1x5 -> 0x0
				Table table( std::span{ data0 }, Col( 1 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data0 ) );
				table.resize( Col( 5 ), Row( 1 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 1 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span( data1 ) );
				table.resize( Col( 8 ), Row( 0 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 0 );
				DOCTEST_FAST_CHECK_EQ( table.span(), std::span< int >{} );
			}
			{	// 0x0 -> 4x5
				Table< int > table;
				table.resize( Col( 5 ), Row( 4 ) );
				DOCTEST_FAST_CHECK_EQ( table.rows(), 4 );
				DOCTEST_FAST_CHECK_EQ( table.cols(), 5 );
			}
		}
		{	// Normal resize.
			Table table( sp, Col( 4 ) );
			DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
			DOCTEST_FAST_CHECK_EQ( table.cols(), 		 4 );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 3 ) ],		13 );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 3 ), Row( 7 ) ],		31 );

			resize_check( table, Col( 9 ), Row( 10 ) );		// Add rows and columns, 8x4 -> 10*9
			resize_check( table, Col( 6 ), Row( 11 ) );		// Add rows, remove columns, 10x9 -> 11x6
			resize_check( table, Col( 8 ), Row(  9 ) );		// Remove rows, add columns, 11x6 -> 9x8
			resize_check( table, Col( 4 ), Row(  8 ) );		// Remove rows and columns, 9x8 -> 8x4
			resize_check( table, Col( 4 ), Row( 10 ) );		// Add rows, 8x4 -> 10x4
			resize_check( table, Col( 4 ), Row(  8 ) );		// Remove rows, 10x4 -> 8x4
			resize_check( table, Col( 6 ), Row(  8 ) );		// Add columns, 8x4 -> 8x6
			resize_check( table, Col( 4 ), Row(  8 ) );		// Remove columns, 8x6 -> 8x4
			resize_check( table, Col( 6 ), Row(  8 ) );		// No change, 8x4 -> 8x4

			// Really small, 8x4 -> 1x1
			table[ Col( 0 ), Row( 0 ) ] = 42;
			table.resize( Col( 1 ), Row( 1 ) );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 0 ) ],		42 );

			// Make big again, 1x1 -> 3x3
			table.resize( Col( 3 ), Row( 3 ) );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 0 ) ],		42 );
		}
	}
	DOCTEST_TEST_CASE( "Table remove_rows" ) { 
		Table table( sp, Col( 8 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 4 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 8 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 7 ), Row( 1 ) ],		15 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 7 ), Row( 2 ) ],		23 );
		
		// table.print( std::cout );
		// table.remove_rows( 2 );
		// table.print( std::cout );
		// DOCTEST_FAST_CHECK_EQ( table.rows(),		 3 );
		// DOCTEST_FAST_CHECK_EQ( table.cols(), 	 8 );
		// DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 7 ) ],		15 );
		// DOCTEST_FAST_CHECK_EQ( table[ Col( 2 ), Row( 7 ) ],		31 );
		//
		// table.remove_rows( 2 );
		// DOCTEST_FAST_CHECK_EQ( table.rows(),		 2 );
		// DOCTEST_FAST_CHECK_EQ( table.cols(), 	 8 );
		// DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 7 ) ],		 7 );
		//
		// table.remove_rows( 1 );
		// DOCTEST_FAST_CHECK_EQ( table.rows(),		 1 );
		// DOCTEST_FAST_CHECK_EQ( table.cols(), 	 8 );
		// DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 7 ) ],		 7 );
		//
		// table.remove_rows( 0 );
		// DOCTEST_FAST_CHECK_EQ( table.rows(),		 0 );
		// DOCTEST_FAST_CHECK_EQ( table.cols(), 	 0 );
	}
	DOCTEST_TEST_CASE( "Table remove_cols" ) { 
		{
			Table table( sp, Col( 4 ) );
			table.remove_cols( Col( 1 ), 2 );
			DOCTEST_FAST_CHECK_EQ( table.rows(),	 8 );
			DOCTEST_FAST_CHECK_EQ( table.cols(), 	 2 );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 7 ) ],	28 );
			DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 7 ) ],	31 );
		}

		Table table( sp, Col( 4 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 4 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 7 ) ],		29 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 2 ), Row( 7 ) ],		30 );
		
		table.remove_cols( Col( 2 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 3 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 1 ), Row( 7 ) ],		29 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 2 ), Row( 7 ) ],		31 );
		
		table.remove_cols( Col( 2 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 2 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 7 ) ],		28 );
		
		table.remove_cols( Col( 1 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 8 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 1 );
		DOCTEST_FAST_CHECK_EQ( table[ Col( 0 ), Row( 7 ) ],		28 );
		
		table.remove_cols( Col( 0 ) );
		DOCTEST_FAST_CHECK_EQ( table.rows(),		 0 );
		DOCTEST_FAST_CHECK_EQ( table.cols(), 		 0 );
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
		static_assert( std::is_nothrow_copy_constructible< T >{} );
		static_assert(!std::is_trivially_copy_constructible< T >{} );

		static_assert( std::is_copy_assignable< T >{} );
		static_assert( std::is_nothrow_copy_assignable< T >{} );
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
