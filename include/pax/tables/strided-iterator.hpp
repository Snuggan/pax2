//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <cassert>
#include <iterator>		// std::random_access_iterator_tag


namespace pax {


	/// A bare bones class implementing a random access iterator with a step that may be other than 1.
	/// - The stride may be negative. 
	/// - It never throws by itself, but the user must naturally see to it that it is in range.
	/// - There are some restrictions on operator-. Not complying is undefined behaviour. 
	template< typename T >
	class Strided_iterator {
		T								  * m_ptr{ nullptr };
		std::ptrdiff_t 						m_stride{ 1 };

	public:
		using difference_type			  = std::ptrdiff_t;
		using stride_type				  = difference_type;
		using element_type				  = T;
		using value_type				  = std::remove_cv_t< element_type >;
		using pointer					  = element_type *;
		using reference					  = element_type &;

		// This iterator type is random access: 
		// - operator++() does "jump" values, so it is not a contiguous_iterator.
		//   Some std functions (i.e. std::copy) will not function correctly if the contiguous_iterator_tag is used.
		using iterator_category			  = std::random_access_iterator_tag;

		constexpr Strided_iterator()										  noexcept	=	default;
		constexpr Strided_iterator( const Strided_iterator & )				  noexcept	=	default;
		constexpr Strided_iterator & operator=( const Strided_iterator & )	  noexcept	=	default;

		constexpr auto operator<=>( const Strided_iterator & )			const noexcept	=	default;
		constexpr bool operator== ( const Strided_iterator & )			const noexcept	=	default;

		constexpr Strided_iterator(
			const pointer					ptr_, 
			const stride_type 				stride_ = 1
		) noexcept : m_ptr{ ptr_ }, m_stride{ stride_ } {}

		constexpr stride_type stride()									const noexcept	{	return m_stride;					}
		constexpr pointer operator->()									const noexcept	{	return m_ptr;						}
		constexpr reference operator*()									const noexcept	{	return *m_ptr;						}
		constexpr reference operator[]( const stride_type o_ )			const noexcept	{	return *( m_ptr + stride()*o_ );	}

		constexpr Strided_iterator & operator++()							  noexcept	{
			m_ptr+= stride();
			return *this;
		}

		constexpr Strided_iterator operator++( int )						  noexcept	{
			const pointer	temp = m_ptr;
			m_ptr+= stride();
			return { temp, stride() };
		}

		constexpr Strided_iterator & operator--()							  noexcept	{
			m_ptr-= stride();
			return *this;
		}

		constexpr Strided_iterator operator--( int )						  noexcept	{
			const pointer	temp = m_ptr;
			m_ptr-= stride();
			return { temp, stride() };
		}

		constexpr Strided_iterator & operator+=( const stride_type o_ )		  noexcept	{
			m_ptr+= stride()*o_;
			return *this;
		}

		constexpr Strided_iterator & operator-=( const stride_type o_ )		  noexcept	{
			m_ptr-= stride()*o_;
			return *this;
		}

		constexpr Strided_iterator operator+( const stride_type o_ )	const noexcept	{
			return { m_ptr + stride()*o_, stride() };
		}

		friend constexpr Strided_iterator operator+(
			const stride_type 			o_, 
			const Strided_iterator		itr_
		) noexcept {
			return itr_ + o_;
		}

		constexpr Strided_iterator operator-( const stride_type o_ )	const noexcept	{
			return { m_ptr - stride()*o_, stride() };
		}

		/// Undefined behaviour, unless the following is true:
		/// - stride() == o_.stride() and
		/// - ( m_ptr - o_.m_ptr )%stride() == 0
		constexpr stride_type operator-( const Strided_iterator o_ )	const noexcept	{
			assert( stride() == o_.stride() );
			assert( ( m_ptr - o_.m_ptr )%stride() == 0 );
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

		constexpr Strider()													  noexcept	=	default;
		constexpr Strider( const Strider & )								  noexcept	=	default;
		constexpr Strider & operator=( const Strider & )					  noexcept	=	default;

		constexpr Strider(
			const iterator					begin_, 
			const std::size_t 				items_
		) noexcept : m_begin{ begin_ }, m_end{ begin_ + items_ } {}

		constexpr auto begin()											const noexcept	{	return m_begin;					}
		constexpr auto end()											const noexcept	{	return m_end;					}
		constexpr auto size()											const noexcept	{	return m_begin.stride();		}
		constexpr auto stride()											const noexcept	{	return end() - begin();			}
	};

	template< typename T >
	Strider( Strided_iterator< T >, std::size_t )	-> Strider< T >;

}	// namespace pax
