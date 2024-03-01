#pragma once

#include <pdal/Filter.hpp>


namespace pax {

	class PDAL_DLL Remove_overlap : public pdal::Filter {
	public:
		using pdal::Filter::Filter;
		std::string getName() 										const override;

	private:
		using coordinate_type		  = double;
		using angle_type			  = int32_t;	// actually int8_t in asprs
		using source_id_type		  = uint32_t;	// actually uint16_t in asprs

		class Angle_source {
			angle_type					m_angle{ std::numeric_limits< angle_type >::max() };
			source_id_type				m_source_id{};

		public:
			constexpr void update(
				angle_type				angle_, 
				const source_id_type	source_id_
			) noexcept {
				angle_				  = ( angle_ >= 0 ) ? angle_ : -angle_;
				if( angle_ < m_angle ) {
					m_angle			  = angle_;
					m_source_id		  = source_id_;
				}
			}
			
			constexpr source_id_type source_id()	const noexcept	{	return m_source_id;	}
		};

		coordinate_type		m_overlap_resolution{ 0.0 };

		void addArgs( pdal::ProgramArgs & args_ )					override;
		void addDimensions( pdal::PointLayoutPtr layout_ )			override;
		pdal::PointViewSet run( pdal::PointViewPtr view_ )			override;
		pdal::PointViewPtr overlap_filter( pdal::PointViewPtr view_ );

		Remove_overlap( const Remove_overlap & )				  = delete;
		Remove_overlap & operator=( const Remove_overlap & )	  = delete;
	};

} // namespace pax