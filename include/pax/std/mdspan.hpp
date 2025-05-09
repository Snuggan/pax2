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

	/// Find what type of layout it is.
	template< typename Layout >	struct is_layout_right;
	template< typename Layout >	constexpr bool is_layout_right_v	= is_layout_right< Layout >::value;
	template<>	struct is_layout_right< std::layout_right >			: std::bool_constant< true >{};
	template<>	struct is_layout_right< std::layout_left >			: std::bool_constant< false >{};


	template< typename T > class mdmeta;
	template< typename T, typename Extents, typename Layout, typename Accessor >
	class mdmeta< std::mdspan< T, Extents, Layout, Accessor > > {
		using mdspan = std::mdspan< T, Extents, Layout, Accessor >;

		template< std::size_t Dim, typename Function >
		static constexpr void on_all_first_extents(
			const mdspan								md_,
			std::array< std::size_t, Extents::rank() >  & idx_,
			Function								 && f_
		) {
			if constexpr( Dim == first )				f_( md_, idx_ );
			else {	// We are not at the final Dim, so loop on.
				while( std::get< Dim >( idx_ ) < md_.extent( Dim ) ) {
					on_all_first_extents< Dim - inc >( md_, idx_, f_ );
					++std::get< Dim >( idx_ );
				}
			}
			std::get< Dim >( idx_ ) = 0;
		}

	public:
		static constexpr bool			is_right	  = is_layout_right_v< Layout >;
		static constexpr std::size_t 	rank		  = Extents::rank();
		static constexpr std::size_t 	first		  = is_right ? rank -1 :  0;
		static constexpr int			inc			  = is_right ?      -1 : +1;
		static constexpr std::size_t 	last		  = is_right ? 0 : rank - 1;

		/// What is the dimention "lesser" than DIM (either Dim - 1 or Extents::rank() - 1).
		template< std::size_t Dim >
		static constexpr std::size_t 	lesser		  = is_right	? ( Dim ? Dim : rank ) - 1
																	: ( Dim + 1 < rank ) ? ( Dim + 1 ) : 0;

		/// A recursive function to loop through a mdspan, chunk by chunk.
		/// f_ will be called for each contiguous lump, e.g. row. 
		template< typename Function >
		static constexpr void on_all_first_extents(
			const mdspan								md_,
			Function								 && f_
		) {
			std::array< std::size_t, rank >				idx{};
			on_all_first_extents< last >( md_, idx, std::forward< Function >( f_ ) );
		}
	};



	/// Rearranges the items of data_ so that they correspond to the destination extent.
	/// For consistency, data_.data() and srce_.data_handle() and dest_.data_handle() must all be the same.
	/// Obviously it requires data_.size >= max( srce_.size() dest_.size() ).
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
		if( srce_.empty() || dest_.empty() ) {				// If either is empty, there is nothing to copy...
			std::fill( data_.begin(), data_.end(), T{} );
			return;
		}

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
			
			/*	Two cases:
				1. If Dim == (Extent::rank() - 1 )*is_layout_right< Layout >: Whole sequences may be removed.
				2. Otherwise: loop through all extents, but when dimension Dim is in [first_, first_+qtity_)
				   that should be removed. 
			*/

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



	/// Returns an iterator to first item at index offset_ in dimension Dim of md_.
	/// Returns the default Strided_iterator if offset_ is to large.
	template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
		requires( ( Dim == 0 ) || ( Dim == Extents::rank() - 1 ) )	// Other dims are not sequencial.
	constexpr Strided_iterator< T > begin( 
		const std::mdspan< T, Extents, Layout, Accessor >		md_,
		const std::size_t 										offset_
	) {
		assert( md_.is_strided() );
		using meta = mdmeta< std::mdspan< T, Extents, Layout, Accessor > >;
		static constexpr std::size_t 	Lesser = meta::template lesser< Dim >;
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
		const char												delimiter_ = 0
	) {
		using meta = mdmeta< std::mdspan< T, Extents, Layout, Accessor > >;

		static const auto f = [ &out_, &delimiter_ ](
			const std::mdspan< T, Extents, Layout, Accessor >	md_,
			std::array< std::size_t, meta::rank >			  & idx_
		) {
			if constexpr( meta::rank > 2 )
				if( std::get< meta::first + meta::inc >( idx_ ) == 0 ) {
					if constexpr( meta::is_right ) {
						std::print( out_, "\n[" );
						for( std::size_t i{}; i<meta::rank-2; ++i )
							std::print( out_, "{}, ", idx_[ i ] );
						std::println( out_, "c, r]:" );
					} else {
						std::print( out_, "\n[r, c" );
						for( std::size_t i{ 2*!meta::is_right }; i<meta::rank; ++i )
							std::print( out_, ", {}", idx_[ i ] );
						std::println( out_, "]:" );
					}
				}
			pax::print( out_, &md_[ idx_ ], &md_[ idx_ ] + md_.extent( meta::first ), delimiter_ );
			std::println( out_ );
		};

		meta::on_all_first_extents( md_, f  );
	}

	template< typename T, typename Extents, typename Layout, typename Accessor >
	void print( 
		const std::mdspan< T, Extents, Layout, Accessor > md_, 
		const char												delimiter_ = 0
	) {	return print( std::cout, md_, delimiter_ );										}

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
	void print_meta( const std::mdspan< T, Extents, Layout, Accessor > md_ ) 
	{	print_meta( std::cout, md_ );												}

}	// namespace pax
