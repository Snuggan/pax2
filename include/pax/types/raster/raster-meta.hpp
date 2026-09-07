//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <string>
#include <string_view>
#include <filesystem>


namespace pax {
	
	/// Get inspiration from <ideas/containers/raster/meta.hpp>

	class Raster_meta {
		std::string		m_georeference{};
		double			m_na_value{};
		bool			m_pixel_is_area{}, m_pixel_is_point{};
		
	public:
		Raster_meta()										  = default;
		Raster_meta( const Raster_meta & )					  = default;
		Raster_meta( Raster_meta && )						  = default;
		Raster_meta & operator=( const Raster_meta & )		  = default;
		Raster_meta & operator=( Raster_meta && )			  = default;
		
		Raster_meta(
			const std::string_view	georeference_, 
			const double 			na_value_, 
			const bool 				pixel_is_area_, 
			const bool 				pixel_is_point_
		) :
			m_georeference		  { georeference_	}, 
			m_na_value			  { na_value_		}, 
			m_pixel_is_area		  { pixel_is_area_	}, 
			m_pixel_is_point	  { pixel_is_point_	}
		{};
		
		Raster_meta( const std::filesystem::path & file_ );
		
		std::string georeference()								const noexcept	{	return m_georeference;		}
		double NA_value()										const noexcept	{	return m_na_value;			}
		bool pixel_is_area()									const noexcept	{	return m_pixel_is_area;		}
		bool pixel_is_point()									const noexcept	{	return m_pixel_is_point;	}
	};
	
}	// namespace pax
