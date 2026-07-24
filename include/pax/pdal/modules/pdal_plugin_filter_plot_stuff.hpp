// plot_stuff.hpp

#pragma once

#include <pax/tables/text-table.hpp>	// Handle a csv file.
#include <pax/types/point-stuff/circle.hpp>
#include <pax/pdal/metrics-infrastructure/function-filter.hpp>

#include <pdal/Filter.hpp>
// #include <pdal/Streamable.hpp>

#include <string>
#include <string_view>
#include <vector>


namespace pax {

	/// A simple container for the spacial data of a plot.
	/// It has the plot stuff + an id and a vector of pdal points. 
	class Plot_w_points : public Circle_w_id< double, 2 > {
		using Plot_w_id					  = Circle_w_id< double, 2 >;
		static_assert( sizeof( pdal::PointId ) == 8 );

		std::vector< pdal::PointId >		m_points_idx{};
		metrics::Point_aggregator			m_metric_agg{};
		pdal::Dimension::Id					m_height_dimension{ pdal::Dimension::Id::Z };
		bool								m_do_metrics{}, m_do_points{}, m_has_return_number{};

	public:
		using Plot_w_id::coord_type;

		constexpr Plot_w_points()									  = default;
		constexpr Plot_w_points( const Plot_w_points & )			  = default;
		constexpr Plot_w_points( Plot_w_points && )					  = default;
		constexpr Plot_w_points & operator=( const Plot_w_points & )  = default;
		constexpr Plot_w_points & operator=( Plot_w_points && )		  = default;

		/// The actual constructor. 
		constexpr Plot_w_points(
			const Plot_w_id				  & plot_,
			const bool						do_metrics, 
			const bool						do_points, 
			const bool						has_return_number,
			const pdal::Dimension::Id		height_dimension
		) noexcept : 
			Plot_w_id					  { plot_ 				},
			m_height_dimension			  { height_dimension	}, 
			m_do_metrics				  { do_metrics			},
			m_do_points					  { do_points			},
			m_has_return_number			  { has_return_number	}
		{}

		/// Process a point. Return true if it was inside the plot.
		bool process( const pdal::PointRef & pt_ ) 			noexcept;

		/// Number of point so far accumulated.
		std::size_t num_of_points()							const noexcept	{	return m_points_idx.size();		}

		/// Access the metrics aggregator.
		/// Calculating the metrics mutates the aggregator (the z-values are sorted in place), so no 'const'. 
		metrics::Point_aggregator & metric_aggregator()		noexcept		{	return m_metric_agg;			}

		/// Save the points of each found plot.
		void save_plot_points( 
			const pdal::PointViewPtr	  & view_ptr_,
			const dir_path				  & plot_points_dest_dir_,
			std::string_view				plot_points_file_format_
		) const;
	};

	


	/// Process point cloud files into individual plot point cloud files. 
	class PDAL_DLL plot_stuff : public pdal::Filter/*, public pdal::Streamable*/ {
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

		std::vector< Plot_w_points > get_plots( pdal::PointViewPtr );
		void setting_needs_PointView( pdal::PointViewPtr );
		void addArgs( pdal::ProgramArgs & )					override;
		void prepared( pdal::PointTableRef table_ )			override;
	    void ready( pdal::PointTableRef table_ )			override;
		bool processOne( pdal::PointRef & pt_ );//				override;
	    // void filter( pdal::PointView & view_ )				override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;
		void done( pdal::PointTableRef table_ )				override;

		std::string					m_plot_file{}, 
									m_metrics_dest{}, 
									m_points_dest_dir{}, 
									m_id_column{}, 
									m_points_format{ ".laz" };
		Text_table< char >			m_all_plots_table{};
		double						m_plot_buffer{ 0.0 };
		pdal::StringList			m_metrics;		// Metric accessor names.
		double						m_metrics_nilsson{ 0.0 };
		
		pdal::PointViewPtr			m_view_ptr{};
		std::vector< Plot_w_points >	m_plots{};		// Binary "table" of plots.
		
		struct metadata {
			std::size_t 			points_processed{}, 
									points_in_plots{};
		};
		mutable metadata			m_metadata{};
		
		bool do_metrics()			const noexcept	{	return !m_metrics_dest.empty();		}
		bool do_points()			const noexcept	{	return !m_points_dest_dir.empty();	}
		
	};

} // namespace pdal
