//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/concepts.hpp>		// traits::character
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.

// To Do:
// – Add statically sized variants of first, not_first, etc.
// – Precalculate N, to adapt it to char arrays with null suffix.


namespace pax {
	constexpr std::size_t dynamic_extent = traits::dynamic_extent;	// From concepts.hpp.

	template< traits::contiguous Str, std::size_t N = traits::extent_v< Str > >
	constexpr std::size_t shaved_extent	  = N - ( traits::character_array< Str > && N );
	
	/// Implements the core for span-like utilities. Static or dynamic size.
	/// Is a minimal std::ranges::contiguous_range.
	template< typename T, std::size_t N = dynamic_extent >
	struct range {
		using element_type				  = T;
		using value_type				  = std::remove_cv_t< element_type >;
		using pointer					  = element_type *;
		
		// Some types of strings may have a '\0' at the end that we want to ignore...
		static constexpr std::size_t 		extent = N 
			- ( traits::character< value_type > && std::is_const_v< element_type > && N ); 

	    constexpr range() noexcept {};
	    constexpr range( pointer src_ )		noexcept : m_source{ src_ } {}

		template< std::ranges::contiguous_range U >
			// requires( shaved_extent< U > == extent )
		constexpr range( U & src_ )			noexcept : range{ std::ranges::data( src_ ) } {}

		constexpr pointer data()			const noexcept	{	return m_source;			}
		static constexpr std::size_t size()	noexcept		{	return extent;				}
		constexpr pointer begin()			const noexcept	{	return data();				}
		constexpr pointer end()				const noexcept	{	return data() + size();		}
 
		template< std::size_t I >			requires( I < extent )
		friend element_type & get( const range & r_ )  noexcept	{	return *( r_.data() + I );	}

	private:
		pointer								m_source{};
	};

	template< typename T >
	struct range< T, dynamic_extent > {
		using element_type				  = T;
		using value_type				  = std::remove_cv_t< element_type >;
		using pointer					  = element_type *;
		static constexpr std::size_t 		extent = dynamic_extent;

	    constexpr range() noexcept {};

	    constexpr range( pointer src_, const std::size_t size_ ) noexcept 
			:	m_source{ src_ }, m_size{ src_ ? size_ : 0u } {}

	    constexpr range( pointer begin_, pointer end_ ) noexcept 
			:	range{ begin_, end_ - begin_ } {}

		template< std::ranges::contiguous_range U >
		constexpr range( U & src_ ) 		noexcept
			:	range{ std::ranges::data( src_ ), std::ranges::size( src_ ) } {}

		constexpr pointer data()			const noexcept	{	return m_source;			}
		constexpr std::size_t size()		const noexcept	{	return m_size;				}
		constexpr pointer begin()			const noexcept	{	return data();				}
		constexpr pointer end()				const noexcept	{	return data() + size();		}

	private:
		pointer								m_source{};
		std::size_t							m_size{};
	};

	template< typename T >
	range( T *, std::size_t )			 -> range< T, dynamic_extent >;

	template< typename T >
	range( T *, T * )					 -> range< T, dynamic_extent >;

	template< typename T, std::size_t N >
	range( T ( & )[ N ] )				 -> range< T, N >;

	template< std::ranges::contiguous_range Cont >
	range( Cont & )						 -> range< traits::element_type_t< Cont >, traits::extent_v< Cont > >;

	

	/// Implements the stuff you want for an interface to contiguous imutable elements.
	/// The interfaced is similar to that of std::string_view and std::span. 
	/// Core needs to implement data() and size() and define element_type, value_type, size_type, 
	/// and difference_type.
	template< typename Core >
	struct base_shadow : Core {
		using element_type				  = typename Core::element_type;
		using value_type				  = typename Core::value_type;
		using pointer					  = element_type *;
		using reference					  = element_type &;
		using iterator					  = pointer;
		using const_iterator			  = element_type const *;
		using size_type					  = std::size_t;
		using difference_type			  = std::ptrdiff_t;

		template< size_type N >
		using shadowN					  = base_shadow< range< element_type, N > >;
		using shadow					  = shadowN< dynamic_extent >;
		
		static constexpr size_type			extent{ Core::extent };
		static constexpr bool				is_static{ extent != dynamic_extent };
		static constexpr bool				is_string{ traits::character< value_type > };

	    using Core::Core, Core::data, Core::size;

		constexpr bool empty()				const noexcept	{	return size() == 0;				}

		/// Element access, possibly mutable if the element type is not const.
	    constexpr		iterator  begin()	const noexcept	{	return data();					}
	    constexpr		iterator  end()		const noexcept	{	return begin() + size();		}
	    constexpr const_iterator  cbegin()	const noexcept	{	return data();					}
	    constexpr const_iterator  cend()	const noexcept	{	return begin() + size();		}
	    constexpr		reference front()	const noexcept	{	return *data();					}
	    constexpr		reference back()	const noexcept	{	return operator[]( size()-1 );	}
	    constexpr		reference operator[]( const size_type i_ )	const noexcept
		{	return data()[ i_ ];																}

 	    template< typename U >
	    constexpr bool operator==( const U & u_ )						const noexcept
		{	return std::ranges::equal( *this, range{ u_ } );								    }
		
 	    template< typename U >
	    constexpr auto operator<=>( const U & u_ )						const noexcept			{
			const auto other = range{ u_ };
	    	// return std::ranges::lexicographical_compare_three_way( *this, range{ u_ } );
	    	return std::lexicographical_compare_three_way( this->begin(), this->end(), other.begin(), other.end() );
	    }
		
		constexpr shadow first( const size_type n_ )					const noexcept
		{	return { data(), ( size() >= n_ ) ? n_ : size() };									}
 
		template< std::size_t I >										requires( I != traits::dynamic_extent )
		[[nodiscard]] constexpr auto first()							const noexcept {
			assert( I <= size( v_ ) && "first< I >( v_ ) requires I <= size( v_ )." );
			return shadow< element_type, I >{ data( v_ ) };
		}

		template< std::size_t I >										requires( I != traits::dynamic_extent )
		[[nodiscard]] constexpr auto first() 							const noexcept {
			static constexpr std::size_t	sz  = std::min( I, traits::extent_v< V > );
			return shadow< element_type, I >( data( v_ ) );
		}

		constexpr shadow not_first( size_type n_ )						const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + n_, size() - n_ };
		}
 
		constexpr shadow last( size_type n_ )							const noexcept			{
			n_ = ( size() > n_ ) ? n_ : size();
			return shadow{ data() + size() - n_, n_ };
		}
 
		constexpr shadow not_last( const size_type n_ )					const noexcept
		{	return shadow{ data(), size() - ( ( size() > n_ ) ? n_ : size() ) };				}
 
		constexpr shadow part( difference_type offs_, const size_type n_ ) const noexcept	{
			offs_ =	( offs_ >= 0 )					  ? std::min( size_type(  offs_ ), size() ) 
				  :	( size_type( -offs_ ) < size() )  ? size()  - size_type( -offs_ ) : 0u;
			return { data() + offs_, ( size() >= offs_ + n_ ) ? n_ : size() - offs_ };
		}

 	    template< typename U >
	    constexpr bool starts_with( const U & u_ )						const noexcept			{
			const range		vw( u_ );
			return first( vw.size() ) == vw;
		}

 	    template< typename U >
	    constexpr bool ends_with( const U & u_ )						const noexcept			{
			const range		vw( u_ );
			return last( vw.size() ) == vw;
		}
 
	    constexpr bool contains( const value_type t_ )					const noexcept
		{	return std::ranges::contains( this->begin(), this->end(), t_ );				   		}

 	    template< typename U >
	    constexpr bool contains( const U & u_ )							const noexcept
		{	return std::ranges::contains_subrange( *this, range{ u_ } );					    }
 
		template< typename Out >
		friend Out & operator<<( Out & out_, const base_shadow & sh_ )							{
			if constexpr ( is_string )		return out_.write( sh_.data(), sh_.size() );
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
	
	template< typename T, std::size_t N = dynamic_extent >
	using shadow = base_shadow< range< T, N > >;

	template< typename T >
	base_shadow( T *, std::size_t )		 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T >
	base_shadow( T *, T * )				 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T, std::size_t N >
	base_shadow( T ( & )[ N ] )			 -> base_shadow< range< T, N > >;

	template< std::ranges::contiguous_range Cont >
	base_shadow( Cont & )				 -> base_shadow< range< traits::element_type_t< Cont >, traits::extent_v< Cont > > >;

	/// Return a shadow instance with const (unmutable) elements. 
	template< typename ...Args >
	auto const_shadow( Args && ...args_ ) {
		using type = decltype( shadow( std::forward< Args >( args_ )... ) );
		return shadow< const typename type::element_type, type::extent >( std::forward< Args >( args_ )... );
	}


	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	struct core_litteral {
		using element_type				  = Char;
		using value_type				  = std::remove_cv_t< element_type >;
		using pointer					  = element_type *;
		using traits_type				  = Traits;
		static constexpr std::size_t 		extent = N;

	    constexpr core_litteral( element_type ( & str_ )[ N ] )	{	std::copy_n( str_, N, value );	}

		constexpr pointer data()			const noexcept	{	return value;					}
		static constexpr std::size_t size()	noexcept		{	return N - ( N > 0 );			}
		constexpr pointer begin()			const noexcept	{	return data();					}
		constexpr pointer end()				const noexcept	{	return data() + size();			}
 
		template< std::size_t I >			requires( ( I < extent ) && ( extent != dynamic_extent ) )
		friend element_type & get( const core_litteral & cl_ )  noexcept	{	return *( cl_.data() + I );	}
		
		value_type							value[ N ];
	};

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	using litteral = base_shadow< core_litteral< Char, N, Traits > >;

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	constexpr litteral< Char, N, Traits > litt( Char ( & src_ )[ N ] ) {	return { src_ };	}


	template< typename Tag, typename T >	struct Tagged;

	template< typename Char, size_t N >
	constexpr Tagged< struct general, litteral< Char, N > > tagged( Char ( & str_ )[ N ] )
	{	return { str_ };							}

	template< typename Tag, typename Char, size_t N >
	constexpr Tagged< Tag, litteral< Char, N > > tagged( Char ( & str_ )[ N ] )
	{	return { str_ };							}
	
}	// namespace pax
