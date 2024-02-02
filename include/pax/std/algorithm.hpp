//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "span2.hpp"
#include "string_view2.hpp"
#include <algorithm>			// min, find, find_if, search, ...


namespace pax {

	template< Contiguous_elements C, bool Dynamic = false >
	struct view_type {
		using type = span2< Value_type_t< C >, Dynamic ? std::dynamic_extent : extent_v< C > >;
	};

	template< typename T, std::size_t N, bool Dynamic >
	struct view_type< span2< T, N >, Dynamic > {
		using type = span2< T, Dynamic ? std::dynamic_extent : N >;
	};

	template< String S, bool Dynamic >
	struct view_type< S, Dynamic > {
		using type = basic_string_view2< std::remove_cvref_t< Value_type_t< S > > >;
	};

	template< String S, bool Dynamic >
		requires( std::remove_cvref_t< S >::traits_type )
	struct view_type< S, Dynamic > {
		using Ch = std::remove_cvref_t< Value_type_t< S > >;
		using Tr = typename std::remove_cvref_t< S >::traits_type;
		using type = basic_string_view2< Ch, Tr >;
	};

	template< typename T, bool Dynamic >
	struct view_type< const T, Dynamic > : view_type< T, Dynamic > {};

	template< Contiguous_elements C, bool Dynamic = false >
	using view_type_t = typename view_type< C, Dynamic >::type;


	/// Turns a Contiguous_elements into its view type. 
	template< Contiguous_elements A >
	constexpr auto view( A && a_ )				noexcept	{
		return view_type_t< A >( a_ );
	}

	/// Turns an array of characters into a string view. Does not include a final '\0' character.
	template< Character Ch, std::size_t N >	
	constexpr auto view( Ch( & c_ )[ N ] )		noexcept	{
		return view_type_t< Ch( & )[ N ] >( c_, ( N == 0 ) ? 0u : N - !( *( c_ + N - 1 ) ) );
	}

	/// Turns a Contiguous_elements into its view type. 
	template< Contiguous_elements A >
	constexpr auto dview( A && a_ )				noexcept	{
		using std::size, std::data;
		return view_type_t< A, true >( a_ );
	}

	/// Turns an array of characters into a string view. Does not include a final '\0' character.
	template< Character Ch, std::size_t N >		// To keep constness, ampersand is necessary:
	constexpr auto dview( Ch( & c_ )[ N ] )		noexcept	{
		return view_type_t< Ch( & )[ N ], true >( c_, ( N == 0 ) ? 0u : N - !( *( c_ + N - 1 ) ) );
	}




	namespace detail {
		[[nodiscard]] inline constexpr std::size_t subview_offset( 
			const std::ptrdiff_t 			offset_, 
			const std::size_t 				size_ 
		) noexcept {
			return	(  offset_ >= 0 )					  ? std::min( std::size_t( offset_ ), size_ ) 
				:	( std::size_t( -offset_ ) < size_ )	  ? size_ + offset_ 
				:											std::size_t{};
		}

		// Return true iff copying to dest_ requires backward copy.
		template< typename T, std::size_t N, typename Itr >
		[[nodiscard]] constexpr bool requires_backward( 
			const span2< T, N >				sp_,
			Itr 							dst_ 
		) noexcept {
			constexpr bool same = std::is_same_v< std::remove_cvref_t< T >, std::remove_cvref_t< Value_type_t< Itr > > >;
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

		constexpr auto is_newline = []( const unsigned c ) noexcept {	return ( c == '\n' ) || ( c == '\r' );	};
	}	// namespace detail


	struct Newline {};




	/// Returns sp_.data() != nullptr. 
	template< Contiguous_elements V >
	[[nodiscard]] constexpr bool valid( const V & v_ ) noexcept {
		using std::data;
		return data( v_ ) != nullptr;
	}

	/// Returns a span2 with const elements.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr auto as_const( const span2< T, N > sp_ ) noexcept {
		if constexpr( std::is_const_v< T > )	return sp_;
		else									return span2< const T, N >( sp_.data(), sp_.size() );
	}

	/// Returns a view with const elements.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto as_const( V && v_ ) noexcept {
		using std::data, std::size;
		if constexpr( std::is_const_v< Value_type_t< V > > )
			return view( v_ );
		else
			return span2< const Value_type_t< V >, extent_v< V > >( data( v_ ), size( v_ ) );
	}

	/// Returns a dynamically sized span2.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr auto as_dynamic( const span2< T, N > sp_ ) noexcept {
		if constexpr( N == dynamic_extent )			return sp_;
		else										return span2( sp_.data(), N );
	}

	/// Returns a dynamically sized span2.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto as_dynamic( V && v_ ) noexcept {
		return as_dynamic( span2( v_ ) );
	}

	/// Return true iff both data() and size() are equal between the two span2s.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool identic(
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return ( data( v0_ ) == data( v1_ ) )
			&& ( size( v0_ ) == size( v1_ ) );
	}

	/// Return true iff any part of v1_ overlaps any part of v2_.
	/// Cheap: 1 addition, 2 comparisons, and 2 bolean &&.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr bool overlap( 
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return	( ( data( v0_ ) > data( v1_ ) )	? ( data( v1_ ) + size( v1_ ) > data( v0_ ) ) 
												: ( data( v0_ ) + size( v0_ ) > data( v1_ ) )	)
			&&	size( v0_ ) && size( v1_ );	// An empty view cannot overlap.
	}




	/// Returns a reference to the first item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< typename V >
	[[nodiscard]] constexpr auto & front( const V & v_ ) noexcept {
		using std::data, std::size;
		if constexpr( Character_array< V > )
			return front( view( v_ ) );
		else {
			static_assert( extent_v< V > != 0 );
			if constexpr ( extent_v< V > == dynamic_extent )
				assert( size( v_ ) && "front( v_ ) requires size( v_ ) > 0" );
			return *data( v_ );
		}
	}

	/// Returns a reference to the last item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< typename V >
	[[nodiscard]] constexpr auto & back( const V & v_ ) noexcept {
		using std::data, std::size;
		if constexpr( Character_array< V > )
			return back( view( v_ ) );
		else {
			static_assert( extent_v< V > != 0 );
			if constexpr ( extent_v< V > == dynamic_extent )
				assert( size( v_ ) && "back( v_ ) requires size( v_ ) > 0" );
			return *( data( v_ ) + size( v_ ) - 1 );
		}
	}


	/// Returns a dynamically sized view of the first i_ elements of v_.
	///	- If i_ > size( v_ ), dview( v_ ) is returned.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto first( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::size;
		if constexpr( Character_array< V > )
				return first( view( v_ ), i_ );
		else	return view_type_t< V, true >( data( v_ ), std::min( i_, size( v_ ) ) );
	}

	/// Returns a statically sized span of the first I elements of v_.
	///	- If i_ > size( v_ ), a span of all v_ is returned.
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto first( V && v_ ) noexcept {
		using std::data;
		static constexpr std::size_t 	sz = std::min( I, extent_v< V > );
		if constexpr( Character_array< V > )
				return first< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, sz >( data( v_ ), sz );
	}

	/// Returns a statically sized span of the first I elements of v_.
	///	- There is an assert( I <= size( v_ ) ).
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto first( V && v_ ) noexcept {
		using std::data, std::size;
		assert( I <= size( v_ ) && "first< I >( v_ ) requires I <= size( v_ )." );
		if constexpr( Character_array< V > )
				return first< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, I >( data( v_ ), I );
	}


	/// Returns a dynamically sized span of the last i_ elements of v_.
	///	- If i_ > size( v_ ), dview( v_ ) is returned.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto last( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::data, std::size;
		if constexpr( Character_array< V > )
				return last( view( v_ ), i_ );
		else	return ( i_ < size( v_ ) )	? view_type_t< V, true >( data( v_ ) + size( v_ ) - i_, i_ )
											: view_type_t< V, true >( v_ );
	}

	/// Returns a statically sized span of the last I elements of v_.
	///	- If i_ > size( v_ ), a span of all v_ is returned.
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto last( V && v_ ) noexcept {
		using std::data;
		static constexpr std::size_t	sz = extent_v< V >;
		static constexpr std::size_t	offset = ( I < sz ) ? sz - I : 0u;
		if constexpr( Character_array< V > )
				return last< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, sz - offset >( data( v_ ) + offset, sz - offset );
	}

	/// Returns a statically sized span of the last I elements of v_.
	///	- There is an assert( I <= size( v_ ) ).
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto last( V && v_ ) noexcept {
		using std::data, std::size;
		assert( I <= size( v_ ) && "last< I >( v_ ) requires I <= size( v_ )." );
		if constexpr( Character_array< V > )
				return last< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, I >( data( v_ ) + size( v_ ) - I, I );
	}


	/// Returns a view of all elements of v_ but the first i_.
	///	- If i_ > size( v_ ), an empty view( end( v_ ) ) is returned.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto not_first( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::data, std::size, std::end;
		if constexpr( Character_array< V > )
				return not_first( view( v_ ), i_ );
		else	return ( i_ < size( v_ ) )	? view_type_t< V, true >( data( v_ ) + i_, size( v_ ) - i_ )
											: view_type_t< V, true >( data( v_ ) + size( v_ ), 0 );
	}

	/// Returns a statically sized span of all elements of v_ except the first I.
	///	- If I > size( v_ ), span< 0 >( end( v_ ) ) is returned.
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto not_first( V && v_ ) noexcept {
		using std::data;
		static constexpr std::size_t		offset = ( I < extent_v< V > ) ? I : extent_v< V >;
		static constexpr std::size_t		sz = extent_v< V > - offset;
		if constexpr( Character_array< V > )
				return not_first< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, sz >( data( v_ ) + offset, sz );
	}


	/// Returns a view of all elements of v_ except the last i_.
	///	- If i_ > size( v_ ), an empty view( begin( v_ ) ) is returned.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto not_last( 
		V								 && v_, 
		const std::size_t 					i_ = 1 
	) noexcept {
		using std::data, std::size;
		if constexpr( Character_array< V > )
				return not_last( view( v_ ), i_ );
		else	return view_type_t< V, true >( data( v_ ), ( i_ < size( v_ ) ) ? size( v_ ) - i_ : 0u );
	}

	/// Returns a statically sized span of all elements of v_ except the first I.
	///	- If I > size( v_ ), span< 0 >( begin( v_ ) ) is returned.
	template< std::size_t I, Contiguous_elements V >
		requires( ( I != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto not_last( V && v_ ) noexcept {
		using std::data;
		static constexpr std::size_t		sz = ( extent_v< V > > I ) ? extent_v< V > - I : 0;
		if constexpr( Character_array< V > )
				return not_last< I >( view( v_ ) );
		else	return span2< Value_type_t< V >, sz >( data( v_ ), sz );
	}


	/// Returns a view of `size_` elements in `v_` starting with `offset_`.
	///	- If `offset_ < 0`, `offset_ += size( v_ )` is used (the offset is seen from the back), 
	///	- If `offset_ + size_ >= size( v_ )`: returns `not_first( v_, offset_ )`.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto subview( 
		V								 && v_, 
		const std::ptrdiff_t 				offset_, 
		const std::size_t 					size_ 
	) noexcept {
		using std::data, std::size;
		if constexpr( Character_array< V > )
			return subview( view( v_ ), offset_, size_ );
		else {
			const auto 						offset = detail::subview_offset( offset_, size( v_ ) );
			return view_type_t< V, true >( data( v_ ) + offset, std::min( size( v_ ) - offset, size_ ) );
		}
	}

	/// Returns a statically sized span of Len elements of v_ starting with offset_.
	///	- If offset_ < 0, an offset_ += size( v_ ) is used (the offset is seen from the back), 
	///	- There is an assert( offset + Len <= size( v_ ).
	template< std::size_t Len, Contiguous_elements V >
		requires( Len != dynamic_extent )
	[[nodiscard]] constexpr auto subview(
		V								 && v_, 
		const std::ptrdiff_t 				offset_ 
	) noexcept	{
		using std::size, std::data;
		if constexpr( Character_array< V > )
			return subview< Len >( view( v_ ), offset_ );
		else {
			const auto 							offset = detail::subview_offset( offset_, size( v_ ) );
			assert( offset + Len <= size( v_ )  && "subview< Len >( v_, offset_ ) requires offset_ + Len <= size( v_ )." );
			return span2< Value_type_t< V >, Len >( data( v_ ) + offset, Len );
		}
	}

	/// Returns a statically sized span of Len elements of v_ starting ay Offset.
	///	- If Offset < 0, an offset = Offset + size( v_ ) is used (the offset is seen from the back), 
	///	- If Offset + Len > size( v_ ), returns not_first< offset >( v_ ).
	template< std::ptrdiff_t Offset, std::size_t Len, Contiguous_elements V >	
		requires( ( Len != dynamic_extent ) && ( extent_v< V > != dynamic_extent ) )
	[[nodiscard]] constexpr auto subview( V && v_ ) noexcept {
		using std::data;
		constexpr auto 						offset = detail::subview_offset( Offset, extent_v< V > );
		constexpr std::size_t 				sz = std::min( extent_v< V > - offset, Len );
		if constexpr( Character_array< V > )
				return subview< Offset, Len >( view( v_ ) );
		else	return span2< Value_type_t< V >, sz >( data( v_ ) + offset, sz );
	}

	/// Returns a statically sized span of Len elements of v_ starting with Offset.
	///	- If Offset < 0, an offset = Offset + size( v_ ) is used (the offset is seen from the back), 
	///	- There is an assert( offset + Len <= size( v_ ).
	template< std::ptrdiff_t Offset, std::size_t Len, Contiguous_elements V >
		requires( ( Len != dynamic_extent ) && ( extent_v< V > == dynamic_extent ) )
	[[nodiscard]] constexpr auto subview( V && v_ ) noexcept {
		using std::data;
		if constexpr( Character_array< V > )
			return subview< Offset, Len >( view( v_ ) );
		else {
			const auto 						offset = detail::subview_offset( Offset, size( v_ ) );
			assert( offset + Len <= size( v_ ) && "subview< Offset, Len >( v_ ) requires Offset + Len <= size( v_ )." );
			return span2< Value_type_t< V >, Len >( data( v_ ) + offset, Len );
		}
	}




	/// Returns the offset to the first occurence of sp1_ in sp0_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V0, Contiguous_elements V1 >
	[[nodiscard]] constexpr std::size_t find(  
		const V0						  & v0_, 
		const V1						  & v1_
	) noexcept {
		using std::begin, std::end;
		if constexpr( Character_array< V1 > )
				return find( v0_, string_view2( v1_ ) );
		else	return std::search( begin( v0_ ), end( v0_ ), begin( v1_ ), end( v1_ ) ) - begin( v0_ );
	}

	/// Returns the offset to the first occurence of t_ in v_.
	/// - Returns v_.size() if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		using std::begin, std::end;
		return std::find( begin( v_ ), end( v_ ), t_ ) - begin( v_ );
	}

	/// Returns the offset to v for the first true occurence of pred_( v ) in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		using std::begin, std::end;
		return std::find_if( begin( v_ ), end( v_ ), pred_ ) - begin( v_ );
	}
	
	/// Returns the offset to the first occurence of either `'\n'` or `'\r'` in `view_`.
	/// - Returns `view_.size()` if there is no such.
	template< String V >
	[[nodiscard]] constexpr std::size_t find(  
		const V							  & v_, 
		Newline	
	) noexcept {
		return find( v_, detail::is_newline );
	}


	/// Returns the offset to the last occurence of t_ in v_.
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		const Value_type_t< V >			  & t_ 
	) noexcept {
		using std::size, std::rbegin, std::rend;
		const auto res = std::find( rbegin( v_ ), rend( v_ ), t_ );
		return ( res == rend( v_ ) ) ? size( v_ ) : rend( v_ ) - res - 1;
	}

	/// Returns the offset for the last true occurence of pred_( ... ).
	/// - Returns size( v_ ) if there is no such.
	template< Contiguous_elements V, typename Pred >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr std::size_t rfind(  
		const V							  & v_, 
		Pred							 && pred_ 
	) noexcept {
		using std::size, std::rbegin, std::rend;
		const auto res = std::find_if( rbegin( v_ ), rend( v_ ), pred_ );
		return ( res == rend( v_ ) ) ? size( v_ ) : rend( v_ ) - res - 1;
	}


	/// Returns true iff find( v_, x_ ) < size( v_ ).
	template< Contiguous_elements V, typename X >
	[[nodiscard]] constexpr bool contains(  
		const V							  & v_, 
		X								 && x_ 
	) noexcept {
		using std::size;
		return find( v_, x_ ) < size( v_ );
	}


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< Contiguous_elements V, typename U >
	constexpr auto until(  
		V								 && v_, 
		U								 && until_this_ 
	) noexcept {
		using std::data;
		if constexpr( Character_array< V > )
				return view_type_t< V, true >( data( v_ ), find( view( v_ ), until_this_ ) );
		else	return view_type_t< V, true >( data( v_ ), find( v_, until_this_ ) );
	}




	/// Returns the size of `v_` if the beginning of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V, Contiguous_elements U >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		U								 && u_
	) noexcept {
		using std::begin, std::end;
		if constexpr( Character_array< U > ) 
				return starts_with( v_, view_type_t< U, true >( u_  ) );	// To remove possible trailing '\0'.
		else	return ( size( v_ ) < size( u_ ) )
					?	0 : std::equal( begin( v_ ), begin( v_ ) + size( u_ ), begin( u_ ), end( u_ ) )
					?	size( u_ ) : 0u;
	}

	/// Returns 1, if the beginning of `view_` is `ch_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		using std::size;
		return size( v_ ) && ( front( v_ ) == t_ );
	}

	/// Returns 2 if `view_` starts with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t starts_with(  
		const V							  & v_, 
		Newline 
	) noexcept {
		if( size( v_ ) ) {
			if constexpr( extent_v< V > == 1 ) {
				return ( v_[ 0 ] == '\n' ) || ( v_[ 0 ] == '\r' );
			} else if constexpr( extent_v< V > > 1 ) {
				switch( front( v_ ) ) {
					case '\n': return 1 + ( ( size( v_ ) > 1 ) && ( v_[ 1 ] == '\r' ) );
					case '\r': return 1 + ( ( size( v_ ) > 1 ) && ( v_[ 1 ] == '\n' ) );
				}
			}
		}
		return 0;
	}


	/// Returns the size of `v_`, if the end of `view_` is lexicographical equal to `v_` and 0 otherwise.
	template< Contiguous_elements V, Contiguous_elements U >
	[[nodiscard]] constexpr std::size_t ends_with( 
		const V							  & v_, 
		U								 && u_
	) noexcept {
		using std::begin, std::end;
		if constexpr( Character_array< V > ) 
				return ends_with( view_type_t< V, true >( v_ ), u_ );	// To remove possible trailing '\0'.
		else if constexpr( Character_array< U > ) 
				return ends_with( v_, view_type_t< U, true >( u_ ) );	// To remove possible trailing '\0'.
		else	return ( size( v_ ) < size( u_ ) )
				?	0 : std::equal( end( v_ ) - size( u_ ), end( v_ ), begin( u_ ), end( u_ ) )
				?	size( u_ ) : 0u;
	}

	/// Returns 1, if the end of `view_` is `ch_` and 0 otherwise.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		using std::size;
		return size( v_ ) && ( back( v_ ) == t_ );
	}

	/// Returns 2 if `view_` ends with `"\n\r"` or `"\r\n"`; 1 if `'\n'` or `'\r'`; and 0 otherwise.
	template< String V >
	[[nodiscard]] constexpr std::size_t ends_with(  
		const V							  & v_, 
		Newline 
	) noexcept {
		if( size( v_ ) ) {
			if constexpr( Character_array< V > ) 
				return ends_with( view_type_t< V, true >( v_ ), Newline{} );	// To remove possible trailing '\0'.
			else if constexpr( extent_v< V > == 1 ) {
				return ( v_[ 0 ] == '\n' ) || ( v_[ 0 ] == '\r' );
			} else if constexpr( extent_v< V > > 1 ) {
				switch( back( v_ ) ) {
					case '\n': return 1 + ( ( size( v_ ) > 1 ) && ( v_[ size( v_ ) - 2u ] == '\r' ) );
					case '\r': return 1 + ( ( size( v_ ) > 1 ) && ( v_[ size( v_ ) - 2u ] == '\n' ) );
				}
			}
		}
		return 0;
	}




	/// Split v_ into two parts: before and after (but not including) the `at_`:th element.
	/// - If `at_ >= size( v_ )`, then `{ v_, last( v_, 0 ) }` is returned.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto split_at( 
		const V							  & v_, 
		const std::size_t 					at_ 
	) noexcept {
		using std::size;
		const auto	i = std::min( at_, size( v_ ) );
		return std::pair{ first( v_, i ), not_first( v_, i + ( i < size( v_ ) ) ) };
	}

	/// Split the v_ into two parts: before and after (but not including) `by_`.
	/// - If `begin( by_ ) <= begin( v_ )`, the first string view returned is `first( v_, 0 )`.
	///	- If `end( by_ )   >= end( v_ )`,   the second string view returned is `last( v_, 0 )`.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V, Contiguous_elements By >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		By								 && by_
	) noexcept {
		using std::data, std::size;
		const std::size_t 					i  = find( v_, by_ );
		const std::size_t 					sz = ( i >= size( view( v_ ) ) ) ? 0 : size( view( by_ ) );
		return std::pair{ first( view( v_ ), i ), not_first( view( v_ ), i + sz ) };
	}

	/// Split `view_` into two parts: before and after the first `x_`, not including it.
	///	- `X` may be anything that works with `find( view_, x_ )`.
	/// Returns a pair of [non-owning] string views into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto split_by( 
		const V							  & v_, 
		const Value_type_t< V >				item_ 
	) noexcept {
		using std::size;
		const auto		frst = first( v_, find( v_, item_ ) );
		return std::pair{ frst, not_first( v_, size( frst ) + ( size( frst ) != size( v_ ) ) ) };
	}

	/// Split into two parts: before and after the first newline (`'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`), but not including it.
	/// Returns a pair of [non-owning] string views into v_.
	template< String V >
	[[nodiscard]] constexpr auto split_by(
		const V							  & v_, 
		Newline 
	) noexcept {
		const std::size_t 	i = find( v_, detail::is_newline );
		const auto			scnd = not_first( v_, i );
		return std::pair{ first( v_, i ), not_first( scnd, starts_with( scnd, Newline{} ) ) };
	}




	/// Returns `v_`, possibly excluding a leading `ch_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_front( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		return not_first( v_, starts_with( v_, t_ ) );
	}

	/// Returns `v_` possibly excluding a trailing `ch_`. 
	/// Returns a [non-owning] string view into v_.
	template< Contiguous_elements V >
	[[nodiscard]] constexpr auto trim_back( 
		const V							  & v_, 
		const Value_type_t< V >				t_ 
	) noexcept {
		return not_last( v_, ends_with( v_, t_ ) );
	}


	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		Pred							 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto								itr = begin( v_ );
		while( ( itr != end( v_ ) ) && p_( *itr ) )		++itr;
		return view_type_t< V, true >{ itr, end( v_ ) };
	}

	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_first( 
		const V							  & v_, 
		Newline 
	) noexcept {
		return not_first( v_, starts_with( v_, Newline{} ) );
	}


	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		Pred							 && p_ 
	) noexcept {
		using std::begin, std::end;
		auto								itr  = end( v_ );
		const auto							end_ = begin( v_ ) - 1;
		while( ( --itr != end_ ) && p_( *itr ) );
		return view_type_t< V, true >{ begin( v_ ), itr + 1 };
	}

	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< String V >
	[[nodiscard]] constexpr auto trim_last( 
		const V							  & v_, 
		Newline 
	) noexcept {
		return not_last( v_, ends_with( v_, Newline{} ) );
	}


	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< typename Pred, Contiguous_elements V >
		requires( std::predicate< Pred, Value_type_t< V > > )
	[[nodiscard]] constexpr auto trim( 
		const V							  & v_, 
		Pred							 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
	}




	///	Sorts the elements in v in non-descending order.
	///	- The order of equal elements is not guaranteed to be preserved.
	/// - https://en.cppreference.com/w/cpp/algorithm/sort
	template< Contiguous_elements V >
	constexpr void sort( V & v_ ) {
		using std::begin, std::end;
		if constexpr( Character_array< V > )
				sort( view( v_ ) );
		else	std::sort( begin( v_ ), end( v_ ) );
	}




	/// A tool to apply binary_ on all pairs of items from v0_/v1_. 
	/// v0_ and v1_ must have the same same size.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_v< Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr void on_each_pair(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		using std::begin;
		if constexpr( Character_array< V0 > ) 
			return on_each_pair( view_type_t< V0, true >( v0_ ), v1_, binary_ );
		else if constexpr( Character_array< V1 > ) 
			return on_each_pair( v0_, view_type_t< V1, true >( v1_ ), binary_ );
		else {
			detail::assert_equal_extent( v0_, v1_ );
			auto							i1 = begin( v1_ );
			for( auto & item : v0_ ) 		binary_( item, *( i1++ ) );
		}
	}

	/// Applies the function f_ on all elements while f_ returns true.
	/** - Returns true iff all pairs are iterated. 
		- Terminates unless v0_.size() == v1_.size().
		- This could be used to mutate elements or aggregate/count in different ways.
	**/
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	constexpr bool on_each_pair_while(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		using std::begin, std::end;
		if constexpr( Character_array< V0 > ) 
			return on_each_pair_while( view_type_t< V0, true >( v0_ ), v1_, binary_ );
		else if constexpr( Character_array< V1 > ) 
			return on_each_pair_while( v0_, view_type_t< V1, true >( v1_ ), binary_ );
		else {
			detail::assert_equal_extent( v0_, v1_ );
			auto 							itr0 = begin( v0_ );
			const auto 						end0 = end( v0_ );
			auto 							itr1 = begin( v1_ );
			while( ( itr0 != end0 ) && binary_( *itr0, *itr1 ) )	{	++itr0; ++itr1;		}
			return itr0 == end0;
		}
	}

	///	Checks if unary predicate p returns true for all elements in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool all_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		if constexpr( Character_array< V > ) 
				return std::ranges::all_of( view_type_t< V, true >( v_ ), p_ );
		else	return std::ranges::all_of( v_, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for all i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool all_of(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		return on_each_pair_while( v0_, v1_, 
			[ binary_ ]( 
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return binary_( t0_, t1_ );
			}
		);
	}

	///	Checks if unary predicate p returns true for at least one element in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool any_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		if constexpr( Character_array< V > ) 
				return std::ranges::any_of( view_type_t< V, true >( v_ ), p_ );
		else	return std::ranges::any_of( v_, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for any i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool any_of(
		const V0						  & v0_,
		const V1						  & v1_,
		Binary							 && binary_
	) noexcept {
		return !on_each_pair_while( v0_, v1_, 
			[ binary_ ]( 
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return !binary_( t0_, t1_ );
			}
		);
	}

	///	Checks if unary predicate p returns true for no elements in v.
	/**	- https://en.cppreference.com/w/cpp/algorithm/all_any_none_of		**/
	template< Contiguous_elements V, typename Pred >
		requires std::is_invocable_r_v< bool, Pred, Value_type_t< V > >
	[[nodiscard]] constexpr bool none_of( 
		const V							  & v_, 
		Pred							 && p_
	) {
		if constexpr( Character_array< V > ) 
			return std::ranges::none_of( view_type_t< V, true >( v_ ), p_ );
		else
			return std::ranges::none_of( v_, p_ );
	}

	/// Checks if binary_( v0_[i], v1_[i] ) is true for no i.
	template< Contiguous_elements V0, Contiguous_elements V1, typename Binary >
		requires std::is_invocable_r_v< bool, Binary, Value_type_t< V0 >, Value_type_t< V1 > >
	[[nodiscard]] constexpr bool none_of(
  		const V0						  & v0_,
  		const V1						  & v1_,
  		Binary							 && binary_
	) noexcept {
		return on_each_pair_while( v0_, v1_, 
			[ binary_ ](
				const Value_type_t< V0 >  & t0_, 
				const Value_type_t< V1 >  & t1_ 
			) {
				return !binary_( t0_, t1_ );
			}
		);
	}



	/// Return the first newline used in view_ (`"\n"`, `"\r"`, `"\n\r"`, or `"\r\n"`).
	/// - If none is found, `"\n"` is returned.
	template< String V >
	[[nodiscard]] auto identify_newline( const V & str_ ) noexcept {
		static constexpr const Value_type_t< V > 	res[] = { '\n', '\r', '\n' };
		if constexpr( Character_array< V > )
			return identify_newline( view( str_ ) );
		else {
			auto 							temp = not_first( str_, find( str_, Newline{} ) );
			temp						  = first( temp, starts_with( temp, Newline{} ) );
			return	temp.empty() ? first( res, 1 ) : subview( res, temp.front() == '\r', size( temp ) );
		}
	}

	/// Calculate the Luhn sum (a control sum).
	/// - https://en.wikipedia.org/wiki/Luhn_algorithm
	template< String V >
	[[nodiscard]] constexpr int luhn_sum( const V & str_ ) noexcept {
		const auto							v = dview( str_ );
		int									sum{};
		int									m{ 2 };
		for( const auto c : v ) {
			const int mul				  = m*( c - '0' );
			m							  = 3 - m;
			sum							 += ( mul / 10 ) + ( mul % 10 );
		}
		return sum;
	}

}	// namespace pax
