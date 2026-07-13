// plot_points.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <string>


namespace pax {


	/// Process point cloud files into individual plot point cloud files. 
	class PDAL_DLL plot_points : public pdal::Filter, public pdal::Streamable {
	public:
		plot_points()			  = default;
		virtual ~plot_points();
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		void addDimensions( pdal::PointLayoutPtr layout_ )	override;
		bool processOne( pdal::PointRef & pt_ )				override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		using coordinate_type	  = double;
		using value_type		  = float;

		struct metadata {
			std::size_t 			points_processed{};
		};

		std::string					m_plot_file{}, m_dest_plot_points_directory{}, 
									m_id_column{ "id" }, m_dest_format{ ".laz" };
		double						m_buffer{ 0.0 };
		pdal::SpatialReference		m_srs;
		
		mutable metadata			m_metadata{};
	};

} // namespace pdal
