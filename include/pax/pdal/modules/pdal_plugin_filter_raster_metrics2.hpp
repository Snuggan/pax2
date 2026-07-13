// Raster_metrics2.hpp

#pragma once

#include <pax/pdal/metrics-infrastructure/function-filter.hpp>	// Point_aggregator, Function_filter
#include <pax/pdal/utilities/bbox_indexer.hpp>
#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <string>
#include <filesystem>

#define PAX_STREAMING	1


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
	class PDAL_DLL Raster_metrics2 : public pdal::Streamable, public pdal::Filter {
	public:
		Raster_metrics2()			  = default;
		virtual ~Raster_metrics2();
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		void addDimensions( pdal::PointLayoutPtr layout_ )	override;
		bool processOne( pdal::PointRef & pt_ )				override;
	    void prepared( pdal::PointTableRef table_ )			override;
	    void ready( pdal::PointTableRef table_ )			override;
	    void filter( pdal::PointView & view_ )				override;
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
		
		// For processing:
		std::vector< metrics::Point_aggregator >	pr_z_accumulators;
		std::vector< metrics::Function_filter >		pr_metrics_set{};
		pdal::Dimension::Id 			pr_height_dimension{};
		bool 							pr_has_return_number{};
		Bbox_indexer					pr_bbox{};
		
		struct metadata {
			std::size_t 	points_processed{};
		};
		metadata			m_metadata{};


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
	};

} // namespace pdal
