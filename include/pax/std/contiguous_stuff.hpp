//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include <cassert>		// assert
#include <algorithm>	// std::min, std::equal, std::lexicographical_compare_three_way


// From https://lemire.me/blog/2024/07/26/safer-code-in-c-with-lifetime-bounds/
#ifndef __has_cpp_attribute
	#define ada_lifetime_bound
#elif __has_cpp_attribute(msvc::lifetimebound)
	#define ada_lifetime_bound [[msvc::lifetimebound]]
#elif __has_cpp_attribute(clang::lifetimebound)
	#define ada_lifetime_bound [[clang::lifetimebound]]
#elif __has_cpp_attribute(lifetimebound)
	#define ada_lifetime_bound [[lifetimebound]]
#else
	#define ada_lifetime_bound
#endif


namespace pax {

	/// Returns false. 
	[[nodiscard]] constexpr bool valid( std::nullptr_t ) 	noexcept {
		return false;
	}

	/// Returns ptr_ != nullptr. 
	template< typename T >
	[[nodiscard]] constexpr bool valid( T * ptr_ ) 			noexcept {
		return ptr_ != nullptr;
	}

	/// Returns sp_.data() != nullptr. 
	template< Contiguous_elements V >
	[[nodiscard]] constexpr bool valid( const V & v_ ) 		noexcept {
		using std::data;
		return valid( data( v_ ) );
	}

	/// Return true iff both data() and size() are equal between the two std::spans.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool identic(
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return ( data( v0_ ) == data( v1_ ) ) && ( size( v0_ ) == size( v1_ ) );
	}

	/// Return true iff any element in v1_ by address is also an element in v2_.
	/// Cheap: 1 addition, 2 comparisons, and 2 bolean &&.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool overlap( 
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return	( ( data( v0_ ) > data( v1_ ) )	? ( data( v1_ ) + size( v1_ ) > data( v0_ ) ) 
												: ( data( v0_ ) + size( v0_ ) > data( v1_ ) )	)
			&&	size( v0_ ) && size( v1_ );		// An empty view cannot overlap.
	}

	/// Returns a reference to the first item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto & front( V && v_ ) noexcept {
		using std::data, std::size;
		static_assert( extent_v< V > != 0 );
		if constexpr ( extent_v< V > == dynamic_extent )
			assert( size( v_ ) && "front( v_ ) requires size( v_ ) > 0" );
		return *data( v_ );
	}



	namespace detail {
		template< std::integral Int >
		[[nodiscard]] static constexpr std::size_t subview_offset( 
			const Int			 			offset_, 
			const std::size_t 				size_ 
		) noexcept {
			if constexpr( std::is_unsigned_v< Int > ) {
				return	std::min( offset_, size_ );
			} else {
				return	( offset_ >= 0 )					  ? std::min( std::size_t( offset_ ), size_ ) 
					:	( std::size_t( -offset_ ) < size_ )	  ? size_ - std::size_t( -offset_ )
					:											std::size_t{};
			}
		}

		// Return true iff copying to dest_ requires backward copy.
		template< Contiguous_elements V, typename Itr >
		[[nodiscard]] constexpr bool requires_backward( 
			V			 && sp_,
			Itr 			dst_ 
		) noexcept {
			static constexpr bool same = std::is_same_v< 
				std::remove_cvref_t< Value_type_t< V > >, std::remove_cvref_t< Value_type_t< Itr > > >;
			if constexpr( same )	return ( dst_ >= sp_.begin() ) && ( dst_ < sp_.end() );
			else					return false;
		}

		template< Contiguous_elements V0, Contiguous_elements V1 >
			requires( ( extent_v< V0 > != dynamic_extent ) && ( extent_v< V1 > != dynamic_extent ) )
		constexpr void assert_equal_extent( const V0 &, const V1 & )			noexcept {
			static_assert( extent_v< V0 > == extent_v< V1 >, "V0 and V1 must have same static size." );
		}

		template< Contiguous_elements V0, Contiguous_elements V1 >
		constexpr void assert_equal_extent( [[maybe_unused]] const V0 & v0_, [[maybe_unused]] const V1 & v1_ )	noexcept {
			using std::size;
			assert( size( v0_ ) == size( v1_ ) && "v0_ and v1_ must have same size." );
		}
	}	// namespace detail

}	// namespace pax
