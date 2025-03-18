//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "span.hpp"

#include <cassert>
#include <span>
#include <mdspan>
#include <algorithm>	// std::copy_n, std::fill_n, std::min


namespace std {

	template<	typename T0, typename Extents0, typename LayoutPolicy0, typename AccessorPolicy0, 
				typename T1, typename Extents1, typename LayoutPolicy1, typename AccessorPolicy1	>
	bool operator==( 
		const std::mdspan< T0, Extents0, LayoutPolicy0, AccessorPolicy0 >	md0_,
		const std::mdspan< T1, Extents1, LayoutPolicy1, AccessorPolicy1 >	md1_
	) {
		return ( md0_.rank() == md1_.rank() ) &&
			std::equal(	md0_.data_handle(), md0_.data_handle() + md0_.size(), 
						md1_.data_handle(), md1_.data_handle() + md1_.size() );
	}

}

namespace pax {

	template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	static void resize(
		const std::span< T >											data_, 		///< The actual data items.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	srce_map_, 	///< The source extents.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	dest_map_ 	///< The destination extents. 
	) {
		// If either source or destination is empty, there is nothing to copy.
		if( !srce_map_.size() || !dest_map_.size() )					return;

		static_assert( Extents::rank() );
		assert( data_.size() >= srce_map_.size() );
		assert( data_.size() >= dest_map_.size() );

		using Ptr					 	  = T*;
		static constexpr std::size_t D	  = Extents::rank() - 1;	// What dimension has contiguous elements? (Either 0 or rank-1.)

		const auto		  & srce_step  = srce_map_.extent( D );
		const auto		  & dest_step  = dest_map_.extent( D );
		const std::size_t	iters	  = std::min( srce_map_.size()/srce_step, dest_map_.size()/dest_step );

		if( srce_step < dest_step ) {				// Expoand: copy from last to first.
			Ptr				srce	  = data_.data() + srce_step*iters;
			Ptr				dest	  = data_.data() + dest_step*iters;
			const Ptr		srce_end  = data_.data();
			const auto		remains	  = dest_step - srce_step;
			while( srce	!= srce_end ) {
				std::copy_n( srce -= srce_step, srce_step, dest -= dest_step );
				std::fill_n( dest +  srce_step, remains, T{} );
			}
		} else if( srce_step > dest_step ) {		// Shrink:  copy from first to last.
			Ptr				srce	  = data_.data();
			Ptr				dest	  = data_.data();
			const Ptr		srce_end  = data_.data() + srce_step*iters;
			while( srce	!= srce_end ) {
				std::copy_n( srce, dest_step, dest );
				srce += srce_step;
				dest += dest_step;
			}
		}
		if( dest_step*iters < dest_map_.size() )	// Zero-out trailing values, if any.
			std::fill_n( data_.data() + dest_step*iters, dest_map_.size() - dest_step*iters, T{} );
	}



	template< typename Out, typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	void print( 
		Out															  & out_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	md_,
		const char 														separator_ = 0
	) {
		constexpr std::size_t 	R		  = md_.rank();
		const std::size_t 		all		  = md_.size();
		const std::size_t 		cols	  = ( R > 0 ) ? md_.extent( R - 1 )			: md_.size() + 1;
		const std::size_t 		line	  = ( R > 2 ) ? md_.extent( R - 3 ) * cols	: cols;
		const std::size_t 		block	  = ( R > 3 ) ? md_.stride( R - 4 )			: md_.size() + 1;
		const std::size_t		colsOffs  = ( R > 2 ) ? md_.stride( R - 3 ) - cols	: 1;
		const std::size_t		blockOffs = ( R > 2 ) ? md_.stride( R - 3 )			: 0;
		std::size_t 			i{};
		auto					ptr		  = md_.data_handle();
		const auto				end		  = ptr + md_.size();
		while( ptr < end ) {
			pax::print( out_, ptr, ptr + cols, separator_ );
			i   += cols;
			ptr += cols;
			if( !( i%all ) )			{	std::println( out_ );								}
			else if( !( i%block ) )		{	std::println( out_, "\n" );							}
			else if( !( i%line ) )		{	std::println( out_ );			ptr -= blockOffs;	}
			else						{	std::print  ( out_, "\t" );		ptr += colsOffs;	}
		}
	}

	template< typename Out, typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	void print( 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	md_,
		const char 														separator_ = 0
	) {	return print( std::cout, md_, separator_ );						}

	template< typename Out, typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	void print_meta( 
		Out															  & out_, 
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	md_
	) {
		constexpr std::size_t 	R		  = md_.rank();
		std::println( out_, "-----------------------------------------------------" );
		std::print  ( out_, "rank {}, size {}, extents [ {}", md_.rank(), md_.size(), md_.extent( 0 ) );
		for( std::size_t i=1; i<R; ++i )	std::print( out_, ", {}", md_.extent( i ) );
		std::print  ( out_, " ], strides [ {}", md_.stride( 0 ) );
		for( std::size_t i=1; i<R; ++i )	std::print( out_, ", {}", md_.stride( i ) );
		std::println( out_, " ]" );
		print( out_, md_ );
		std::println( out_, "-----------------------------------------------------" );
	}

}	// namespace pax
