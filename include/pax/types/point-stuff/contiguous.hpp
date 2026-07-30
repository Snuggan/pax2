//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Vact: peder ( at ) axensten.se


#pragma once

#include "base.hpp"

#include <span>
#include <algorithm>			// std::ranges::equal, std::lexicographical_compare_three_way, etc.
#include <assert.h>				// The classic assert macro.


#if defined( NDEBUG )
#	define TEST( ... )		
#else
#	define TEST( ... )		static_assert( __VA_ARGS__ );
#endif


namespace std {

	template< std::size_t I, pax::traits::sized_contiguous V >				requires( I < extent_v< V > )
	[[nodiscard]] auto & get( V && v_  )												noexcept	{
		using std::begin;
		return *( begin( v_ ) + I );
	}

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous V0, pax::traits::contiguous V1 >
	[[nodiscard]] constexpr bool operator==( V0 && v0_ , V1 && v1_ )					noexcept	{
		using std::begin;
		return std::equal(	begin( v0_ ), pax::no_nullchar_end( v0_ ), 
							begin( v1_ ), pax::no_nullchar_end( v1_ ) );
	}

	/// In strings a terminating \0 is ignored.
	/// If a string ends with a '\0', it is ignored.
	template< pax::traits::contiguous V0, pax::traits::contiguous V1 >
	[[nodiscard]] constexpr auto operator<=>( V0 && v0_, V1 && v1_ )					noexcept	{
		using std::begin;
		return std::lexicographical_compare_three_way(	begin( v0_ ), pax::no_nullchar_end( v0_ ), 
														begin( v1_ ), pax::no_nullchar_end( v1_ ) );
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
	
	using std::data, std::begin;
	using traits::contiguous, traits::element_type_t, traits::extent_v;

	template< contiguous V, std::size_t N = traits::dynamic_extent >
	using Span = std::span< element_type_t< V >, N >;

	struct linebreak {
		static constexpr auto check = []( const traits::character auto c_ ) {
			return ( c_ == '\n' ) || ( c_ == '\r' );
		};
	};


	/// Create a std::span of elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto make_span( V && const_ )								noexcept	{
		using Sp = std::span< element_type_t< V >, No_null_extent< V > >;
		return Sp{ begin( const_ ), no_nullchar_end( const_ ) };
	}
	TEST( make_span( "abc" ).size() == 3u );
	TEST( make_span( "abc" ).extent == 3u );


	/// Create a std::span of const elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto make_const_span( V && v_ )								noexcept	{
		using Sp = std::span< const element_type_t< V >, No_null_extent< V > >;
		return Sp{ begin( v_ ), no_nullchar_end( v_ ) };
	}
	TEST( make_const_span( "abc" ).size() == 3u );
	TEST( make_const_span( "abc" ).extent == 3u );


	/// Returns a dynamically sized std::span.
	template< traits::contiguous V >
	[[nodiscard]] constexpr auto make_dynamic_span( V && v_ ) 							noexcept {
		using Sp = std::span< element_type_t< V > >;
		return Sp{ begin( v_ ), no_nullchar_end( v_ ) };
	}
	TEST( make_dynamic_span( "abc" ).size() == 3u );
	TEST( make_dynamic_span( "abc" ).extent == traits::dynamic_extent );


	/// Returns false. 
	[[nodiscard]] constexpr bool valid( std::nullptr_t ) 	noexcept	{	return false;				}


	/// Returns ptr_ != nullptr. 
	template< typename T >
	[[nodiscard]] constexpr bool valid( T * ptr_ ) 			noexcept	{	return ptr_ != nullptr;		}


	/// Returns sp_.data() != nullptr. 
	template< contiguous V >
	[[nodiscard]] constexpr bool valid( const V & v_ ) 		noexcept	{	return valid( data( v_ ) );	}
	TEST( !valid( std::span< int >{} ) );


	/// Return `true`, iff `ptr_` references an element in this..
	template< contiguous V >
	[[nodiscard]] constexpr bool within( V && v_, element_type_t< V > * ptr_ )			noexcept	{
		return ( data( v_ ) <= ptr_ ) && ( ptr_ < data( v_ ) + no_nullchar_size( v_ ) );
	}


	/// Return true iff any element in v1_ by address is also an element in v2_.
	/// Cheap: 1 addition, 2 comparisons, and 2 bolean &&.
	template< contiguous V0, contiguous V1 >
	[[nodiscard]] constexpr bool overlap( V0 && v0_, V1 && v1_ )						noexcept	{
		const std::size_t sz0	  = no_nullchar_size( v0_ );
		const std::size_t sz1	  = no_nullchar_size( v1_ );
		return	( ( data( v0_ ) > data( v1_ ) )	? ( data( v1_ ) + sz1 > data( v0_ ) ) 
												: ( data( v0_ ) + sz0 > data( v1_ ) )	)
			&&	sz0 && sz1;			// An empty view cannot overlap.
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Partial (specify what you want)
	////////////////////////////////////////////////////////////////////////////////////////////////

	/// Returns a reference to the first item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< traits::contiguous V >						requires( extent_v< V > > 0 )
	[[nodiscard]] constexpr auto & front( V && v_ ) 									noexcept	{
		if constexpr ( !is_static< extent_v< V > > )	assert( no_nullchar_size( v_ ) && "requires size( v_ ) > 0" );
		return *data( v_ );
	}
	TEST( front( "abcdefghi" ) == 'a' );


	/// Returns a reference to the first item. 
	/// UB, if v_ has a dynamic size that is zero.
	template< traits::contiguous V >						requires( extent_v< V > > 0 )
	[[nodiscard]] constexpr auto & back( V && v_ ) 										noexcept	{
		const auto sz = no_nullchar_size( v_ );
		if constexpr ( !is_static< extent_v< V > > )	assert( sz && "requires size( v_ ) > 0" );
		return *( data( v_ ) + sz - 1u );
	}
	TEST( back( "abcdefghi" ) == 'i' );


	/// Return a dynamic shadow of the first min(n_, size()) elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto first( V && v_, std::size_t n_ = 1 )					noexcept	{
		return std::span{ data( v_ ), std::min( n_, no_nullchar_size( v_ ) ) };
	}
	TEST( first( "abcdefghi",  3 ) == "abc" );
	TEST( first( "abcdefghi", 12 ) == "abcdefghi" );


	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static< I > ).
	template< std::size_t I, contiguous V >									requires( is_static< I > )
	[[nodiscard]] constexpr auto first( V && v_ ) 										noexcept	{
		static constexpr std::size_t 	N = No_null_extent< V >;
		if constexpr( !is_static< N > )	assert( I <= no_nullchar_size( v_ ) 
			&& "first< I >( v_ ) requires I <= size( v_ )." );
		return Span< V, std::min( I, N ) >( data( v_ ), std::min( I, N ) );
	}
	TEST( first<  3 >( "abcdefghi" ) == "abc" );
	TEST( first<  3 >( "abcdefghi" ).extent == 3 );
	TEST( first< 12 >( "abcdefghi" ) == "abcdefghi" );
	TEST( first< 12 >( "abcdefghi" ).extent == 9 );


	/// Return a dynamic shadow of the last size() - min(n_, size()) elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto not_first( V && v_, std::size_t n_ = 1 )				noexcept	{
		const auto sz = no_nullchar_size( v_ );
		n_ = std::min( n_, sz );
		return std::span{ data( v_ ) + n_, sz - n_ };
	}
	TEST( not_first( "abcdefghi",  3 ) == "defghi" );
	TEST( not_first( "abcdefghi", 12 ) == "" );


	/// Return a static shadow of the last size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous V >									requires( is_static< I > )
	[[nodiscard]] constexpr auto not_first( V && v_ ) 									noexcept	{
		static constexpr std::size_t 	N = No_null_extent< V >;
		if constexpr( !is_static< N > )	assert( I <= no_nullchar_size( v_ ) 
			&& "not_first< I >( v_ ) requires I <= size( v_ )." );
		return Span< V, N - std::min( I, N ) >{ data( v_ ) + std::min( I, N ), N - std::min( I, N ) };
	}
	TEST( not_first<  3 >( "abcdefghi" ) == "defghi" );
	TEST( not_first<  3 >( "abcdefghi" ).extent == 6 );
	TEST( not_first< 12 >( "abcdefghi" ) == "" );
	TEST( not_first< 12 >( "abcdefghi" ).extent == 0 );


	/// Return a dynamic shadow of the last min(n_, size()) elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto last( V && v_, std::size_t n_ = 1 )					noexcept	{
		const auto sz = no_nullchar_size( v_ );
		n_ = std::min( n_, sz );
		return std::span{ data( v_ ) + sz - n_, n_ };
	}
	TEST( last( "abcdefghi",  3 ) == "ghi" );
	TEST( last( "abcdefghi", 12 ) == "abcdefghi" );


	/// Return a static shadow of the first min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous V >									requires( is_static< I > )
	[[nodiscard]] constexpr auto last( V && v_ ) 										noexcept	{
		const auto sz = no_nullchar_size( v_ );
		static constexpr std::size_t 	N = No_null_extent< V >;
		if constexpr( !is_static< N > )	assert( I <= sz 
			&& "last< I >( v_ ) requires I <= size( v_ )." );
		return Span< V, std::min( I, N ) >{ data( v_ ) + sz - std::min( I, N ), std::min( I, N ) };
	}
	TEST( last<  3 >( "abcdefghi" ) == "ghi" );
	TEST( last<  3 >( "abcdefghi" ).extent == 3 );
	TEST( last< 12 >( "abcdefghi" ) == "abcdefghi" );
	TEST( last< 12 >( "abcdefghi" ).extent == 9 );


	/// Return a dynamic shadow of the first size() - min(n_, size()) elements.
	template< contiguous V >
	[[nodiscard]] constexpr auto not_last( V && v_, std::size_t n_ = 1 )				noexcept	{
		const auto sz = no_nullchar_size( v_ );
		return std::span{ data( v_ ), sz - std::min( n_, sz ) };
	}
	TEST( not_last( "abcdefghi",  3 ) == "abcdef" );
	TEST( not_last( "abcdefghi", 12 ) == "" );


	/// Return a static shadow of the first size() - min(N, extent) elements.
	/// Does assert( N <= size() && !is_static ).
	template< std::size_t I, contiguous V >									requires( is_static< I > )
	[[nodiscard]] constexpr auto not_last( V && v_ ) 									noexcept	{
		static constexpr std::size_t 	N = No_null_extent< V >;
		return first< N - std::min( I, N ) >( v_ );
	}
	TEST( not_last<  3 >( "abcdefghi" ) == "abcdef" );
	TEST( not_last<  3 >( "abcdefghi" ).extent == 6 );
	TEST( not_last< 12 >( "abcdefghi" ) == "" );
	TEST( not_last< 12 >( "abcdefghi" ).extent == 0 );


	/// Return a dynamic shadow of the n_ elements starting with offs_, but restricted to the bounds of this.
	/// A negative offs_ is counted from the end.
	template< contiguous V >
	[[nodiscard]] constexpr auto mid( V && v_, std::ptrdiff_t offs_, std::size_t n_ )	noexcept	{
		const auto sz = no_nullchar_size( v_ );
		offs_ =	( offs_ >= 0 )	?		std::min( std::size_t(  offs_ ), sz )
								: sz -	std::min( std::size_t( -offs_ ), sz );
		return std::span{ data( v_ ) + offs_, std::min( sz - offs_, n_ ) };
	}
	TEST( mid( "abcdefghi",  2,  3 ) == "cde" );
	TEST( mid( "abcdefghi", -5,  3 ) == "efg" );
	TEST( mid( "abcdefghi",  2, 12 ) == "cdefghi" );
	TEST( mid( "abcdefghi", -5, 12 ) == "efghi" );


	/// Return a static shadow of the N elements starting with offs_, but restricted to the bounds of sp_.
	/// A negative offs_ is counted from the back. Does assert( offs_ + N <= sp_.size() ).
	template< std::size_t I, contiguous V >						requires( is_static< I > )
	[[nodiscard]] constexpr auto mid( V && v_, std::ptrdiff_t offs_ )					noexcept	{
		const auto 			sz = no_nullchar_size( v_ );
		offs_ =	( offs_ >= 0 )	?		std::min( std::size_t(  offs_ ), sz )
								: sz -	std::min( std::size_t( -offs_ ), sz );
		assert( offs_ + I <= sz 
			&& "mid< I >( v_ ) requires offs_ + I <= size( v_ )." );
		return Span< V, I >{ data( v_ ) + offs_, I };
	}
	TEST( mid<  3 >( "abcdefghi",  2 ) == "cde" );
	TEST( mid<  3 >( "abcdefghi",  2 ).extent == 3 );
	TEST( mid<  3 >( "abcdefghi", -5 ) == "efg" );
	TEST( mid<  3 >( "abcdefghi", -5 ).extent == 3 );
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Trim (specify what you don't want)
	////////////////////////////////////////////////////////////////////////////////////////////////

	/// Returns `v_`, but excluding any leading elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< contiguous V, typename Pred >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_first( 
		V			 && v_, 
		Pred		 && p_ 
	) noexcept {
		auto			itr = begin( v_ );
		const auto		end = no_nullchar_end( v_ );
		while( ( itr != end ) && p_( *itr ) )		++itr;
		return std::span{ itr, end };
	}


	/// Returns `v_`, but excluding all leading `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< contiguous V >
	[[nodiscard]] constexpr auto trim_first( 
		V								 && v_, 
		const traits::value_type_t< V >   & t_ 
	) noexcept {
		return trim_first( v_, [ t_ ]( auto c ){ return c == t_; } );
	}
	TEST( trim_first( "", '+' ) 			==	"" );
	TEST( trim_first( "++++abcdef++", '+' ) ==	"abcdef++" );


	/// Returns `v_`, but excluding a leading `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< contiguous V >
	[[nodiscard]] constexpr auto trim_first( 
		V			 && v_, 
		linebreak 
	) noexcept {
		return trim_first( v_, linebreak::check );
	}
	TEST( trim_first( "abcdefgh",			linebreak{} )	==	"abcdefgh" );
	TEST( trim_first( "\n\r",				linebreak{} )	==	"" );
	TEST( trim_first( "\n\rabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );


	/// Returns `v_`, but excluding any trailing elements `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< contiguous V, typename Pred >
		requires( std::predicate< Pred, traits::value_type_t< V > > )
	[[nodiscard]] constexpr auto trim_last( 
		V			 && v_, 
		Pred		 && p_ 
	) noexcept {
		const auto		b   = begin( v_ );
		auto			itr = no_nullchar_end( v_ );
		if( itr != b )	while( ( --itr != b ) && p_( *itr ) );
		return std::span{ b, itr + 1 - p_( *itr ) };
	}


	/// Returns `v_`, but excluding all trailing `t_`, if any.
	/// Returns a [non-owning] string view into v_.
	template< contiguous V >
	[[nodiscard]] constexpr auto trim_last( 
		V								 && v_, 
		const traits::value_type_t< V >		t_ 
	) noexcept {
		return trim_last( v_, [ t_ ]( auto c ){ return c == t_; } );
	}
	TEST( trim_last( "", '+' ) 			   ==	"" );
	TEST( trim_last( "++++abcdef++", '+' ) ==	"++++abcdef" );


	/// Returns `v_`, but excluding a trailing `'\n'`, `'\r'`, `"\n\r"`, or `"\r\n"`. 
	/// Returns a [non-owning] string view into v_.
	template< contiguous V >
	[[nodiscard]] constexpr auto trim_last( 
		V			 && v_, 
		linebreak 
	) noexcept {
		return trim_last( v_, linebreak::check );
	}
	TEST( trim_last( "abcdefgh",			linebreak{} )	==	"abcdefgh" );
	TEST( trim_last( "\n\r",				linebreak{} )	==	"" );
	TEST( trim_last( "\n\rabcdefgh\n\r",	linebreak{} )	==	"\n\rabcdefgh" );


	/// Returns `v_`, but without any leading or trailing values `v` that satisfy `p_( v )`.
	/// Returns a [non-owning] string view into v_.
	template< contiguous V, typename T >
	[[nodiscard]] constexpr auto trim( 
		V			 && v_, 
		T			 && p_ 
	) noexcept {
		return trim_last( trim_first( v_, p_ ), p_ );
	}
	
	
	////////////////////////////////////////////////////////////////////////////////////////////////
	//	Finding stuff
	////////////////////////////////////////////////////////////////////////////////////////////////

	/// Return true iff u_ equals the first elements of this.
	template< contiguous V0, contiguous V1 >
	[[nodiscard]] constexpr bool starts_with( V0 && v0_, V1 && v1_ )					noexcept	{
		const std::size_t sz0	  = no_nullchar_size( v0_ );
		const std::size_t sz1	  = no_nullchar_size( v1_ );
		return ( sz1 <= sz0 ) && ( first( v0_, sz1 ) == first( v1_, sz1 ) );
	}
	TEST(  starts_with( "abcdefghi", "abc" ) );
	TEST(  starts_with( "abcdefghi", "abcdefghi" ) );
	TEST( !starts_with( "abcdefghi", "abcdefghij" ) );


	/// Return true iff u_ equals the last elements of this.
	template< contiguous V0, contiguous V1 >
	[[nodiscard]] constexpr bool ends_with( V0 && v0_, V1 && v1_ )						noexcept	{
		const std::size_t sz0	  = no_nullchar_size( v0_ );
		const std::size_t sz1	  = no_nullchar_size( v1_ );
		return ( sz1 <= sz0 ) && ( last( v0_, sz1 ) == first( v1_, sz1 ) );
	}
	TEST(  ends_with( "abcdefghi", "ghi" ) );
	TEST(  ends_with( "abcdefghi", "abcdefghi" ) );
	TEST( !ends_with( "abcdefghi", "abcdefghij" ) );


	/// Return the offset of where t_ is -- or size(), if not found.
	template< contiguous V >
	[[nodiscard]] constexpr std::size_t find( V && v_, element_type_t< V > t_ )			noexcept	{
		const auto end			  = no_nullchar_end( v_ );
		const auto result		  = std::find( begin( v_ ), end, t_ );
		return result - begin( v_ );
	}
	TEST( find( "abcdefghi", 'g' ) == 6 );
	TEST( find( "abcdefghi", 'x' ) == 9 );


	/// Return a std::span of where t_ is -- or a zereo-sized shadow located at end().
	template< contiguous V >
	[[nodiscard]] constexpr Span< V > find_span( V && v_, element_type_t< V > t_ )		noexcept	{
		const auto end			  = no_nullchar_end( v_ );
		const auto result		  = std::find( begin( v_ ), end, t_ );
		return { result, result != end };
	}
	TEST( find_span( "abcdefghi", 'g' ) == "g" );
	TEST( find_span( "abcdefghi", 'x' ) == ""  );


	/// Return the offset of where u_ is -- or size(), if not found.
	template< contiguous V0, contiguous V1 >
	[[nodiscard]] constexpr std::size_t find( V0 && v0_, V1 && v1_ )					noexcept	{
		const auto end0			  = no_nullchar_end( v0_ );
		const auto end1			  = no_nullchar_end( v1_ );
		const auto result		  = std::search( begin( v0_ ), end0, begin( v1_ ), end1 );
		return result - begin( v0_ );
	}
	TEST( find( "abcdefghi", "ghi" ) == 6 );
	TEST( find( "abcdefghi", "ghx" ) == 9 );


	/// Return a shadow of where u_ is -- or a zereo-sized shadow located at end().
	template< contiguous V0, contiguous V1 >
	[[nodiscard]] constexpr Span< V0 > find_span( V0 && v0_, V1 && v1_ )				noexcept	{
		const auto end0			  = no_nullchar_end( v0_ );
		const auto end1			  = no_nullchar_end( v1_ );
		const auto result		  = std::search( begin( v0_ ), end0, begin( v1_ ), end1 );
		return { result, ( result == end0 ) ? 0u : std::size_t( end1 - begin( v1_ ) ) };
	}
	TEST( find_span( "abcdefghi", "ghi" ) == "ghi" );
	TEST( find_span( "abcdefghi", "ghx" ) == ""  );


	/// Return a shadow of the first contigous range where all Test( value ) are true.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous V, typename Test >	requires std::is_invocable_r_v< bool, Test, element_type_t< V > >
	[[nodiscard]] constexpr std::size_t find( V && v_, Test && test_ )					noexcept	{
		const auto end			  = no_nullchar_end( v_ );
		const auto result		  = std::find_if( begin( v_ ), end, test_ );
		return result - begin( v_ );
	}
	TEST( find( "abcdefghi", []( auto c ){ return c == 'g'; } ) == 6 );
	TEST( find( "abcdefghi", []( auto c ){ return c == 'x'; } ) == 9 );


	/// Return a shadow of the first contigous range where all Test( value ) are true.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous V, typename Test >	requires std::is_invocable_r_v< bool, Test, element_type_t< V > >
	[[nodiscard]] constexpr Span< V > find_span( V && v_, Test && test_ )				noexcept	{
		const auto end			  = no_nullchar_end( v_ );
		auto b					  = std::find_if( begin( v_ ), end, test_ );
		auto e					  = b;
		while( ( e != end ) && test_( *e ) ) 	++e;
		return { b, e };
	}
	TEST( find_span( "abcdefghi_", []( auto c ){ return c >= 'g'; } ) == "ghi" );
	TEST( find_span( "abcdefghi_", []( auto c ){ return c == 'x'; } ) == "" );


	/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous V >
	[[nodiscard]] constexpr std::size_t find(
		V		  && v_, 
		linebreak
	) noexcept {
		return find( v_, linebreak::check );
	};
	TEST( find( "abcdefghi",	linebreak{} ) == 9 );
	TEST( find( "abcd\nefghi",	linebreak{} ) == 4 );
	TEST( find( "abcd\refghi",	linebreak{} ) == 4 );


	/// Find any of "\n\r", "\n", "\r\n", or "\r" and return a shadow reference to it.
	/// If none is found, { end(), 0u } is returned.
	template< contiguous V >
	[[nodiscard]] constexpr Span< V > find_span(
		V			 && v_, 
		linebreak
	) noexcept {
		const auto end	  = no_nullchar_end( v_ );
		std::remove_cv_t< element_type_t< V > >	previous{ ' ' };
		for( auto & c : v_ )	{
			[[unlikely]] if( previous == '\n' )	return { &c - 1, 1u + ( c == '\r' ) };
			[[unlikely]] if( previous == '\r' )	return { &c - 1, 1u + ( c == '\n' ) };
			previous = c;
		}
		return { end - linebreak::check( previous ), end };
	};
	TEST( find_span( "abcdefghi",		linebreak{} ) == "" );
	TEST( find_span( "abcd\nefghi",		linebreak{} ) == "\n" );
	TEST( find_span( "abcd\n\refghi",	linebreak{} ) == "\n\r" );
	TEST( find_span( "abcd\refghi",		linebreak{} ) == "\r" );
	TEST( find_span( "abcd\r\nefghi",	linebreak{} ) == "\r\n" );
	TEST( find_span( "abcd\n\n\refghi",	linebreak{} ) == "\n" );


	/// Return the beginning of v_ up to but not including the first until_this_.
	/// - If no until_this_ is found, v_ is returned.
	template< traits::contiguous V, typename U >
	constexpr auto until( V && v_, U && until_this_ ) noexcept {
		return first( v_, find( v_, until_this_ ) );
	}
	TEST( until( "abcdefghi", "ghi" ) == "abcdef" );
	TEST( until( "abcdefghi", "ghx" ) == "abcdefghi" );


	/// Returns true iff find( v_, x_ ) < size( v_ ).
	template< traits::contiguous V, typename X >
	[[nodiscard]] constexpr bool contains( V && v_, X && x_ )							noexcept	{
		return find( v_, x_ ) < no_nullchar_size( v_ );
	}
	TEST(  contains( "abcdefghi", "def" ) );
	TEST( !contains( "abcdefghi", "dex" ) );
	TEST(  contains( "abc\nghi",  linebreak{} ) );
	TEST( !contains( "abcdefghi", linebreak{} ) );
	TEST(  contains( "abc\nghi",  []( auto c_ ) { return c_ == '\n'; } ) );
	TEST( !contains( "abcdefghi", []( auto c_ ) { return c_ == '\n'; } ) );

	

	template< typename T >
	struct split_result {
		std::span< T > first{}, rest{};
		constexpr bool empty()	const	{	return first.empty() && rest.empty();	}
	};


	/// Split this in two at offset t_ so that first.end() == rest.begin() and first.size() == t_.
	template< contiguous V >
	[[nodiscard]] constexpr auto split_at( V && v_, std::size_t mid_ )					noexcept	{
		using Result			  = split_result< element_type_t< V > >;
		const auto sz			  = no_nullchar_size( v_ );
		mid_					  = std::min( mid_, sz );
		return Result{ { begin( v_ ), mid_ }, { begin( v_ ) + mid_, sz - mid_ } };
	}
	TEST( split_at( "abcdefghi",  6 ).first == "abcdef" );
	TEST( split_at( "abcdefghi",  6 ).rest  == "ghi" );
	TEST( split_at( "abcdefghi", 12 ).first == "abcdefghi" );
	TEST( split_at( "abcdefghi", 12 ).rest  == "" );


	/// Split this in two: before and after gap_, but everything clamped to [begin(), end()].
	template< contiguous V0, typename V1 >
	[[nodiscard]] constexpr auto split( V0 && v_, V1 && gap_ )							noexcept	{
		using Result			  = split_result< element_type_t< V0 > >;
		const auto gap			  = find_span( v_, gap_ );
		const auto end			  = data( v_ ) + no_nullchar_size( v_ );
		return Result{	{ data( v_ ), std::clamp( data( gap ),  data( v_ ), end ) },
						{ std::clamp( data( gap ) + no_nullchar_size( gap ), data( v_ ), end ), end } };
	}
	TEST( split( "abcdefghi", "def" ).first == "abc" );
	TEST( split( "abcdefghi", "def" ).rest  == "ghi" );
	TEST( split( "abcdefghi", "dex" ).first == "abcdefghi" );
	TEST( split( "abcdefghi", "dex" ).rest  == "" );
	TEST( split( "abc\nghi",  linebreak{} ).first == "abc" );
	TEST( split( "abc\nghi",  linebreak{} ).rest  == "ghi" );
	TEST( split( "abcdefghi", linebreak{} ).first == "abcdefghi" );
	TEST( split( "abcdefghi", linebreak{} ).rest  == "" );
	TEST( split( "abc\nghi",  []( auto c_ ) { return c_ == '\n'; } ).first == "abc" );
	TEST( split( "abc\nghi",  []( auto c_ ) { return c_ == '\n'; } ).rest  == "ghi" );
	TEST( split( "abcdefghi", []( auto c_ ) { return c_ == '\n'; } ).first == "abcdefghi" );
	TEST( split( "abcdefghi", []( auto c_ ) { return c_ == '\n'; } ).rest  == "" );




	/// A class to simplify iterating using ´split_by´. It uses views, so the original string must remain static.
	/// - Example usage: ´for( const auto item : String_view_splitter( "A\nNumber\nof\nRows", linebreak{} ) ) { ... }´. 
	/// - The Divider type may be any that is accepted by ´split_by( ..., Divider )´. 
	/// - String_view_splitter is constexpr [and never throws]. 
	template< traits::character Char, typename Divider >
	class String_view_splitter {
		class End{};
		using Span					  = std::span< Char >;

		split_result< Char >			m_parts;
		Divider							m_divider;
		
	public:
		// constexpr String_view_splitter( const Value str_, const Divider div_ ) 					noexcept
		// 	: m_parts{ split( str_, div_ ) }, m_divider{ div_ } {}

		template< contiguous V >						requires( std::is_same_v< Char, element_type_t< V > > )
		constexpr String_view_splitter( V && v_, const Divider div_ ) 	noexcept
			: m_parts{ split( v_, div_ ) }, m_divider{ div_ } {}

		constexpr String_view_splitter & operator++()	noexcept		{
			m_parts = split( m_parts.rest, m_divider );
			return *this;
		}

		constexpr Span operator*()						const noexcept	{	return m_parts.first;			}
		constexpr bool operator==( End )				const noexcept	{	return m_parts.empty();			}
		constexpr String_view_splitter begin()			const noexcept	{	return *this;					}
		constexpr End end()								const noexcept	{	return {};						}
	};

	template< traits::string S, typename D >
	String_view_splitter( S &&, D )	-> String_view_splitter< traits::element_type_t< S >, D >;

	template< traits::character Ch, typename D >
	String_view_splitter( Ch *, D )	-> String_view_splitter< std::remove_reference_t< Ch >, D >;

}	// namespace pax
#undef TEST
