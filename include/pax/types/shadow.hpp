//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/concepts.hpp>		// Character
#include <algorithm>			// std::equal, std::lexicographical_compare_three_way


// To do:
// – Implement a struct hard_shadow, where the size is static. 


namespace pax {
	
	/// Implements the core for span-like utilities.
	template< typename T >
	struct soft_shadow {
		using element_type		  = T;
		using value_type		  = std::remove_cv_t< element_type >;
		using pointer			  = value_type const *;

	    constexpr soft_shadow()		noexcept {};

	    constexpr soft_shadow( const pointer source_, const std::size_t size_ ) noexcept 
			:	m_source{ source_ }, m_size{ source_ ? size_ : 0u } {}

		template< std::size_t N >
	    constexpr soft_shadow( value_type const ( & source_ )[ N ] ) noexcept
			:	soft_shadow{ source_, N - ( Character< value_type > && N>0 && !source_[ N - 1 ] ) } {}

		template< Contiguous_elements_object U >
		constexpr soft_shadow( const U & source_ ) noexcept 
			:	soft_shadow{ get_data( source_ ), get_size( source_ ) } {}

		constexpr auto data()		const noexcept	{	return m_source;				}
		constexpr auto size()		const noexcept	{	return m_size;					}

	private:
		pointer						m_source{};
		std::size_t					m_size{};
	};

	

	/// Implements the stuff you want for an interface to contiguous imutable elements.
	/// The interfaced is similar to that of std::string_view and std::span. 
	/// Core needs to implement data() and size() and define element_type, value_type, size_type, 
	/// and difference_type.
	template< typename Core >
	struct base_shadow : Core {
		using element_type			  = typename Core::element_type;
		using value_type			  = typename Core::value_type;
		using pointer				  = value_type const *;
		using iterator				  = pointer;
		using size_type				  = std::size_t;
		using difference_type		  = std::ptrdiff_t;
		using shadow				  = base_shadow< soft_shadow< element_type > >;
		
		static constexpr bool			is_string{ Character< value_type > };

	    using Core::Core, Core::data, Core::size;
 
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
	    	return std::lexicographical_compare_three_way( this->begin(), this->end(), b, ender( b, end( u_ ) ) );
	    }
		
		constexpr shadow first( const size_type n_ )				const noexcept
		{	return { data(), ( size() >= n_ ) ? n_ : size() };								}
 
		constexpr shadow not_first( size_type n_ )					const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + n_, size() - n_ };
		}
 
		constexpr shadow last( size_type n_ )						const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + size() - n_, n_ };
		}
 
		constexpr shadow not_last( const size_type n_ )				const noexcept
		{	return shadow{ data(), size() - ( ( size() > n_ ) ? n_ : size() ) };			}
 
		constexpr shadow part( difference_type offs_, const size_type n_ ) const noexcept	{
			offs_ =	( offs_ >= 0 )					  ? std::min( size_type(  offs_ ), size() ) 
				  :	( size_type( -offs_ ) < size() )  ? size()  - size_type( -offs_ ) : 0u;
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
 
	    constexpr bool contains( const value_type t_ )				const noexcept
		{	return std::ranges::contains( this->begin(), this->end(), t_ );				    }

 	    template< typename U >
	    constexpr bool contains( const U & u_ )						const noexcept			{
	    	using std::begin, std::end;
			const auto b = begin( u_ );
	    	return std::ranges::contains_subrange( this->begin(), this->end(), b, ender( b, end( u_ ) ) );
	    }
 
		template< typename Out >
		friend Out & operator<<( Out & out_, const base_shadow & sh_ )						{
			if constexpr ( is_string )
				return out_.write( sh_.data(), sh_.size() );
			else {
				bool comma = false;
				out_ << '[';
				for( const auto & item : sh_ ) {
					out_ << ( comma ? ", " : "" ) << item;
					comma = true;
				}
				return out_ << ']';
			}
		}
	};
	
	template< typename T >
	using shadow = base_shadow< soft_shadow< T > >;

	template< typename T, std::size_t N >
	base_shadow( T ( & )[ N ] ) -> base_shadow< soft_shadow< T > >;

	template< typename T >
	base_shadow( T const *, std::size_t ) -> base_shadow< soft_shadow< T > >;

	template< Contiguous_elements_object Cont >
	base_shadow( const Cont & ) -> base_shadow< soft_shadow< typename Cont::data_type > >;



	template< Character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	struct core_litteral {
		using element_type					  = Char;
		using value_type					  = std::remove_cv_t< element_type >;
		using pointer						  = value_type const *;
		using traits_type					  = Traits;

	    constexpr core_litteral( value_type const ( & str_ )[ N ] )	{	std::copy_n( str_, N, value );	}

		constexpr pointer data()				const noexcept	{	return value;					}
		static constexpr std::size_t size()		noexcept		{	return N - ( N > 0 );			}
		
		value_type								value[ N ];
	};

	template< Character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	using litteral = base_shadow< core_litteral< Char, N, Traits > >;

	template< Character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	constexpr litteral< Char, N, Traits > litt( Char ( & source_ )[ N ] ) {	return { source_ };		}


	template< typename Tag, typename T >	struct Tagged;

	template< typename Char, size_t N >
	constexpr Tagged< struct general, litteral< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}

	template< typename Tag, typename Char, size_t N >
	constexpr Tagged< Tag, litteral< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}
	
}	// namespace pax
