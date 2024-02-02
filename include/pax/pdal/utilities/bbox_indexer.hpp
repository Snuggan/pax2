//	Copyright (c) 2014-2021, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "bbox.hpp"
#include <pax/reporting/error_message.hpp>
#include <pax/math/adjust.hpp>		// align_le, align_ge

#include <pdal/Filter.hpp>			// PointViewPtr, PointId


namespace pax {
	
	
	/// Manage a two-dimensional bounding box + resolution. 
	struct Bbox_indexer {
		using coord_type		  = double;
		using index_type		  = std::size_t;
		
		static_assert( std::is_floating_point_v< coord_type > );
		static_assert( std::is_unsigned_v< index_type >, 
			"This class assumes that index_type{ -1 } is positive, that index_type is unsigned." );

		/// Bbox_indexer is a bounding box that can calculate column, row, and vector index for a coordinate pair.
		Bbox_indexer( 
			const Box2			  & bbox_, 
			const coord_type 		resolution_ 
		) :
			m_minx{ align_le( bbox_.minx, resolution_ ) }, 	m_maxx{ align_ge( bbox_.maxx, resolution_ ) }, 
			m_miny{ align_le( bbox_.miny, resolution_ ) }, 	m_maxy{ align_ge( bbox_.maxy, resolution_ ) }, 
			m_resolution{ resolution_ }, 
			m_rows{ minimum_1( static_cast< index_type >( ( maxy() - miny() )/resolution_ ) ) }, 
			m_cols{ minimum_1( static_cast< index_type >( ( maxx() - minx() )/resolution_ ) ) }
		{
			// Check the contract:
			if( minx() > maxx() )	throw error_message( "Bbox_indexer failed: min_x > max_x." );
			if( miny() > maxy() )	throw error_message( "Bbox_indexer failed: min_y > max_y." );
			if( resolution() <= 0 )	throw error_message( "Bbox_indexer failed: resolution <= 0." );
		}

		constexpr index_type rows()			const noexcept	{	return m_rows;								}
		constexpr index_type cols()			const noexcept	{	return m_cols;								}
		constexpr index_type pixels()		const noexcept	{	return rows() * cols();						}
		constexpr coord_type resolution()	const noexcept	{	return m_resolution;						}
		constexpr coord_type minx()			const noexcept	{	return m_minx;								}
		constexpr coord_type maxx()			const noexcept	{	return m_maxx;								}
		constexpr coord_type miny()			const noexcept	{	return m_miny;								}
		constexpr coord_type maxy()			const noexcept	{	return m_maxy;								}
		Box2 bbox()							const noexcept	{	return { minx(), maxx(), miny(), maxy() };	}


		/// The affine transformation of the Bbox_indexer, defined as in gdal.
		constexpr auto affine_vector()		const noexcept	{
			return std::array< coord_type, 6 >{
	    		minx(),		resolution(),	 0.0,
				maxy(),		0.0,			-resolution()
			};
		}


		/// Does the point belong to the bbox?
		bool contains( const coord_type x_, const coord_type y_ ) const noexcept {
			return ( minx() <= x_ ) && ( x_ <= maxx() )
				&& ( miny() <= y_ ) && ( y_ <= maxy() );
		}


		index_type col( const coord_type x_ ) const {
			const index_type c( std::floor( ( x_ - minx() )/resolution() ) );

			if( c >= cols() ) {	// Assumes that index_type( -1 ) is positive, that index_type is unsigned.
				// Are we right on the bbox limit? If so, adjust minimally.
				const auto nudge{ 0.125*resolution() };
				if     ( inclusive( minx(), x_, minx() + nudge ) )	return col( x_ + nudge );
				else if( inclusive( maxx() - nudge, x_, maxx() ) )	return col( x_ - nudge );
				else {
					// No, we are well outside, so the given bbox was plain wrong.
					throw	error_message( 
						( x_ <= minx() ) ?	std20::format( "x < min_x: {} < {} (diff {})", x_, minx(), minx()-x_ ) : 
						( x_ >= maxx() ) ?	std20::format( "x > max_x: {} > {} (diff {})", x_, maxx(), x_-maxx() ) : 
											std20::format( "Calculated col >= cols: {} >= {}", c, cols() ) 
					);
				}
			}
			return c;
		}


		index_type row( const coord_type y_ ) const {
			// When dealing with rasters, origo is at the upper left corner.
			// So y is the other way compared to mathematics (direction is "down" and not "up"). 
			const index_type r( std::floor( ( maxy() - y_ )/resolution() ) );

			if( r >= rows() ) {	// Assumes that index_type( -1 ) is positive, that index_type is unsigned.
				// Are we right on the bbox limit? If so, adjust minimally.
				const auto nudge{ 0.125*resolution() };
				if     ( inclusive( miny(), y_, miny() + nudge ) )	return row( y_ + nudge );
				else if( inclusive( maxy() - nudge, y_, maxy() ) )	return row( y_ - nudge );
				else {
					// No, we are well outside, so the given bbox was plain wrong.
					throw	error_message( 
						( y_ <= miny() ) ?	std20::format( "y < min_y: {} < {} (diff {})", y_, miny(), miny()-y_ ) : 
						( y_ >= maxy() ) ?	std20::format( "y > max_y: {} > {} (diff {})", y_, maxy(), y_-maxy() ) : 
											std20::format( "Calculated row >= rows: {} >= {}", r, rows() )
					);
				}
			}
			return r;
		}


		/// Given this point, what index does its coordinates produce? (row first index)
		/** Works for minx() <= x_ <= maxx() and  miny() <= y_ <= maxy(). 
			If either coordinate is outside the bounding box, an exception is thrown. 		**/
		index_type index( const coord_type x_, const coord_type y_ )	const	{
			return cols()*row( y_ ) + col( x_ );	// row first indexing. 
		}


		friend std::string to_string( const Bbox_indexer & bb_ ) {
			return std20::format( "{}E[{}, {}], N[{}, {}], res {}{}",	
				'{', bb_.minx(), bb_.maxx(), bb_.miny(), bb_.maxy(), bb_.resolution(), '}' );
		}


		template< typename CharT, typename Traits >
		friend std::basic_ostream< CharT, Traits > & operator<<(
			std::basic_ostream< CharT, Traits >	  & out_,
			const Bbox_indexer					  & bb_
		) {
			return	out_ << to_string( bb_ );
		}
	
	private:
		coord_type		m_minx, m_maxx, m_miny, m_maxy, m_resolution;
		index_type	 	m_rows, m_cols;
		
		static constexpr index_type minimum_1( const index_type i_ ) noexcept {
			return ( i_ > 1u ) ? i_ : 1u;
		}
		
		template< typename U >
		static constexpr bool inclusive( const U min_, const U u_, const U max_ ) noexcept {
			return ( min_ <= u_ ) && ( u_ <= max_ );
		}
	};



	
	/// A pdal-specific Bbox_indexer. 
	struct PointView_indexer : Bbox_indexer {
		using coord_type = Bbox_indexer::coord_type;

		/// Create a 2d bounding box that just encloses all points of view while being aligned to alignment_ (usually the resolution).
		/** Throws if view_ is [almost] empty.		**/
		PointView_indexer( const pdal::PointViewPtr view_, const coord_type alignment_ ) 
			: Bbox_indexer{ get_aligned_indexer( view_, alignment_ ) } {}


		/// Given this point, what index does its coordinates produce?
		index_type index( const pdal::PointViewPtr view_, const pdal::PointId idx_ ) const {
			return Bbox_indexer::index(
				view_->getFieldAs< coord_type >( pdal::Dimension::Id::X, idx_ ), 	// x-coord
				view_->getFieldAs< coord_type >( pdal::Dimension::Id::Y, idx_ )		// y-coord
			);
		}

		/// Given this point, what index does its coordinates produce?
		index_type index( const pdal::PointRef & pt_ref_ ) const {
			return Bbox_indexer::index(
				pt_ref_.getFieldAs< coord_type >( pdal::Dimension::Id::X ), 		// x-coord
				pt_ref_.getFieldAs< coord_type >( pdal::Dimension::Id::Y )			// y-coord
			);
		}
	
	private:
		static Bbox_indexer get_aligned_indexer( const pdal::PointViewPtr view_, const coord_type alignment_ ) {
			if( view_->size() == 0 )	throw error_message( "Bbox_indexer failed: Can not create a raster for an empty point-cloud." );
			if( view_->size() == 1 )	throw error_message( "Bbox_indexer failed: Will not create a raster for a one-point point-cloud." );
			return Bbox_indexer{ pax::bbox( view_ ), alignment_ };
		}
	};

}	// namespace pax
