#pragma once

#include <pdal/Filter.hpp>


namespace pax {

	class PDAL_DLL slu_lm_old : public pdal::Filter {
	public:
		slu_lm_old()										  = default;
		slu_lm_old( const slu_lm_old & )					  = delete;
		slu_lm_old( slu_lm_old &&)							  = delete;
		slu_lm_old & operator=( const slu_lm_old & )		  = delete;
		slu_lm_old & operator=( slu_lm_old && )				  = delete;

		virtual ~slu_lm_old();

		using pdal::Filter::Filter;
		std::string getName() 									const override;

	private:
		void addArgs( pdal::ProgramArgs & args_ )				override;
		void addDimensions( pdal::PointLayoutPtr layout_ )		override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )		override;
		pdal::PointViewPtr slu_filter( pdal::PointViewPtr view_ ) const;

		using coordinate_type		  = double;

        coordinate_type		m_min_z{ -2.0 };
        coordinate_type		m_max_z{ 50.0 };
        bool				m_lm_filter{ false };
	};

} // namespace pax