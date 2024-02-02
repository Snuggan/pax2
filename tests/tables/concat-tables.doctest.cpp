//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/tables/concat-tables.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax {
	
	using std::filesystem::path;

	DOCTEST_TEST_CASE( "concatinate-tables" ) {
		const dir_path	 	source_folder( doctest_data_root() / "csv" );
		const dir_path	 	good_folder  ( source_folder / "consistent-headers" );
		const file_path		A    { good_folder/"A.csv" };
		const file_path		B    { good_folder/"B.csv" };
		const file_path		empty{ good_folder/"empty.csv" };
		const file_path		Bad  { source_folder/"other-header.csv" };
		
		const std::string	a    { "A;B;C\n1;2;3\n4;5;6" };
		const std::string	b    { "A;B;C\n10;11;12\n13;14;15" };
		const std::string	bad  { "A;B;D\n1;2;3\n4;5;6" };
		const std::string	aggr { "A;B;C\n1;2;3\n4;5;6\n10;11;12\n13;14;15\n" };
		
		{	// Preliminaries
			DOCTEST_FAST_CHECK_EQ( read_string( A ),		a );
			DOCTEST_FAST_CHECK_EQ( read_string( B ),		b );
			DOCTEST_FAST_CHECK_EQ( read_string( Bad ),		bad );
			DOCTEST_FAST_CHECK_EQ( read_string( empty ),	std::string{} );
		}
		{	// Consisten (ok) case
			Append_tables	concat{};
			concat.process( good_folder );
			DOCTEST_FAST_CHECK_EQ( concat.str(),			aggr );
		}
		{	// Inconsisten case
			Append_tables	concat{};
			DOCTEST_CHECK_THROWS( concat.process( source_folder ) );
		}
		{	// No originals changed
			DOCTEST_FAST_CHECK_EQ( read_string( A ),		a );
			DOCTEST_FAST_CHECK_EQ( read_string( B ),		b );
			DOCTEST_FAST_CHECK_EQ( read_string( Bad ),		bad );
			DOCTEST_FAST_CHECK_EQ( read_string( empty ),	std::string{} );
		}
		{	// Complete process.
			const auto 	dest = std::filesystem::temp_directory_path() / "aggregate.csv";
			concat_tables( good_folder, dest );
			DOCTEST_FAST_CHECK_EQ( read_string( dest ),		aggr );
		}
	}
}		// namespace pax
