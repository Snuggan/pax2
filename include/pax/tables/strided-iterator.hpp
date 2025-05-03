//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cassert>		// assert()
#include <iterator>		// std::random_access_iterator_tag


namespace pax {

	/// A bare bones class implementing a random access iterator with a step that may be other than 1.
	/// - The stride may be negative. 
	/// - It never throws.
	/// - As it strides, it is not a contiguous_iterator.
	template< typename T >
	class Strided_iterator {
		using Strided					  = Strided_iterator< T >;
		T								  * m_ptr{ nullptr };
		std::ptrdiff_t 						m_stride{ 1 };

		constexpr Strided & offset( const std::ptrdiff_t offs_ )	  noexcept	{	m_ptr+= offs_;		return *this;		}

	public:
		using stride_type				  = std::ptrdiff_t;
		using element_type				  = T;
		using value_type				  = std::remove_cv_t< element_type >;
		using pointer					  = element_type *;
		using reference					  = element_type &;
		using iterator_category			  = std::random_access_iterator_tag;

		constexpr Strided_iterator()								  noexcept	=	default;
		constexpr Strided_iterator( const Strided & )				  noexcept	=	default;
		constexpr Strided & operator=( const Strided & )			  noexcept	=	default;
		constexpr auto operator<=>( const Strided & )			const noexcept	=	default;
		constexpr bool operator== ( const Strided & )			const noexcept	=	default;

		constexpr Strided_iterator(
			const pointer					ptr_, 
			const stride_type 				stride_ = 1
		) noexcept : m_ptr{ ptr_ }, m_stride{ stride_ } {}

		constexpr stride_type stride()							const noexcept	{	return m_stride;						}
		constexpr pointer   ptr()								const noexcept	{	return m_ptr;							}
		constexpr pointer   operator->()						const noexcept	{	return m_ptr;							}
		constexpr reference operator*()							const noexcept	{	return *m_ptr;							}
		constexpr reference operator[]( const stride_type n_ )	const noexcept	{	return *( m_ptr + stride()*n_ );		}
		constexpr Strided & operator++()							  noexcept	{	return offset(  stride() );				}
		constexpr Strided & operator+=( const stride_type o_ )		  noexcept	{	return offset(  stride()*o_ );			}
		constexpr Strided   operator+ ( const stride_type o_ ) 	const noexcept	{	return Strided{ *this }+= o_;			}
		constexpr Strided & operator--()							  noexcept	{	return offset( -stride() );				}
		constexpr Strided & operator-=( const stride_type o_ )		  noexcept	{	return offset( -stride()*o_ );			}
		constexpr Strided   operator- ( const stride_type o_ ) 	const noexcept	{	return Strided{ *this }-= o_;			}

		constexpr Strided   operator++( int ) noexcept {	const Strided temp = *this;	m_ptr+= stride();	return temp;	}
		constexpr Strided   operator--( int ) noexcept {	const Strided temp = *this;	m_ptr-= stride();	return temp;	}

		/// Undefined behaviour if ( stride() != o_.stride() ) || ( ( m_ptr - o_.m_ptr )%stride() != 0 ).
		constexpr stride_type operator-( const Strided o_ ) 	const noexcept	{
			assert( stride() == o_.stride() );					// Otherwise it doesn't make sense...
			assert( ( m_ptr - o_.m_ptr )%stride() == 0 );		// They must be in sync.
			return  ( m_ptr - o_.m_ptr )/stride();
		}
	};


	/// A convenience class for a pair of strided iterators.
	/// Easy to use in loops: for( const auto item : strider ) { ... }.
	template< typename T >
	class Strider {
		using 		iterator = Strided_iterator< T >;
		iterator	m_begin{}, m_end{};
		
	public:
		using element_type				  = T;
		using value_type				  = std::remove_cv_t< element_type >;

		constexpr Strider()											  noexcept	=	default;
		constexpr Strider( const Strider & )						  noexcept	=	default;
		constexpr Strider & operator=( const Strider & )			  noexcept	=	default;

		constexpr Strider(
			const iterator					begin_, 
			const std::size_t 				items_
		) noexcept : m_begin{ begin_ }, m_end{ begin_ + items_ } {}

		constexpr auto begin()									const noexcept	{	return m_begin;							}
		constexpr auto end()									const noexcept	{	return m_end;							}
		constexpr auto size()									const noexcept	{	return end() - begin();					}
		constexpr auto stride()									const noexcept	{	return m_begin.stride();				}
		constexpr auto & operator[]( const std::size_t n_ )		const noexcept	{	return *( begin().ptr() + stride()*n_ );}
	};

	template< typename T >
	Strider( Strided_iterator< T >, std::size_t )	-> Strider< T >;

}	// namespace pax
