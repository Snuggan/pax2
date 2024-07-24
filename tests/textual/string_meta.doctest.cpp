//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/textual/string_meta.hpp>

#include <pax/doctest.hpp>


namespace pax {
	
	DOCTEST_TEST_CASE( "String_meta" ) { 
		{	// empty
			constexpr String_meta		count{ std::string_view( "" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			0 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			0 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			0 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	0 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	';' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	1 );
		}
		{	// no_rows
			constexpr String_meta		count{ std::string_view( "a,b,c;d" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			7 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			7 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			1 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	',' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	3 );
		}
		{	// no cols
			constexpr String_meta		count{ std::string_view( "\n\r\n\r\r\n" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			6 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			6 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			3 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	0 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	';' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	1 );
		}
		{	// one col
			constexpr String_meta		count{ std::string_view( "a\nb\rc\n\rd\r\n" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			10 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			10 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			4 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	4 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	';' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	1 );
		}
		{	// good
			constexpr String_meta		count{ std::string_view( "a1,a2\nb1,b2\nc1,c2\n" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			18 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			18 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			3 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	',' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	2 );
		}
		{	// empty_row
			constexpr String_meta		count{ std::string_view( "a1,a2\nb1,b2\n\nc1,c2\n" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			19 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			19 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			4 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	',' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	2 );
		}
		{	// bad
			constexpr String_meta		count{ std::string_view( "a1,a2a\nb1,b2\nc1\n" ) };
			DOCTEST_FAST_CHECK_EQ( count.size(),			16 );
			DOCTEST_FAST_CHECK_EQ( count.ascii(),			16 );
			DOCTEST_FAST_CHECK_EQ( count.non_ascii(),		0 );
			DOCTEST_FAST_CHECK_EQ( count.rows(),			3 );
			DOCTEST_FAST_CHECK_EQ( count.non_empty_rows(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.col_delimiter(),	',' );
			DOCTEST_FAST_CHECK_EQ( count.cols_in_first(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.minimum_cols(),	1 );
			DOCTEST_FAST_CHECK_EQ( count.maximum_cols(),	2 );
			DOCTEST_FAST_CHECK_EQ( count.statistics( 'a' ).max(),	3 );
			DOCTEST_FAST_CHECK_EQ( count.statistics( 'q' ).max(),	0 );
			DOCTEST_FAST_CHECK_EQ( count.statistics( 'a' ).min(),	0 );
			DOCTEST_FAST_CHECK_EQ( count.statistics( 'q' ).min(),	0 );
		}
	}
	DOCTEST_TEST_CASE( "parse2table" ) { 
		{	// empty
			const auto[ cells, cols, delimit ] = parse2table( std::string_view( "" ) );
			DOCTEST_FAST_CHECK_EQ( cells.size(),			0 );
			DOCTEST_FAST_CHECK_EQ( cols,					1 );
			DOCTEST_FAST_CHECK_EQ( delimit,					';' );
		}
		{	// no_rows
			const auto[ cells, cols, delimit ] = parse2table( std::string_view( "a,b,c;d" ) );
			DOCTEST_FAST_CHECK_EQ( cells.size(),			3 );
			DOCTEST_FAST_CHECK_EQ( cells.back(),			"c;d" );
			DOCTEST_FAST_CHECK_EQ( cols,					3 );
			DOCTEST_FAST_CHECK_EQ( delimit,					',' );
		}
		{	// no_cols
			const auto[ cells, cols, delimit ] = parse2table( std::string_view( "a\nb\rc\n\rd\r\n" ) );
			DOCTEST_FAST_CHECK_EQ( cells.size(),			4 );
			DOCTEST_FAST_CHECK_EQ( cells[ 2 ],				"c" );
			DOCTEST_FAST_CHECK_EQ( cols,					1 );
			DOCTEST_FAST_CHECK_EQ( delimit,					';' );
		}
		{	// good
			const auto[ cells, cols, delimit ] = parse2table( std::string_view( "a1,a2\nb1,b2\nc1,c2\n" ) );
			DOCTEST_FAST_CHECK_EQ( cells.size(),			6 );
			DOCTEST_FAST_CHECK_EQ( cells[ 4 ],				"c1" );
			DOCTEST_FAST_CHECK_EQ( cols,					2 );
			DOCTEST_FAST_CHECK_EQ( delimit,					',' );
		}
		{	// empty_row
			const auto[ cells, cols, delimit ] = parse2table( std::string_view( "a1,a2\nb1,b2\n\nc1,c2\n" ) );
			DOCTEST_FAST_CHECK_EQ( cells.size(),			6 );
			DOCTEST_FAST_CHECK_EQ( cells[ 4 ],				"c1" );
			DOCTEST_FAST_CHECK_EQ( cols,					2 );
			DOCTEST_FAST_CHECK_EQ( delimit,					',' );
		}
		{	// One row with fewer cols
			DOCTEST_CHECK_THROWS_WITH_AS(
				parse2table( std::string_view( "a1,a2\nb1,b2\nc1\n" ) ), 
				"parse2table: Varying number of columns (smallest 1, largest 2).\n", 
				Runtime_exception
			);
		}
	}

}	// namespace pax
