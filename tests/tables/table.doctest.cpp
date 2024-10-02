//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/table.hpp>

#include <pax/doctest.hpp>
#include <pax/std/span.hpp>		// Needed for more flexible comparison. 


namespace pax {

	constexpr std::size_t 		sz = 32;
	constexpr std::size_t 		cols = 4;
	constexpr std::size_t 		rows = sz/cols;
	static constexpr int arr[ sz ] = {
		 0,  1,  2,  3, 
		 4,  5,  6,  7, 
		 8,  9, 10, 11, 
		12, 13, 14, 15, 
		16, 17, 18, 19, 
		20, 21, 22, 23, 
		24, 25, 26, 27, 
		28, 29, 30, 31
	};
	constexpr std::span			sp{ arr };
	
	
	template< typename T, std::size_t N >
	constexpr T sum( std::span< T, N > sp_ ) {
		std::remove_cv_t< T >	result{};
		for( auto item : sp_ )	result += item;
		return result;
	}


	DOCTEST_TEST_CASE( "Table" ) { 
		Table					table{ sp, cols };
		
		{	// Construction.
			DOCTEST_FAST_CHECK_EQ( table.rows(),			rows );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			cols );
			DOCTEST_FAST_CHECK_EQ( table.size(),			sz );
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
		{	// Resize (for more resizing tests, see resize-reorder.doctest.cpp)
			// Add rows and columns
			table = Table( sp, cols );
			table.resize( 8 + 3, 4 + 5 );
			DOCTEST_FAST_CHECK_EQ( table[ 3, 1 ],			13 );
			DOCTEST_FAST_CHECK_EQ( table.rows(),			8 + 3 );
			DOCTEST_FAST_CHECK_EQ( table.cols(),			4 + 5 );
			DOCTEST_FAST_CHECK_EQ( table.size(),			11*9 );
			DOCTEST_FAST_CHECK_EQ( table.span().size(),		table.size() );
			DOCTEST_FAST_CHECK_EQ( sum( table.span() ),		31*16 );

			table.resize( 8, 4 );
			DOCTEST_FAST_CHECK_EQ( table.span(),			Table( sp, cols ).span() );
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
