//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once


#if __has_include( <mdspan> )
#	include <mdspan>
#else
	//	https://github.com/kokkos/mdspan
#	define MDSPAN_IMPL_STANDARD_NAMESPACE std
#	include "../external/mdspan/mdspan.hpp"
#endif

#include "span.hpp"
#include <cassert>
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
	
	template< typename Layout >	struct is_layout_right;
	template< typename Layout >	constexpr bool is_layout_right_v	= is_layout_right< Layout >::value;
	template<>	struct is_layout_right< std::layout_left >			: std::bool_constant< false >{};
	template<>	struct is_layout_right< std::layout_right >			: std::bool_constant< true >{};
	// template<>	struct is_layout_right< std::layout_left_padded >	: std::bool_constant< false >{};
	// template<>	struct is_layout_right< std::layout_right_padded >	: std::bool_constant< true >{};


	/// Rearranges the items of data_ so that they correspond to the destination extent.
	/// For consistency, data_.data() and srce_.data_handle() and dest_.data_handle() must all be the same.
	/// Obviously, data_.size >= max( srce_.size() dest_.size() ).
	template< typename T, typename Extents, typename LayoutPolicy, typename AccessorPolicy >
	static void resize(
		const std::span< T >											data_, 	///< The actual data items.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	srce_, 	///< The source extents.
		const std::mdspan< T, Extents, LayoutPolicy, AccessorPolicy >	dest_ 	///< The destination extents. 
	) {
		static_assert( Extents::rank() );
		assert( srce_.is_strided() );
		assert( dest_.is_strided() );
		assert( data_.data() == srce_.data_handle() );		// Not strictly necessary, but for consistency...
		assert( data_.data() == dest_.data_handle() );		// Not strictly necessary, but for consistency...
		assert( data_.size() >= srce_.size() );				// data_ must have a sufficient size.
		assert( data_.size() >= dest_.size() );				// data_ must have a sufficient size.
		if( srce_.empty() || dest_.empty() )	return;		// If either is empty, there is nothing to copy...

		using Ptr					 = T*;
		using Size					 = std::size_t;
		static constexpr Size contig = is_layout_right_v< LayoutPolicy >*( Extents::rank() - 1 );
		const Ptr		data		 = data_.data();
		const Size		srce_step	 = srce_.extent( contig );
		const Size		dest_step	 = dest_.extent( contig );
		const Size		iters		 = std::min( srce_.size()/srce_step, dest_.size()/dest_step );

		if( srce_step < dest_step ) {						// Expoand: copy from last to first.
			Ptr			srce		 = data + srce_step*iters;
			Ptr			dest		 = data + dest_step*iters;
			const Ptr	srce_end	 = data;
			const Size	remains		 = dest_step - srce_step;
			while( srce	!= srce_end ) {
				std::copy_n( srce	-= srce_step, srce_step, dest -= dest_step );
				std::fill_n( dest	+  srce_step, remains, T{} );
			}
		} else if( srce_step > dest_step ) {				// Shrink:  copy from first to last.
			Ptr			srce		 = data;
			Ptr			dest		 = data;
			const Ptr	srce_end	 = data + srce_step*iters;
			while( srce	!= srce_end ) {
				std::copy_n( srce, dest_step, dest );
				srce				+= srce_step;
				dest				+= dest_step;
			}
		}
		if( dest_step*iters < dest_.size() )				// Zero-out trailing values, if any.
			std::fill_n( data + dest_step*iters, dest_.size() - dest_step*iters, T{} );
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
