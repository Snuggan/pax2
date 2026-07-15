// plot_stuff.hpp

#pragma once

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>
#include <string>
#include <string_view>


namespace pax {

	class Plot_points;	// In pax/pdal/process/plot-points.hpp
	class Plot_points;
	

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

		template< typename BBox >
		static std::vector< Plot_points > get_plots(
			const std::string_view	plots_table_, 
			const std::string_view 	id_column_,
			const coordinate_type 	max_disdance_,
			const BBox			  & bbox_
		);

		void addArgs( pdal::ProgramArgs & )					override;
	    void prepared( pdal::PointTableRef table_ )			override;
		bool processOne( pdal::PointRef & pt_ )				override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;

		std::string					m_plot_file{}, 
									m_dest_plot_points_directory{}, 
									m_id_column{ "id" }, 
									m_dest_format{ ".laz" };
		double						m_buffer{ 0.0 };
		pdal::SpatialReference		m_srs;
		
		pdal::PointViewPtr			m_view_ptr;
		std::vector< Plot_points >	m_plots;		// Binary "table" of plots.
		pdal::PointViewPtr			m_pt_view_ptr;
		
		struct metadata {
			std::size_t 			plots_processed{}, points_processed{}, points_found{};
		};

		mutable metadata			m_metadata{};
	};

} // namespace pdal
