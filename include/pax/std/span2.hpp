//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"
#include <span>
#include <cassert>


namespace pax {

	/// A more "strict" variant of std::span.
	/// - All member functions are constexpr, const, and noexcept. 
	/// - Only operator[](), front(), and back() have UB, Undefined Behaviour, in certain cases.
	/// - Additional member functions to return a sub-span2: not_first() and not_last().
	/// - Many std::span member functions stay the same, but some no longer have UB (i.e. subspan). 
	template< typename T, std::size_t N = std::dynamic_extent >
	class span2 : public std::span< T, N > {
		using base = std::span< T, N >;

#if( __cpp_lib_span >= 202311L )
		using base::at;									// Came in C++26, might throw an exception.
#endif
		
		[[nodiscard]] static constexpr std::size_t subspan_offset( 
			const std::ptrdiff_t 	offset_, 
			const std::size_t 		size_ 
		) noexcept {
			return	(  offset_ >= 0 )					  ? std::min( std::size_t( offset_ ), size_ ) 
				:	( std::size_t( -offset_ ) < size_ )	  ? size_ - std::size_t( -offset_ )
				:											std::size_t{};
		}

		template< Character Ch >
		[[nodiscard]] static constexpr std::size_t charlen( Ch * c_ ) noexcept {
			auto					itr = c_;
			if( itr && *itr )		while( *( ++itr ) );
			return itr - c_;
		}

	public:
		using base::span;
		
		constexpr span2( const span2 & )				noexcept = default;
		constexpr span2( span2 && )						noexcept = default;
		constexpr span2 & operator=( const span2 & )	noexcept = default;
		constexpr span2 & operator=( span2 && )			noexcept = default;

		/// Turns a Contiguous_elements into a span.
		/// If a_ is statically sized the resulting span2 will be too. 
		template< Contiguous_elements A >
		constexpr span2( A && a_ )			noexcept : base( std::data( a_ ), std::size( a_ ) ) {}

		/// Turns an array into a static span2.
		template< std::size_t N0 >
		constexpr span2( T( & c_ )[ N0 ] )	noexcept requires ( N == N0 )		: base( c_, N ) {}

		/// Turns an array of characters into a dynamic span2. Does not include a final '\0' character.
		constexpr span2( T * & c_ )			noexcept requires Character< T >	: base( c_, charlen( c_ ) ) {}

		/// Turns an array of characters into a dynamic span2. Does not include a final '\0' character.
		constexpr span2( T * const & c_ )	noexcept requires Character< T >	: base( c_, charlen( c_ ) ) {}


		/// Check if all elements are equal to the corresponding elements of v_.
		template< Contiguous_elements V >
		[[nodiscard]] constexpr bool operator==( V && v_ )						const noexcept {
			using std::begin, std::end;
			return std::equal( base::begin(), base::end(), begin( v_ ), end( v_ ) );
		}

		template< Character_array V >
		[[nodiscard]] constexpr bool operator==( V && v_ )						const noexcept {
			return operator==( span2( v_ ) );
		}

		/// Lexicographically compare with v_ using three-way comparison.
		template< Contiguous_elements V >
		[[nodiscard]] constexpr auto operator<=>( V && v_ )						const noexcept {
			using std::begin, std::end;
			return std::lexicographical_compare_three_way( base::begin(), base::end(), begin( v_ ), end( v_ ) );
		}

		template< Character_array V >
		[[nodiscard]] constexpr auto operator<=>( V && v_ )						const noexcept {
			return operator<=>( span2( v_ ) );
		}


		/// Returns`data() != nullptr.
		[[nodiscard]] constexpr bool valid() 									const noexcept {
			return base::data() != nullptr;
		}

		/// Returns a dynamically sized span2 of the first i_ elements.
		///	- If i_ > size(), span2< T >( data(), size() ) is returned.
		[[nodiscard]] constexpr auto first( const std::size_t i_ = 1 )			const noexcept {
			return span2< T >( base::data(), std::min( i_, base::size() ) );
		}

		/// Returns a statically sized span2 of the first I elements.
		///	- If I > extent, *this is returned.
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent != std::dynamic_extent ) )
		[[nodiscard]] constexpr auto first()									const noexcept {
			static constexpr std::size_t 	sz = std::min( I, base::extent );
			return span2< T, sz >( base::data(), sz );
		}

		/// Returns a statically sized span2 of the first I elements.
		///	- There is an assert( I <= size() ).
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent == std::dynamic_extent ) )
		[[nodiscard]] constexpr auto first()									const noexcept {
			assert( I <= base::size() && "first< I >() requires I <= size()." );
			return span2< T, I >( base::data(), I );
		}

		/// Returns a dynamically sized span2 of the last i_ elements.
		///	- If i_ > size(), span2< T >( data(), size() ) is returned.
		[[nodiscard]] constexpr auto last( const std::size_t i_ = 1 )			const noexcept {
			return ( i_ < base::size() ) ? span2< T >( base::end() - i_, i_ ) : *this;
		}

		/// Returns a statically sized span2 of the last I elements.
		///	- If I > extent, *this is returned.
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent != std::dynamic_extent ) )
		[[nodiscard]] constexpr auto last()										const noexcept {
			static constexpr std::size_t	offset = ( I < base::extent ) ? base::extent - I : 0u;
			return span2< T, base::extent - offset >( base::data() + offset, base::extent - offset );
		}

		/// Returns a statically sized span2 of the last I elements.
		///	- There is an assert( I <= size() ).
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent == std::dynamic_extent ) )
		[[nodiscard]] constexpr auto last()										const noexcept {
			assert( I <= base::size() && "last< I >() requires I <= size()." );
			return span2< T, I >( base::end() - I, I );
		}

		/// Returns a span2 of all elements but the first i_.
		///	- If i_ > size(), span2< T >( data() + size(), 0 ) is returned.
		[[nodiscard]] constexpr auto not_first( const std::size_t i_ = 1 )		const noexcept {
			return ( i_ < base::size() )	? span2< T >( base::data() + i_, base::size() - i_ )
											: span2< T >( base::end(), 0 );
		}

		/// Returns a statically sized span2 of all elements except the first I.
		///	- If I > extent, span2< 0 >( data() + size(), 0 ) is returned.
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent != std::dynamic_extent ) )
		[[nodiscard]] constexpr auto not_first()								const noexcept {
			static constexpr std::size_t		offset = ( I < base::extent ) ? I : base::extent;
			static constexpr std::size_t		sz = base::extent - offset;
			return span2< T, sz >( base::data() + offset, sz );
		}

		/// Returns a span2 of all elements except the last i_.
		///	- If i_ > size(), span2< T >( data() + size(), 0 ) is returned.
		[[nodiscard]] constexpr auto not_last( const std::size_t  i_ = 1 )		const noexcept {
			return span2< T >( base::data(), ( i_ < base::size() ) ? base::size() - i_ : 0u );
		}

		/// Returns a statically sized span2 of all elements except the first I.
		///	- If I > extent, span< 0 >( data(), 0 ) is returned.
		template< std::size_t I >
			requires( ( I != std::dynamic_extent ) && ( base::extent != std::dynamic_extent ) )
		[[nodiscard]] constexpr auto not_last()									const noexcept {
			static constexpr std::size_t 	sz = ( I < base::extent ) ? base::extent - I : 0u;
			return span2< T, sz >( base::data(), sz );
		}

		/// Returns a span2 of `size_` elements starting at `offset_`.
		///	- If `offset_ < 0`, `offset_ += size()` is used (the offset is seen from the back), 
		///	- If `offset_ + size_ >= size()`: returns `not_first( offset_ )`.
		[[nodiscard]] constexpr auto subspan( 
			const std::ptrdiff_t 	offset_, 
			const std::size_t 		size_ 
		) const noexcept {
			const auto 				offset = subspan_offset( offset_, base::size() );
			return span2< T >( base::data() + offset, std::min( base::size() - offset, size_ ) );
		}

		/// Returns a statically sized span2 of Len elements starting at offset_.
		///	- If offset_ < 0, an offset_ += size() is used (the offset is seen from the back), 
		///	- There is an assert( offset_ + Len <= size() ).
		template< std::size_t Len >
			requires( Len != std::dynamic_extent )
		[[nodiscard]] constexpr auto subspan( const std::ptrdiff_t offset_ )	const noexcept	{
			const auto 				offset = subspan_offset( offset_, base::size() );
			assert( offset + Len <= base::size()  && "subspan< Len >( v_, offset_ ) requires offset_ + Len <= size()." );
			return span2< T, Len >( base::data() + offset, Len );
		}

		/// Returns a statically sized span2 of Len elements starting at Offset.
		///	- If Offset < 0, an offset = Offset + size() is used (the offset is seen from the back), 
		///	- If Offset + Len > size(), returns not_first< offset >().
		template< std::ptrdiff_t Offset, std::size_t Len >	
			requires( ( Len != std::dynamic_extent ) && ( base::extent != std::dynamic_extent ) )
		[[nodiscard]] constexpr auto subspan()									const noexcept {
			static constexpr auto	offset = subspan_offset( Offset, base::extent );
			static constexpr auto 	len    = std::min( base::extent - offset, Len );
			return span2< T, len >( base::data() + offset, len );
		}

		/// Returns a statically sized span2 of Len elements starting at Offset.
		///	- If Offset < 0, an offset = Offset + size() is used (the offset is seen from the back), 
		///	- There is an assert( offset + Len <= size() ).
		template< std::ptrdiff_t Offset, std::size_t Len >
			requires( ( Len != std::dynamic_extent ) && ( base::extent == std::dynamic_extent ) )
		[[nodiscard]] constexpr auto subspan()									const noexcept {
			const auto 				offset = subspan_offset( Offset, base::size() );
			assert( offset + Len <= base::size() && "subspan< Offset, Len >( v_ ) requires Offset + Len <= size()." );
			return span2< T, Len >( base::data() + offset, Len );
		}

		/// Copy the string referenced to by `*this` to dest_. Returns `size().
		constexpr std::size_t copy( T *dest_ )									const noexcept {
			return base::copy( dest_, base::size() );
		}
	};

	template< typename It, typename EndOrSize >
	span2( It, EndOrSize ) 		-> span2< std::remove_reference_t< std::iter_reference_t< It > > >;

	template< Contiguous_elements A >
	span2( A && ) 				-> span2< Value_type_t< A >, extent_v< A > >;

	template< typename T, std::size_t N >
	span2( T ( & c_ )[ N ] )	-> span2< std::remove_reference_t< T >, N >;

	template< Character Ch, std::size_t N >
	span2( Ch( & c_ )[ N ] )	-> span2< std::remove_reference_t< Ch >, ( N == 0 ) ? 0u : N-1u >;

	template< Character Ch >
	span2( Ch * const & c_ )	-> span2< std::remove_reference_t< Ch >, std::dynamic_extent >;

}	// namespace pax
namespace std {

	/// Stream the contents of sp_ to dest_ in the form "[i0, i1, ...]".
	template< typename Dest, typename T, std::size_t N >
	constexpr Dest & operator<<(
		Dest					  & dest_,
		const std::span< T, N >		sp_
	) {
		if constexpr( pax::Character< T > ) {
			if constexpr( N != 0 ) if( sp_.size() )
				dest_.write( sp_.data(), sp_.size() - !sp_.back() );
		} else {
			// If the is a string type, put items inside "".
			constexpr auto			delimit = pax::String< T > ? "\"" : "";
			dest_ << '[';
			if constexpr( N != 0 ) for( bool leading{ true }; const auto & item : sp_ )
				dest_ << ( ( leading ? ( leading = false, true ) : false ) ? "" : ", " ) << delimit << item << delimit;
			dest_ << ']';
		}
		return dest_;
	}

	/// Check if all elements of sp_ are equal to the corresponding elements of v_.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr bool operator==(
		const std::span< T, N >		sp_,
		V						 && v_
	) noexcept {
		if constexpr( pax::Character_array< V > )
				return operator==( sp_, pax::span2( v_ ) );
		else	return std::equal( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Lexicographically compare sp_ with v_ using three-way comparison.
	template< typename T, std::size_t N, pax::Contiguous_elements V >
	[[nodiscard]] constexpr auto operator<=>(  
		const std::span< T, N >		sp_, 
		V						 && v_
	) noexcept {
		if constexpr( pax::Character_array< V > ) 
				return operator<=>( sp_, pax::span2( v_ ) );
		else	return std::lexicographical_compare_three_way( begin( sp_ ), end( sp_ ), begin( v_ ), end( v_ ) );
	}

	/// Implement get for statically sized std::span.
	/// Negative I will acces I + N.
	template< std::ptrdiff_t I, typename T, std::size_t N >
		requires( ( N != std::dynamic_extent ) && ( ( ( I < 0 ) ? -I : I ) < N ) )
	[[nodiscard]] constexpr T & get( const std::span< T, N > v_ )  				noexcept	{
		return *( v_.data() + ( ( I >= 0 ) ? I : I + N ) );
	}

}	// namespace std
