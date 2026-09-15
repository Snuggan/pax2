//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "raster-meta.hpp"
#include "../point-stuff/box.hpp"
#include <pax/tables/table.hpp>

#include <filesystem>


namespace pax {


	/// Similar to pax::Table, but with georeference coordinates too. Reads/writes rasters through gdal. 
	template< arithmetic A >
	class Raster : public Table< A, Box_indexer2d > {
		using value_type				  = std::remove_cv_t< A >;
		using Base						  = Table< value_type, Box_indexer2d >;
		
		Raster(
			std::vector< A >			 && data_,
			const Box_indexer2d			  & bindexer_
		) : Base{ data_, bindexer_.extents() } {}
		
	public:
		Raster()											  = default;
		Raster( const Raster & )							  = default;
		Raster( Raster && )									  = default;
		Raster & operator=( const Raster & )				  = default;
		Raster & operator=( Raster && )						  = default;
		
		Raster( const std::filesystem::path & file_, const unsigned band_ = 1u );
		Raster( const std::filesystem::path & file_, const Box2d & area_, const unsigned band_ = 1u );

		void save( const std::filesystem::path & file_ )		const;

		/// Get an element reference via a point.
		const Box2d & bbox()									const noexcept	{	return Base::box();	}
	};
	
}	// namespace pax
