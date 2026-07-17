#pragma once

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>


namespace pax {

	class PDAL_DLL slu_lm : public pdal::Filter, public pdal::Streamable {
	public:
		slu_lm()											  = default;
		slu_lm( const slu_lm & )							  = delete;
		slu_lm( slu_lm &&)									  = delete;
		slu_lm & operator=( const slu_lm & )				  = delete;
		slu_lm & operator=( slu_lm && )						  = delete;

		virtual ~slu_lm() {};

		using pdal::Filter::Filter;
		std::string getName() 									const override;

	private:
		void addArgs( pdal::ProgramArgs & args_ )				override;
	    void prepared( pdal::PointTableRef table_ )				override;
	    void ready( pdal::PointTableRef table_ )				override;
		bool processOne( pdal::PointRef & pt_ )					override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )		override;
		void done( pdal::PointTableRef )						override;

		using coordinate_type		  = double;
		
		struct metadata {
			std::size_t 	points_in{}, points_out{};
			std::size_t 	z_negative{}, z_small{}, z_large{}, not_lm{};
		};

        coordinate_type		m_min_z{ -2.0 };
        coordinate_type		m_max_z{ 50.0 };
        bool				m_lm_filter{ false };
		
		metadata			m_metadata{};
		pdal::Dimension::Id	m_height_id{};
	};

} // namespace pax