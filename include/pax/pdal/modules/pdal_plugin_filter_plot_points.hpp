// plot_points.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <string>


namespace pax {


	/// Process point cloud files into individual plot point cloud files. 
	class PDAL_DLL plot_points : public pdal::Filter {
	public:
		plot_points()				  = default;
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		void addDimensions( pdal::PointLayoutPtr layout_ )	override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		using coordinate_type		  = double;
		using value_type			  = float;

		// pax member variables:
		std::string						m_plot_file{}, m_dest_plot_points{}, m_id_column{ "id" }, m_dest_format{ ".laz" };
		double							m_buffer{ 0.0 };
		pdal::SpatialReference			m_srs;
	};

} // namespace pdal
