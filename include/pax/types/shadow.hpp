//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/concepts.hpp>		// traits::character
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <assert.h>

// To Do:
// – Add statically sized variants of first, not_first, etc.
// – Precalculate N, to adapt it to char arrays with null suffix.


/// Shadow: string_view and span in one
/**	Test the feasability of having one class for all uses of string_view and span, both static as well as dynamic sizesing. And a variant can also be used for text in template parameters. It consists of a common base class for most of the functionality and small classes with specialisations for handling [the references to] data. A main idea is to keep the number of member functions limited and "basic" and they are all unmutable. The data they reference is mutable, unless the element type is const. Functions such as find(), contains(), etc. can be implemented by external functions. Note for instance that by having first(), not_first(), last(), not_last(), and part() as member functions many variands of string_view::find() are not needed directly. Other member functions are the iterators, comparisons and output. 

**/


namespace pax {
	constexpr std::size_t dynamic_extent = traits::dynamic_extent;	// From concepts.hpp.

	template< typename T >
	[[nodiscard]] constexpr auto no_nullchar_end( const T & t_ ) {
		using std::begin, std::size;
		return begin( t_ ) + size( t_ );
	}

	template< traits::character Char, std::size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Char const * no_nullchar_end( Char const ( & str_ )[ N ] )
	{	return str_ + N - !str_[ N - 1 ];													}
	

	/// Implements the core for span-like utilities. Static or dynamic size.
	/// Is a minimal std::ranges::contiguous_range.
	template< typename T, std::size_t N = dynamic_extent >
	struct range {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		
		// Some types of strings may have a '\0' at the end that we want to ignore...
		static constexpr std::size_t 					extent = N 
			- ( traits::character< value_type > && std::is_const_v< element_type > && N ); 

	    constexpr range() noexcept {};
	    constexpr range( pointer src_ )					noexcept : m_source{ src_ } {}

		template< std::ranges::contiguous_range U >
		constexpr range( U & src_ )						noexcept : range{ std::ranges::data( src_ ) } {}

		constexpr pointer data()						const noexcept	{	return m_source;			}
		static constexpr std::size_t size()				noexcept		{	return extent;				}
		constexpr pointer begin()						const noexcept	{	return data();				}
		constexpr pointer end()							const noexcept	{	return data() + size();		}
 
		template< std::size_t I >						requires( I < extent )
		friend element_type & get( const range & r_ )	noexcept	{	return *( r_.data() + I );	}

	private:
		pointer											m_source{};
	};

	template< typename T >
	struct range< T, dynamic_extent > {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		static constexpr std::size_t 					extent = dynamic_extent;

	    constexpr range() noexcept {};

	    constexpr range( pointer src_, const std::size_t size_ ) noexcept 
			:	m_source{ src_ }, m_size{ src_ ? size_ : 0u } {}

	    constexpr range( pointer begin_, pointer end_ )	noexcept : range{ begin_, end_ - begin_ } {}

		template< std::ranges::contiguous_range U >
		constexpr range( U & src_ ) noexcept : range{ std::ranges::data( src_ ), std::ranges::size( src_ ) } {}

		constexpr pointer data()						const noexcept	{	return m_source;				}
		constexpr std::size_t size()					const noexcept	{	return m_size;					}
		constexpr pointer begin()						const noexcept	{	return data();					}
		constexpr pointer end()							const noexcept	{	return data() + size();			}

	private:
		pointer											m_source{};
		std::size_t										m_size{};
	};

	template< typename T >
	range( T *, std::size_t )	 -> range< T, dynamic_extent >;

	template< typename T >
	range( T *, T * )			 -> range< T, dynamic_extent >;

	template< typename T, std::size_t N >
	range( T ( & )[ N ] )		 -> range< T, N >;

	// template< traits::character T, std::size_t N >
	// range( T const ( & )[ N ] )	 -> range< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	range( Cont & )				 -> range< traits::element_type_t< Cont >, traits::extent_v< Cont > >;

	

	/// Implements the stuff you want for an interface to contiguous imutable elements.
	/// The interfaced is similar to that of std::string_view and std::span. 
	/// Core needs to implement data() and size() and define element_type, value_type, size_type, 
	/// and difference_type.
	template< typename Core >
	struct base_shadow : public Core {
		using element_type							  = typename Core::element_type;
		using value_type							  = typename Core::value_type;
		using pointer								  = element_type *;
		using reference								  = element_type &;
		using iterator								  = pointer;
		using const_iterator						  = element_type const *;
		using size_type								  = std::size_t;
		using difference_type						  = std::ptrdiff_t;

		template< size_type N >
		using shadowN								  = base_shadow< range< element_type, N > >;
		using shadow								  = shadowN< dynamic_extent >;
		
		static constexpr size_type						extent{ Core::extent };
		static constexpr bool							is_static{ extent != dynamic_extent };
		static constexpr bool							is_string{ traits::character< value_type > };

	    using Core::Core, Core::data, Core::size;

		constexpr bool empty()							const noexcept	{	return size() == 0;				}
		constexpr explicit operator bool()				const noexcept	{	return empty();					}

		/// Element access, possibly mutable if the element type is not const.
	    constexpr		iterator  begin()				const noexcept	{	return data();					}
	    constexpr		iterator  end()					const noexcept	{	return begin() + size();		}
	    constexpr const_iterator  cbegin()				const noexcept	{	return data();					}
	    constexpr const_iterator  cend()				const noexcept	{	return begin() + size();		}
	    constexpr		reference front()				const noexcept	{	return *data();					}
	    constexpr		reference back()				const noexcept	{	return operator[]( size()-1 );	}
	    constexpr		reference operator[]( const size_type i_ ) const noexcept {	return data()[ i_ ];	}

		/// Utility function that returns true iff a string and the last character is \0.
		constexpr auto shave_zero_suffix_end()			const noexcept	{
			if constexpr(  is_static && is_string && extent )				return end() - !back();
			else 															return end();
		}

		/// If this is a string and the last character is \0 it is ignored. 
 	    template< typename U >
	    constexpr bool operator==( const U & u_ )		const noexcept	{
			using std::begin, std::size;
			return std::equal( this->begin(), shave_zero_suffix_end(), begin( u_ ), no_nullchar_end( u_ ) );
		}
		
		/// If this is a string and the last character is \0 it is ignored. 
 	    template< typename U >
	    constexpr auto operator<=>( const U & u_ )		const noexcept	{
			using std::begin;
	    	return std::lexicographical_compare_three_way( this->begin(), shave_zero_suffix_end(), 
														begin( u_ ), no_nullchar_end( u_ ) );
	    }

		/// Return a dynamic shadow of the first min(n_, size()) elements. 
		constexpr shadow first( const size_type n_ )	const noexcept	{
			return { data(), ( size() >= n_ ) ? n_ : size() };
		}
 
		/// Return a static shadow of the first N elements. Ub, if N > size().
		template< std::size_t N >		requires( !is_static && ( N != traits::dynamic_extent ) )
		[[nodiscard]] constexpr auto first()			const noexcept	{
			assert( N <= size() && "first< N >() requires N <= size()." );
			return base_shadow< range< element_type, N + ( is_string && is_static ) > >{ data() };
		}

		/// Return a static shadow of the first min(N, extent) elements. 
		template< std::size_t N >		requires(  is_static && ( N != traits::dynamic_extent ) )
		[[nodiscard]] constexpr auto first() 			const noexcept	{
			static constexpr std::size_t	sz = ( N < extent ) ? N : extent;
			return base_shadow< range< element_type, sz + is_string > >( data() );
		}

		/// Return a dynamic shadow of the last size() - min(n_, size()) elements. 
		constexpr shadow not_first( size_type n_ )		const noexcept	{
			n_ = ( size() > n_ ) ? n_ : size();
			return { data() + n_, size() - n_ };
		}

		/// Return a static shadow of the last size() - min(N, extent) elements. 
		template< std::size_t N >		requires(  is_static )
		[[nodiscard]] constexpr auto not_first() 		const noexcept	{
			return last< extent - ( ( N < extent ) ? N : extent ) >();
		}

		/// Return a dynamic shadow of the last min(n_, size()) elements. 
		constexpr shadow last( size_type n_ )			const noexcept	{
			n_ = ( size() > n_ ) ? n_ : size();
			return { data() + size() - n_, n_ };
		}
 
		/// Return a static shadow of the last N elements. Ub, if N > size().
		template< std::size_t N >		requires( !is_static && ( N != traits::dynamic_extent ) )
		[[nodiscard]] constexpr auto last()				const noexcept	{
			assert( N <= size() && "last< N >() requires N <= size()." );
			return base_shadow< range< element_type, N > >( data() + size() - N );
		}

		/// Return a static shadow of the first min(N, extent) elements. 
		template< std::size_t N >		requires(  is_static && ( N != traits::dynamic_extent ) )
		[[nodiscard]] constexpr auto last() 			const noexcept	{
			static constexpr std::size_t	sz = extent;
			static constexpr std::size_t	offset = ( N < sz ) ? sz - N : 0u;
			return base_shadow< range< element_type, sz + is_string - offset > >( data() + offset );
		}
 
		/// Return a dynamic shadow of the first size() - min(n_, size()) elements. 
		constexpr shadow not_last( const size_type n_ )	const noexcept	{
			return { data(), ( size() > n_ ) ? size() - n_ : 0u };
		}

		/// Return a static shadow of the first size() - min(N, extent) elements. 
		template< std::size_t N >		requires( traits::has_extent< base_shadow > )
		[[nodiscard]] constexpr auto not_last() 		const noexcept 	{
			return first< ( extent > N ) ? extent - N : 0 >();
		}

		/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
		/// A negative offs_ is counted from the back. 
		constexpr shadow part( difference_type offs_, const size_type n_ ) const noexcept	{
			offs_ =	( offs_ >= 0 )					  ? std::min( size_type(  offs_ ), size() ) 
				  :	( size_type( -offs_ ) < size() )  ? size()  - size_type( -offs_ ) : 0u;
			return { data() + offs_, ( size() >= offs_ + n_ ) ? n_ : size() - offs_ };
		}

		/// Return true if u_ equals the first elements of this.
 	    template< typename U >
	    constexpr bool starts_with( const U & u_ )		const noexcept	{
			const range		vw( u_ );
			return first( vw.size() ) == vw;
		}

		/// Return true if u_ equals the last elements of this.
 	    template< typename U >
	    constexpr bool ends_with( const U & u_ )		const noexcept	{
			const range		vw( u_ );
			return last( vw.size() ) == vw;
		}
 
	    constexpr bool contains( const value_type t_ )	const noexcept	{
			return std::ranges::contains( begin(), end(), t_ );
		}

 	    template< typename U >
	    constexpr bool contains( const U & u_ )			const noexcept	{
			return std::ranges::contains_subrange( *this, range{ u_ } );
		}
 
		/// Stream all elements to out_.
		template< typename Out >
		friend Out & operator<<( Out & out_, const base_shadow & sh_ )	{
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
	base_shadow( T *, std::size_t )	 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T >
	base_shadow( T *, T * )			 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T, std::size_t N >
	base_shadow( T ( & )[ N ] )		 -> base_shadow< range< T, N > >;

	// template< traits::character T, std::size_t N >
	// base_shadow( T const ( & )[ N ] ) -> base_shadow< range< T const, N-(N>0) > >;

	template< std::ranges::contiguous_range Cont >
	base_shadow( Cont & )			 -> base_shadow< range< traits::element_type_t< Cont >, traits::extent_v< Cont > > >;

	/// Return a shadow instance with const (unmutable) elements. 
	template< typename ...Args >
	auto const_shadow( Args && ...args_ ) {
		using type = decltype( shadow( std::forward< Args >( args_ )... ) );
		return shadow< const typename type::element_type, type::extent >( std::forward< Args >( args_ )... );
	}


	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	struct core_litteral {
		using element_type							  = Char;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using traits_type							  = Traits;
		static constexpr std::size_t			 		extent = N;

	    constexpr core_litteral( value_type       ( & str_ )[  N  ] )	{	std::copy_n( str_, N, value );	}
	    constexpr core_litteral( value_type const ( & str_ )[ N+1 ] )	{	std::copy_n( str_, N, value );	}

		constexpr pointer data()						const noexcept	{	return value;					}
		static constexpr std::size_t size()				noexcept		{	return N;						}
		constexpr pointer begin()						const noexcept	{	return data();					}
		constexpr pointer end()							const noexcept	{	return data() + size();			}
 
		template< std::size_t I >			requires( ( I < extent ) && ( extent != dynamic_extent ) )
		friend element_type & get( const core_litteral & cl_ ) noexcept	{	return *( cl_.data() + I );		}
		
		value_type							value[ N ];
	};

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	using litteral = base_shadow< core_litteral< Char, N, Traits > >;

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	constexpr litteral< Char, N, Traits > litt( Char ( & src_ )[ N ] )	{	return { src_ };				}

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< const Char > >
	constexpr litteral< Char const, N-(N>0), Traits > litt( Char const ( & src_ )[ N ] ) {	return { src_ };	}


	template< typename Tag, typename T >	struct Tagged;

	template< traits::character Char, size_t N >
	constexpr Tagged< struct general, litteral< Char, N > > tagged( Char ( & str_ )[ N ] )	{
		return { str_ };
	}

	template< traits::character Char, size_t N >
	constexpr Tagged< struct general, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( ( N == 0 ) || !str_[ N-1 ] && "Removing a zero character suffix that isn't zero!." );
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >
	constexpr Tagged< Tag, litteral< Char, N > > tagged( Char ( & str_ )[ N ] )	{
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >
	constexpr Tagged< Tag, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( ( N == 0 ) || !str_[ N-1 ] && "Removing a zero character suffix that isn't zero!." );
		return { str_ };
	}
	
}	// namespace pax
