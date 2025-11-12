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
#include <numeric>		// std::accumulate
#include <functional>	// std::multiplies
#include <print>		// std::print
#include <iostream>		// std*::cout


namespace std {

	template< typename T0, typename T1, typename Extents0, typename Extents1, typename Layout, typename Accessor >
		requires( Extents0::rank() == Extents1::rank() )
	constexpr bool operator==( 
		const std::mdspan< T0, Extents0, Layout, Accessor >	md0_,
		const std::mdspan< T1, Extents1, Layout, Accessor >	md1_
	) {
		return ( md0_.extents() == md1_.extents() ) &&
			std::equal(	md0_.data_handle(), md0_.data_handle() + md0_.size(), 
						md1_.data_handle(), md1_.data_handle() + md1_.size() );
	}

}	// namespace std

namespace pax {

	/// Stream the header items to out_ using col_mark_ as column deligneater. 
	///
	template< typename Out, typename Itr >
	void print(
		Out							  & out_,
		Itr								itr_, 
		const Itr						end_, 
		const char						separator_ = 0
	) {
		// std::print( out_, "{}", pax::spanWr( itr_, end_ ) );
		const char						temp[]	  = { separator_, 0 };
		const char					  * separator =   separator_ ? temp : ", ";
		if( !separator_ )				std::print( out_, "[" );
		if( itr_ != end_ ) {
			std::print( out_, "{}", *itr_ );
			while( ++itr_ != end_ )		std::print( out_, "{}{}", separator, *itr_ );
		}
		if( !separator_ )				std::print( out_, "]" );
	}

	template< typename Itr, typename Ch = char >
	void print(
		Itr								itr_, 
		const Itr						end_, 
		const char 						separator_ = 0
	) {	return print( std::cout, itr_, end_, separator_ );				}



	/// Find what type of layout it is.
	template< typename Layout >	struct is_layout_right;
	template< typename Layout >	constexpr bool is_layout_right_v	= is_layout_right< Layout >::value;
	template<>	struct is_layout_right< std::layout_right >			: std::bool_constant< true >{};
	template<>	struct is_layout_right< std::layout_left >			: std::bool_constant< false >{};


	/// Return the extents as a std::array. 
	template< typename T, typename Extents, typename Layout, typename Accessor >
	static constexpr auto extents( const std::mdspan< T, Extents, Layout, Accessor > md_ ) {
		std::array< std::size_t, md_.rank() >			exts;
		for( std::size_t i{}; i<md_.rank(); ++i )		exts[ i ] = md_.extent( i );
		return exts;
	}

	template< typename T > class mdmeta;
	template< typename T, typename Extents, typename Layout, typename Accessor >
	class mdmeta< std::mdspan< T, Extents, Layout, Accessor > > {
		using Size					  = std::size_t;
		using mdspan				  = std::mdspan< T, Extents, Layout, Accessor >;

		template< Size Atdim, Size Dim, typename Function >
		static constexpr void on_all_first_extents(
			const mdspan								md_,
			std::array< Size, Extents::rank() >		  & idx_,
			Function								 && f_
		) {
			if constexpr( Atdim == Dim )				f_( md_, idx_ );
			else {	// We are not at the final Dim, so loop on.
				while( std::get< Dim >( idx_ ) < md_.extent( Dim ) ) {
					on_all_first_extents< Atdim, Dim - inc >( md_, idx_, f_ );
					++std::get< Dim >( idx_ );
				}
			}
			std::get< Dim >( idx_ ) = 0;
		}

	public:
		static constexpr bool			is_right	  = is_layout_right_v< Layout >;
		static constexpr Size 			rank		  = Extents::rank();
		static constexpr Size		 	first		  = is_right ? rank -1 :  0;
		static constexpr int			inc			  = is_right ?      -1 : +1;
		static constexpr Size		 	last		  = is_right ? 0 : rank - 1;

		/// What is the dimention "lesser" than DIM (either Dim - 1 or Extents::rank() - 1).
		template< Size Dim >
		static constexpr Size 			lesser		  = is_right	? ( Dim ? Dim : rank ) - 1
																	: ( Dim + 1 < rank ) ? ( Dim + 1 ) : 0;

		/// A recursive function to loop through a mdspan, chunk by chunk.
		/// f_ will be called for each contiguous lump, e.g. row. 
		template< Size Atdim, typename Function >
		static constexpr void on_all_first_extents(
			const mdspan								md_,
			Function								 && f_
		) {
			std::array< Size, rank >					idx{};
			on_all_first_extents< Atdim, last >( md_, idx, std::forward< Function >( f_ ) );
		}
	};


	/// A new (more efficient?) remove...
	template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
		requires( ( Extents::rank() > 0 ) && ( Dim < Extents::rank() ) )
	constexpr auto remove2(
		const std::mdspan< T, Extents, Layout, Accessor >	md_, 		///< The extents etc.
		const std::size_t 									first_, 	///< Index to items to remove.
		const std::size_t 									qtity_ = 1 	///< Number of additional indeces to remove.
	) -> std::mdspan< T, Extents, Layout, Accessor > {
		using Ptr						  = T*;
		using Size						  = std::size_t;
		using Mdspan					  = std::mdspan< T, Extents, Layout, Accessor >;
		using meta						  = mdmeta< Mdspan >;

		const Ptr	data				  = md_.data_handle();
		auto		exts				  = extents( md_ );
		assert( first_ + qtity_ <= exts[ Dim ] );			// Items to remove must be within rage. .
		assert( md_.is_strided() );							// Items must be stored contigously.
		if( md_.empty() )					return md_;		// If md_ is empty, there is nothing to do...

		exts[ Dim ]						 -= qtity_;
		const Mdspan						new_md( data, exts );

		if constexpr( Dim == meta::last ) {
			// Remove the last data of the last dim by just shortening the array.
			// ...we don't need to do anything, new_md already has the correct extents.
		} else if( first_ + qtity_ == md_.extent( Dim ) ) {
			// Remove the last data of dim Dim by just resizeing.
			resize(	md_, new_md );
		} else {
			// The general case, that entails lots of iterations.
			std::array< Size, meta::rank >	index{};
			index[ Dim ]				  = first_;
			Ptr			dest			  = &md_[ index ];
			index[ Dim ]				  = first_ + 1;
			Ptr			srce			  = &md_[ index ];
			const Size	small_step		  = srce - dest;
			const Ptr	end				  = data + md_.size();

			const Size	step			  = md_.stride( Dim );
			const Size	small_chunk		  = ( srce - dest )/qtity_;
			while( srce < end ) {
				for( auto i = qtity_ + 1; --i > 0; ) {
					std::copy_n( srce, small_chunk, dest );
					srce				 += small_step;
					dest				 += small_step;	// Wrong! Must include general tightening.
				}
				srce					 += step;
				dest					 += step;
			}
		}
		return new_md;
	}



	/// Rearranges the items of srce_ so that they correspond to the destination extent.
	template< typename T, typename Extents, typename Layout, typename Accessor >
		requires( Extents::rank() >  0 )
	constexpr void resize(
		const std::mdspan< T, Extents, Layout, Accessor >	srce_, 	///< The source extents.
		const std::mdspan< T, Extents, Layout, Accessor >	dest_ 	///< The destination extents. 
	) {
		using Ptr						 = T*;
		using Size						 = std::size_t;
		using meta						 = mdmeta< std::mdspan< T, Extents, Layout, Accessor > >;
	
		// Items must be stored contigously.
		static_assert( std::mdspan< T, Extents, Layout, Accessor >::is_always_strided() );
		assert( srce_.is_strided() && dest_.is_strided() );

		const Ptr			srce_data	 = srce_.data_handle();
		const Ptr			dest_data	 = dest_.data_handle();

		if( srce_.empty() || dest_.empty() ) {	// If either is empty, there is nothing to copy...
			std::fill( dest_data, dest_data + srce_.size(), T{} );
		} else {
			const Size		srce_step	 = srce_.extent( meta::first );
			const Size		dest_step	 = dest_.extent( meta::first );
			const Size		iters		 = std::min( srce_.size()/srce_step, dest_.size()/dest_step );

			if( srce_step < dest_step ) {				// Expand: copy from last to first.
				Ptr			srce		 = srce_data + srce_step*iters;
				Ptr			dest		 = dest_data + dest_step*iters;
				const Size	remains		 = dest_step - srce_step;
				while( srce	!= srce_data ) {
					std::copy_n( srce	-= srce_step, srce_step, dest -= dest_step );
					std::fill_n( dest	+  srce_step, remains, T{} );
				}
			} else if( srce_step > dest_step ) {		// Shrink: copy from first to last.
				Ptr			srce		 = srce_data;
				Ptr			dest		 = dest_data;
				const Ptr	srce_end	 = srce_data + srce_step*iters;
				while( srce	!= srce_end ) {
					std::copy_n( srce, dest_step, dest );
					srce				+= srce_step;
					dest				+= dest_step;
				}
			}
			if( dest_step*iters < dest_.size() )		// Zero-out trailing values, if any.
				std::fill_n( dest_data + dest_step*iters, dest_.size() - dest_step*iters, T{} );
		}
	}



	/// Rearranges the items of srce_ so that they correspond to the destination extent.
	template< typename T, typename Extents, typename Layout, typename Accessor, std::unsigned_integral UI >
		requires( Extents::rank() >  0 )
	constexpr void resize(
		const std::mdspan< T, Extents, Layout, Accessor >	srce_, 	///< The source extents.
		const std::span< UI, Extents::rank() >				exts_ 	///< The new extents. 
	) {
		return resize(
			srce_,
			std::mdspan< T, Extents, Layout, Accessor >( srce_.data_handle(), exts_ )
		);
	}



	/// Rearranges the items of data_ so that size of extension first_ is decreased by qtity_.
	template< std::size_t Dim, typename T, typename Extents, typename Layout, typename Accessor >
		requires( ( Extents::rank() > 0 ) && ( Dim < Extents::rank() ) )
	constexpr auto remove(
		const std::mdspan< T, Extents, Layout, Accessor >	md_, 		///< The extents etc.
		const std::size_t 									first_, 	///< Index to items to remove.
		const std::size_t 									qtity_ = 1 	///< Number of additional indeces to remove.
	) {
		using Ptr						  = T*;
		using Size						  = std::size_t;
		using Mdspan					  = std::mdspan< T, Extents, Layout, Accessor >;
		using meta						  = mdmeta< Mdspan >;
		
		const Ptr	data				  = md_.data_handle();
		auto		exts				  = extents( md_ );
		assert( first_ + qtity_ <= exts[ Dim ] );			// Items to remove must be within rage. .
		assert( md_.is_strided() );							// Items must be stored contigously.
		if( md_.empty() )					return md_;		// If md_ is empty, there is nothing to do...

		exts[ Dim ]						 -= qtity_;
		const Mdspan						new_md( data, exts );

		if( md_.size() && ( first_ < md_.extent( Dim ) ) ) {
			if constexpr( Dim == meta::last ) {
				// Remove the last data of the last dim by just shortening the array.
				// ...we don't need to do anything, new_md already has the correct extents.
			} else if( first_ + qtity_ == md_.extent( Dim ) ) {
				// Remove the last data of dim Dim by just resizeing.
				resize(	md_, new_md );
			} else {
				// Remove the data by iteration and copy.
				Ptr			srce		  = data;
				const Size	srce_step	  = md_.extent( is_layout_right_v< Layout >*( Extents::rank() - 1 ) );
				const Size	iters		  = md_.size()/srce_step;
				Ptr			dest		  = data;
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
				std::fill_n( dest, data + md_.size() - dest, T{} );
			}
		}
		return new_md;
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
		return ( offset_ < md_.extent( Dim ) ) 
			? Strided_iterator< T >( md_.data_handle() + offset_*md_.stride( Dim ), md_.stride( meta::template lesser< Dim > ) )
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
	constexpr void print( 
		Out													  & out_, 
		const std::mdspan< T, Extents, Layout, Accessor >		md_, 
		const char												delimiter_ = 0
	) {
		using meta = mdmeta< std::mdspan< T, Extents, Layout, Accessor > >;

		// Due to rules of lambda capture, the next line must not be static.
		const auto f = [ &out_, delimiter_ ](
			const std::mdspan< T, Extents, Layout, Accessor >	md_,
			std::array< std::size_t, meta::rank >			  & idx_
		) {
			if constexpr( meta::rank > 2 )
				if( std::get< meta::first + meta::inc >( idx_ ) == 0 ) {
					constexpr std::size_t 	end	= meta::rank - ( meta::is_right ? 2 : 0 );
					std::print(   out_, meta::is_right ? "\n[" : "\n[r, c" );
					for( std::size_t i = meta::is_right ? 0 : 2; i < end; ++i )
						std::print( out_, meta::is_right ? "{}, " : ", {}", idx_[ i ] );
					std::println( out_, meta::is_right ? "c, r]:" : "]:" );
				}
			pax::print( out_, &md_[ idx_ ], &md_[ idx_ ] + md_.extent( meta::first ), delimiter_ );
			std::println( out_ );
		};
		meta::template on_all_first_extents< meta::first >( md_, f );
	}

	template< typename T, typename Extents, typename Layout, typename Accessor >
	constexpr void print( 
		const std::mdspan< T, Extents, Layout, Accessor > md_, 
		const char												delimiter_ = 0
	) {	return print( std::cout, md_, delimiter_ );										}

	template< typename Out, typename T, typename Extents, typename Layout, typename Accessor >
	constexpr void print_meta( 
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
	constexpr void print_meta( const std::mdspan< T, Extents, Layout, Accessor > md_ ) 
	{	print_meta( std::cout, md_ );												}

}	// namespace pax
