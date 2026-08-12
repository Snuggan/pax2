//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../point-stuff/box.hpp"
#include <pax/tables/table.hpp>

#include <filesystem>


namespace pax {
	
	template< arithmetic A >
	class Raster : public Table< A, Indexer< 2 > > {
		using value_type				  = std::remove_cv_t< A >;
		using Base						  = Table< value_type, Indexer< 2 > >;

		Box_indexer2d						m_bindexer{};
		
		Raster(
			std::vector< A >			 && data_,
			const Box_indexer2d			  & bindexer_
		) : Base{ data_, bindexer_.extents() }, m_bindexer{ bindexer_ } {}
		
	public:
		Raster()											  = default;
		Raster( const Raster & )							  = default;
		Raster( Raster && )									  = default;
		Raster & operator=( const Raster & )				  = default;
		Raster & operator=( Raster && )						  = default;
		
		friend Raster read_raster( const std::filesystem::path & file_ );
		friend Raster read_raster( const std::filesystem::path & file_, const Box2d & area_ );
		void save( const std::filesystem::path & file_ )		const;

		/// Get an element value via a point.
		A   operator[]( const Point2d pt_ )						const noexcept	{
			return Base::operator[]( m_bindexer.scalar_index( pt_ ) );
		}
		
		/// Get an element reference via a point.
		A & operator[]( const Point2d pt_ )						noexcept		{
			return Base::operator[]( m_bindexer.scalar_index( pt_ ) );
		}
		
		/// Get an element reference via a point.
		const Box2d & bbox()									const noexcept	{	return m_bindexer.box();	}
	};
	
}	// namespace pax
