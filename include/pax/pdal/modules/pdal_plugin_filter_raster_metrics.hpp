// Raster_metrics.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <string>
#include <filesystem>


namespace pax {

	/// Process point cloud files into raster metric files. 
	/** This code is executed with a command similar to

		pdal pipeline raster_metrics.json --readers.las.filename=infile.laz --writers.raster_metrics.dest=outfile.tif --metadata=outfile.tif.json
		
		Where raster_metrics.json is
	
		{
			"pipeline":[
				{
					"type":"readers.las"
				},
				{
					"type":"writers.raster_metrics",
					"dest":"out/18H001_72250_7750_25.tif",
					"data_type":"float",
					"metrics":"p5,p55",
					"resolution":"12.5",
					"nilsson_level":"1.5", 
					"gdalopts":"BIGTIFF=IF_SAFER,COMPRESS=DEFLATE"
				}
			]
		}
	**/
	class PDAL_DLL Raster_metrics : public pdal::Filter {
	public:
		Raster_metrics()              = default;
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		using coordinate_type		  = double;
		using value_type			  = float;

		// pax member variables:
		std::string						m_dest_rasters{};
		pdal::StringList				m_metrics;			// Metric accessor names.
		double							m_nilsson{ 0.0 };
		coordinate_type					m_alignment{ 0.0 };
		coordinate_type					m_resolution{ 12.5 };
		std::string						m_drivername{ "GTiff" };
		pdal::StringList				m_options{};
		pdal::Dimension::Type			m_dataType{ pdal::Dimension::Type::Float };
		double							m_noData{ std::numeric_limits<double>::quiet_NaN() };
	    pdal::SpatialReference			m_srs;

		static std::filesystem::path insert_suffix(
			const std::filesystem::path		  & dest_,
			const std::string_view				suffix_
		) noexcept {	
			return { 
				  ( dest_.parent_path() / dest_.stem() ).native() 
				+ std::string{ "." }
				+ std::string{ suffix_ }
				+ dest_.extension().native()
			};
		}

		// static_assert( insert_suffix( "/dir/file.tif",	"wow" ).native() == "/dir/file.wow.tif" );
		// static_assert( insert_suffix(  "dir/file.tif",	"wow" ).native() == "dir/file.wow.tif" );
		// static_assert( insert_suffix( "/file.tif",		"wow" ).native() == "/file.wow.tif" );
		// static_assert( insert_suffix(  "file.tif",		"wow" ).native() == "file.wow.tif" );
		// static_assert( insert_suffix( "/dir/",			"wow" ).native() == "/dir/.wow" );
		// static_assert( insert_suffix(  "dir/",			"wow" ).native() == "dir/.wow" );
		// static_assert( insert_suffix( "/",				"wow" ).native() == "/.wow" );
		// static_assert( insert_suffix(  "",				"wow" ).native() == ".wow" );
	};

} // namespace pdal
