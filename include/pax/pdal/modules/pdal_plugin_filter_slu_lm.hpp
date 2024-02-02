#pragma once

#include <pdal/Filter.hpp>


namespace pax {

	class PDAL_DLL Slu_lm : public pdal::Filter {
	public:
		using pdal::Filter::Filter;
		std::string getName() 								const override;

	private:
		void addArgs( pdal::ProgramArgs & args_ )			override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )	override;
		pdal::PointViewPtr slu_filter( pdal::PointViewPtr view_ ) const;

		using coordinate_type		  = double;

        coordinate_type		m_min_z{ -2.0 };
        coordinate_type		m_max_z{ 50.0 };
        bool				m_lm_filter{ false };

		Slu_lm( const Slu_lm & )						  = delete;
		Slu_lm & operator=( const Slu_lm & )			  = delete;
	};

} // namespace pax