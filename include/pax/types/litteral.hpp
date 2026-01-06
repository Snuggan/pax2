//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/std/format.hpp>		// std20::formatter
#include <string_view>


namespace pax {

	/// A class to simplify string use at compile time, e.g. as template arguments.
	/// The interfaced is similar to that of std::string_view and std::span. 
	template< typename Char, size_t N, typename Traits = std::char_traits< Char > >
	struct Litt {
		using value_type		  = std::remove_cv_t< Char >;
		using pointer			  = value_type const *;
		using iterator			  = pointer;
		using const_iterator	  = pointer;
		using size_type			  = std::size_t;
		using difference_type	  = std::ptrdiff_t;
		using view				  = std::basic_string_view< value_type, Traits >;
		static constexpr size_type extent  = N - 1u;

	    constexpr Litt( value_type const ( & str_ )[ N ] )	{	std::copy_n( str_, N, value );	}
 
	    constexpr operator view()				const noexcept	{	return { data(), extent };	}
		
	    constexpr value_type operator[]( const size_type i_ )		const noexcept
		{	return ( i_ < extent ) ? data()[ i_ ] : '\0';										}
		
	    constexpr const_iterator begin()		const noexcept	{	return data();;				}
	    constexpr const_iterator end()			const noexcept	{	return begin() + extent;	}
		
	    constexpr value_type front()			const noexcept	{	return operator[]( 0 );		}
	    constexpr value_type back()				const noexcept	{	return operator[]( extent-1 );	}
		
		constexpr view first( const size_type n_ )					const noexcept
		{	return { data(), ( extent >= n_ ) ? n_ : extent };									}
 
		constexpr view last( const size_type n_ )					const noexcept
		{	return ( extent > n_ ) ? view{ data() + extent - n_, n_ } : operator view();		}
 
		constexpr view subview(
			size_type	 		offs_, 
			const size_type 	n_ = extent
		) const noexcept {
			offs_ = ( extent >= offs_ ) ? offs_ : extent;
			return { data() + offs_, ( extent >= offs_ + n_ ) ? n_ : extent - offs_ };
		}
 
 	    template< typename U >
	    constexpr bool operator==( const U & u_ )					const noexcept
		{   return operator view() == view( u_ );												}

 	    template< typename U >
	    constexpr auto operator<=>( const U & u_ )					const noexcept
		{   return operator view() <=> view( u_ );												}

 	    template< typename U >
	    constexpr bool starts_with( const U & u_ )					const noexcept				{
			const view		vw( u_ );
			return first( vw.size() ) == vw;
		}

 	    template< typename U >
	    constexpr bool ends_with( const U & u_ )					const noexcept				{
			const view		vw( u_ );
			return last( vw.size() ) == vw;
		}
 
 	    template< typename U >
	    constexpr bool contains( const U & u_ )						const noexcept
		{	return operator view().contains( u_ );												}
 
 	    template< typename ...U >
	    constexpr size_type find( U && ...u_ )						const noexcept
		{	return operator view().find( std::forward< U >( u_ )... );							}

		constexpr value_type const * data()		const noexcept	{	return value;				}

		static constexpr bool empty()			noexcept		{	return extent == 0;			}
		static constexpr size_type size()		noexcept		{	return extent;				}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const Litt & str_ ) 
		{	return out_.write( str_.value, extent );											}
		
		value_type		value[ N ];
	};

	template< typename Char, size_t N >
	Litt( Char const ( & )[ N ] ) -> Litt< Char, N >;



	template< typename Tag, typename T >	struct Tagged;

	template< typename Char, size_t N >
	constexpr Tagged< struct general, Litt< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}

	template< typename Tag, typename Char, size_t N >
	constexpr Tagged< Tag, Litt< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}



	static_assert( Litt{ "text" }					== "text" );
	static_assert( Litt{ "text" }					>  "test" );
	static_assert( Litt{ "text" }[ 4 ]				== 0 );
	static_assert( *( Litt{ "text" }.begin() + 1 ) 	== 'e' );
	static_assert( *( Litt{ "text" }.end()   - 2 ) 	== 'x' );
	static_assert( Litt{ "text" }.front() 			== 't' );
	static_assert( Litt{ "text" }.back() 			== 't' );
	static_assert( Litt{ "text" }.size()			== 4u );
	static_assert( Litt{ "text" }.first( 2 )		== "te" );
	static_assert( Litt{ "text" }.first( 9 )		== "text" );
	static_assert( Litt{ "text" }.last( 2 )			== "xt" );
	static_assert( Litt{ "text" }.last( 9 )			== "text" );
	static_assert( Litt{ "text" }.subview( 1, 2 )	== "ex" );
	static_assert( Litt{ "text" }.subview( 9, 2 )	== "" );
	static_assert( Litt{ "text" }.subview( 1, 9 )	== "ext" );
	static_assert( Litt{ "text" }.starts_with( "tex" ) );
	static_assert( Litt{ "text" }.ends_with( "ext" ) );
#if defined( __clang__ )	// These are not constexpr in gcc...
	static_assert( Litt{ "text" }.contains( 'x' ) );			
	static_assert( Litt{ "text" }.contains( "xt" ) );
	static_assert( Litt{ "text" }.find( "xt" )		== 2u );
#endif
	// Test template usage.
	template< auto V >	struct Litt_test	{	static constexpr decltype( V ) v = V;	};
	static_assert( Litt_test< Litt{ "text" } >::v	== "text" );
	
}	// namespace pax

namespace std20 {
	template< typename Char, size_t N >
	struct formatter< pax::Litt< Char, N > > : formatter< Char const * > {
	    constexpr auto format( const pax::Litt< Char, N > & str_, format_context & ctx_ )	const {
			return formatter< Char const * >::format( str_.value, ctx_ );
	    }
	};
}	// namespace std
