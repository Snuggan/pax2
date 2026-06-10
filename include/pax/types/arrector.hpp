//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "shadow.hpp"
#include <vector>


namespace pax {

	/// Implements a simple container, kind of similar to std::span -- a hybrid od std::array and std::vector.
	template< typename T, std::size_t N >
	struct base_arrector {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		static constexpr std::size_t 					extent = N;

		[[nodiscard]] constexpr base_arrector() noexcept = default;
		[[nodiscard]] constexpr base_arrector( const base_arrector & ) noexcept	= default;
		constexpr base_arrector & operator=( const base_arrector & ) noexcept = default;

		[[nodiscard]] constexpr base_arrector( T( & ptr_ )[ N ] ) noexcept : base_arrector( ptr_, N ) {}

		[[nodiscard]] constexpr base_arrector( const pointer ptr_, const std::size_t sz_ ) noexcept {
			assert( sz_ == N );
			std::ranges::copy_n( ptr_, std::min( sz_, N ), m_source );
		}

		[[nodiscard]] constexpr base_arrector( const pointer begin_, const pointer end_ ) noexcept 
			: base_arrector( begin_, std::size_t( end_ - begin_ ) ) {}

		template< typename... Args >
			requires( ( true && ... && std::is_arithmetic_v< Args > ) && ( sizeof...( Args ) == N ) )
		[[nodiscard]] constexpr base_arrector( Args && ...args_ ) noexcept
			: m_source{ T( std::forward< Args >( args_ ) )... } {}

		template< typename U >
		[[nodiscard]] constexpr base_arrector( const U ( & str_ )[ N+1 ] ) noexcept 
			// Somewhat constructed due to disambiguation:
		requires( std::is_same_v< U, value_type > && traits::character< value_type > ) {
			assert( !str_[ N ] && "Removing a non-zero character suffix!" );
			std::ranges::copy_n( str_, N, m_source );
		}

		[[nodiscard]] constexpr pointer data()			const noexcept	{	return pointer( m_source );		}
		[[nodiscard]] static constexpr std::size_t size()	  noexcept	{	return extent;					}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();					}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();			}

	private:
		value_type										m_source[ N ];
	};

	template< typename T >
	struct base_arrector< T, dynamic_extent > {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		static constexpr std::size_t 					extent = dynamic_extent;

		[[nodiscard]] constexpr base_arrector() noexcept = default;
		[[nodiscard]] constexpr base_arrector( const base_arrector & ) noexcept = default;
		[[nodiscard]] constexpr base_arrector( base_arrector && ) noexcept = default;
		constexpr base_arrector & operator=( const base_arrector & ) noexcept = default;
		constexpr base_arrector & operator=( base_arrector && ) noexcept = default;

		[[nodiscard]] constexpr base_arrector( const pointer src_, const std::size_t sz ) noexcept 
			: m_source{ src_, src_ ? sz : 0u } {}

		[[nodiscard]] constexpr base_arrector( const pointer begin_, const pointer end_ ) noexcept 
			: base_arrector( begin_, std::size_t( end_ - begin_ ) ) {}

		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr base_arrector( U & src_ ) noexcept 
			: base_arrector( std::ranges::data( src_ ), std::ranges::size( src_ ) ) {}

		[[nodiscard]] constexpr pointer data()			const noexcept	{	return m_source.data();			}
		[[nodiscard]] constexpr std::size_t size()		const noexcept	{	return m_source.size();			}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();					}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();			}

	private:
		std::vector< value_type >						m_source;
	};



	template< typename T, std::size_t N = dynamic_extent >
	struct arrector : public contiguous_shell< base_arrector, T, N > {
		using contiguous_shell< base_arrector, T, N >::contiguous_shell;
	};

	template< typename T >
	arrector( T *, std::size_t )	 -> arrector< T, dynamic_extent >;

	template< typename T >
	arrector( T *, T * )			 -> arrector< T, dynamic_extent >;

	template< typename T, std::size_t N >
	arrector( T( & )[ N ] )			 -> arrector< T, N >;

	template< traits::character T, std::size_t N >
	arrector( T const( & )[ N ] )	 -> arrector< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	arrector( Cont & )				 -> arrector< traits::element_type_t< Cont >, traits::extent_v< Cont > >;

}	// namespace pax
