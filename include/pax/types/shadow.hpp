//	Copyright (c) 2014-2026, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/concepts.hpp>		// traits:: stuff.
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <iterator>				// std::reverse_iterator.
#include <assert.h>

/// shadow: string_view and span in one
///	A class for all uses of string_view and span with static or dynamic size. It consists of a common base class 
///	for most of the functionality and small classes implementing static or dynamic size -- and a variant usable for 
///	template parameter text. The main idea is to keep member functions basic, unmutable, and with a minimum of ub. 
///	The referenced data is mutable (unless declared const). 

namespace pax {
	constexpr std::size_t dynamic_extent = traits::dynamic_extent;

	template< typename T >
	[[nodiscard]] constexpr auto no_nullchar_end( const T & t_ )	{	using std::end;	return end( t_ );	}

	template< traits::character Char, std::size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Char const * no_nullchar_end( Char const ( & str_ )[ N ] )	{
		return str_ + N - !str_[ N - 1 ];
	}


	/// The core for span-like utilities of static or dynamic size. A minimal std::ranges::contiguous_range.
	template< typename T, std::size_t N = dynamic_extent >
	struct range {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		static constexpr std::size_t 					extent = N;

		constexpr range()								noexcept = default;
		constexpr range( const range & )				noexcept = default;
		constexpr range & operator=( const range & )	noexcept = default;
		constexpr range( pointer src_ )					noexcept : m_source{ src_ } {}

		template< typename U >
		constexpr range( const U ( & str_ )[ N+1 ] )	noexcept // Somewhat constructed due to disambiguation:
			requires( std::is_same_v< U, value_type > && traits::character< value_type > )
			:	m_source{ str_ }	{	assert( !str_[ N ] && "Removing a non-zero character suffix!" );	}

		template< std::ranges::contiguous_range U >
		constexpr range( U & src_ )						noexcept : m_source{ std::ranges::data( src_ ) } {}

		[[nodiscard]] constexpr pointer data()			const noexcept	{	return m_source;				}
		[[nodiscard]] static constexpr std::size_t size()	  noexcept	{	return extent;					}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();					}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();			}

		template< std::size_t I >						requires( I < extent )
		[[nodiscard]] friend element_type & get( const range & r_ )	noexcept {	return *( r_.data() + I );	}

	private:
		pointer											m_source{ nullptr };
	};

	template< typename T >
	struct range< T, dynamic_extent > {
		using element_type							  = T;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		static constexpr std::size_t 					extent = dynamic_extent;

		constexpr range()								noexcept = default;
		constexpr range( const range & )				noexcept = default;
		constexpr range & operator=( const range & )	noexcept = default;
		constexpr range( pointer src_, std::size_t sz )	noexcept : m_source{ src_ }, m_size{ src_ ? sz : 0u } {}
		constexpr range( pointer begin_, pointer end_ )	noexcept : range{ begin_, std::size_t( end_ - begin_ ) } {}

		template< std::ranges::contiguous_range U >
		constexpr range( U & src_ ) noexcept : range{ std::ranges::data( src_ ), std::ranges::size( src_ ) } {}

		[[nodiscard]] constexpr pointer data()			const noexcept	{	return m_source;				}
		[[nodiscard]] constexpr std::size_t size()		const noexcept	{	return m_size;					}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();					}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();			}

	private:
		pointer											m_source{ nullptr };
		std::size_t										m_size{};
	};

	template< typename T >
	range( T *, std::size_t )	 -> range< T, dynamic_extent >;

	template< typename T >
	range( T *, T * )			 -> range< T, dynamic_extent >;

	template< typename T, std::size_t N >
	range( T ( & )[ N ] )		 -> range< T, N >;

	template< traits::character T, std::size_t N >
	range( T const ( & )[ N ] )	 -> range< T const, N-(N>0) >;

	template< std::ranges::contiguous_range Cont >
	range( Cont & )				 -> range< traits::element_type_t< Cont >, traits::extent_v< Cont > >;



	/// Implements most of the functionality of a class referensing contiguous elements.
	/// The interface is similar to that of std::string_view and std::span.
	/// Core must implement data(), size(), and define element_type and extent (possibly = dynamic_extent).
	template< typename Core >
	struct base_shadow : public Core {
		using element_type							  = typename Core::element_type;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using reference								  = element_type &;
		using iterator								  = pointer;
		using const_iterator						  = element_type const *;
		using reverse_iterator						  = std::reverse_iterator< iterator >;
		using const_reverse_iterator				  = std::reverse_iterator< const_iterator >;
		using size_type								  = std::size_t;
		using difference_type						  = std::ptrdiff_t;

		template< size_type N >
		using shadowN								  = base_shadow< range< element_type, N > >;
		using shadow								  = shadowN< dynamic_extent >;
		using pair									  = std::pair< shadow, shadow >;

		static constexpr size_type						extent{ Core::extent };
		static constexpr bool							is_static{ extent != dynamic_extent };
		static constexpr bool							is_string{ traits::character< value_type > };

		using Core::Core, Core::data, Core::size;

		[[nodiscard]] constexpr bool valid()			const noexcept	{	return  data() != nullptr;				}
		[[nodiscard]] constexpr bool empty()			const noexcept	{	return !size();							}
		[[nodiscard]] constexpr explicit operator bool()const noexcept	{	return  size();							}

		/// Element access (mutable if element type is not const).
		[[nodiscard]] constexpr iterator begin()		const noexcept	{	return data();							}
		[[nodiscard]] constexpr iterator end()			const noexcept	{	return begin() + size();				}
		[[nodiscard]] constexpr const_iterator cbegin()	const noexcept	{	return begin();							}
		[[nodiscard]] constexpr const_iterator cend()	const noexcept	{	return end();							}
		[[nodiscard]] constexpr auto rbegin()			const noexcept	{	return std::reverse_iterator( end() );	}
		[[nodiscard]] constexpr auto rend()				const noexcept	{	return std::reverse_iterator( begin() );}
		[[nodiscard]] constexpr auto crbegin()			const noexcept	{	return rbegin();						}
		[[nodiscard]] constexpr auto crend()			const noexcept	{	return rend();							}
		[[nodiscard]] constexpr reference front()		const noexcept	{	return at( 0u );						}
		[[nodiscard]] constexpr reference back()		const noexcept	{	return at( size() - 1 );				}
		[[nodiscard]] constexpr reference operator[]( const size_type i_ ) const noexcept { return data()[ i_ ]; 	}
		[[nodiscard]] constexpr reference at( const size_type i_ ) const noexcept {
			assert( i_ < size() && "Index out of range." );
			return data()[ i_ ];
		}

		/// In strings a terminating \0 is ignored.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool operator==( const U & u_ )				const noexcept	{
			using std::begin;
			return std::equal(	this->begin(), this->end(), begin( u_ ), no_nullchar_end( u_ ) );
		}

		/// In strings a terminating \0 is ignored.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr auto operator<=>( const U & u_ )			const noexcept	{
			using std::begin;
			return std::lexicographical_compare_three_way(
								this->begin(), this->end(), begin( u_ ), no_nullchar_end( u_ ) );
		}

		/// Return a dynamic shadow of the first min(n_, size()) elements.
		[[nodiscard]] constexpr auto first( const size_type n_ )			const noexcept	{
			return shadow{ data(), std::min( n_, size() ) };
		}

		/// Return a static shadow of the first min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N >		requires( N != traits::dynamic_extent )
		[[nodiscard]] constexpr auto first() 								const noexcept	{
			if constexpr( !is_static )	assert( N <= size() && "first< N >() requires N <= size()." );
			return shadowN< std::min( N, extent ) >( data() );
		}

		/// Return a dynamic shadow of the last size() - min(n_, size()) elements.
		[[nodiscard]] constexpr auto not_first( size_type n_ = 1 )			const noexcept	{
			n_ = std::min( n_, size() );
			return shadow{ data() + n_, size() - n_ };
		}

		/// Return a static shadow of the last size() - min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N >		requires( is_static )
		[[nodiscard]] constexpr auto not_first() 							const noexcept	{
			return last< extent - std::min( N, extent ) >();
		}

		/// Return a dynamic shadow of the last min(n_, size()) elements.
		[[nodiscard]] constexpr auto last( size_type n_ )					const noexcept	{
			n_ = std::min( n_, size() );
			return shadow{ data() + size() - n_, n_ };
		}

		/// Return a static shadow of the first min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N >		requires( N != traits::dynamic_extent )
		[[nodiscard]] constexpr auto last() 								const noexcept	{
			if constexpr( !is_static )	assert( N <= size() && "last< N >() requires N <= size()." );
			return shadowN< std::min( N, extent ) >{ data() + size() - std::min( N, size() ) };
		}

		/// Return a dynamic shadow of the first size() - min(n_, size()) elements.
		[[nodiscard]] constexpr auto not_last( const size_type n_ = 1 )		const noexcept	{
			return shadow{ data(), size() - std::min( n_, size() ) };
		}

		/// Return a static shadow of the first size() - min(N, extent) elements.
		/// Does assert( N <= size() && !is_static ).
		template< std::size_t N >		requires( traits::has_extent< base_shadow > )
		[[nodiscard]] constexpr auto not_last() 							const noexcept 	{
			return first< extent - std::min( N, size() ) >();
		}

		/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
		/// A negative offs_ is counted from the end.
		[[nodiscard]] constexpr auto mid( difference_type offs_, const size_type n_ )	const noexcept	{
			offs_ =	( offs_ >= 0 )	?		   std::min( size_type(  offs_ ), size() )
									: size() - std::min( size_type( -offs_ ), size() );
			return shadow{ data() + offs_, std::min( size() - offs_, n_ ) };
		}

		/// Return a static shadow of the N elements starting with offs_, but restricted to the bounds of this.
		/// A negative offs_ is counted from the back. Does assert( offs_ + N <= size() ).
		template< std::size_t N >		requires( N != traits::dynamic_extent && N <= extent )
		[[nodiscard]] constexpr auto mid( difference_type offs_ )			const noexcept	{
			offs_ =	( offs_ >= 0 )	?		   std::min( size_type(  offs_ ), size() )
									: size() - std::min( size_type( -offs_ ), size() );
			assert( offs_ + N <= size() && "mid< N >() requires offs_ + N <= size()." );
			return shadowN< N >{ data() + offs_ };
		}

		/// Return true iff u_ equals the first elements of this.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool starts_with( const U & u_ )			const noexcept	{
			return std::ranges::starts_with( *this, range( u_ ) );
		}

		/// Return true iff u_ equals the last elements of this.
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr bool ends_with( const U & u_ )				const noexcept	{
			return std::ranges::ends_with( *this, range( u_ ) );
		}

		/// Return a shadow of where t_ is -- or a zereo-sized shadow located at end().
		[[nodiscard]] constexpr shadow find( const value_type t_ )			const noexcept	{
			const auto result = std::find( begin(), end(), t_ );
			return { result, result != end() };
		}

		/// Return a shadow of where u_ is -- or a zereo-sized shadow located at end().
 		template< std::ranges::contiguous_range U >
		[[nodiscard]] constexpr shadow find( const U & u_ )					const noexcept	{
			const range		vw( u_ );
			const auto result = std::search( begin(), end(), vw.begin(), vw.end() );
			return { result, ( result == end() ) ? 0u : vw.size() };
		}

		/// Return a shadow of the first contigous range where all Test( value ) are true.
		/// If none is found, { end(), 0u } is returned.
		template< typename Test >	requires std::is_invocable_r_v< bool, Test, value_type >
		[[nodiscard]] constexpr shadow find( Test && test_ )				const noexcept	{
			const auto	b = std::ranges::find_if( *this, test_ );
			auto 		e = b;
			while( ( e != end() ) && test_( *e ) ) 	++e;
			return { b, e };
		}

		/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
		/// If none is found, { end(), 0u } is returned.
		[[nodiscard]] constexpr shadow find_linebreak()						const noexcept
		requires( is_string ) {
			value_type		previous{};
			for( element_type & c : *this ) switch( previous ) {
				case '\n': 	return { &c - 1, 1u + ( c == '\r' ) };
				case '\r': 	return { &c - 1, 1u + ( c == '\n' ) };
				default: 	previous = c;	break;
			}
			return { end(), end() };
		};

		/// Split this in two at offset t_ so that first.end() == second.begin() and first.size() == t_.
		[[nodiscard]] constexpr pair split( size_type mid_ )				const noexcept	{
			mid_	  = std::min( mid_, size() );
			return { { begin(), mid_ }, { begin() + mid_, end() } };
		}

		/// Split this in two: before and after gap_, but everything clamped to [begin(), end()].
		template< size_type I >
		[[nodiscard]] constexpr pair split( const shadowN< I > gap_ )		const noexcept	{
			return { { begin(), std::clamp( gap_.begin(), begin(), end() ) },
							  { std::clamp( gap_.end(),   begin(), end() ), end() } };
		}

		/// Stream the elements to out_.
		template< typename Out >
		friend Out & operator<<( Out & out_, const base_shadow & sh_ )	{
			if constexpr( is_string )	out_.write( sh_.data(), sh_.size() );
			else if( sh_.empty() )		out_ << "[]";
			else {
				out_ << '[' << sh_.front();
				for( const auto & item : sh_.not_first() )	out_ << ", " << item;
				out_ << ']';
			}
			return out_;
		}
 	};

	template< typename T, std::size_t N = dynamic_extent >
	using shadow = base_shadow< range< T, N > >;

	template< typename T >
	base_shadow( T *, std::size_t )	 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T >
	base_shadow( T *, T * )			 -> base_shadow< range< T, dynamic_extent > >;

	template< typename T, std::size_t N >
	base_shadow( T( & )[ N ] )		 -> base_shadow< range< T, N > >;

	template< traits::character T, std::size_t N >
	base_shadow( T const( & )[ N ] ) -> base_shadow< range< T const, N-(N>0) > >;

	template< std::ranges::contiguous_range Cont >
	base_shadow( Cont & )			 -> base_shadow< range< traits::element_type_t< Cont >, traits::extent_v< Cont > > >;

	/// Return a shadow instance with const (unmutable) elements.
	template< typename ...Args >
	[[nodiscard]] auto const_shadow( Args && ...args_ ) {
		using type = decltype( shadow( std::forward< Args >( args_ )... ) );
		return shadow< const typename type::element_type, type::extent >( std::forward< Args >( args_ )... );
	}


	/// The core for a string_view-like type that can be used as a template argument.
	/// The alias litteral defines the actual type and the function litt creates it. 
	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	struct core_litteral {
		using element_type							  = Char;
		using value_type							  = std::remove_cv_t< element_type >;
		using pointer								  = element_type *;
		using traits_type							  = Traits;
		static constexpr std::size_t			 		extent = N;

		constexpr core_litteral( value_type       ( & str_ )[  N  ] )	{	std::copy_n( str_, N, value );		}
		constexpr core_litteral( value_type const ( & str_ )[ N+1 ] )	{	std::copy_n( str_, N, value );		}

		[[nodiscard]] constexpr pointer data()			const noexcept	{	return value;						}
		[[nodiscard]] static constexpr std::size_t size()	  noexcept	{	return N;							}
		[[nodiscard]] constexpr pointer begin()			const noexcept	{	return data();						}
		[[nodiscard]] constexpr pointer end()			const noexcept	{	return data() + size();				}

		template< std::size_t I >			requires( ( I < extent ) && ( extent != dynamic_extent ) )
		[[nodiscard]] friend element_type & get( const core_litteral & cl_ ) noexcept { return *( cl_.data() + I );	}

		value_type							value[ N ];
	};

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	using litteral = base_shadow< core_litteral< Char, N, Traits > >;

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< Char > >
	[[nodiscard]] constexpr litteral< Char, N, Traits > litt( Char ( & src_ )[ N ] )				   { return { src_ }; }

	template< traits::character Char, std::size_t N, typename Traits = std::char_traits< const Char > >
	[[nodiscard]] constexpr litteral< Char const, N-(N>0), Traits > litt( Char const ( & src_ )[ N ] ) { return { src_ }; }


	template< typename Tag, typename T >	struct Tagged;

	template< traits::character Char, size_t N >
	[[nodiscard]] constexpr Tagged< struct general, litteral< Char, N > > tagged( Char ( & str_ )[ N ] )		{
		return { str_ };
	}

	template< traits::character Char, size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Tagged< struct general, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( !str_[ N-1 ] && "Removing a non-zero character suffix!" );
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >
	[[nodiscard]] constexpr Tagged< Tag, litteral< Char, N > > tagged( Char ( & str_ )[ N ] ) {
		return { str_ };
	}

	template< typename Tag, traits::character Char, size_t N >	requires( N > 0 )
	[[nodiscard]] constexpr Tagged< Tag, litteral< Char const, N-(N>0) > > tagged( Char const ( & str_ )[ N ] ) {
		assert( !str_[ N-1 ] && "Removing a non-zero character suffix!" );
		return { str_ };
	}

}	// namespace pax
