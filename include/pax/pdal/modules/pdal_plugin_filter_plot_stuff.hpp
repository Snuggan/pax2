// plot_stuff.hpp

#pragma once

#include <pax/tables/text-table.hpp>	// Handle a csv file.
#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <string>
#include <string_view>


namespace pax {

	class Plot_stuff;	// In pax/pdal/utilities/plot-stuff.hpp
	

	/// Process point cloud files into individual plot point cloud files. 
	class PDAL_DLL plot_stuff : public pdal::Filter, public pdal::Streamable {
	public:
		plot_stuff()									  = default;
		plot_stuff( const plot_stuff & )				  = delete;
		plot_stuff( plot_stuff &&)						  = delete;
		plot_stuff & operator=( const plot_stuff & )	  = delete;
		plot_stuff & operator=( plot_stuff && )			  = delete;

		virtual ~plot_stuff();
		std::string getName()								const override;

	private:
		using coordinate_type	  = double;
		using value_type		  = float;

		template< typename Point_table >
		std::vector< Plot_stuff > get_plots( Point_table & pt_table_ );

		void addArgs( pdal::ProgramArgs & )					override;
	    void prepared( pdal::PointTableRef table_ )			override;
		bool processOne( pdal::PointRef & pt_ )				override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		std::string					m_plot_file{}, 
									m_metrics_dest_directory{}, 
									m_points_dest_directory{}, 
									m_points_id_column{ "id" }, 
									m_points_format{ ".laz" };
		Text_table< char >			m_all_plots_table{};
		double						m_plot_buffer{ 0.0 };
		pdal::StringList			m_metrics;		// Metric accessor names.
		double						m_metrics_nilsson{ 0.0 };
		
		pdal::PointViewPtr			m_view_ptr{};
		std::vector< Plot_stuff >	m_plots{};		// Binary "table" of plots.
		
		struct metadata {
			std::size_t 			points_processed{}, 
									points_in_plots{};
		};
		mutable metadata			m_metadata{};
	};

} // namespace pdal
