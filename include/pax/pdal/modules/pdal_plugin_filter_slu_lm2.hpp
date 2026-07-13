#pragma once

#include <pdal/Filter.hpp>
#include <pdal/Streamable.hpp>


namespace pax {

	class PDAL_DLL Slu_lm2 : public pdal::Filter, public pdal::Streamable {
	public:
		Slu_lm2()											  = default;
		Slu_lm2( const Slu_lm2 & )							  = delete;
		Slu_lm2( Slu_lm2 &&)								  = delete;
		Slu_lm2 & operator=( const Slu_lm2 & )				  = delete;
		Slu_lm2 & operator=( Slu_lm2 && )					  = delete;

		virtual ~Slu_lm2();

		using pdal::Filter::Filter;
		std::string getName() 									const override;

	private:
		void addArgs( pdal::ProgramArgs & args_ )				override;
		void addDimensions( pdal::PointLayoutPtr layout_ )		override;
		bool processOne( pdal::PointRef & pt_ )					override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )		override;

		using coordinate_type		  = double;
		
		struct metadata {
			std::size_t 	points_in{}, points_out{};
			std::size_t 	z_negative{}, z_small{}, z_large{}, not_lm{};
		};

        coordinate_type		m_min_z{ -2.0 };
        coordinate_type		m_max_z{ 50.0 };
        bool				m_lm_filter{ false };
		
		mutable metadata	m_metadata{};
		pdal::Dimension::Id	m_height_id{};
	};

} // namespace pax