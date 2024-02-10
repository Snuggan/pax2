//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/pdal/process/plot-metrics.hpp>

#include <pdal/io/LasReader.hpp>
#include <pdal/io/LasHeader.hpp>

#include <pax/reporting/debug.hpp>
#include <pax/external/doctest/doctest-pax.hpp>


namespace pax { 
	
	constexpr std::size_t all_rows			=   7;
	constexpr std::size_t plot_id			=   2;
	constexpr std::size_t count_all			=  36;
	constexpr std::size_t count_1ret		=  79;

	const dir_path laz_folder		= doctest_data_root() / "plot-metric" / "more-laz";
	const file_path csv_file		= laz_folder / ".." / "plots.csv";
	const file_path file0			= laz_folder / "18H001_72250_7750_25-quarter.laz";
	const file_path file1			= laz_folder / "19G026_72450_7275_25-quarter.laz";
	const file_path file2			= laz_folder / "19G026_72450_7600_25-quarter.laz";
	

	auto bbox( const pdal::LasHeader & h_ ) noexcept {
		using Point3				= std::array< double, 3 >;
		using Box					= std::array< Point3, 2 >;
		return Box{
			Point3{ h_.minX(), h_.minY(), h_.minZ() }, 
			Point3{ h_.maxX(), h_.maxY(), h_.maxZ() }
		};
	}

	/// Process an individual point cloud file. May be called multiple times.
	void process_file( 
		Process_plots_metrics		  & processor_, 
		const file_path				  & pc_file_ 
	) {
		try {
			pdal::Options					las_opts{};
			las_opts.add( pdal::Option{ "filename", pc_file_.native() } );
			pdal::LasReader					reader{};
			reader.setOptions( las_opts );
			pdal::PointTable				table{};
			reader.prepare( table );

			const pdal::PointViewSet	point_view_set	= pdal::PointViewSet{ reader.execute( table ) };
			pdal::PointViewPtr			point_view_ptr	= pdal::PointViewPtr{ * point_view_set.begin() };

			// Now we can get the bounding box!
			processor_.process( point_view_ptr, bbox( reader.header() ) );

		} catch( const std::exception & error_ ) {
			throw error_message( std20::format( "{} (Reading point cloud file {}.)", error_.what(), to_string( pc_file_ ) ) );
		} catch( ... ) {
			throw error_message( std20::format( "Unknown error reading point cloud file {}.", to_string( pc_file_ ) ) );
		}
	}
	
	std::vector< std::size_t > find_rows( const Text_table< char > & table ) {
		std::vector< std::size_t >	result{};
		const auto					sources = table.export_values< std::size_t >( "count_all" );
		for( std::size_t i{}; i<sources.size(); ++i )
			if( sources[ i ] > 0u )	result.push_back( i );
		return result;
	}
	
	void check_plot(
		const Text_table< char >  & table,
		const std::size_t 			i, 
		const std::string_view		id,				// plot_id
		const std::size_t 			all,			// count_all
		const std::size_t 			ret1			// count_1ret
	) {
		DOCTEST_FAST_CHECK_EQ( table[ i, plot_id ],								id );
		DOCTEST_FAST_CHECK_EQ( from_string< int >( table[ i, count_all ] ),		all );
		DOCTEST_FAST_CHECK_EQ( from_string< int >( table[ i, count_1ret ] ),	ret1 );
	}

	DOCTEST_TEST_CASE( "plots-metrics save" ) { 
		try {
			const auto				metric_set0 = metrics::metric_set( "extra-allt", 1.5 );
			const auto				metric_set  = std::span( metric_set0 );

			const auto				temppath = std::filesystem::temp_directory_path() / "temptable.csv";

			{
				Process_plots_metrics	processor( csv_file );
				process_file( processor, file0 );
				process_file( processor, file1 );
				process_file( processor, file2 );
				processor.save( temppath, metric_set );
			}

			const Text_table< char > table{ temppath };
			
			DOCTEST_FAST_CHECK_EQ( table.rows(), 							all_rows );
			DOCTEST_FAST_CHECK_EQ( table.header().index( "plot_id" ),		plot_id );
			DOCTEST_FAST_CHECK_EQ( table.header().index( "count_all" ),		count_all );
			DOCTEST_FAST_CHECK_EQ( table.header().index( "count_1ret" ),	count_1ret );
		
			const auto				rows = find_rows( table );
			DOCTEST_FAST_REQUIRE_EQ( rows.size(), 7 );
			check_plot( table, rows[ 0 ], "201726533120", 229, 128 );
			check_plot( table, rows[ 1 ], "201726534120", 132, 132 );
			check_plot( table, rows[ 2 ], "201726532060", 201, 118 );
			check_plot( table, rows[ 3 ], "201726533060", 212, 125 );
			check_plot( table, rows[ 4 ], "201826602060", 171, 120 );
			check_plot( table, rows[ 5 ], "201826602120", 191, 102 );
			check_plot( table, rows[ 6 ], "201826603060", 198, 121 );
		} catch( std::exception & error_ ) {
			Debug{} << error_.what();
		}
	}

}	// namespace pax
