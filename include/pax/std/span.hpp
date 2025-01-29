//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"
#include <span>
#include <algorithm>


namespace pax {

	template< typename T >
	concept Not_character_array	  = Contiguous_elements< T > && !Character_array< T >;

	template< typename T >
	concept Not_string			  = Contiguous_elements< T > && !String< T >;

}


namespace std {

	/// Stream the contents of `sp_` to `dest_` in the form `[i0, i1, ...]`.
	/// - If T is a string type, each item is surrounded by '"'.
	template< typename Dest, typename T, std::size_t N >
	constexpr Dest & operator<<(
		Dest					  & dest_,
		const std::span< T, N >		sp_
	) {
		if constexpr( !pax::Character< T > ) {
			constexpr auto			delimit  = pax::String< T > ? "\", \"" : ", ";
			dest_ << '[';
			if constexpr( N > 0 ) if( sp_.size() > 0 ) {
				if constexpr( pax::String< T > )	dest_ << '"' << sp_.front();
				else								dest_ << sp_.front();
				if constexpr( N > 1 ) if( sp_.size() > 1 ) 
					for( const auto & item : sp_.template subspan< 1 >() ) 
						dest_ << delimit << item;
				if constexpr( pax::String< T > )	dest_ << '"';
			}
			dest_ << ']';
		} else if constexpr( N > 0 ) if( sp_.size() > 0 ) {
			dest_.write( sp_.data(), sp_.size() - !sp_.back() );
		}
		return dest_;
	}

	/// Check if all elements of `sp_` are equal to the corresponding elements of `v_`.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr bool operator==(
		const std::span< T, N >		sp_,
		V						 && v_
	) noexcept {
		using std::begin, std::end;
		return std::equal( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Lexicographically compare `sp_` with `v_` using three-way comparison.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr auto operator<=>(
		const std::span< T, N >		sp_,
		V						 && v_
	) noexcept {
		using std::begin, std::end;
		return std::lexicographical_compare_three_way( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Implement `get` for statically sized `std::span`.
	/// Negative `I` will acces `I + N`.
	template< std::ptrdiff_t I, typename T, std::size_t N >
		requires( ( N != std::dynamic_extent ) && ( ( ( I < 0 ) ? -I : I ) < N ) )
	[[nodiscard]] constexpr T & get( const std::span< T, N > v_ )  					noexcept	{
		return *( v_.data() + ( ( I >= 0 ) ? I : I + N ) );
	}

}	// namespace std



namespace pax {

	/// Returns a std::span.
	template< Not_character_array A >
	constexpr auto make_span( A && a_ ) 											noexcept {
		using std::begin, std::end;
		return std::span< Value_type_t< A >, extent_v< A > >{ begin( a_ ), end( a_ ) };
	}

	/// Returns a std::span.
	template< typename T, std::size_t N >
	constexpr auto make_span( T( & c_ )[ N ] ) 										noexcept {
		return std::span< std::remove_reference_t< T >, N >{ c_, N };
	}

	/// Returns a std::span.
	template< Character Ch, std::size_t N >
	constexpr auto make_span( Ch( & c_ )[ N ] ) 									noexcept {
		constexpr std::size_t 	sz = ( N == 0 ) ? 0u : N-1u;
		return std::span< std::remove_reference_t< Ch >, sz >{ c_, sz };
	}

	/// Returns a std::span.
	template< Character Ch >
	constexpr auto make_span( Ch * const & c_ ) 									noexcept {
		using std::size;
		return std::span< std::remove_reference_t< Ch > >{ c_, size( c_ ) };
	}

	/// Returns a std::span with const elements.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr auto make_const_span( const std::span< T, N > sp_ )		noexcept {
		if constexpr( std::is_const_v< T > )	return sp_;
		else									return std::span< const T, N >( sp_.data(), sp_.size() );
	}

	/// Returns a view with const elements.
	template< Not_character_array V >
	[[nodiscard]] constexpr auto make_const_span( V && v_ ) 						noexcept {
		using std::data, std::size;
		return std::span< const Value_type_t< V >, extent_v< V > >( data( v_ ), size( v_ ) );
	}

	/// Returns a dynamically sized std::span.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr auto make_dynamic_span( const std::span< T, N > sp_ )	noexcept {
		if constexpr( N == dynamic_extent )		return sp_;
		else									return std::span( sp_.data(), N );
	}

	/// Returns a dynamically sized std::span.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto make_dynamic_span( V && v_ ) 						noexcept {
		return make_dynamic_span( make_span( v_ ) );
	}



	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< typename V >
	[[nodiscard]] constexpr auto & back( V && v_ ) 							noexcept {
		using std::data, std::size;
		static_assert( extent_v< V > != 0, "back( v_ ) requires size( v_ ) > 0" );
		if constexpr ( extent_v< V > == dynamic_extent ) {
			assert( size( v_ ) && "back( v_ ) requires size( v_ ) > 0" );
			return *( data( v_ ) + size( v_ ) - 1 );
		} else {
			return *( data( v_ ) + ( extent_v< V > - 1 ) );
		}
	}


	/// Returns a dynamically sized span of the first i_ elements of v_.
	///	- If i_ > size( v_ ), span( v_ ) is returned.
	template< Not_string V >
	[[nodiscard]] constexpr auto first( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::data, std::size;
		return std::span( data( v_ ), std::min( i_, size( v_ ) ) );
	}

	/// Returns a statically sized span of the first I elements of v_.
	///	- There is an assert( I <= size( v_ ) ).
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto first( V const & v_ )								noexcept {
		using std::data, std::size;
		assert( I <= size( v_ ) && "first< I >( v_ ) requires I <= size( v_ )." );
		return std::span< Value_type_t< V >, I >{ data( v_ ), I };
	}

	/// Returns a statically sized span of the first I elements of v_.
	///	- If I > size( v_ ), a span of all v_ is returned.
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto first( V const & v_ ) 								noexcept {
		using std::data;
		static constexpr std::size_t	sz  = std::min( I, extent_v< V > );
		return std::span< Value_type_t< V >, sz >( data( v_ ), sz );
	}



	/// Returns a dynamically sized span of the last i_ elements of v_.
	///	- If i_ > size( v_ ), span( v_ ) is returned.
	template< Not_string V >
	[[nodiscard]] constexpr auto last( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::data, std::size;
		return ( i_ < size( v_ ) )	? std::span( data( v_ ) + size( v_ ) - i_, i_ )
									: std::span( v_ );
	}

	/// Returns a statically sized span of the last I elements of v_.
	///	- There is an assert( I <= size( v_ ) ).
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto last( V && v_ ) 									noexcept {
		using std::data, std::size;
		assert( I <= size( v_ ) && "last< I >( v_ ) requires I <= size( v_ )." );
		return std::span< Value_type_t< V >, I >( data( v_ ) + size( v_ ) - I, I );
	}

	/// Returns a statically sized span of the last I elements of v_.
	///	- If i_ > size( v_ ), a span of all v_ is returned.
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto last( V && v_ ) 									noexcept {
		using std::data;
		static constexpr std::size_t	sz = extent_v< V >;
		static constexpr std::size_t	offset = ( I < sz ) ? sz - I : 0u;
		return std::span< Value_type_t< V >, sz - offset >( data( v_ ) + offset, sz - offset );
	}



	/// Returns a span of all elements of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty span( end( v_ ) ) is returned.
	template< Not_string V >
	[[nodiscard]] constexpr auto not_first( 
		V					 && v_, 
		const std::size_t 		i_ = 1 
	) noexcept {
		using std::data, std::size;
		return ( i_ < size( v_ ) )	? std::span( data( v_ ) + i_, size( v_ ) - i_ )
									: std::span( data( v_ ) + size( v_ ), 0 );
	}

	/// Returns a statically sized span of all elements of v_ except the first I.
	///	- If I > size( v_ ), span< 0 >( end( v_ ) ) is returned.
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto not_first( V && v_ ) 								noexcept {
		using std::data;
		static constexpr std::size_t	offset = ( I < extent_v< V > ) ? I : extent_v< V >;
		static constexpr std::size_t	sz = extent_v< V > - offset;
		return std::span< Value_type_t< V >, sz >( data( v_ ) + offset, sz );
	}



	/// Returns a view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty view( begin( v_ ) ) is returned.
	template< Not_string V >
	[[nodiscard]] constexpr auto not_last( 
		V					 && v_, 
		const std::size_t 		i_ = 1 
	) noexcept {
		using std::data, std::size;
		return std::span( data( v_ ), ( i_ < size( v_ ) ) ? size( v_ ) - i_ : 0u );
	}

	/// Returns a statically sized span of all elements of v_ except the first I.
	///	- If I > size( v_ ), span< 0 >( begin( v_ ) ) is returned.
	template< std::size_t I, Not_character_array V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto not_last( V && v_ ) 								noexcept {
		using std::data;
		static constexpr std::size_t	sz = ( extent_v< V > > I ) ? extent_v< V > - I : 0;
		return std::span< Value_type_t< V >, sz >( data( v_ ), sz );
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
	}

	/// Returns a span of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< Not_string V >
	[[nodiscard]] constexpr auto subview( 
		V					 && v_, 
		const std::ptrdiff_t 	offset_, 
		const std::size_t 		size_ 
	) noexcept {
		using std::data, std::size;
		const auto 				offset = detail::subview_offset( offset_, size( v_ ) );
		return std::span( data( v_ ) + offset, std::min( size( v_ ) - offset, size_ ) );
	}

	/// Returns a statically sized span of Len elements of v_ starting with offset_.
	///	- If offset_ < 0, an offset_ += size( v_ ) is used (the offset is seen from the back), 
	///	- There is an assert( offset + Len <= size( v_ ).
	template< std::size_t Len, Not_character_array V >
		requires( ( Len != dynamic_extent ) )
	[[nodiscard]] constexpr auto subview(
		V					 && v_, 
		const std::ptrdiff_t 	offset_ 
	) noexcept {
		using std::size, std::data;
		const auto 				offset = detail::subview_offset( offset_, size( v_ ) );
		assert( offset + Len <= size( v_ )  && "subview< Len >( v_, offset_ ) requires offset_ + Len <= size( v_ )." );
		return std::span< Value_type_t< V >, Len >( data( v_ ) + offset, Len );
	}

	/// Returns a statically sized span of Len elements of v_ starting ay Offset.
	///	- If Offset < 0, an offset = Offset + size( v_ ) is used (the offset is seen from the back), 
	///	- If Offset + Len > size( v_ ), returns not_first< offset >( v_ ).
	template< std::ptrdiff_t Offset, std::size_t Len, Not_character_array V >	
		requires( ( Len != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto subview( V && v_ ) 								noexcept {
		using std::data;
		static constexpr auto 			offset = detail::subview_offset( Offset, extent_v< V > );
		static constexpr std::size_t 	sz = std::min( extent_v< V > - offset, Len );
		return std::span< Value_type_t< V >, sz >( data( v_ ) + offset, sz );
	}

	/// Returns a statically sized span of Len elements of v_ starting with Offset.
	///	- If Offset < 0, an offset = Offset + size( v_ ) is used (the offset is seen from the back), 
	///	- There is an assert( offset + Len <= size( v_ ).
	template< std::ptrdiff_t Offset, std::size_t Len, Not_character_array V >
		requires( ( Len != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto subview( V && v_ ) 								noexcept {
		using std::data, std::size;
		const auto 						offset = detail::subview_offset( Offset, size( v_ ) );
		assert( offset + Len <= size( v_ ) && "subview< Offset, Len >( v_ ) requires Offset + Len <= size( v_ )." );
		return std::span< Value_type_t< V >, Len >( data( v_ ) + offset, Len );
	}



	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< Contiguous_elements V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}

}	// namespace pax
