//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "shadow.hpp"
#include <vector>


namespace pax {

	template< typename T, std::size_t N >
	struct arrector_core {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using const_pointer							  = element_type const *;
		static constexpr std::size_t 					extent = N;

		[[nodiscard]] constexpr arrector_core() noexcept = default;
		[[nodiscard]] constexpr arrector_core( const arrector_core & ) noexcept	= default;
		constexpr arrector_core & operator=( const arrector_core & ) noexcept = default;

		[[nodiscard]] constexpr arrector_core( T( & ptr_ )[ N ] ) noexcept {	copy( ptr_ );		}

		[[nodiscard]] constexpr arrector_core( const const_pointer ptr_, const std::size_t sz_ ) noexcept {
			assert( sz_ == N );
			if( ptr_ )		copy( ptr_ );
		}

		[[nodiscard]] constexpr arrector_core( const const_pointer begin_, const const_pointer end_ ) noexcept 
			: arrector_core( begin_, std::size_t( end_ - begin_ ) ) {}

		template< std::ranges::contiguous_range U >		requires( N == traits::extent_v< U > )
		[[nodiscard]] constexpr arrector_core( U & src_ ) noexcept { copy( std::ranges::data( src_ ) );	}

		template< typename U >
		[[nodiscard]] constexpr arrector_core( const U ( & str_ )[ N+1 ] ) noexcept 
			requires( std::is_same_v< U, value_type > && traits::character< value_type > ) {
			assert( !str_[ N ] && "Removing a non-zero character suffix!" );
			copy( str_ );
		}

		template< typename... Args >
		requires( N == sizeof...( Args ) && std::is_nothrow_convertible_v< std::common_type_t< Args... >, T > ) 
		[[nodiscard]] constexpr arrector_core( Args && ...args_ ) noexcept
			: m_source{ value_type( std::forward< Args >( args_ ) )... } {}

		[[nodiscard]] constexpr const_pointer data()	const noexcept	{	return m_source;			}
		[[nodiscard]] constexpr pointer data()				  noexcept	{	return m_source;			}
		[[nodiscard]] static constexpr std::size_t size()	  noexcept	{	return extent;				}

	private:
		value_type										m_source[ N ];

		constexpr void copy( const const_pointer ptr_ )		  noexcept	{
			std::ranges::copy_n( ptr_, N, m_source );
		}
	};

	template< typename T >
	struct arrector_core< T, dynamic_extent > {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using const_pointer							  = element_type const *;
		static constexpr std::size_t 					extent = dynamic_extent;

		[[nodiscard]] constexpr arrector_core() noexcept = default;
		[[nodiscard]] constexpr arrector_core( const arrector_core & ) noexcept = default;
		[[nodiscard]] constexpr arrector_core( arrector_core && ) noexcept = default;
		constexpr arrector_core & operator=( const arrector_core & ) noexcept = default;
		constexpr arrector_core & operator=( arrector_core && ) noexcept = default;

		[[nodiscard]] constexpr arrector_core( const const_pointer src_, const std::size_t sz_ ) noexcept {
			m_source.resize( sz_ );
			if( src_ )		m_source.assign( src_, src_ + sz_ );
		}

		[[nodiscard]] constexpr arrector_core( const const_pointer begin_, const const_pointer end_ ) noexcept 
			: m_source( begin_, end_ ) {}

		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr arrector_core( U & src_ ) noexcept 
			: m_source( std::ranges::begin( src_ ), std::ranges::end( src_ ) ) {}

		[[nodiscard]] constexpr const_pointer data()	const noexcept	{	return m_source.data();		}
		[[nodiscard]] constexpr pointer data()				  noexcept	{	return m_source.data();		}
		[[nodiscard]] constexpr std::size_t size()		const noexcept	{	return m_source.size();		}
		[[nodiscard]] constexpr void resize( std::size_t i_ ) noexcept	{	m_source.resize( i_ );		}

	private:
		std::vector< value_type >						m_source;
	};



	/// Implements a simple container, kind of similar to std::span -- a hybrid od std::array and std::vector.
	template< typename T, std::size_t N = dynamic_extent >
	struct arrector : public contiguous_shell< arrector_core, T, N > {
		using contiguous_shell< arrector_core, T, N >::contiguous_shell;
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

	template< typename... Args >		requires( ( ... && !std::is_pointer_v< Args > ) )
	arrector( Args && ...args_ )	 -> arrector< std::remove_cv_t< std::common_type_t< Args... > >, sizeof ...( Args ) >;

}	// namespace pax
