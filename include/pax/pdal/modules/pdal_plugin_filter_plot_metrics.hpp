// plot_metrics.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <string>


namespace pax {


	/// Process point cloud files into plot metric files. 
	class PDAL_DLL plot_metrics : public pdal::Filter {
	public:
		plot_metrics()				  = default;
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		using coordinate_type		  = double;
		using value_type			  = float;

		// pax member variables:
		std::string						m_plot_file{}, m_dest_plots{};
		pdal::StringList				m_metrics;			// Metric accessor names.
		double							m_nilsson{ 0.0 };
		pdal::SpatialReference			m_srs;
	};

} // namespace pdal
