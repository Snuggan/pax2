//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"

#include <span>
#include <array>
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <assert.h>				// The classic assert macro.


namespace pax {
	
	using std::array, std::span;


	/// Create a span of elements.
	template< std::ranges::contiguous_range Cont >
	constexpr auto make_span( Cont && const_ )												noexcept	{
		using std::begin;
		using Sp = std::span< traits::element_type_t< Cont >, traits::extent_v< Cont > >;
		return Sp{ begin( const_ ), no_nullchar_end( const_ ) };
	}

	/// Create a span of elements.
	template< traits::contiguous Cont >
	constexpr auto make_const_span( Cont && cont_ )											noexcept	{
		using std::begin;
		using Sp = std::span< const traits::element_type_t< Cont >, traits::extent_v< Cont > >;
		return Sp{ begin( cont_ ), no_nullchar_end( cont_ ) };
	}

	template< traits::contiguous Cont >
	[[nodiscard]] constexpr std::span< traits::element_type_t< Cont > > 
		not_first( Cont && cont_, std::size_t n_ = 1 ) noexcept;

}	// namespace pax


namespace std {

	template< std::size_t I, pax::traits::sized_contiguous Cont >			requires( I < pax::traits::extent_v< Cont > )
	[[nodiscard]] auto & get( Cont && cont_  )												noexcept	{
		using std::begin;
		return *( begin( cont_ ) + I );
	}

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous Cont0, pax::traits::contiguous Cont1 >
	[[nodiscard]] constexpr bool operator==( Cont0 && cont0_ , Cont1 && cont1_ )			noexcept	{
		using std::begin;
		return std::equal(	begin( cont0_ ), pax::no_nullchar_end( cont0_ ), begin( cont1_ ), pax::no_nullchar_end( cont1_ ) );
	}

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous Cont0, pax::traits::contiguous Cont1 >
	[[nodiscard]] constexpr auto operator<=>( Cont0 && cont0_, Cont1 && cont1_ )			noexcept	{
		using std::begin;
		return std::lexicographical_compare_three_way(
							begin( cont0_ ), pax::no_nullchar_end( cont0_ ), begin( cont1_ ), pax::no_nullchar_end( cont1_ ) );
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

	/// Return `true`, iff `ptr_` references an element in this..
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr bool within( span< T, N > sp_, T * ptr_ )						noexcept	{
		return ( sp_.data() <= ptr_ ) && ( ptr_ < &*no_nullchar_end( sp_ ) );
	}

	/// Return a dynamic shadow of the first min(n_, size()) elements.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto first( Cont && cont_, std::size_t n_ = 1 )					noexcept	{
		using std::data, std::size;
		return span{ data( cont_ ), std::min( n_, size( cont_ ) ) };
	}

	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static< I > ).
	template< std::size_t I, typename T, std::size_t N >					requires( is_static< I > )
	[[nodiscard]] constexpr auto first( span< T, N > sp_ ) 									noexcept	{
		if constexpr( !is_static< N > )	assert( I <= sp_.size() && "first< I >( sp_ ) requires I <= sp_.size()." );
		return span< T, std::min( I, N ) >( sp_.data(), std::min( I, N ) );
	}

	/// Return a dynamic shadow of the last size() - min(n_, size()) elements.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr std::span< traits::element_type_t< Cont > > 
										not_first( Cont && cont_, std::size_t n_ )			noexcept	{
		using std::data, std::size;
		n_ = std::min( n_, size( cont_ ) );
		return span{ data( cont_ ) + n_, size( cont_ ) - n_ };
	}

	/// Return a static shadow of the last size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, typename T, std::size_t N >						requires( is_static< I > )
	[[nodiscard]] constexpr auto not_first( span< T, N > sp_ ) 								noexcept	{
		if constexpr( !is_static< N > )	assert( I <= sp_.size() && "not_first< I >( sp_ ) requires I <= sp_.size()." );
		return span< T, N - std::min( I, N ) >{ sp_.data() + std::min( I, N ), N - std::min( I, N ) };
	}

	/// Return a dynamic shadow of the last min(n_, size()) elements.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto last( Cont && cont_, std::size_t n_ = 1 )					noexcept	{
		using std::data, std::size;
		n_ = std::min( n_, size( cont_ ) );
		return span{ data( cont_ ) + size( cont_ ) - n_, n_ };
	}

	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, typename T, std::size_t N >						requires( is_static< I > )
	[[nodiscard]] constexpr auto last( span< T, N > sp_ ) 									noexcept	{
		if constexpr( !is_static< N > )	assert( I <= sp_.size() && "last< I >( sp_ ) requires I <= sp_.size()." );
		return span< T, std::min( I, N ) >{ sp_.data() + sp_.size() - std::min( I, N ), std::min( I, N ) };
	}

	/// Return a dynamic shadow of the first size() - min(n_, size()) elements.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto not_last( Cont && cont_, std::size_t n_ = 1 )				noexcept	{
		using std::data, std::size;
		return span{ data( cont_ ), size( cont_ ) - std::min( n_, size( cont_ ) ) };
	}

	/// Return a static shadow of the first size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, typename T, std::size_t N >						requires( is_static< I > )
	[[nodiscard]] constexpr auto not_last( span< T, N > sp_ ) 								noexcept 	{
		return first< N - std::min( I, N ) >( sp_ );
	}

	/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
	/// A negative offs_ is counted from the end.
	template< traits::contiguous Cont >
	[[nodiscard]] constexpr auto mid( Cont && cont_, std::ptrdiff_t offs_, std::size_t n_ ) noexcept	{
		offs_ =	( offs_ >= 0 )	?					std::min( std::size_t(  offs_ ), size( cont_ ) )
								: size( cont_ ) -	std::min( std::size_t( -offs_ ), size( cont_ ) );
		return span{ data( cont_ ) + offs_, std::min( size( cont_ ) - offs_, n_ ) };
	}

	/// Return a static shadow of the N elements starting with offs_, but restricted to the bounds of sp_.
	/// A negative offs_ is counted from the back. Does assert( offs_ + N <= sp_.size() ).
	template< std::size_t I, typename T, std::size_t N >						requires( is_static< I > )
	[[nodiscard]] constexpr auto mid( span< T, N > sp_, std::ptrdiff_t offs_ )				noexcept	{
		offs_ =	( offs_ >= 0 )	?				std::min( std::size_t(  offs_ ), sp_.size() )
								: sp_.size() -	std::min( std::size_t( -offs_ ), sp_.size() );
		assert( offs_ + I <= sp_.size() && "mid< I >( sp_ ) requires offs_ + I <= sp_.size()." );
		return span< T, I >{ sp_.data() + offs_, I };
	}

	/// Return true iff u_ equals the first elements of this.
	template< typename T, std::size_t N, std::ranges::contiguous_range Cont >
	[[nodiscard]] constexpr bool starts_with( span< T, N > sp_, Cont && cont_ )				noexcept	{
		using std::begin;
		const std::size_t sz	  = no_nullchar_end( cont_ ) - begin( cont_ );
		return ( sz <= sp_.size() ) && ( first( sp_, sz ) == first( make_span( cont_ ), sz ) );
	}

	/// Return true iff u_ equals the last elements of this.
	template< typename T, std::size_t N, std::ranges::contiguous_range Cont >
	[[nodiscard]] constexpr bool ends_with( span< T, N > sp_, Cont && cont_ )				noexcept	{
		using std::begin;
		const std::size_t sz	  = no_nullchar_end( cont_ ) - begin( cont_ );
		return ( sz <= sp_.size() ) && ( last( sp_, sz ) == make_span( cont_ ) );
	}

	/// Return a shadow of where t_ is -- or a zereo-sized shadow located at end().
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr span< T > find( span< T, N > sp_, std::type_identity_t< T > t_ ) noexcept	{
		const auto end = no_nullchar_end( sp_ );
		auto result = std::find( sp_.begin(), end, t_ );
		return { result, result != end };
	}

	/// Return a shadow of where u_ is -- or a zereo-sized shadow located at end().
	template< typename T, std::size_t N, std::ranges::contiguous_range Cont >
	[[nodiscard]] constexpr span< T > find( span< T, N > sp_, Cont && cont_ )				noexcept	{
		using std::begin, std::size;
		const auto end		  = no_nullchar_end( sp_ );
		const auto cont_end	  = no_nullchar_end( cont_ );
		auto result = std::search( sp_.begin(), end, begin( cont_ ), cont_end );
		return { result, ( result == end ) ? 0u : std::size_t( cont_end - begin( cont_ ) ) };
	}

	/// Return a shadow of the first contigous range where all Test( value ) are true.
	/// If none is found, { end(), 0u } is returned.
	template< typename T, std::size_t N, typename Test >	requires std::is_invocable_r_v< bool, Test, T >
	[[nodiscard]] constexpr span< T > find( span< T, N > sp_, Test && test_ )				noexcept	{
		const auto end = no_nullchar_end( sp_ );
		auto	b = std::ranges::find_if( sp_, test_ );
		auto 	e = b;
		while( ( e != end ) && test_( *e ) ) 	++e;
		return { b, e };
	}

	/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
	/// If none is found, { end(), 0u } is returned.
	template< typename C, std::size_t N >
	[[nodiscard]] constexpr span< C > find_linebreak( span< C, N > sp_ )					noexcept	{
		const auto 				e = no_nullchar_end( sp_ );
		std::remove_cv_t< C >	previous{ ' ' };
		for( auto & c : sp_ )	{
			[[unlikely]] if( previous == '\n' )	return { &c - 1, 1u + ( c == '\r' ) };
			[[unlikely]] if( previous == '\r' )	return { &c - 1, 1u + ( c == '\n' ) };
			previous = c;
		}
		return { e - ( ( previous == '\n' ) || ( previous == '\r' ) ), e };
	};
	
	
	template< typename T >
	struct split_result {	std::span< T > first, rest;		};

	/// Split this in two at offset t_ so that first.end() == rest.begin() and first.size() == t_.
	template< typename T, std::size_t N >
	[[nodiscard]] constexpr split_result< T > split( span< T, N > sp_, std::size_t mid_ )	noexcept	{
		mid_		  = std::min( mid_, sp_.size() );
		return { { sp_.begin(), mid_ }, { sp_.begin() + mid_, no_nullchar_end( sp_ ) } };
	}

	/// Split this in two: before and after gap_, but everything clamped to [begin(), end()].
	template< typename T, std::size_t N, std::size_t G >
	[[nodiscard]] constexpr split_result< T > split( span< T, N > sp_, span< T, G > gap_ )	noexcept	{
		const auto 		e = no_nullchar_end( sp_ );
		return { { sp_.begin(), std::clamp( gap_.begin(), sp_.begin(), e ) },
				 { std::clamp( no_nullchar_end( gap_ ),   sp_.begin(), e ), e } };
	}

}	// namespace pax
