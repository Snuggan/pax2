//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/concepts.hpp>		// Character
#include <pax/std/format.hpp>	// std20::formatter
#include <algorithm>			// std::equal, std::lexicographical_compare
#include <string_view>
#include <span>


namespace pax {
	
	/// Implements the core for span-like utilities.
	template< typename T >
	struct core_shadow {
		using value_type		  = std::remove_cv_t< T >;
		using pointer			  = value_type const *;
		using size_type			  = std::size_t;
		using difference_type	  = std::ptrdiff_t;

		static constexpr bool		is_string{ Character< value_type > };

	    constexpr core_shadow()		noexcept		{};

		template< size_type N >
	    constexpr core_shadow( value_type const ( & source_ )[ N ] ) noexcept
			:	m_source{ source_ }, 
				m_size{ N - ( is_string && ( N > 0 ) && ( source_[ N - 1 ] == 0 ) ) } {}

	    constexpr core_shadow( 
			const pointer source_, 
			const size_type size_ 
		)	noexcept : m_source{ source_ }, m_size{ source_ ? size_ : 0u } {}

		template< Contiguous_elements_object U >
		constexpr core_shadow( const U & source_ ) noexcept 
			:	core_shadow{ data( source_ ), size( source_ ) } {}

		constexpr pointer data()	const noexcept	{	return m_source;				}
		constexpr size_type size()	const noexcept	{	return m_size;					}
				
		template< Contiguous_elements_object U >
		static constexpr pointer data( const U & source_ ) noexcept {
			using std::data;
			return data( source_ );
		}
		
		template< Contiguous_elements_object U >
		static constexpr size_type size( const U & source_ ) noexcept {
			using std::data;
			return size( source_ );
		}

	private:
		pointer						m_source{};
		size_type					m_size{};
	};

	template< typename T, std::size_t N >
	core_shadow( T ( & )[ N ] ) -> core_shadow< T >;
	


	/// Implements the stuff you want for an interface to contiguous imutable elements.
	/// The interfaced is similar to that of std::string_view and std::span. 
	/// Core needs to implement data() and size() and define value_type, size_type, and difference_type.
	template< typename Core >
	struct base_shadow : Core {
		using value_type		  = typename Core::value_type;
		using pointer			  = value_type const *;
		using iterator			  = pointer;
		using size_type			  = typename Core::size_type;
		using difference_type	  = typename Core::difference_type;
		using shadow			  = base_shadow< core_shadow< value_type > >;
		
		static constexpr bool		is_string{ Character< value_type > };

	    using Core::Core;
		using Core::data;
		using Core::size;
 
	    constexpr value_type operator[]( const size_type i_ )		const noexcept
		{	return ( i_ < size() ) ? data()[ i_ ] : value_type{};							}
		
	    constexpr iterator begin()		const noexcept	{	return data();					}
	    constexpr iterator end()		const noexcept	{	return begin() + size();		}
		
	    constexpr value_type front()	const noexcept	{	return operator[]( 0 );			}
	    constexpr value_type back()		const noexcept	{	return operator[]( size()-1 );	}

		constexpr bool empty()			const noexcept	{	return size() == 0;				}
 
		template< typename Itr > // Shorten by one if a string and not empty and ends in zero.
		static constexpr auto ender( const Itr b, const Itr e )
		{	return e - ( is_string && ( b != e ) && ( *( e - 1 ) == 0 ) );					}

 	    template< typename U >
	    constexpr bool operator==( const U & u_ )					const noexcept			{
	    	using std::begin, std::end;
			const auto b = begin( u_ );
			return std::equal( this->begin(), this->end(), b, ender( b, end( u_ ) ) );
	    }
		
 	    template< typename U >
	    constexpr auto operator<=>( const U & u_ )					const noexcept			{
	    	using std::begin, std::end;
			const auto b = begin( u_ );
	    	return std::lexicographical_compare( this->begin(), this->end(), b, ender( b, end( u_ ) ) );
	    }
		
		constexpr shadow first( const size_type n_ )				const noexcept			{
			return { data(), ( size() >= n_ ) ? n_ : size() };
		}
 
		constexpr shadow not_first( size_type n_ )					const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + n_, size() - n_ };
		}
 
		constexpr shadow last( size_type n_ )						const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + size() - n_, n_ };
		}
 
		constexpr shadow not_last( const size_type n_ )				const noexcept			{
			return shadow{ data(), size() - ( ( size() > n_ ) ? n_ : size() ) };
		}
 
		constexpr shadow part(
			size_type		 	offs_, 
			const size_type 	n_ = size()
		) const noexcept {
			offs_ = ( size() >= offs_ ) ? offs_ : size();
			return { data() + offs_, ( size() >= offs_ + n_ ) ? n_ : size() - offs_ };
		}

 	    template< typename U >
	    constexpr bool starts_with( const U & u_ )					const noexcept			{
			const shadow		vw( u_ );
			return first( vw.size() ) == vw;
		}

 	    template< typename U >
	    constexpr bool ends_with( const U & u_ )					const noexcept			{
			const shadow		vw( u_ );
			return last( vw.size() ) == vw;
		}
 
	    constexpr bool contains( const value_type t_ )				const noexcept			{
	    	return std::ranges::contains( this->begin(), this->end(), t_ );
	    }

 	    template< typename U >
	    constexpr bool contains( const U & u_ )						const noexcept			{
	    	using std::begin, std::end;
			const auto b = begin( u_ );
	    	return std::ranges::contains_subrange( this->begin(), this->end(), b, ender( b, end( u_ ) ) );
	    }
 
		template< typename Out >
		friend Out & operator<<( Out & out_, const base_shadow & shadow_ )
		{	return out_ << std20::format( "{}", shadow_ );									}
	};
	
	
	template< typename T >
	using shadow = base_shadow< core_shadow< T > >;

	template< typename T, std::size_t N >
	base_shadow( T ( & )[ N ] ) -> base_shadow< core_shadow< T > >;

	template< Contiguous_elements_object T >
	base_shadow( const T & ) -> base_shadow< core_shadow< typename T::data_type > >;




	template< Character Char, size_t N, typename Traits = std::char_traits< Char > >
	struct core_litteral {
		using value_type					  = std::remove_cv_t< Char >;
		using pointer						  = value_type const *;
		using size_type						  = std::size_t;
		using difference_type				  = std::ptrdiff_t;
		using traits_type					  = Traits;
		static constexpr size_type extent	  = N - 1u;

	    constexpr core_litteral( value_type const ( & str_ )[ N ] )	{	std::copy_n( str_, N, value );	}

		constexpr pointer data()				const noexcept	{	return value;					}
		static constexpr size_type size()		noexcept		{	return extent;					}
		
		value_type								value[ N ];
	};

	template< Character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	using litteral = base_shadow< core_litteral< Char, N, Traits > >;

	template< Character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	constexpr litteral< Char, N, Traits > litt( Char ( & source_ )[ N ] ) {	return { source_ };		}
	
}	// namespace pax

namespace std20 {
	template< typename T >		requires( pax::base_shadow< T >::is_string )
	struct formatter< pax::base_shadow< T > > : formatter< basic_string_view< typename T::value_type > > {
	    constexpr auto format( const pax::base_shadow< T > & shadow_, format_context & ctx_ )	const {
			// std::format_to_n( ctx_.out(), shadow_.size(), "{}", shadow_.data() );
			// return ctx_.out();
			return formatter< basic_string_view< typename T::value_type > >
				::format( basic_string_view{ shadow_.data(), shadow_.size() }, ctx_ );
	    }
	};

	template< typename T >
	struct formatter< pax::base_shadow< T > > : formatter< span< typename T::value_type > > {
	    constexpr auto format( const pax::base_shadow< T > & shadow_, format_context & ctx_ )	const {
			return formatter< span< typename T::value_type > >
				::format( span{ shadow_.data(), shadow_.size() }, ctx_ );
	    }
	};

	// template< typename T >
	// struct formatter< pax::base_shadow< T > > : formatter< T const * > {
	//     constexpr auto format( const pax::base_shadow< T > & str_, format_context & ctx_ )	const {
	// 		return formatter< T const * >::format( str_.value, ctx_ );
	//     }
	// };
}	// namespace std
