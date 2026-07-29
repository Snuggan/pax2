//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"

#include <span>
#include <string_view>
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <assert.h>				// The classic assert macro.


namespace std {

	template< std::size_t I, pax::traits::sized_contiguous Cont >				requires( I < extent_v< Cont > )
	[[nodiscard]] auto & get( Cont && cont_  )												noexcept	{
		using std::begin;
		return *( begin( cont_ ) + I );
	}

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous Cont0, pax::traits::contiguous Cont1 >
	[[nodiscard]] constexpr bool operator==( Cont0 && cont0_ , Cont1 && cont1_ )			noexcept	{
		using std::begin;
		return std::equal(	begin( cont0_ ), pax::no_nullchar_end( cont0_ ), 
							begin( cont1_ ), pax::no_nullchar_end( cont1_ ) );
	}
	static_assert( std::string_view( "abc" ).size() == 3 );
	static_assert( "abc" == std::string_view( "abc" ) );
	static_assert( std::string_view( "abc" ) == "abc" );

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous Cont0, pax::traits::contiguous Cont1 >
	[[nodiscard]] constexpr auto operator<=>( Cont0 && cont0_, Cont1 && cont1_ )			noexcept	{
		using std::begin;
		return std::lexicographical_compare_three_way(	begin( cont0_ ), pax::no_nullchar_end( cont0_ ), 
														begin( cont1_ ), pax::no_nullchar_end( cont1_ ) );
	}

	/// Stream the elements to out_.
	template< typename Out, typename T, std::size_t N >
	Out & operator<<( Out & out_, span< T, N > sp_ ) {
		using std::data, std::size;
		auto		itr = data( sp_ );
		const auto	end = itr + size( sp_ );
		if constexpr( pax::traits::character< T > )
			out_.write( itr, size( sp_ ) );
		else if( itr == end )
			out_ << "[]";
		else {
			out_ << '[' << *itr;
			while( ++itr < end )	out_ << ", " << *itr;
			out_ << ']';
		}
		return out_;
	}

}	// namespace std


namespace pax {
	
	using	traits::contiguous, traits::element_type_t, traits::extent_v;

	template< contiguous Cont, std::size_t N = traits::dynamic_extent >
	using Span = std::span< element_type_t< Cont >, N >;


	/// Create a std::span of elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto make_span( Cont && const_ )								noexcept	{
		using std::begin;
		using Sp = std::span< element_type_t< Cont >, No_null_extent< Cont > >;
		return Sp{ begin( const_ ), no_nullchar_end( const_ ) };
	}
	static_assert( make_span( "abc" ).size() == 3u );
	static_assert( make_span( "abc" ).extent == 3u );


	/// Create a std::span of const elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto make_const_span( Cont && cont_ )							noexcept	{
		using std::begin;
		using Sp = std::span< const element_type_t< Cont >, No_null_extent< Cont > >;
		return Sp{ begin( cont_ ), no_nullchar_end( cont_ ) };
	}
	static_assert( make_const_span( "abc" ).size() == 3u );
	static_assert( make_const_span( "abc" ).extent == 3u );


	/// Returns a dynamically sized std::span.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto make_dynamic_span( Cont && v_ ) 							noexcept {
		using std::begin;
		using Sp = std::span< element_type_t< Cont > >;
		return Sp{ begin( v_ ), no_nullchar_end( v_ ) };
	}
	static_assert( make_dynamic_span( "abc" ).size() == 3u );
	static_assert( make_dynamic_span( "abc" ).extent == traits::dynamic_extent );


	/// Return `true`, iff `ptr_` references an element in this..
	template< contiguous Cont >
	[[nodiscard]] constexpr bool within( Cont && cont_, element_type_t< Cont > * ptr_ )		noexcept	{
		using std::data, std::size;
		return ( data( cont_ ) <= ptr_ ) && ( ptr_ < data( cont_ ) + size( cont_ ) );
	}

	/// Return a dynamic shadow of the first min(n_, size()) elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto first( Cont && cont_, std::size_t n_ = 1 )					noexcept	{
		using std::data;
		return std::span{ data( cont_ ), std::min( n_, no_nullchar_size( cont_ ) ) };
	}
	static_assert( first( "abcdefghi", 3 ) == "abc" );

	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static< I > ).
	template< std::size_t I, contiguous Cont >									requires( is_static< I > )
	[[nodiscard]] constexpr auto first( Cont && cont_ ) 									noexcept	{
		using std::data;
		static constexpr std::size_t 	N = No_null_extent< Cont >;
		if constexpr( !is_static< N > )	assert( I <= no_nullchar_size( cont_ ) 
			&& "first< I >( cont_ ) requires I <= size( cont_ )." );
		return Span< Cont, std::min( I, N ) >( data( cont_ ), std::min( I, N ) );
	}
	static_assert( first< 3 >( "abcdefghi" ) == "abc" );
	static_assert( first< 3 >( "abcdefghi" ).extent == 3 );

	/// Return a dynamic shadow of the last size() - min(n_, size()) elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto not_first( Cont && cont_, std::size_t n_ = 1 )				noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		n_ = std::min( n_, sz );
		return std::span{ data( cont_ ) + n_, sz - n_ };
	}
	static_assert( not_first( "abcdefghi", 3 ) == "defghi" );

	/// Return a static shadow of the last size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous Cont >									requires( is_static< I > )
	[[nodiscard]] constexpr auto not_first( Cont && cont_ ) 								noexcept	{
		using std::data;
		static constexpr std::size_t 	N = No_null_extent< Cont >;
		if constexpr( !is_static< N > )	assert( I <= no_nullchar_size( cont_ ) 
			&& "not_first< I >( cont_ ) requires I <= size( cont_ )." );
		return Span< Cont, N - std::min( I, N ) >{ data( cont_ ) + std::min( I, N ), N - std::min( I, N ) };
	}
	static_assert( not_first< 3 >( "abcdefghi" ) == "defghi" );
	static_assert( not_first< 3 >( "abcdefghi" ).extent == 6 );

	/// Return a dynamic shadow of the last min(n_, size()) elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto last( Cont && cont_, std::size_t n_ = 1 )					noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		n_ = std::min( n_, sz );
		return std::span{ data( cont_ ) + sz - n_, n_ };
	}
	static_assert( last( "abcdefghi", 3 ) == "ghi" );

	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous Cont >									requires( is_static< I > )
	[[nodiscard]] constexpr auto last( Cont && cont_ ) 										noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		static constexpr std::size_t 	N = No_null_extent< Cont >;
		if constexpr( !is_static< N > )	assert( I <= sz 
			&& "last< I >( cont_ ) requires I <= size( cont_ )." );
		return Span< Cont, std::min( I, N ) >{ data( cont_ ) + sz - std::min( I, N ), std::min( I, N ) };
	}
	static_assert( last< 3 >( "abcdefghi" ) == "ghi" );
	static_assert( last< 3 >( "abcdefghi" ).extent == 3 );

	/// Return a dynamic shadow of the first size() - min(n_, size()) elements.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto not_last( Cont && cont_, std::size_t n_ = 1 )				noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		return std::span{ data( cont_ ), sz - std::min( n_, sz ) };
	}
	static_assert( not_last( "abcdefghi", 3 ) == "abcdef" );

	/// Return a static shadow of the first size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous Cont >									requires( is_static< I > )
	[[nodiscard]] constexpr auto not_last( Cont && cont_ ) 									noexcept	{
		static constexpr std::size_t 	N = No_null_extent< Cont >;
		return first< N - std::min( I, N ) >( cont_ );
	}
	static_assert( not_last< 3 >( "abcdefghi" ) == "abcdef" );
	static_assert( not_last< 3 >( "abcdefghi" ).extent == 6 );

	/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
	/// A negative offs_ is counted from the end.
	template< contiguous Cont >
	[[nodiscard]] constexpr auto mid( Cont && cont_, std::ptrdiff_t offs_, std::size_t n_ ) noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		offs_ =	( offs_ >= 0 )	?		std::min( std::size_t(  offs_ ), sz )
								: sz -	std::min( std::size_t( -offs_ ), sz );
		return std::span{ data( cont_ ) + offs_, std::min( sz - offs_, n_ ) };
	}
	static_assert( mid( "abcdefghi",  2, 3 ) == "cde" );
	static_assert( mid( "abcdefghi", -5, 3 ) == "efg" );

	/// Return a static shadow of the N elements starting with offs_, but restricted to the bounds of sp_.
	/// A negative offs_ is counted from the back. Does assert( offs_ + N <= sp_.size() ).
	template< std::size_t I, contiguous Cont >						requires( is_static< I > )
	[[nodiscard]] constexpr auto mid( Cont && cont_, std::ptrdiff_t offs_ )					noexcept	{
		using std::data;
		const auto sz = no_nullchar_size( cont_ );
		offs_ =	( offs_ >= 0 )	?		std::min( std::size_t(  offs_ ), sz )
								: sz -	std::min( std::size_t( -offs_ ), sz );
		assert( offs_ + I <= sz 
			&& "mid< I >( cont_ ) requires offs_ + I <= size( cont_ )." );
		return Span< Cont, I >{ data( cont_ ) + offs_, I };
	}
	static_assert( mid< 3 >( "abcdefghi",  2 ) == "cde" );
	static_assert( mid< 3 >( "abcdefghi",  2 ).extent == 3 );
	static_assert( mid< 3 >( "abcdefghi", -5 ) == "efg" );
	static_assert( mid< 3 >( "abcdefghi", -5 ).extent == 3 );

	/// Return true iff u_ equals the first elements of this.
	template< contiguous Cont0, contiguous Cont1 >
	[[nodiscard]] constexpr bool starts_with( Cont0 && cont0_, Cont1 && cont1_ )			noexcept	{
		using std::begin;
		const std::size_t sz0	  = no_nullchar_size( cont0_ );
		const std::size_t sz1	  = no_nullchar_size( cont1_ );
		return ( sz1 <= sz0 ) && ( first( cont0_, sz1 ) == first( cont1_, sz1 ) );
	}
	static_assert( starts_with( "abcdefghi", "abc" ) );

	/// Return true iff u_ equals the last elements of this.
	template< contiguous Cont0, contiguous Cont1 >
	[[nodiscard]] constexpr bool ends_with( Cont0 && cont0_, Cont1 && cont1_ )				noexcept	{
		using std::begin;
		const std::size_t sz0	  = no_nullchar_size( cont0_ );
		const std::size_t sz1	  = no_nullchar_size( cont1_ );
		return ( sz1 <= sz0 ) && ( last( cont0_, sz1 ) == first( cont1_, sz1 ) );
	}
	static_assert( ends_with( "abcdefghi", "ghi" ) );

	/// Return a shadow of where t_ is -- or a zereo-sized shadow located at end().
	template< contiguous Cont >
	[[nodiscard]] constexpr Span< Cont > find( Cont && cont_, element_type_t< Cont > t_ )	noexcept	{
		using std::begin;
		const auto end			  = no_nullchar_end( cont_ );
		auto result				  = std::find( begin( cont_ ), end, t_ );
		return { result, result != end };
	}

	/// Return a shadow of where u_ is -- or a zereo-sized shadow located at end().
	template< contiguous Cont0, contiguous Cont1 >
	[[nodiscard]] constexpr Span< Cont0 > find( Cont0 && cont0_, Cont1 && cont1_ )			noexcept	{
		using std::begin;
		const auto end0			  = no_nullchar_end( cont0_ );
		const auto end1			  = no_nullchar_end( cont1_ );
		auto result = std::search( begin( cont0_ ), end0, begin( cont1_ ), end1 );
		return { result, ( result == end0 ) ? 0u : std::size_t( end1 - begin( cont1_ ) ) };
	}

	/// Return a shadow of the first contigous range where all Test( value ) are true.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous Cont, typename Test >	requires std::is_invocable_r_v< bool, Test, element_type_t< Cont > >
	[[nodiscard]] constexpr Span< Cont > find( Cont && cont_, Test && test_ )				noexcept	{
		using std::begin;
		const auto end			  = no_nullchar_end( cont_ );
		auto b					  = std::find_if( begin( cont_ ), end, test_ );
		auto e					  = b;
		while( ( e != end ) && test_( *e ) ) 	++e;
		return { b, e };
	}

	/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous Cont >
	[[nodiscard]] constexpr Span< Cont > find_linebreak( Cont && cont_ )					noexcept	{
		const auto end			  = no_nullchar_end( cont_ );
		std::remove_cv_t< element_type_t< Cont > >	previous{ ' ' };
		for( auto & c : cont_ )	{
			[[unlikely]] if( previous == '\n' )	return { &c - 1, 1u + ( c == '\r' ) };
			[[unlikely]] if( previous == '\r' )	return { &c - 1, 1u + ( c == '\n' ) };
			previous = c;
		}
		return { end - ( ( previous == '\n' ) || ( previous == '\r' ) ), end };
	};


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< traits::contiguous V, typename U >
	constexpr auto until(  
		V					 && v_, 
		U					 && until_this_ 
	) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}
	
	

	template< typename T >
	struct split_result {	std::span< T > first, rest;		};

	/// Split this in two at offset t_ so that first.end() == rest.begin() and first.size() == t_.
	template< contiguous Cont >
	[[nodiscard]] constexpr split_result< element_type_t< Cont > > 
										split( Cont && cont_, std::size_t mid_ )			noexcept	{
		using std::begin, std::size;
		mid_					  = std::min( mid_, size( cont_ ) );
		return { { begin( cont_ ), mid_ }, { begin( cont_ ) + mid_, no_nullchar_end( cont_ ) } };
	}

	/// Split this in two: before and after gap_, but everything clamped to [begin(), end()].
	template< contiguous Cont0, contiguous Cont1 >
	[[nodiscard]] constexpr split_result< element_type_t< Cont0 > > 
										split( Cont0 && cont_, Cont1 && gap_ )				noexcept	{
		using std::begin;
		const auto end			  = no_nullchar_end( cont_ );
		return { { begin( cont_ ), std::clamp( begin( gap_ ),  begin( cont_ ), end ) },
				 { std::clamp( no_nullchar_end( gap_ ), begin( cont_ ), end ), end } };
	}

}	// namespace pax
