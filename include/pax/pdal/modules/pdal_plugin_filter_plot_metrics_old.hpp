// plot_metrics_old.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <string>


namespace pax {


	/// Process point cloud files into plot metric files. 
	class PDAL_DLL plot_metrics_old : public pdal::Filter {
	public:
		plot_metrics_old()			  = default;
		std::string getName()								const override;

	private:
		void addArgs( pdal::ProgramArgs & )					override;
		void addDimensions( pdal::PointLayoutPtr layout_ )	override;
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
