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

#include "../tables/strided-iterator.hpp"
#include "span.hpp"
#include <cassert>
#include <algorithm>	// std::copy_n, std::fill_n, std::min


namespace std {

	template< typename T0, typename T1, typename Extents0, typename Extents1, typename Layout, typename Accessor >
		requires( Extents0::rank() == Extents1::rank() )
	bool operator==( 
		const std::mdspan< T0, Extents0, Layout, Accessor >	md0_,
		const std::mdspan< T1, Extents1, Layout, Accessor >	md1_
	) {
		return ( md0_.extents() == md1_.extents() ) &&
			std::equal(	md0_.data_handle(), md0_.data_handle() + md0_.size(), 
						md1_.data_handle(), md1_.data_handle() + md1_.size() );
	}

}	// namespace std

namespace pax {

	template< typename Layout >	struct is_layout_right;
	template< typename Layout >	constexpr bool is_layout_right_v	= is_layout_right< Layout >::value;
	template<>	struct is_layout_right< std::layout_right >			: std::bool_constant< true >{};
	template<>	struct is_layout_right< std::layout_left >			: std::bool_constant< false >{};


/// Rearranges the items of data_ so that they correspond to the destination extent.
/// For consistency, data_.data() and srce_.data_handle() and dest_.data_handle() must all be the same.
/// Obviously, data_.size >= max( srce_.size() dest_.size() ).
template< typename T, typename Extents, typename Layout, typename Accessor >
	requires( Extents::rank() >  0 )
static void resize(
	const std::span< T >								data_, 	///< The actual data items.
	const std::mdspan< T, Extents, Layout, Accessor >	srce_, 	///< The source extents.
	const std::mdspan< T, Extents, Layout, Accessor >	dest_ 	///< The destination extents. 
) {
	assert( srce_.is_strided() );
	assert( dest_.is_strided() );
	assert( data_.data() == srce_.data_handle() );		// Not strictly necessary, but for consistency...
	assert( data_.data() == dest_.data_handle() );		// Not strictly necessary, but for consistency...
	assert( data_.size() >= srce_.size() );				// data_ must have a sufficient size.
	assert( data_.size() >= dest_.size() );				// data_ must have a sufficient size.
	if( srce_.empty() || dest_.empty() )	return;		// If either is empty, there is nothing to copy...

	using Ptr					 = T*;
	using Size					 = std::size_t;
	const Ptr		data		 = data_.data();
	const Size		srce_step	 = srce_.extent( is_layout_right_v< Layout >*( srce_.rank() - 1 ) );
	const Size		dest_step	 = dest_.extent( is_layout_right_v< Layout >*( dest_.rank() - 1 ) );
	const Size		iters		 = std::min( srce_.size()/srce_step, dest_.size()/dest_step );

	if( srce_step < dest_step ) {						// Expand: copy from last to first.
		Ptr			srce		 = data + srce_step*iters;
		Ptr			dest		 = data + dest_step*iters;
		const Size	remains		 = dest_step - srce_step;
		while( srce	!= data ) {
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



/// Rearranges the items of data_ so that size of extension first_ is decreased by qtity_.
template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
	requires( Extents::rank() > 0 )
static auto remove(
	const std::span< T >								data_, 		///< The actual data items.
	const std::mdspan< T, Extents, Layout, Accessor >	md_, 		///< The extents etc.
	const std::size_t 									first_, 	///< Index to items to remove.
	const std::size_t 									qtity_ = 1 	///< Number of additional indeces to remove.
) {
	assert( md_.is_strided() );
	assert( data_.size() >= md_.size() );				// data_ must have a sufficient size.
	std::array< std::size_t, Extents::rank() >			extents;
	for( std::size_t i{}; i<Extents::rank(); ++i )
		extents[ i ]				  = md_.extent( i );

	if( md_.size() && ( first_ < md_.extent( Dim ) ) ) {
		using Ptr					  = T*;
		using Size					  = std::size_t;

		if( qtity_ >= md_.extent( Dim ) - first_ ) {	// We are de facto resizing, so call resize.
			extents[ Dim ]			  = first_;
			resize(	data_, md_, std::mdspan< T, Extents, Layout, Accessor >( data_.data(), extents ) );
		} else {
			extents[ Dim ]			 -= qtity_;
			Ptr			srce		  = data_.data();
			const Size	srce_step	  = md_.extent( is_layout_right_v< Layout >*( Extents::rank() - 1 ) );
			const Size	iters		  = md_.size()/srce_step;
			Ptr			dest		  = data_.data();
			const Size	dest_step	  = srce_step - qtity_;
			const Ptr	dest_end	  = dest + dest_step*iters;
			const Size	gap			  = first_ + qtity_;
			const Size	tail		  = dest_step - first_;

			while( dest	!= dest_end ) {
				std::copy_n( srce,		 first_, dest          );
				std::copy_n( srce + gap, tail,   dest + first_ );
				srce				 += srce_step;
				dest				 += dest_step;
			}

			// Zero-out trailing values.
			std::fill_n( dest, data_.data() + data_.size() - dest, T{} );
		}
	}
	return extents;
}



	template< std::size_t Dim, typename MDspan >	struct lesser_dim;
	template< std::size_t Dim, typename MDspan >	constexpr std::size_t lesser_dim_v	= lesser_dim< Dim, MDspan >::value;
	
	template< std::size_t Dim, typename T, typename Extents, typename Accessor >
	struct lesser_dim< Dim, std::mdspan< T, Extents, std::layout_right, Accessor > > 
		: std::integral_constant< std::size_t, Dim ? ( Dim - 1 ) : Extents::rank() - 1 > {};
	
	template< std::size_t Dim, typename T, typename Extents, typename Accessor >
	struct lesser_dim< Dim, std::mdspan< T, Extents, std::layout_left, Accessor > > 
		: std::integral_constant< std::size_t, ( Dim + 1 < Extents::rank() ) ? ( Dim + 1 ) : 0 > {};
	

	/// Returns an iterator to first item at index offset_ in dimension Dim of md_.
	/// Returns the default Strided_iterator if offset_ is to large.
	template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
		requires( ( Dim == 0 ) || ( Dim == Extents::rank() - 1 ) )	// Other dims are not sequencial.
	constexpr Strided_iterator< T > begin( 
		const std::mdspan< T, Extents, Layout, Accessor >		md_,
		const std::size_t 										offset_
	) {
		assert( md_.extent( Dim ) > offset_ );
		assert( md_.is_strided() );
		static constexpr std::size_t 	Lesser = lesser_dim_v< Dim, std::mdspan< T, Extents, Layout, Accessor > >;
		return ( offset_ < md_.extent( Dim ) ) 
			? Strided_iterator< T >( md_.data_handle() + offset_*md_.stride( Dim ), md_.stride( Lesser ) )
			: Strided_iterator< T >{};
	};

	/// Returns an iterator to next after last item at index offset_ in dimension Dim of md_.
	/// Returns the default Strided_iterator if offset_ is to large.
	template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
	constexpr Strided_iterator< T > end( 
		const std::mdspan< T, Extents, Layout, Accessor >		md_,
		const std::size_t 										offset_
	) {
		return ( offset_ < md_.extent( Dim ) ) 
			? begin< Dim >( md_, offset_ ) + md_.size()/md_.extent( Dim )
			: Strided_iterator< T >{};
	};



	template< typename Out, typename T, typename Extents, typename Layout, typename Accessor >
	void print( 
		Out													  & out_, 
		const std::mdspan< T, Extents, Layout, Accessor >		md_,
		const char 												separator_ = 0
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

	template< typename T, typename Extents, typename Layout, typename Accessor >
	void print( 
		const std::mdspan< T, Extents, Layout, Accessor >		md_,
		const char 												separator_ = 0
	) {	return print( std::cerr, md_, separator_ );												}

	template< typename Out, typename T, typename Extents, typename Layout, typename Accessor >
	void print_meta( 
		Out													  & out_, 
		const std::mdspan< T, Extents, Layout, Accessor >		md_
	) {
		constexpr std::size_t 	R		  = md_.rank();
		std::print  ( out_, "rank {}, size {}, {}, extents [ {}", md_.rank(), md_.size(), 
			( is_layout_right_v< Layout > ? "layout_right" : "layout_left" ), md_.extent( 0 ) );
		for( std::size_t i=1; i<R; ++i )	std::print( out_, ", {}", md_.extent( i ) );
		std::print  ( out_, " ], strides [ {}", md_.stride( 0 ) );
		for( std::size_t i=1; i<R; ++i )	std::print( out_, ", {}", md_.stride( i ) );
		std::println( out_, " ]" );
	}

	template< typename T, typename Extents, typename Layout, typename Accessor >
	void print_meta( const std::mdspan< T, Extents, Layout, Accessor > md_ ) {
		print_meta( std::cerr, md_ );
	}

}	// namespace pax
