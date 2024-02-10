//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/pdal/process/plot-points.hpp>

#include <pax/external/doctest/doctest-pax.hpp>

#include <pdal/io/LasReader.hpp>
#include <pdal/io/LasHeader.hpp>


namespace pax { 




	constexpr bool TEST_PROCESS_PLOTS_POINTS = false;




	/// Process an individual point cloud file. May be called multiple times.
	void process_points( 
		const file_path				  & pc_file_, 
		const file_path				  & plots_source_, 
		const dir_path				  & dest_, 
		const double					max_disdance_ = 10.0
	) {
		try {
			pdal::Options				las_opts{};
			las_opts.add( pdal::Option{ "filename",		pc_file_.native() } );
			pdal::LasReader				reader{};
			reader.setOptions( las_opts );
			pdal::PointTable			table{};
			reader.prepare( table );

			const pdal::PointViewSet	view_set	= pdal::PointViewSet{ reader.execute( table ) };
			pdal::PointViewPtr			view_ptr	= pdal::PointViewPtr{ * view_set.begin() };

			// Now we can get the bounding box!
			Process_plots_points::process( view_ptr, plots_source_, dest_, 10.0 );

		} catch( const std::exception & error_ ) {
			throw error_message( std20::format(
				  "Error:            '{}'."
				"\nPoint cloud file: '{}'"
				"\nPlot file:        '{}'"
				"\nDestination:      '{}'"
				"\nMax distance:      {}",
				error_.what(), to_string( pc_file_ ), to_string( plots_source_ ), to_string( dest_ ), max_disdance_
			) );
		} catch( ... ) {
			throw error_message( std20::format(
				  "Error:            '{}'."
				"\nPoint cloud file: '{}'"
				"\nPlot file:        '{}'"
				"\nDestination:      '{}'"
				"\nMax distance:      {}",
				"<unknown>", to_string( pc_file_ ), to_string( plots_source_ ), to_string( dest_ ), max_disdance_
			) );
		}
	}
	
	DOCTEST_TEST_CASE( "Process_plots_points" ) { 
		try {
			const dir_path laz_folder	  = doctest_data_root() / "plot-metric" / "more-laz";
			const file_path csv_file	  = laz_folder / ".." / "plots.csv";
			const file_path file0		  = laz_folder / "18H001_72250_7750_25-quarter.laz";
			const file_path file1		  = laz_folder / "19G026_72450_7275_25-quarter.laz";
			const file_path file2		  = laz_folder / "19G026_72450_7600_25-quarter.laz";

			if constexpr( TEST_PROCESS_PLOTS_POINTS ) {
				const auto						tempdir = std::filesystem::temp_directory_path() / "plots_points";
				std::filesystem::create_directory( tempdir );
				process_points( file0, csv_file, tempdir, 10 );
				process_points( file1, csv_file, tempdir, 10 );
				process_points( file2, csv_file, tempdir, 10 );
			}
		} catch( const std::exception & error_ ) {
			std::cerr << error_.what();
		}
	}

}	// namespace pax
