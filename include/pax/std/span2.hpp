//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "span.hpp"
#include <cassert>	// assert


namespace pax {

	/// A more "strict" variant of `std::span`.
	/// - All member functions are `constexpr`, `const`, and `noexcept`. 
	/// - Only `operator[]()`, `front()`, and `back()` have UB (Undefined Behaviour) in specific cases.
	/// - Most `std::span` member functions stay the same, but some no longer have UB (i.e. `subspan`). 
	template< typename T, std::size_t N = std::dynamic_extent >
	class span2 : public std::span< T, N > {
		using     base = std::span< T, N >;
		
#if( __cpp_lib_span >= 202311L )
		using base::at;		// Came in C++26, might throw an exception.
#endif
		
		template< std::integral Int >
		[[nodiscard]] static constexpr std::size_t subspan_offset( 
			const Int			 	offset_, 
			const std::size_t 		size_ 
		) noexcept {
			if constexpr( std::is_unsigned_v< Int > ) {
				return	std::min( offset_, size_ );
			} else {
				return	( offset_ >= 0 )					  ? std::min( std::size_t( offset_ ), size_ ) 
					:	( std::size_t( -offset_ ) < size_ )	  ? size_ - std::size_t( -offset_ )
					:											std::size_t{};
			}
		}

		template< typename TT >
		[[nodiscard]] static constexpr std::size_t size0( TT && c_ )	noexcept	{
			using std::size;
			return size( std::forward< TT >( c_ ) );
		}

		template< std::size_t I = base::extent >
		static constexpr bool	is_dynamic = ( I == std::dynamic_extent );
		template< std::size_t I = base::extent >
		static constexpr bool	is_static  = !is_dynamic< I >;

	public:
		using base::span;

		constexpr span2()								noexcept = default;
		constexpr span2( const span2 & )				noexcept = default;
		constexpr span2( span2 && )						noexcept = default;
		constexpr span2 & operator=( const span2 & )	noexcept = default;
		constexpr span2 & operator=( span2 && )			noexcept = default;

		/// Turns a `Contiguous_elements` into a `span2`.
		/// If `a_` is statically sized, the resulting `span2` will be too. 
		template< Contiguous_elements A >
		constexpr span2( A && a_ )			noexcept 
			: base( std::data( a_ ), std::size( a_ ) ) {}

		/// Turns an array into a static `span2`.
		template< std::size_t N0 >
		constexpr span2( T( & c_ )[ N0 ] )	noexcept requires ( is_static <> && ( N == N0 ) )
			: base( c_, N ) {}

		/// Turns an array of characters into a dynamic `span2`. Does not include a final `'\0'`.
		constexpr span2( T * & c_ )			noexcept requires ( is_dynamic<> && Character< T > )
			: base( c_, size0( c_ ) ) {}

		/// Turns an array of const characters into a dynamic `span2`. Does not include a final `'\0'`.
		constexpr span2( T * const & c_ )	noexcept requires ( is_dynamic<> && Character< T > )
			: base( c_, size0( c_ ) ) {}


		/// Returns `data() != nullptr`.
		[[nodiscard]] constexpr bool valid() 									const noexcept {
			return base::data() != nullptr;
		}

		/// Returns a dynamically sized `span2` of the first `i_` elements.
		///	- If `i_ > size()`, `*this` is returned.
		[[nodiscard]] constexpr auto first( const std::size_t i_ = 1 )			const noexcept {
			return span2< T >( base::data(), std::min( i_, base::size() ) );
		}

		/// Returns a statically sized `span2` of the first `I` elements.
		///	- If `I > extent`, `*this` is returned.
		template< std::size_t I >		requires( is_static< I > && is_static<> )
		[[nodiscard]] constexpr auto first()									const noexcept {
			static constexpr std::size_t 	sz = std::min( I, base::extent );
			return span2< T, sz >( base::data(), sz );
		}

		/// Returns a statically sized `span2` of the first `I` elements.
		///	- There is an `assert( I <= size() )`.
		template< std::size_t I >		requires( is_static< I > && is_dynamic<> )
		[[nodiscard]] constexpr auto first()									const noexcept {
			assert( I <= base::size() && "requires I <= size()." );
			return span2< T, I >( base::data(), I );
		}

		/// Returns a dynamically sized `span2` of the last `i_` elements.
		///	- If `i_ > size()`, `*this` is returned.
		[[nodiscard]] constexpr auto last( const std::size_t i_ = 1 )			const noexcept {
			return ( i_ < base::size() ) ? span2< T >( base::end() - i_, i_ ) : *this;
		}

		/// Returns a statically sized `span2` of the last `I` elements.
		///	- If `I > extent`, `*this` is returned.
		template< std::size_t I >		requires( is_static< I > && is_static<> )
		[[nodiscard]] constexpr auto last()										const noexcept {
			static constexpr std::size_t	offset = ( I < base::extent ) ? base::extent - I : 0u;
			return span2< T, base::extent - offset >( base::data() + offset, base::extent - offset );
		}

		/// Returns a statically sized `span2` of the last `I` elements.
		///	- There is an `assert( I <= size() )`.
		template< std::size_t I >		requires( is_static< I > && is_dynamic<> )
		[[nodiscard]] constexpr auto last()										const noexcept {
			assert( I <= base::size() && "requires I <= size()." );
			return span2< T, I >( base::end() - I, I );
		}

		/// Returns a dynamically sized `span2` of all elements except the first `i_`.
		///	- If `i_ > size()`, `span2< T >( data() + size(), 0 )` is returned.
		[[nodiscard]] constexpr auto not_first( const std::size_t i_ = 1 )		const noexcept {
			return ( i_ < base::size() )	? span2< T >( base::data() + i_, base::size() - i_ )
											: span2< T >( base::end(), 0 );
		}

		/// Returns a statically sized `span2` of all elements except the first `I`.
		///	- If `I > extent`, `span2< T, 0 >( data() + size(), 0 )` is returned.
		template< std::size_t I >			requires( is_static< I > && is_static<> )
		[[nodiscard]] constexpr auto not_first()								const noexcept {
			static constexpr std::size_t	offset = ( I < base::extent ) ? I : base::extent;
			static constexpr std::size_t	sz = base::extent - offset;
			return span2< T, sz >( base::data() + offset, sz );
		}

		/// Returns a dynamically sized `span2` of all elements except the last `i_`.
		///	- If `i_ > size()`, `span2< T >( data(), 0 )` is returned.
		[[nodiscard]] constexpr auto not_last( const std::size_t  i_ = 1 )		const noexcept {
			return span2< T >( base::data(), ( i_ < base::size() ) ? base::size() - i_ : 0u );
		}

		/// Returns a statically sized `span2` of all elements except the last `I`.
		///	- If `I > extent`, `span< T, 0 >( data(), 0 )` is returned.
		template< std::size_t I >			requires( is_static< I > && is_static<> )
		[[nodiscard]] constexpr auto not_last()									const noexcept {
			static constexpr std::size_t 	sz = ( I < base::extent ) ? base::extent - I : 0u;
			return span2< T, sz >( base::data(), sz );
		}

		/// Returns a dynamically sized `span2` of `size_` elements starting at `offset_`.
		///	- If `offset_ < 0`, `offset_ += size()` is used (the offset is seen from the back), 
		///	- If `offset_ + size_ >= size()`: returns `not_first( offset_ )`.
		template< std::integral Int >
		[[nodiscard]] constexpr auto subspan( 
			const Int			 			offset_, 
			const std::size_t 				size_ 
		) const noexcept {
			const auto 						offset = subspan_offset( offset_, base::size() );
			return span2< T >( base::data() + offset, std::min( base::size() - offset, size_ ) );
		}

		/// Returns a statically sized `span2` of `Len` elements starting at `offset_`.
		///	- If `offset_ < 0`, an `offset_ += size()` is used (the offset is seen from the back), 
		///	- There is an `assert( offset_ + Len <= size() )`.
		template< std::size_t Len, std::integral Int >		requires is_static< Len >
		[[nodiscard]] constexpr auto subspan( const Int offset_ )				const noexcept	{
			const auto 						offset = subspan_offset( offset_, base::size() );
			assert( offset + Len <= base::size()  && "requires offset_ + Len <= size()." );
			return span2< T, Len >( base::data() + offset, Len );
		}

		/// Returns a statically sized `span2` of `Len` elements starting at `Offset`.
		///	- If `Offset < 0`, an offset = `Offset + size()` is used (the offset is seen from the back), 
		///	- If `Offset + Len > size()`, returns `not_first< offset >()`.
		template< std::ptrdiff_t Offset, std::size_t Len >	
			requires( is_static< Len > && is_static<> )
		[[nodiscard]] constexpr auto subspan()									const noexcept {
			static constexpr auto			offset = subspan_offset( Offset, base::extent );
			static constexpr auto 			len    = std::min( base::extent - offset, Len );
			return span2< T, len >( base::data() + offset, len );
		}

		/// Returns a statically sized `span2` of `Len` elements starting at `Offset`.
		///	- If `Offset < 0`, an offset = `Offset + size()` is used (the offset is seen from the back), 
		///	- There is an `assert( offset + Len <= size() )`.
		template< std::ptrdiff_t Offset, std::size_t Len >
			requires( is_static< Len > && is_dynamic<> )
		[[nodiscard]] constexpr auto subspan()									const noexcept {
			const auto 						offset = subspan_offset( Offset, base::size() );
			assert( offset + Len <= base::size() && "requires Offset + Len <= size()." );
			return span2< T, Len >( base::data() + offset, Len );
		}

		/// Copy the values referenced by `*this` to `dest_`. Returns `size()`.
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
