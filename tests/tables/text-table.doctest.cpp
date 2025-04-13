//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/text-table.hpp>

#include <pax/doctest.hpp>


namespace pax {



	DOCTEST_TEST_CASE( "Text_table" ) { 
		Text_table< char >		table;

		{	// Create a Text_table.
			const std::string_view		table_contents  ( "r1;r2;r3\na1;a2;1.3\nb1;b2;2.3\n\nc1;c2;3.3\n" );
			const std::string_view		table_contentsA ( "r1;r2;r3\na1;a2;1.3\nb1;b2;2.3\nc1;c2;3.3\n" );
			table					  = Text_table{ std::string{ table_contents } };
			DOCTEST_FAST_CHECK_EQ( table.as_str(), table_contentsA );
		}
		{	// Insert columns.
			const std::string_view		table_contentsB ( "R1;r2;R3\nA4;1;A5\nB4;2;B5\nC4;3;C5\n" );
			const std::string_view		table_contentsAB( "r1;r2;r3;R1;R3\na1;1;1.3;A4;A5\nb1;2;2.3;B4;B5\nc1;3;3.3;C4;C5\n" );
			table.insert_cols( Text_table{ std::string{ table_contentsB } } );
			DOCTEST_FAST_CHECK_EQ( table.as_str(), table_contentsAB );
			DOCTEST_FAST_CHECK_EQ( table.rows(),				3 );
			DOCTEST_FAST_CHECK_EQ( table.cols(),				5 );
			DOCTEST_FAST_CHECK_EQ( table.size(),				15 );
			DOCTEST_FAST_CHECK_EQ( table[ 1, 3 ],				"B4" );
			DOCTEST_FAST_CHECK_EQ( table.row( 1 )[ 3 ],			"B4" );
			DOCTEST_FAST_CHECK_EQ( table.row( 1 ).size(),		table.cols() );
			DOCTEST_FAST_CHECK_EQ( table.begin()[ 8 ],			"B4" );
			DOCTEST_FAST_CHECK_EQ( table.begin_row( 1 )[ 3 ],	"B4" );
			DOCTEST_FAST_CHECK_EQ( table.begin_col( 3 )[ 1 ],	"B4" );
		}
		{	// Export a scalar.
			const auto exported	  = table.export_values< int >( "r2" );
			DOCTEST_FAST_CHECK_EQ( exported.size(),				3 );
			DOCTEST_FAST_CHECK_EQ( exported[ 0 ],				1 );
			DOCTEST_FAST_CHECK_EQ( exported[ 1 ],				2 );
			DOCTEST_FAST_CHECK_EQ( exported[ 2 ],				3 );
		}
		{	// Export a tuple.
			using Testtuple = std::tuple< double, int, std::string >;
			static constexpr class_meta< Testtuple, double, int, std::string >	meta{ "r3", "r2", "R1" };

			const auto exported	  = table.export_values( meta );
			DOCTEST_FAST_CHECK_EQ( exported.size(),				3 );
			DOCTEST_FAST_CHECK_EQ( get< 2 >( exported[ 0 ] ),	"A4" );
			DOCTEST_FAST_CHECK_EQ( get< 1 >( exported[ 1 ] ),	2 );
			DOCTEST_FAST_CHECK_EQ( get< 0 >( exported[ 2 ] ),	3.3 );
		}
	}

	namespace Text_table_ns {
		using T = Text_table< char >;
		
		static_assert(!std::has_virtual_destructor< T >{} );
		static_assert( std::is_nothrow_destructible< T >{} );
		static_assert(!std::is_trivially_destructible< T >{} );

		static_assert( std::is_default_constructible< T >{} );
		static_assert( std::is_nothrow_default_constructible< T >{} );
		static_assert(!std::is_trivially_default_constructible< T >{} );

		static_assert(!std::is_copy_constructible< T >{} );
		static_assert(!std::is_nothrow_copy_constructible< T >{} );
		static_assert(!std::is_trivially_copy_constructible< T >{} );

		static_assert(!std::is_copy_assignable< T >{} );
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
