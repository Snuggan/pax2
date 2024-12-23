//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/format.hpp>
#include <pax/std/algorithm.hpp>
#include <pax/doctest.hpp>

#include <pax/textual/as_ascii.hpp>
#include <pax/reporting/debug.hpp>

#include <sstream>
#include <array>
#include <vector>


namespace pax {
	
	


	constexpr auto								str		= "abcdefghijkl";
	constexpr auto	 							strN	= make_span( "abcdefghijkl" );
	constexpr const std::string_view			e		= view( "" );

	constexpr const std::size_t 				N = 12;
	constexpr const int							ints0[ N ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	constexpr const std::span< const int >		ints	= dview( ints0 );
	constexpr const std::span< const int, N >	intsN	=  view( ints0 );
	constexpr const auto						abc		= std::span< const char >{ str, 12 };
	
	template< typename T >
	constexpr bool is_value_const = std::is_const_v< std::remove_reference_t< Value_type_t< T > > >;
	
	template< typename T >
	constexpr auto safe_subview( T && t_, int i_, int sz_ ) {
		using std::size;
		if( i_ < 0 )	i_ += size( t_ );
		if( i_ < 0 )	i_  = 0;
		return first( not_first( t_, i_ ), sz_ );
	}
	
	
	
	namespace intrinsics {
		// std::span< const int >
			using Span = std::span< const int >;
			static_assert(!std::is_aggregate_v< Span > );
			static_assert( std::is_standard_layout_v< Span > );
			static_assert( std::is_trivially_copyable_v< Span > );
			static_assert(!std::has_virtual_destructor_v< Span > );
			static_assert( std::is_nothrow_destructible_v< Span > );
			static_assert( std::is_trivially_destructible_v< Span > );
			static_assert( std::is_nothrow_default_constructible_v< Span > == ( Span::extent == std::dynamic_extent ) );
			static_assert(!std::is_trivially_default_constructible_v< Span > );
			static_assert( std::is_nothrow_copy_constructible_v< Span > );
			static_assert( std::is_trivially_copy_constructible_v< Span > );
			static_assert( std::is_nothrow_copy_assignable_v< Span > );
			static_assert( std::is_trivially_copy_assignable_v< Span > );
			static_assert( std::is_nothrow_move_constructible_v< Span > );
			static_assert( std::is_trivially_move_constructible_v< Span > );
			static_assert( std::is_nothrow_move_assignable_v< Span > );
			static_assert( std::is_trivially_move_assignable_v< Span > );
			static_assert( std::is_nothrow_swappable_v< Span > );

		// std::span< const int, 3 >
			using Span3 = std::span< const int, 3 >;
			static_assert(!std::is_aggregate_v< Span3 > );
			static_assert( std::is_standard_layout_v< Span3 > );
			static_assert( std::is_trivially_copyable_v< Span3 > );
			static_assert(!std::has_virtual_destructor_v< Span3 > );
			static_assert( std::is_nothrow_destructible_v< Span3 > );
			static_assert( std::is_trivially_destructible_v< Span3 > );
			static_assert( std::is_nothrow_default_constructible_v< Span3 > == ( Span3::extent == std::dynamic_extent ) );
			static_assert(!std::is_trivially_default_constructible_v< Span3 > );
			static_assert( std::is_nothrow_copy_constructible_v< Span3 > );
			static_assert( std::is_trivially_copy_constructible_v< Span3 > );
			static_assert( std::is_nothrow_copy_assignable_v< Span3 > );
			static_assert( std::is_trivially_copy_assignable_v< Span3 > );
			static_assert( std::is_nothrow_move_constructible_v< Span3 > );
			static_assert( std::is_trivially_move_constructible_v< Span3 > );
			static_assert( std::is_nothrow_move_assignable_v< Span3 > );
			static_assert( std::is_trivially_move_assignable_v< Span3 > );
			static_assert( std::is_nothrow_swappable_v< Span3 > );
		// std::string_view
			using View = std::string_view;
			static_assert(!std::is_aggregate_v< View > );
			static_assert( std::is_standard_layout_v< View > );
			static_assert( std::is_trivially_copyable_v< View > );
			static_assert(!std::has_virtual_destructor_v< View > );
			static_assert( std::is_nothrow_destructible_v< View > );
			static_assert( std::is_trivially_destructible_v< View > );
			static_assert(!std::is_trivially_default_constructible_v< View > );
			static_assert( std::is_nothrow_copy_constructible_v< View > );
			static_assert( std::is_trivially_copy_constructible_v< View > );
			static_assert( std::is_nothrow_copy_assignable_v< View > );
			static_assert( std::is_trivially_copy_assignable_v< View > );
			static_assert( std::is_nothrow_move_constructible_v< View > );
			static_assert( std::is_trivially_move_constructible_v< View > );
			static_assert( std::is_nothrow_move_assignable_v< View > );
			static_assert( std::is_trivially_move_assignable_v< View > );
			static_assert( std::is_nothrow_swappable_v< View > );
	}


	DOCTEST_TEST_CASE( "textual" ) {
		{	// Newline::is_newline
			static_assert(  Newline::is_newline( '\n' ) );
			static_assert(  Newline::is_newline( '\r' ) );
			static_assert( !Newline::is_newline( '\a' ) );
			static_assert( !Newline::is_newline( ' '  ) );
		}
		{	// Newline::is_newline2
			static_assert( Newline::is_newline2( '\n', '\r' ) == 2 );
			static_assert( Newline::is_newline2( '\r', '\n' ) == 2 );
			static_assert( Newline::is_newline2( '\n', '\n' ) == 1 );
			static_assert( Newline::is_newline2( '\r', '\r' ) == 1 );
			static_assert( Newline::is_newline2( '\n', '\a' ) == 1 );
			static_assert( Newline::is_newline2( '\r', '\a' ) == 1 );
			static_assert( Newline::is_newline2( '\a', '\n' ) == 0 );
			static_assert( Newline::is_newline2( '\a', '\r' ) == 0 );
		}
		{	// std::format output
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", std::string_view( "abc" ) ),	"abc" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( abc, 3 ) ),				"abc" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", view( first( abc, 3 ) ) ),		"abc" );

			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", make_span( first( abc, 3 ) ) ),	"['a', 'b', 'c']" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 0 ) ),	"[]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 1 ) ),	"[0]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 3 ) ),	"[0, 1, 2]" );
		}
	}
	DOCTEST_TEST_CASE( "view_type, make_span" ) {
		{	// view, dview, view_type, view_type
			static_assert( std::is_same_v< view_type_t< std::string_view >,				std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::string_view >,				std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::string >,					std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::span< int > >,				std::span< int > > );
			static_assert( std::is_same_v< view_type_t< std::span< int, 4 > >,			std::span< int, 4 > > );
			static_assert( std::is_same_v< view_type_t< std::vector< int > >,			std::span< int > > );
			static_assert( std::is_same_v< view_type_t< std::array < int, 4 > >,		std::span< int, 4 > > );

			static_assert( std::is_same_v< view_type_t< std::string_view, true >,		std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::string_view, true >,		std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::string, true >,			std::string_view > );
			static_assert( std::is_same_v< view_type_t< std::span< int >, true >,		std::span< int > > );
			static_assert( std::is_same_v< view_type_t< std::span< int, 4 >, true >,	std::span< int > > );
			static_assert( std::is_same_v< view_type_t< std::vector< int >, true >,		std::span< int > > );
			static_assert( std::is_same_v< view_type_t< std::array < int, 4 >, true >,	std::span< int > > );

			{
				auto constexpr abcde = make_span( "abcde" );
				static_assert( std::string_view( "abcde" )	==	"abcde" );
				static_assert( abcde.size()					==	5 );
				static_assert( abcde.back()					==	'e' );
				static_assert( abcde						==	std::string_view( "abcde" ) );
				static_assert( abcde						<=	std::string_view( "abcde" ) );
				static_assert( abcde						>=	std::string_view( "abcde" ) );
			} {
				static constexpr const char					res[] = { '\n', '\r', '\n' };
				constexpr auto								v  = view( res );
				constexpr auto								dv = dview( res );
				static_assert( size( v )					==	3 );
				static_assert( size( dv )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				constexpr auto								v  = view( std::span< int >{} );
				constexpr auto								dv = dview( std::span< int >{} );
				static_assert( size( v )					==	0 );
				static_assert( v.extent						==	dynamic_extent );
				static_assert( size( dv )					==	0 );
				static_assert( dv.extent					==	dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::span< int > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::span< int > > );
			} {
				constexpr auto								v  = view( std::span< int, 0 >{} );
				constexpr auto								dv = dview( std::span< int, 0 >{} );
				static_assert( size( v )					==	0 );
				static_assert( v.extent						==	0 );
				static_assert( size( dv )					==	0 );
				static_assert( dv.extent					==	dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::span< int, 0 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::span< int > > );
			} {
				constexpr auto								v  = view( std::string_view{} );
				constexpr auto								dv = dview( std::string_view{} );
				static_assert( size( v )					==	0 );
				static_assert( size( dv )					==	0 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				constexpr auto								v  = view( "" );
				constexpr auto								dv = dview( "" );
				static_assert( size( v )					==	0 );
				static_assert( size( dv )					==	0 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				const std::vector< int >					arr{ 1, 2, 3 };
				const auto									v  = view( arr );
				const auto									dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),			3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::span< const int > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::span< const int > > );
			} {
				static constexpr std::array< int, 3 >		arr{ 1, 2, 3 };
				constexpr auto								v  = view( arr );
				constexpr auto								dv = dview( arr );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::span< const int, 3 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::span< const int > > );
			} {
				static constexpr const int					arr[ 3 ] = { 1, 2, 3 };
				constexpr auto								v  = view( arr );
				constexpr auto								dv = dview( arr );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::span< const int, 3 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::span< const int > > );
			} {
				static constexpr const std::string			arr = "abc";
				constexpr auto								v  = view( arr );
				constexpr auto								dv = dview( arr );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				constexpr const char					  * arr = "abc";
				constexpr auto								v  = view( arr );
				constexpr auto								dv = dview( arr );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				static constexpr const char					arr[ 4 ] = "abc";
				constexpr auto								v  = view( arr );
				constexpr auto								dv = dview( arr );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			} {
				constexpr auto								v = view( "abc" );
				constexpr auto								dv = dview( "abc" );
				static_assert( size( v )					==	3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		std::string_view > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	std::string_view > );
			}
		}
		{	// make_span
			{
				const std::vector< int >		arr{ 1, 2, 3 };
				const auto						sp = make_span( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),	3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,	std::dynamic_extent );
			} {
				static constexpr std::array< int, 3 >	arr{ 1, 2, 3 };
				constexpr auto					sp = make_span( arr );
				static_assert( size( sp )	==	3 );
				static_assert( sp.extent	==	3 );
			} {
				static constexpr int			arr[ 3 ] = { 1, 2, 3 };
				constexpr auto					sp = make_span( arr );
				static_assert( size( sp )	==	3 );
				static_assert( sp.extent	==	3 );
			} {
				constexpr auto					sp = make_span( "abc" );
				static_assert( size( sp )	==	3 );
				static_assert( sp.extent	==	3 );
			} {
				static constexpr const char		arr[ 4 ] = "abc";
				constexpr auto					sp = make_span( arr );
				static_assert( size( sp )	==	3 );
				static_assert( sp.extent	==	3 );
			} {
				constexpr const char		  * arr = "abc";
				constexpr auto					sp = make_span( arr );
				static_assert( size( sp )	==	3 );
				static_assert( sp.extent	==	std::dynamic_extent );
			}
		}
	}
	DOCTEST_TEST_CASE( "specials" ) {
		{	// valid
			constexpr int		*ptr{};
			static_assert( !valid( ptr ) );
			static_assert( !valid( nullptr ) );

			static_assert( !valid( std::string_view{} ) );
			static_assert(  valid( std::string{} ) );
			static_assert(  valid( "" ) );
			static_assert(  valid( e ) );
			static_assert(  valid( str ) );

			static_assert( !valid( std::span< int >{} ) );
			static_assert( !valid( std::vector< int >{} ) );
			static_assert( !valid( std::array< int, 0 >{} ) );
			static_assert(  valid( ints ) );
			static_assert(  valid( intsN ) );
		}
		{	// empty
			using std::empty;
			static_assert(  empty( std::string_view{} ) );
			static_assert(  empty( std::string{} ) );
			static_assert( !std::empty( "" ) );
			static_assert(  empty( view( "" ) ) );
			static_assert(  empty( e ) );
			static_assert( !empty( view( str ) ) );

			static_assert(  empty( std::span< int >{} ) );
			static_assert(  empty( std::vector< int >{} ) );
			static_assert(  empty( std::array< int, 0 >{} ) );
			static_assert( !empty( ints ) );
			static_assert( !empty( intsN ) );
		}
		{	// as_const
			int					sp0[ 3 ] = { 0, 1, 2 };
			const std::span		sp	= dview( sp0 );

			static_assert(  is_value_const< std::string_view > );
			static_assert( !is_value_const< decltype( sp.front() ) > );
			static_assert(  is_value_const< decltype( as_const( sp ).front() ) > );
		}
		{	// as_dynamic
			static_assert( as_dynamic( std::string_view{} ).extent	== dynamic_extent );
			static_assert( as_dynamic( e ).extent					== dynamic_extent );
			static_assert( as_dynamic( str ).extent					== dynamic_extent );

			static_assert( as_dynamic( ints ).extent				== dynamic_extent );
			static_assert( as_dynamic( intsN ).extent				== dynamic_extent );

			static_assert( std::span< int >{}.extent				== dynamic_extent );
			static_assert( ints.extent								== dynamic_extent );
			static_assert( intsN.extent								== N );
		}
		{	// identic
			DOCTEST_FAST_CHECK_UNARY(  identic( "abc", "abc" ) );
			DOCTEST_FAST_CHECK_UNARY( !identic( "abc", first( "abcdef", 3 ) ) );

			constexpr auto	str2 = view( str );
			static_assert(  identic( str, str ) );
			static_assert( !identic( str2, first( str2, 2 ) ) );
			static_assert(  identic( str2, first( str2, str2.size() ) ) );

			static_assert(  identic( ints, ints ) );
			static_assert(  identic( ints, intsN ) );
			static_assert( !identic( ints, intsN. first( 2 ) ) );
			static_assert(  identic( ints, intsN. first( intsN.size() ) ) );
		}
		{	// overlap
			constexpr std::span				subc{ subview( str, 3, 3 ) };	// "def"
			static_assert( !overlap( std::string_view{}, std::string_view{} ) );
			static_assert(  overlap( str, str ) );
			static_assert(  overlap( subc, subc ) );
			static_assert(  overlap( subc, str ) );
			static_assert(  overlap( str, subc ) );
			static_assert( !overlap( str, subview( str, 3, 0 ) ) );			// An emptry span cannot overlap
			static_assert( !overlap( subview( str, 3, 0 ), str ) );			// An emptry span cannot overlap
			static_assert( !overlap( subc, first( str, 3 ) ) );				// Kloss i kloss
			static_assert(  overlap( subc, first( str, 4 ) ) );				// First overlapion
			static_assert(  overlap( subc, not_first( str, 4 ) ) );			// Middle overlapion
			static_assert(  overlap( subc, not_first( str, 5 ) ) );			// Last overlapion
			static_assert( !overlap( subc, not_first( str, 6 ) ) );			// Kloss i kloss
			static_assert( !overlap( subc, subview( str, 2, 0 ) ) );
			static_assert( !overlap( subc, subview( str, 3, 0 ) ) );
			static_assert( !overlap( subc, subview( str, 4, 0 ) ) );
			static_assert( !overlap( subc, subview( str, 5, 0 ) ) );
			static_assert( !overlap( subc, subview( str, 6, 0 ) ) );

			constexpr std::span				subi{ subview( ints, 3, 3 ) };	// { 3, 4, 5 }
			static_assert( !overlap( std::span< int >{}, std::span< int >{} ) );
			static_assert(  overlap( ints, ints ) );
			static_assert(  overlap( subi, subi ) );
			static_assert(  overlap( subi, ints ) );
			static_assert(  overlap( ints, subi ) );
			static_assert( !overlap( ints, subview( ints, 3, 0 ) ) );		// An emptry span cannot overlap
			static_assert( !overlap( subview( ints, 3, 0 ), ints ) );		// An emptry span cannot overlap
			static_assert( !overlap( subi, first( ints, 3 ) ) );			// Kloss i kloss
			static_assert(  overlap( subi, first( ints, 4 ) ) );			// First overlapion
			static_assert(  overlap( subi, not_first( ints, 4 ) ) );		// Middle overlapion
			static_assert(  overlap( subi, not_first( ints, 5 ) ) );		// Last overlapion
			static_assert( !overlap( subi, not_first( ints, 6 ) ) );		// Kloss i kloss
			static_assert( !overlap( subi, subview( ints, 2, 0 ) ) );
			static_assert( !overlap( subi, subview( ints, 3, 0 ) ) );
			static_assert( !overlap( subi, subview( ints, 4, 0 ) ) );
			static_assert( !overlap( subi, subview( ints, 5, 0 ) ) );
			static_assert( !overlap( subi, subview( ints, 6, 0 ) ) );
		}
	}
	DOCTEST_TEST_CASE( "parts" ) {
		constexpr auto						first_	= "abcde";
		constexpr auto						last_	= "hijkl";
		constexpr const std::string_view	null_{};
		constexpr auto						empty_	= "";
		
		{	// front, back
			static_assert( front( "abc" ) == 'a' );
			static_assert( back ( "abc" ) == 'c' );

			char						str2[ 10 ] = { '1','2','3','4','5','6','7','8','9','0' };
			const std::span				v( str2 );
			v[ 0 ]			= 'a';		DOCTEST_FAST_CHECK_EQ( front( v ), 'a' );
			front( v )		= 'c';		DOCTEST_FAST_CHECK_EQ( front( v ), 'c' );
			back( v )		= 'd';		DOCTEST_FAST_CHECK_EQ( back( v ),  'd' );
			*v.begin()		= 'e';		DOCTEST_FAST_CHECK_EQ( front( v ), 'e' );
			*v.rbegin()		= 'f';		DOCTEST_FAST_CHECK_EQ( back( v ),  'f' );
		}
		{	// first (character)
			static_assert( first( "abcdefghijkl", 22 )		==	str );
			static_assert( first< 22 >( "abcdefghijkl" )	==	"abcdefghijkl" );
			static_assert( first< 22 >( "abcdefghijkl" ).size()	==	12 );
			static_assert( first<  5 >( "abcdefghijkl" )	==	first_ );
			static_assert( first< 12 >( "abcdefghijkl" )	==	str );

			static_assert( first( str,  0 )					==	empty_ );
			static_assert( first( str,  5 )					==	first_ );
			static_assert( identic( first( str, 12 ), str ) );
			static_assert( identic( first( str, 22 ), str ) );

			static_assert( first( strN,  0 )				==	empty_ );
			static_assert( first( strN,  5 )				==	first_ );
			static_assert( identic( first( strN, 12 ), strN ) );
			static_assert( identic( first( strN, 22 ), strN ) );

			static_assert( first<  0 >( str )				==	null_ );
			static_assert( first<  5 >( str )				==	first_ );
			static_assert( identic( first< 12 >( str ), str ) );

			static_assert( first<  0 >( strN )				==	null_ );
			static_assert( first<  5 >( strN )				==	first_ );
			static_assert( identic( first< 12 >( strN ), strN ) );
			static_assert( identic( first< 22 >( strN ), strN ) );
		}
		{	// first (int)
			static_assert( first( ints,   0 ).size()	==	0 );
			static_assert( first( ints,   5 ).back()	==	4 );
			static_assert( identic( first( ints, 12 ), ints ) );
			static_assert( identic( first( ints, 22 ), ints ) );

			static_assert( first( intsN,  0 ).size()	==	0 );
			static_assert( first( intsN,  5 ).back()	==	4 );
			static_assert( identic( first( intsN, 12 ), intsN ) );
			static_assert( identic( first( intsN, 22 ), intsN ) );

			static_assert( first<  0 >( ints  ).extent	==	0 );
			static_assert( first<  5 >( ints  ).back()	==	4 );
			static_assert( first<  5 >( ints  ).extent	==	5 );
			static_assert( identic( first< 12 >( ints ), ints ) );

			static_assert( first<  0 >( intsN ).extent	==	0 );
			static_assert( first<  5 >( intsN ).back()	==	4 );
			static_assert( first<  5 >( intsN ).extent	==	5 );
			static_assert( identic( first< 12 >( intsN ), intsN ) );
			static_assert( identic( first< 22 >( intsN ), intsN ) );
		}
		{	// last (character)
			static_assert( last( "abcdefghijkl", 22 )		==	str );
			static_assert( last< 22 >( "abcdefghijkl" )		==	"abcdefghijkl" );
			static_assert( last< 22 >( "abcdefghijkl" ).size()	==	12 );
			static_assert( last<  5 >( "abcdefghijkl" )		==	last_ );
			static_assert( last< 12 >( "abcdefghijkl" )		==	str );

			static_assert( last( str,  0 )					==	empty_ );
			static_assert( last( str,  5 )					==	last_ );
			static_assert( identic( last( str, 12 ), str ) );
			static_assert( identic( last( str, 22 ), str ) );

			static_assert( last( strN,  0 )					==	empty_ );
			static_assert( last( strN,  5 )					==	last_ );
			static_assert( identic( last( strN, 12 ), strN ) );
			static_assert( identic( last( strN, 22 ), strN ) );

			static_assert( last<  0 >( str )				==	null_ );
			static_assert( last<  5 >( str )				==	last_ );
			static_assert( identic( last< 12 >( str ), str ) );

			static_assert( last<  0 >( strN )				==	null_ );
			static_assert( last<  5 >( strN )				==	last_ );
			static_assert( identic( last< 12 >( strN ), strN ) );
			static_assert( identic( last< 22 >( strN ), strN ) );
		}
		{	// last (int)
			static_assert( last( ints,   0 ).size()		==	0 );
			static_assert( last( ints,   5 ).front()	==	7 );
			static_assert( identic( last( ints, 12 ), ints ) );
			static_assert( identic( last( ints, 22 ), ints ) );

			static_assert( last( intsN,  0 ).size()		==	0 );
			static_assert( last( intsN,  5 ).front()	==	7 );
			static_assert( identic( last( intsN, 12 ), intsN ) );
			static_assert( identic( last( intsN, 22 ), intsN ) );

			static_assert( last<  0 >( ints  ).extent	==	0 );
			static_assert( last<  5 >( ints  ).front()	==	7 );
			static_assert( last<  5 >( ints  ).extent	==	5 );
			static_assert( identic( last< 12 >( ints ), ints ) );

			static_assert( last<  0 >( intsN ).extent	==	0 );
			static_assert( last<  5 >( intsN ).front()	==	7 );
			static_assert( last<  5 >( intsN ).extent	==	5 );
			static_assert( identic( last< 12 >( intsN ), intsN ) );
			static_assert( identic( last< 22 >( intsN ), intsN ) );
		}
		{	// not_first (character)
			static_assert( not_first( "abcdefghijkl", 0 )		==	str );
			static_assert( not_first< 0 >( "abcdefghijkl" )		==	"abcdefghijkl" );
			static_assert( not_first< 0 >( "abcdefghijkl" ).size()	==	12 );
			static_assert( not_first<  5 >( "abcdefghijkl" )	==	last< 7 >( str ) );
			static_assert( not_first< 12 >( "abcdefghijkl" )	==	last< 0 >( str ) );

			static_assert( identic( not_first( str,   0 ), str ) );
			static_assert( identic( not_first( str,   5 ), last< 7 >( str ) ) );
			static_assert( identic( not_first( str,  12 ), last< 0 >( str ) ) );
			static_assert( identic( not_first( str,  22 ), last< 0 >( str ) ) );

			static_assert( identic( not_first( strN,  0 ), strN ) );
			static_assert( identic( not_first( strN,  5 ), last< 7 >( strN ) ) );
			static_assert( identic( not_first( strN, 12 ), last< 0 >( strN ) ) );
			static_assert( identic( not_first( strN, 22 ), last< 0 >( strN ) ) );
			static_assert( identic( not_first( null_, 22 ), null_ ) );

			static_assert( identic( not_first<  0 >( strN ), strN ) );
			static_assert( identic( not_first<  5 >( strN ), last< 7 >( strN ) ) );
			static_assert( identic( not_first< 12 >( strN ), last< 0 >( strN ) ) );
			static_assert( identic( not_first< 22 >( strN ), last< 0 >( strN ) ) );
		}
		{	// not_first (int)
			static_assert( identic( not_first( ints,   0 ), ints ) );
			static_assert( identic( not_first( ints,   5 ), last< 7 >( ints ) ) );
			static_assert( identic( not_first( ints,  12 ), last< 0 >( ints ) ) );
			static_assert( identic( not_first( ints,  22 ), last< 0 >( ints ) ) );

			static_assert( identic( not_first( intsN,  0 ), intsN ) );
			static_assert( identic( not_first( intsN,  5 ), last< 7 >( intsN ) ) );
			static_assert( identic( not_first( intsN, 12 ), last< 0 >( intsN ) ) );
			static_assert( identic( not_first( intsN, 22 ), last< 0 >( intsN ) ) );

			static_assert( identic( not_first<  0 >( intsN ), intsN ) );
			static_assert( identic( not_first<  5 >( intsN ), last< 7 >( intsN ) ) );
			static_assert( identic( not_first< 12 >( intsN ), last< 0 >( intsN ) ) );
			static_assert( identic( not_first< 22 >( intsN ), last< 0 >( intsN ) ) );
		}
		{	// not_last (character)
			static_assert( not_last( "abcdefghijkl", 0 )		==	str );
			static_assert( not_last< 0 >( "abcdefghijkl" )		==	"abcdefghijkl" );
			static_assert( not_last< 0 >( "abcdefghijkl" ).size()	==	12 );
			static_assert( not_last<  5 >( "abcdefghijkl" )		==	first< 7 >( str ) );
			static_assert( not_last< 12 >( "abcdefghijkl" )		==	first< 0 >( str ) );

			static_assert( identic( not_last( str,   0 ), str ) );
			static_assert( identic( not_last( str,   5 ), first< 7 >( str ) ) );
			static_assert( identic( not_last( str,  12 ), first< 0 >( str ) ) );
			static_assert( identic( not_last( str,  22 ), first< 0 >( str ) ) );

			static_assert( identic( not_last( strN,  0 ), strN ) );
			static_assert( identic( not_last( strN,  5 ), first< 7 >( strN ) ) );
			static_assert( identic( not_last( strN, 12 ), first< 0 >( strN ) ) );
			static_assert( identic( not_last( strN, 22 ), first< 0 >( strN ) ) );
			static_assert( identic( not_last( null_, 22 ), null_ ) );

			static_assert( identic( not_last<  0 >( strN ), strN ) );
			static_assert( identic( not_last<  5 >( strN ), first< 7 >( strN ) ) );
			static_assert( identic( not_last< 12 >( strN ), first< 0 >( strN ) ) );
			static_assert( identic( not_last< 22 >( strN ), first< 0 >( strN ) ) );
		}
		{	// not_last (int)
			static_assert( identic( not_last( ints,   0 ), ints ) );
			static_assert( identic( not_last( ints,   5 ), first< 7 >( ints ) ) );
			static_assert( identic( not_last( ints,  12 ), first< 0 >( ints ) ) );
			static_assert( identic( not_last( ints,  22 ), first< 0 >( ints ) ) );

			static_assert( identic( not_last( intsN,  0 ), intsN ) );
			static_assert( identic( not_last( intsN,  5 ), first< 7 >( intsN ) ) );
			static_assert( identic( not_last( intsN, 12 ), first< 0 >( intsN ) ) );
			static_assert( identic( not_last( intsN, 22 ), first< 0 >( intsN ) ) );

			static_assert( identic( not_last<  0 >( intsN ), intsN ) );
			static_assert( identic( not_last<  5 >( intsN ), first< 7 >( intsN ) ) );
			static_assert( identic( not_last< 12 >( intsN ), first< 0 >( intsN ) ) );
			static_assert( identic( not_last< 22 >( intsN ), first< 0 >( intsN ) ) );
		}
		{	// subview (character)
			static_assert( identic( subview( null_,   0,  0 ),	safe_subview( null_,   0,  0 ) ) );
			static_assert( identic( subview( null_,   0,  3 ),	safe_subview( null_,   0,  3 ) ) );
			static_assert( identic( subview( null_,   0, 12 ),	safe_subview( null_,   0, 12 ) ) );
			static_assert( identic( subview( null_,   0, 22 ),	safe_subview( null_,   0, 22 ) ) );
			static_assert( identic( subview( null_,   5,  0 ),	safe_subview( null_,   5,  0 ) ) );
			static_assert( identic( subview( null_,   5,  3 ),	safe_subview( null_,   5,  3 ) ) );
			static_assert( identic( subview( null_,   5, 12 ),	safe_subview( null_,   5, 12 ) ) );
			static_assert( identic( subview( null_,   5, 22 ),	safe_subview( null_,   5, 22 ) ) );
			static_assert( identic( subview( null_,  -5,  0 ),	safe_subview( null_,  -5,  0 ) ) );
			static_assert( identic( subview( null_,  -5,  3 ),	safe_subview( null_,  -5,  3 ) ) );
			static_assert( identic( subview( null_,  -5, 12 ),	safe_subview( null_,  -5, 12 ) ) );
			static_assert( identic( subview( null_,  -5, 22 ),	safe_subview( null_,  -5, 22 ) ) );
			static_assert( identic( subview( null_,  12,  0 ),	safe_subview( null_,  12,  0 ) ) );
			static_assert( identic( subview( null_,  12,  3 ),	safe_subview( null_,  12,  3 ) ) );
			static_assert( identic( subview( null_,  12, 12 ),	safe_subview( null_,  12, 12 ) ) );
			static_assert( identic( subview( null_,  12, 22 ),	safe_subview( null_,  12, 22 ) ) );
			static_assert( identic( subview( null_, -12,  0 ),	safe_subview( null_, -12,  0 ) ) );
			static_assert( identic( subview( null_, -12,  3 ),	safe_subview( null_, -12,  3 ) ) );
			static_assert( identic( subview( null_, -12, 12 ),	safe_subview( null_, -12, 12 ) ) );
			static_assert( identic( subview( null_, -12, 22 ),	safe_subview( null_, -12, 22 ) ) );
			static_assert( identic( subview( null_,  22,  0 ),	safe_subview( null_,  22,  0 ) ) );
			static_assert( identic( subview( null_,  22,  5 ),	safe_subview( null_,  22,  3 ) ) );
			static_assert( identic( subview( null_,  22, 12 ),	safe_subview( null_,  22, 12 ) ) );
			static_assert( identic( subview( null_,  22, 22 ),	safe_subview( null_,  22, 22 ) ) );
			static_assert( identic( subview( null_, -22,  0 ),	safe_subview( null_, -22,  0 ) ) );
			static_assert( identic( subview( null_, -22,  5 ),	safe_subview( null_, -22,  3 ) ) );
			static_assert( identic( subview( null_, -22, 12 ),	safe_subview( null_, -22, 12 ) ) );
			static_assert( identic( subview( null_, -22, 22 ),	safe_subview( null_, -22, 22 ) ) );

			static_assert( identic( subview( str,   0,  0 ),	safe_subview( str,   0,  0 ) ) );
			static_assert( identic( subview( str,   0,  3 ),	safe_subview( str,   0,  3 ) ) );
			static_assert( identic( subview( str,   0, 12 ),	safe_subview( str,   0, 12 ) ) );
			static_assert( identic( subview( str,   0, 22 ),	safe_subview( str,   0, 22 ) ) );
			static_assert( identic( subview( str,   5,  0 ),	safe_subview( str,   5,  0 ) ) );
			static_assert( identic( subview( str,   5,  3 ),	safe_subview( str,   5,  3 ) ) );
			static_assert( identic( subview( str,   5, 12 ),	safe_subview( str,   5, 12 ) ) );
			static_assert( identic( subview( str,   5, 22 ),	safe_subview( str,   5, 22 ) ) );
			static_assert( identic( subview( str,  -5,  0 ),	safe_subview( str,  -5,  0 ) ) );
			static_assert( identic( subview( str,  -5,  3 ),	safe_subview( str,  -5,  3 ) ) );
			static_assert( identic( subview( str,  -5, 12 ),	safe_subview( str,  -5, 12 ) ) );
			static_assert( identic( subview( str,  -5, 22 ),	safe_subview( str,  -5, 22 ) ) );
			static_assert( identic( subview( str,  12,  0 ),	safe_subview( str,  12,  0 ) ) );
			static_assert( identic( subview( str,  12,  3 ),	safe_subview( str,  12,  3 ) ) );
			static_assert( identic( subview( str,  12, 12 ),	safe_subview( str,  12, 12 ) ) );
			static_assert( identic( subview( str,  12, 22 ),	safe_subview( str,  12, 22 ) ) );
			static_assert( identic( subview( str, -12,  0 ),	safe_subview( str, -12,  0 ) ) );
			static_assert( identic( subview( str, -12,  3 ),	safe_subview( str, -12,  3 ) ) );
			static_assert( identic( subview( str, -12, 12 ),	safe_subview( str, -12, 12 ) ) );
			static_assert( identic( subview( str, -12, 22 ),	safe_subview( str, -12, 22 ) ) );
			static_assert( identic( subview( str,  22,  0 ),	safe_subview( str,  22,  0 ) ) );
			static_assert( identic( subview( str,  22,  3 ),	safe_subview( str,  22,  3 ) ) );
			static_assert( identic( subview( str,  22, 12 ),	safe_subview( str,  22, 12 ) ) );
			static_assert( identic( subview( str,  22, 22 ),	safe_subview( str,  22, 22 ) ) );
			static_assert( identic( subview( str, -22,  0 ),	safe_subview( str, -22,  0 ) ) );
			static_assert( identic( subview( str, -22,  3 ),	safe_subview( str, -22,  3 ) ) );
			static_assert( identic( subview( str, -22, 12 ),	safe_subview( str, -22, 12 ) ) );
			static_assert( identic( subview( str, -22, 22 ),	safe_subview( str, -22, 22 ) ) );

			static_assert( identic( subview( strN,  0,  0 ),	safe_subview( strN,  0,  0 ) ) );
			static_assert( identic( subview( strN,  0,  3 ),	safe_subview( strN,  0,  3 ) ) );
			static_assert( identic( subview( strN,  0, 12 ),	safe_subview( strN,  0, 12 ) ) );
			static_assert( identic( subview( strN,  0, 22 ),	safe_subview( strN,  0, 22 ) ) );
			static_assert( identic( subview( strN,  5,  0 ),	safe_subview( strN,  5,  0 ) ) );
			static_assert( identic( subview( strN,  5,  3 ),	safe_subview( strN,  5,  3 ) ) );
			static_assert( identic( subview( strN,  5, 12 ),	safe_subview( strN,  5, 12 ) ) );
			static_assert( identic( subview( strN,  5, 22 ),	safe_subview( strN,  5, 22 ) ) );
			static_assert( identic( subview( strN, -5,  0 ),	safe_subview( strN, -5,  0 ) ) );
			static_assert( identic( subview( strN, -5,  3 ),	safe_subview( strN, -5,  3 ) ) );
			static_assert( identic( subview( strN, -5, 12 ),	safe_subview( strN, -5, 12 ) ) );
			static_assert( identic( subview( strN, -5, 22 ),	safe_subview( strN, -5, 22 ) ) );
			static_assert( identic( subview( strN, 12,  0 ),	safe_subview( strN, 12,  0 ) ) );
			static_assert( identic( subview( strN, 12,  3 ),	safe_subview( strN, 12,  3 ) ) );
			static_assert( identic( subview( strN, 12, 12 ),	safe_subview( strN, 12, 12 ) ) );
			static_assert( identic( subview( strN, 12, 22 ),	safe_subview( strN, 12, 22 ) ) );
			static_assert( identic( subview( strN,-12,  0 ),	safe_subview( strN,-12,  0 ) ) );
			static_assert( identic( subview( strN,-12,  3 ),	safe_subview( strN,-12,  3 ) ) );
			static_assert( identic( subview( strN,-12, 12 ),	safe_subview( strN,-12, 12 ) ) );
			static_assert( identic( subview( strN,-12, 22 ),	safe_subview( strN,-12, 22 ) ) );
			static_assert( identic( subview( strN, 22,  0 ),	safe_subview( strN, 22,  0 ) ) );
			static_assert( identic( subview( strN, 22,  3 ),	safe_subview( strN, 22,  3 ) ) );
			static_assert( identic( subview( strN, 22, 12 ),	safe_subview( strN, 22, 12 ) ) );
			static_assert( identic( subview( strN, 22, 22 ),	safe_subview( strN, 22, 22 ) ) );
			static_assert( identic( subview( strN,-22,  0 ),	safe_subview( strN,-22,  0 ) ) );
			static_assert( identic( subview( strN,-22,  3 ),	safe_subview( strN,-22,  3 ) ) );
			static_assert( identic( subview( strN,-22, 12 ),	safe_subview( strN,-22, 12 ) ) );
			static_assert( identic( subview( strN,-22, 22 ),	safe_subview( strN,-22, 22 ) ) );

			static_assert( identic( subview<  0 >( str,   0 ),	safe_subview( str,   0,  0 ) ) );
			static_assert( identic( subview<  0 >( str,   5 ),	safe_subview( str,   5,  0 ) ) );
			static_assert( identic( subview<  0 >( str,  -5 ),	safe_subview( str,  -5,  0 ) ) );
			static_assert( identic( subview<  0 >( str,  12 ),	safe_subview( str,  12,  0 ) ) );
			static_assert( identic( subview<  0 >( str, -12 ),	safe_subview( str, -12,  0 ) ) );
			static_assert( identic( subview<  0 >( str,  22 ),	safe_subview( str,  22,  0 ) ) );
			static_assert( identic( subview<  3 >( str,   0 ),	safe_subview( str,   0,  3 ) ) );
			static_assert( identic( subview<  3 >( str,   5 ),	safe_subview( str,   5,  3 ) ) );
			static_assert( identic( subview<  3 >( str,  -5 ),	safe_subview( str,  -5,  3 ) ) );
			static_assert( identic( subview< 12 >( str,   0 ),	safe_subview( str,   0, 12 ) ) );
			static_assert( identic( subview< 12 >( str,   0 ),	safe_subview( str,   0, 12 ) ) );

			static_assert( identic( subview<  0 >( strN,  0 ),	safe_subview( strN,  0,  0 ) ) );
			static_assert( identic( subview<  0 >( strN,  5 ),	safe_subview( strN,  5,  0 ) ) );
			static_assert( identic( subview<  0 >( strN, -5 ),	safe_subview( strN, -5,  0 ) ) );
			static_assert( identic( subview<  0 >( strN, 12 ),	safe_subview( strN, 12,  0 ) ) );
			static_assert( identic( subview<  0 >( strN,-12 ),	safe_subview( strN,-12,  0 ) ) );
			static_assert( identic( subview<  0 >( strN, 22 ),	safe_subview( strN, 22,  0 ) ) );
			static_assert( identic( subview<  3 >( strN,  0 ),	safe_subview( strN,  0,  3 ) ) );
			static_assert( identic( subview<  3 >( strN,  5 ),	safe_subview( strN,  5,  3 ) ) );
			static_assert( identic( subview<  3 >( strN, -5 ),	safe_subview( strN, -5,  3 ) ) );
			static_assert( identic( subview< 12 >( strN,  0 ),	safe_subview( strN,  0, 12 ) ) );

			static_assert( identic( subview<  0,  0 >( str  ),	safe_subview( str,   0,  0 ) ) );
			static_assert( identic( subview<  0,  3 >( str  ),	safe_subview( str,   0,  3 ) ) );
			static_assert( identic( subview<  0, 12 >( str  ),	safe_subview( str,   0, 12 ) ) );
			// static_assert( identic( subview<  0, 22 >( str  ),	safe_subview( str,   0, 22 ) ) );
			static_assert( identic( subview<  5,  0 >( str  ),	safe_subview( str,   5,  0 ) ) );
			static_assert( identic( subview<  5,  3 >( str  ),	safe_subview( str,   5,  3 ) ) );
			// static_assert( identic( subview<  5, 12 >( str  ),	safe_subview( str,   5, 12 ) ) );
			// static_assert( identic( subview<  5, 22 >( str  ),	safe_subview( str,   5, 22 ) ) );
			static_assert( identic( subview< -5,  0 >( str  ),	safe_subview( str,  -5,  0 ) ) );
			static_assert( identic( subview< -5,  3 >( str  ),	safe_subview( str,  -5,  3 ) ) );
			// static_assert( identic( subview< -5, 12 >( str  ),	safe_subview( str,  -5, 12 ) ) );
			// static_assert( identic( subview< -5, 22 >( str  ),	safe_subview( str,  -5, 22 ) ) );
			static_assert( identic( subview< 12,  0 >( str  ),	safe_subview( str,  12,  0 ) ) );
			// static_assert( identic( subview< 12,  3 >( str  ),	safe_subview( str,  12,  3 ) ) );
			// static_assert( identic( subview< 12, 12 >( str  ),	safe_subview( str,  12, 12 ) ) );
			// static_assert( identic( subview< 12, 22 >( str  ),	safe_subview( str,  12, 22 ) ) );
			static_assert( identic( subview<-12,  0 >( str  ),	safe_subview( str, -12,  0 ) ) );
			// static_assert( identic( subview<-12,  3 >( str  ),	safe_subview( str, -12,  3 ) ) );
			// static_assert( identic( subview<-12, 12 >( str  ),	safe_subview( str, -12, 12 ) ) );
			// static_assert( identic( subview<-12, 22 >( str  ),	safe_subview( str, -12, 22 ) ) );
			static_assert( identic( subview< 22,  0 >( str  ),	safe_subview( str,  22,  0 ) ) );
			// static_assert( identic( subview< 22,  3 >( str  ),	safe_subview( str,  22,  3 ) ) );
			// static_assert( identic( subview< 22, 12 >( str  ),	safe_subview( str,  22, 12 ) ) );
			// static_assert( identic( subview< 22, 22 >( str  ),	safe_subview( str,  22, 22 ) ) );

			static_assert( identic( subview<  0,  0 >( strN ),	safe_subview( strN,  0,  0 ) ) );
			static_assert( identic( subview<  0,  3 >( strN ),	safe_subview( strN,  0,  3 ) ) );
			static_assert( identic( subview<  0, 12 >( strN ),	safe_subview( strN,  0, 12 ) ) );
			static_assert( identic( subview<  0, 22 >( strN ),	safe_subview( strN,  0, 22 ) ) );
			static_assert( identic( subview<  5,  0 >( strN ),	safe_subview( strN,  5,  0 ) ) );
			static_assert( identic( subview<  5,  3 >( strN ),	safe_subview( strN,  5,  3 ) ) );
			static_assert( identic( subview<  5, 12 >( strN ),	safe_subview( strN,  5, 12 ) ) );
			static_assert( identic( subview<  5, 22 >( strN ),	safe_subview( strN,  5, 22 ) ) );
			static_assert( identic( subview< -5,  0 >( strN ),	safe_subview( strN, -5,  0 ) ) );
			static_assert( identic( subview< -5,  3 >( strN ),	safe_subview( strN, -5,  3 ) ) );
			static_assert( identic( subview< -5, 12 >( strN ),	safe_subview( strN, -5, 12 ) ) );
			static_assert( identic( subview< -5, 22 >( strN ),	safe_subview( strN, -5, 22 ) ) );
			static_assert( identic( subview< 12,  0 >( strN ),	safe_subview( strN, 12,  0 ) ) );
			static_assert( identic( subview< 12,  3 >( strN ),	safe_subview( strN, 12,  3 ) ) );
			static_assert( identic( subview< 12, 12 >( strN ),	safe_subview( strN, 12, 12 ) ) );
			static_assert( identic( subview< 12, 22 >( strN ),	safe_subview( strN, 12, 22 ) ) );
			static_assert( identic( subview<-12,  0 >( strN ),	safe_subview( strN,-12,  0 ) ) );
			static_assert( identic( subview<-12,  3 >( strN ),	safe_subview( strN,-12,  3 ) ) );
			static_assert( identic( subview<-12, 12 >( strN ),	safe_subview( strN,-12, 12 ) ) );
			static_assert( identic( subview<-12, 22 >( strN ),	safe_subview( strN,-12, 22 ) ) );
			static_assert( identic( subview< 22,  0 >( strN ),	safe_subview( strN, 22,  0 ) ) );
			static_assert( identic( subview< 22,  3 >( strN ),	safe_subview( strN, 22,  3 ) ) );
			static_assert( identic( subview< 22, 12 >( strN ),	safe_subview( strN, 22, 12 ) ) );
			static_assert( identic( subview< 22, 22 >( strN ),	safe_subview( strN, 22, 22 ) ) );
			static_assert( identic( subview<-22,  0 >( strN ),	safe_subview( strN,-22,  0 ) ) );
			static_assert( identic( subview<-22,  3 >( strN ),	safe_subview( strN,-22,  3 ) ) );
			static_assert( identic( subview<-22, 12 >( strN ),	safe_subview( strN,-22, 12 ) ) );
			static_assert( identic( subview<-22, 22 >( strN ),	safe_subview( strN,-22, 22 ) ) );
		}
		{	// subview (int)
			static_assert( identic( subview( ints,   0,  0 ),	safe_subview( ints,   0,  0 ) ) );
			static_assert( identic( subview( ints,   0,  3 ),	safe_subview( ints,   0,  3 ) ) );
			static_assert( identic( subview( ints,   0, 12 ),	safe_subview( ints,   0, 12 ) ) );
			static_assert( identic( subview( ints,   0, 22 ),	safe_subview( ints,   0, 22 ) ) );
			static_assert( identic( subview( ints,   5,  0 ),	safe_subview( ints,   5,  0 ) ) );
			static_assert( identic( subview( ints,   5,  3 ),	safe_subview( ints,   5,  3 ) ) );
			static_assert( identic( subview( ints,   5, 12 ),	safe_subview( ints,   5, 12 ) ) );
			static_assert( identic( subview( ints,   5, 22 ),	safe_subview( ints,   5, 22 ) ) );
			static_assert( identic( subview( ints,  -5,  0 ),	safe_subview( ints,  -5,  0 ) ) );
			static_assert( identic( subview( ints,  -5,  3 ),	safe_subview( ints,  -5,  3 ) ) );
			static_assert( identic( subview( ints,  -5, 12 ),	safe_subview( ints,  -5, 12 ) ) );
			static_assert( identic( subview( ints,  -5, 22 ),	safe_subview( ints,  -5, 22 ) ) );
			static_assert( identic( subview( ints,  12,  0 ),	safe_subview( ints,  12,  0 ) ) );
			static_assert( identic( subview( ints,  12,  3 ),	safe_subview( ints,  12,  3 ) ) );
			static_assert( identic( subview( ints,  12, 12 ),	safe_subview( ints,  12, 12 ) ) );
			static_assert( identic( subview( ints,  12, 22 ),	safe_subview( ints,  12, 22 ) ) );
			static_assert( identic( subview( ints, -12,  0 ),	safe_subview( ints, -12,  0 ) ) );
			static_assert( identic( subview( ints, -12,  3 ),	safe_subview( ints, -12,  3 ) ) );
			static_assert( identic( subview( ints, -12, 12 ),	safe_subview( ints, -12, 12 ) ) );
			static_assert( identic( subview( ints, -12, 22 ),	safe_subview( ints, -12, 22 ) ) );
			static_assert( identic( subview( ints,  22,  0 ),	safe_subview( ints,  22,  0 ) ) );
			static_assert( identic( subview( ints,  22,  3 ),	safe_subview( ints,  22,  3 ) ) );
			static_assert( identic( subview( ints,  22, 12 ),	safe_subview( ints,  22, 12 ) ) );
			static_assert( identic( subview( ints,  22, 22 ),	safe_subview( ints,  22, 22 ) ) );
			static_assert( identic( subview( ints, -22,  0 ),	safe_subview( ints, -22,  0 ) ) );
			static_assert( identic( subview( ints, -22,  3 ),	safe_subview( ints, -22,  3 ) ) );
			static_assert( identic( subview( ints, -22, 12 ),	safe_subview( ints, -22, 12 ) ) );
			static_assert( identic( subview( ints, -22, 22 ),	safe_subview( ints, -22, 22 ) ) );

			static_assert( identic( subview( intsN,  0,  0 ),	safe_subview( intsN,  0,  0 ) ) );
			static_assert( identic( subview( intsN,  0,  3 ),	safe_subview( intsN,  0,  3 ) ) );
			static_assert( identic( subview( intsN,  0, 12 ),	safe_subview( intsN,  0, 12 ) ) );
			static_assert( identic( subview( intsN,  0, 22 ),	safe_subview( intsN,  0, 22 ) ) );
			static_assert( identic( subview( intsN,  5,  0 ),	safe_subview( intsN,  5,  0 ) ) );
			static_assert( identic( subview( intsN,  5,  3 ),	safe_subview( intsN,  5,  3 ) ) );
			static_assert( identic( subview( intsN,  5, 12 ),	safe_subview( intsN,  5, 12 ) ) );
			static_assert( identic( subview( intsN,  5, 22 ),	safe_subview( intsN,  5, 22 ) ) );
			static_assert( identic( subview( intsN, -5,  0 ),	safe_subview( intsN, -5,  0 ) ) );
			static_assert( identic( subview( intsN, -5,  3 ),	safe_subview( intsN, -5,  3 ) ) );
			static_assert( identic( subview( intsN, -5, 12 ),	safe_subview( intsN, -5, 12 ) ) );
			static_assert( identic( subview( intsN, -5, 22 ),	safe_subview( intsN, -5, 22 ) ) );
			static_assert( identic( subview( intsN, 12,  0 ),	safe_subview( intsN, 12,  0 ) ) );
			static_assert( identic( subview( intsN, 12,  3 ),	safe_subview( intsN, 12,  3 ) ) );
			static_assert( identic( subview( intsN, 12, 12 ),	safe_subview( intsN, 12, 12 ) ) );
			static_assert( identic( subview( intsN, 12, 22 ),	safe_subview( intsN, 12, 22 ) ) );
			static_assert( identic( subview( intsN,-12,  0 ),	safe_subview( intsN,-12,  0 ) ) );
			static_assert( identic( subview( intsN,-12,  3 ),	safe_subview( intsN,-12,  3 ) ) );
			static_assert( identic( subview( intsN,-12, 12 ),	safe_subview( intsN,-12, 12 ) ) );
			static_assert( identic( subview( intsN,-12, 22 ),	safe_subview( intsN,-12, 22 ) ) );
			static_assert( identic( subview( intsN, 22,  0 ),	safe_subview( intsN, 22,  0 ) ) );
			static_assert( identic( subview( intsN, 22,  3 ),	safe_subview( intsN, 22,  3 ) ) );
			static_assert( identic( subview( intsN, 22, 12 ),	safe_subview( intsN, 22, 12 ) ) );
			static_assert( identic( subview( intsN, 22, 22 ),	safe_subview( intsN, 22, 22 ) ) );
			static_assert( identic( subview( intsN,-22,  0 ),	safe_subview( intsN,-22,  0 ) ) );
			static_assert( identic( subview( intsN,-22,  3 ),	safe_subview( intsN,-22,  3 ) ) );
			static_assert( identic( subview( intsN,-22, 12 ),	safe_subview( intsN,-22, 12 ) ) );
			static_assert( identic( subview( intsN,-22, 22 ),	safe_subview( intsN,-22, 22 ) ) );

			static_assert( identic( subview<  0 >( intsN,  0 ),	safe_subview( intsN,  0,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN,  5 ),	safe_subview( intsN,  5,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN, -5 ),	safe_subview( intsN, -5,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN, 12 ),	safe_subview( intsN, 12,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN,-12 ),	safe_subview( intsN,-12,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN, 22 ),	safe_subview( intsN, 22,  0 ) ) );
			static_assert( identic( subview<  0 >( intsN,-22 ),	safe_subview( intsN,-22,  0 ) ) );
			static_assert( identic( subview<  3 >( intsN,  0 ),	safe_subview( intsN,  0,  3 ) ) );
			static_assert( identic( subview<  3 >( intsN,  5 ),	safe_subview( intsN,  5,  3 ) ) );
			static_assert( identic( subview<  3 >( intsN, -5 ),	safe_subview( intsN, -5,  3 ) ) );
			static_assert( identic( subview< 12 >( intsN,  0 ),	safe_subview( intsN,  0, 12 ) ) );

			static_assert( identic( subview<  0,  0 >( intsN ),	safe_subview( intsN,  0,  0 ) ) );
			static_assert( identic( subview<  0,  3 >( intsN ),	safe_subview( intsN,  0,  3 ) ) );
			static_assert( identic( subview<  0, 12 >( intsN ),	safe_subview( intsN,  0, 12 ) ) );
			static_assert( identic( subview<  0, 22 >( intsN ),	safe_subview( intsN,  0, 22 ) ) );
			static_assert( identic( subview<  5,  0 >( intsN ),	safe_subview( intsN,  5,  0 ) ) );
			static_assert( identic( subview<  5,  3 >( intsN ),	safe_subview( intsN,  5,  3 ) ) );
			static_assert( identic( subview<  5, 12 >( intsN ),	safe_subview( intsN,  5, 12 ) ) );
			static_assert( identic( subview<  5, 22 >( intsN ),	safe_subview( intsN,  5, 22 ) ) );
			static_assert( identic( subview< -5,  0 >( intsN ),	safe_subview( intsN, -5,  0 ) ) );
			static_assert( identic( subview< -5,  3 >( intsN ),	safe_subview( intsN, -5,  3 ) ) );
			static_assert( identic( subview< -5, 12 >( intsN ),	safe_subview( intsN, -5, 12 ) ) );
			static_assert( identic( subview< -5, 22 >( intsN ),	safe_subview( intsN, -5, 22 ) ) );
			static_assert( identic( subview< 12,  0 >( intsN ),	safe_subview( intsN, 12,  0 ) ) );
			static_assert( identic( subview< 12,  3 >( intsN ),	safe_subview( intsN, 12,  3 ) ) );
			static_assert( identic( subview< 12, 12 >( intsN ),	safe_subview( intsN, 12, 12 ) ) );
			static_assert( identic( subview< 12, 22 >( intsN ),	safe_subview( intsN, 12, 22 ) ) );
			static_assert( identic( subview<-12,  0 >( intsN ),	safe_subview( intsN,-12,  0 ) ) );
			static_assert( identic( subview<-12,  3 >( intsN ),	safe_subview( intsN,-12,  3 ) ) );
			static_assert( identic( subview<-12, 12 >( intsN ),	safe_subview( intsN,-12, 12 ) ) );
			static_assert( identic( subview<-12, 22 >( intsN ),	safe_subview( intsN,-12, 22 ) ) );
			static_assert( identic( subview< 22,  0 >( intsN ),	safe_subview( intsN, 22,  0 ) ) );
			static_assert( identic( subview< 22,  3 >( intsN ),	safe_subview( intsN, 22,  3 ) ) );
			static_assert( identic( subview< 22, 12 >( intsN ),	safe_subview( intsN, 22, 12 ) ) );
			static_assert( identic( subview< 22, 22 >( intsN ),	safe_subview( intsN, 22, 22 ) ) );
			static_assert( identic( subview<-22,  0 >( intsN ),	safe_subview( intsN,-22,  0 ) ) );
			static_assert( identic( subview<-22,  3 >( intsN ),	safe_subview( intsN,-22,  3 ) ) );
			static_assert( identic( subview<-22, 12 >( intsN ),	safe_subview( intsN,-22, 12 ) ) );
			static_assert( identic( subview<-22, 22 >( intsN ),	safe_subview( intsN,-22, 22 ) ) );
		}
	}
	DOCTEST_TEST_CASE( "searching-related" ) {
		constexpr auto						null_ = "";
		constexpr auto						abd = "abddefffijkl";
		constexpr auto f = []( auto c ){ return c == 'f'; };
		constexpr auto x = []( auto c ){ return c == 'x'; };

		{	// find
			DOCTEST_FAST_CHECK_EQ( find( null_, 0 ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( null_, f ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( abd,  f  ),					5 );
			DOCTEST_FAST_CHECK_EQ( find( abd, 'f' ),					5 );
			DOCTEST_FAST_CHECK_EQ( find( abd,  x  ),					12 );
			DOCTEST_FAST_CHECK_EQ( find( abd, 'x' ),					12 );

			DOCTEST_FAST_CHECK_EQ( find( null_, str ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( str, null_ ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( null_, null_ ),				0 );

			DOCTEST_FAST_CHECK_EQ( find( str, "cde" ), 					2 );
			DOCTEST_FAST_CHECK_EQ( find( str, "cdf" ), 					view( str ).size() );
			DOCTEST_FAST_CHECK_EQ( find( "abcdefghijkl", "cde" ), 		2 );
			DOCTEST_FAST_CHECK_EQ( find( str, first( str, 5 ) ), 		0 );
			DOCTEST_FAST_CHECK_EQ( find( str, last ( str, 7 ) ), 		5 );

			DOCTEST_FAST_CHECK_EQ( find( null_, Newline{} ),			0 );
			DOCTEST_FAST_CHECK_EQ( find( "abcd\r\nefgh", Newline{} ), 	4 );
		}
		{	// rfind
			DOCTEST_FAST_CHECK_EQ( rfind( null_, f ),					0 );
			DOCTEST_FAST_CHECK_EQ( rfind( abd,  f  ),					7 );
			DOCTEST_FAST_CHECK_EQ( rfind( abd, 'a' ),					0 );
			DOCTEST_FAST_CHECK_EQ( rfind( null_, 'f' ),					0 );
			DOCTEST_FAST_CHECK_EQ( rfind( abd, 'f' ),					7 );
			DOCTEST_FAST_CHECK_EQ( rfind( abd,  x  ),					12 );
			DOCTEST_FAST_CHECK_EQ( rfind( abd, 'x' ),					12 );
			// DOCTEST_FAST_CHECK_EQ( rfind( null_, str ),					0 );
			// DOCTEST_FAST_CHECK_EQ( rfind( str, first( str, 5 ) ), 		0 );
			// DOCTEST_FAST_CHECK_EQ( rfind( str, last ( str, 7 ) ), 		5 );
			// DOCTEST_FAST_CHECK_EQ( rfind( null_, Newline{} ),			0 );
			// DOCTEST_FAST_CHECK_EQ( rfind( "abcd\r\nefgh", Newline{} ),	4 );
		}
		{	// contains
			DOCTEST_FAST_CHECK_EQ( contains( str, 'd' ), 						true );
			DOCTEST_FAST_CHECK_EQ( contains( str, '8' ), 						false );
			DOCTEST_FAST_CHECK_EQ( contains( str, first( str, 5 ) ), 			true );
			DOCTEST_FAST_CHECK_EQ( contains( str, last ( str, 5 ) ), 			true );
			DOCTEST_FAST_CHECK_EQ( contains( "abcdefghijkl", first( str, 5 ) ), true );
			DOCTEST_FAST_CHECK_EQ( contains( "abcdefghijkl", "cdef" ), 			true );
			DOCTEST_FAST_CHECK_EQ( contains( "abcd\r\nefgh", Newline{} ), 		true );
		}
		{	// until
			using std::data, std::size;
			constexpr auto base = subview( abc, 3, 4 );
			DOCTEST_ASCII_CHECK_EQ( base, "defg" );

			DOCTEST_ASCII_CHECK_EQ( until( base, 'a' ), base );				// None
			DOCTEST_ASCII_CHECK_EQ( until( base, 'd' ), first( base, 0 ) );	// First
			DOCTEST_ASCII_CHECK_EQ( until( base, 'f' ), first( base, 2 ) );	// Middle
			DOCTEST_ASCII_CHECK_EQ( until( base, 'g' ), first( base, 3 ) );	// Last

			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh", 'g' ),  "abcdef" );	// Last
			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh", "fg" ), "abcde" );	// Last

			// Newline 
			DOCTEST_FAST_CHECK_EQ ( until( "abcd\r\nefgh"	, Newline{} ).size(), 	4 );
			DOCTEST_ASCII_CHECK_EQ( until( ""				, Newline{} ),	"" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh"		, Newline{} ),	"abcdefgh" );

			DOCTEST_ASCII_CHECK_EQ( until( "\nabcdefgh"		, Newline{} ),	"" );
			DOCTEST_ASCII_CHECK_EQ( until( "\n\r\nabcdefgh"	, Newline{} ),	"" );
			DOCTEST_ASCII_CHECK_EQ( until( "\rabcdefgh"		, Newline{} ),	"" );
			DOCTEST_ASCII_CHECK_EQ( until( "\r\nabcdefgh"	, Newline{} ),	"" );

			DOCTEST_ASCII_CHECK_EQ( until( "abcd\nefgh"		, Newline{} ),	"abcd" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcd\n\refgh"	, Newline{} ),	"abcd" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcd\refgh"		, Newline{} ),	"abcd" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcd\r\nefgh"	, Newline{} ),	"abcd" );

			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh\n"		, Newline{} ),	"abcdefgh" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh\n\r"	, Newline{} ),	"abcdefgh" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh\r"		, Newline{} ),	"abcdefgh" );
			DOCTEST_ASCII_CHECK_EQ( until( "abcdefgh\r\n"	, Newline{} ),	"abcdefgh" );
		}
		{	// equal
			DOCTEST_FAST_CHECK_UNARY(  equal( "", "" ) );
			DOCTEST_FAST_CHECK_UNARY(  equal( str, str ) );
			DOCTEST_FAST_CHECK_UNARY(  equal( "abcdefghijkl", "abcdefghijkl" ) );
			DOCTEST_FAST_CHECK_UNARY(  equal( str, "abcdefghijkl" ) );
			DOCTEST_FAST_CHECK_UNARY(  equal( "abcdefghijkl", str ) );
			DOCTEST_FAST_CHECK_UNARY( !equal( "abcdef", "abc" ) );
			DOCTEST_FAST_CHECK_UNARY( !equal( "abc", "abcdef" ) );
		}
		{	// starts_with
			DOCTEST_FAST_CHECK_EQ( starts_with( "abcdef", "abc" ),				3 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "abcdef", "abd" ),				0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( str, ""    ),					0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( null_, "123" ),					0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( str, "abc" ),					3 );
			DOCTEST_FAST_CHECK_EQ( starts_with( str, "def" ),					0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( null_, ' ' ),					0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( str, 'a' ),						1 );
			DOCTEST_FAST_CHECK_EQ( starts_with( str, 'b' ),						0 );

			DOCTEST_FAST_CHECK_EQ( starts_with( ""            ,	Newline{} ),	0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "abcd\r\nefgh",	Newline{} ),	0 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "\nabcdefgh"  ,	Newline{} ),	1 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "\n\rabcdefgh",	Newline{} ),	2 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "\rabcdefgh"  ,	Newline{} ),	1 );
			DOCTEST_FAST_CHECK_EQ( starts_with( "\r\nabcdefgh",	Newline{} ),	2 );
		}
		{	// ends_with
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdef", "def" ),				3 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdef", "deg" ),				0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( str, ""    ),						0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( null_, "jkl" ),					0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdefghijkl", last( str, 3 ) ),	3 );
			DOCTEST_FAST_CHECK_EQ( ends_with( str, "jkl" ),						3 );
			DOCTEST_FAST_CHECK_EQ( ends_with( str, "ijk" ),						0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( null_, ' ' ),						0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( str, 'l' ),						1 );
			DOCTEST_FAST_CHECK_EQ( ends_with( str, 'k' ),						0 );

			DOCTEST_FAST_CHECK_EQ( ends_with( ""            ,	Newline{} ),	0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcd\r\nefgh",	Newline{} ),	0 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdefgh\n"  ,	Newline{} ),	1 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdefgh\n\r",	Newline{} ),	2 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdefgh\r"  ,	Newline{} ),	1 );
			DOCTEST_FAST_CHECK_EQ( ends_with( "abcdefgh\r\n",	Newline{} ),	2 );
		}
		{	// split_at, spolit_by
			{	// at index
				auto div = split_at( "abcdefghijkl", 24 );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "abcdefghijkl" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "" ) );

				div = split_at( "abcdefghijkl", 12 );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "abcdefghijkl" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "" ) );

				div = split_at( "abcdefghijkl", 4 );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "abcd" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "fghijkl" ) );

				div = split_at( abc, 0 );
				DOCTEST_ASCII_CHECK_EQ( div.first , first( abc, 0 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, not_first( abc, 1 ) );

				div = split_at( abc, 2 );
				DOCTEST_ASCII_CHECK_EQ( div.first , first( abc, 2 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, not_first( abc, 3 ) );

				div = split_at( abc, abc.size() - 1 );
				DOCTEST_ASCII_CHECK_EQ( div.first , first( abc, abc.size() - 1 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( abc, 0 ) );

				div = split_at( abc, abc.size() );
				DOCTEST_ASCII_CHECK_EQ( div.first , abc );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( abc, 0 ) );

				div = split_at( abc, abc.size() + 1 );
				DOCTEST_ASCII_CHECK_EQ( div.first , abc );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( abc, 0 ) );
			}
			{	// by value
				constexpr auto base = subview( abc, 3, 4 );
				DOCTEST_FAST_CHECK_EQ( base, "defg" );
				
				auto div = split_by( "abcdefghijkl", 'e' );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "abcd" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "fghijkl" ) );

				div = split_by( base, 'a' );					// None
				DOCTEST_ASCII_CHECK_EQ( div.first , base );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( base, 0 ) );

				div = split_by( base, 'd' );					// First
				DOCTEST_ASCII_CHECK_EQ( div.first , first( base, 0 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, not_first( base, 1 ) );

				div = split_by( base, 'f' );					// Middle
				DOCTEST_ASCII_CHECK_EQ( div.first , first( base, 2 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, not_first( base, 3 ) );

				div = split_by( base, 'g' );					// Last
				DOCTEST_ASCII_CHECK_EQ( div.first , first( base, 3 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, not_first( base, 4 ) );
			}
			{	// by string_view
				constexpr auto base = subview( abc, 3, 4 );
				DOCTEST_FAST_CHECK_EQ( base, "defg" );
				
				auto div = split_by( "defg", "ef" );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "d" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "g" ) );

				div = split_by( str, first( str, 2 ) );			// First
				DOCTEST_FAST_CHECK_EQ ( find( str, first( str, 2 ) ), 0 );
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str,  0 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str, 10 ) );
				DOCTEST_FAST_CHECK_EQ ( div.first.data() , view( str ).data() );

				div = split_by( str, subview( str, 4, 2 ) );	// Middle
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str,  4 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str,  6 ) );

				div = split_by( str, last( str, 2 ) );			// End
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str, 10 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str,  0 ) );
				DOCTEST_FAST_CHECK_EQ ( div.second.data(), view( str ).data() + view( str ).size() );

				div = split_by( str, "aaa" );					// None
				DOCTEST_ASCII_CHECK_EQ( div.first , str );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( str, 0 ) );
			}
			{	// by Newline
				// None
				auto div = split_by( std::string_view( "bcdefgh" ), Newline{} );
				DOCTEST_FAST_CHECK_EQ( div.first , view( "bcdefgh" ) );
				DOCTEST_FAST_CHECK_EQ( div.second, view( "" ) );

				div = split_by( std::string_view( "" ), Newline{} );
				DOCTEST_FAST_CHECK_EQ( div.first , "" );
				DOCTEST_FAST_CHECK_EQ( div.second, "" );

				{	// Beginning
					div = split_by( "\nbcdefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , view( "" ) );
					DOCTEST_FAST_CHECK_EQ( div.second, view( "bcdefgh" ) );

					div = split_by( "\n\rbcdefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "" );
					DOCTEST_FAST_CHECK_EQ( div.second, "bcdefgh" );

					div = split_by( "\rbcdefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "" );
					DOCTEST_FAST_CHECK_EQ( div.second, "bcdefgh" );

					div = split_by( "\r\nbcdefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "" );
					DOCTEST_FAST_CHECK_EQ( div.second, "bcdefgh" );
				}
				{	// Middle
					div = split_by( "bcd\nefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , view( "bcd" ) );
					DOCTEST_FAST_CHECK_EQ( div.second, view( "efgh" ) );

					div = split_by( "bcd\n\refgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcd" );
					DOCTEST_FAST_CHECK_EQ( div.second, "efgh" );

					div = split_by( "bcd\refgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcd" );
					DOCTEST_FAST_CHECK_EQ( div.second, "efgh" );

					div = split_by( "bcd\r\nefgh", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcd" );
					DOCTEST_FAST_CHECK_EQ( div.second, "efgh" );
				}
				{	// Ending
					div = split_by( "bcdefgh\n", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , view( "bcdefgh" ) );
					DOCTEST_FAST_CHECK_EQ( div.second, view( "" ) );

					div = split_by( "bcdefgh\n\r", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcdefgh" );
					DOCTEST_FAST_CHECK_EQ( div.second, "" );

					div = split_by( "bcdefgh\r", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcdefgh" );
					DOCTEST_FAST_CHECK_EQ( div.second, "" );

					div = split_by( "bcdefgh\r\n", Newline{} );
					DOCTEST_FAST_CHECK_EQ( div.first , "bcdefgh" );
					DOCTEST_FAST_CHECK_EQ( div.second, "" );
				}
			}
		}
		{	// trim
			constexpr auto					text = "++++abcdef++";
			constexpr auto					pred = []( char c ) { return c <= 'b'; };
			constexpr auto 					is_plus = []( auto x_ ){	return x_ == '+'; };

			{	// trim_front
				DOCTEST_ASCII_CHECK_EQ( trim_front( "++++abcdef++", '+' ), view( "+++abcdef++" ) );
				DOCTEST_ASCII_CHECK_EQ( trim_front( text, '+' ),		not_first( text, 1 ) );
				DOCTEST_ASCII_CHECK_EQ( trim_front( text, '-' ),		text );
			}
			{	// trim_back
				DOCTEST_ASCII_CHECK_EQ( trim_back( "++++abcdef++", '+' ), view( "++++abcdef+" ) );
				DOCTEST_ASCII_CHECK_EQ( trim_back( text, '+' ),			not_last( text, 1 ) );
				DOCTEST_ASCII_CHECK_EQ( trim_back( text, '-' ),			text );
			}
			{	// trim_first
				{	// Character
					DOCTEST_ASCII_CHECK_EQ( trim_first( "abc", '+' ),		"abc" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( text, '+' ),		not_first( text, 4 ) );
				}
				{	// Predicate
					DOCTEST_ASCII_CHECK_EQ( trim_first( text, is_plus ),	not_first( text, 4 ) );
					DOCTEST_ASCII_CHECK_EQ( trim_first( text, pred ),		not_first( text, 6 ) );
				}
				{	// Newline
					DOCTEST_ASCII_CHECK_EQ( trim_first( "",					Newline{} ),	"" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "\n\r",				Newline{} ),	"" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "abcdefgh\n\r",		Newline{} ),	"abcdefgh\n\r" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "\nabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "\n\rabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "\rabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
					DOCTEST_ASCII_CHECK_EQ( trim_first( "\r\nabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
				}
			}
			{	// trim_last
				{	// Character
					DOCTEST_ASCII_CHECK_EQ( trim_last( "abc", '+' ),		"abc" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( text, '+' ),			not_last( text, 2 ) );
				}
				{	// Predicate
					DOCTEST_ASCII_CHECK_EQ( trim_last( text, is_plus ),		not_last( text, 2 ) );
					DOCTEST_ASCII_CHECK_EQ( trim_last( text, pred ),		not_last( text, 2 ) );
				}
				{	// Newline
					DOCTEST_ASCII_CHECK_EQ( trim_last( "",					Newline{} ),	"" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\r",				Newline{} ),	"" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh",		Newline{} ),	"\n\rabcdefgh" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\n",	Newline{} ),	"\n\rabcdefgh" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\n\r",	Newline{} ),	"\n\rabcdefgh" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\r",	Newline{} ),	"\n\rabcdefgh" );
					DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\r\n",	Newline{} ),	"\n\rabcdefgh" );
				}
			}
			{	// trim
				DOCTEST_ASCII_CHECK_EQ( trim( text, is_plus ),	subview( text, 4, 6 ) );
				DOCTEST_ASCII_CHECK_EQ( trim( text, pred ),		subview( text, 6, 4 ) );
			}
		}
	}
	DOCTEST_TEST_CASE( "other..." ) {
		constexpr auto						abd = "abddefffijkl";

		{	// sort
			
		}
		{	// all_of
			DOCTEST_FAST_CHECK_UNARY(  all_of ( "abcdefghijkl", []( auto c ){ return c != '\0'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  all_of ( str, []( auto c ){ return c >= 'a'; } ) );
			DOCTEST_FAST_CHECK_UNARY( !all_of ( str, []( auto c ){ return c >= 'd'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::all_of ( str, "abcdefghijkl", []( auto c, auto d ){ return c == d; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::all_of ( str, str, []( auto c, auto d ){ return c == d; } ) );
			DOCTEST_FAST_CHECK_UNARY( !all_of ( str, abd, []( auto c, auto d ){ return c == d; } ) );
		}
		{	// any_of
			DOCTEST_FAST_CHECK_UNARY( !any_of ( "abcdefghijkl", []( auto c ){ return c == '\0'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  any_of ( str, []( auto c ){ return c >= 'a'; } ) );
			DOCTEST_FAST_CHECK_UNARY( !any_of ( str, []( auto c ){ return c == 'x'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::any_of ( str, "abcdefghijkl", []( auto c, auto d ){ return c == d; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::any_of ( str, str, []( auto c, auto d ){ return c == d; } ) );
			DOCTEST_FAST_CHECK_UNARY(  any_of ( str, abd, []( auto c, auto d ){ return c != d; } ) );
			DOCTEST_FAST_CHECK_UNARY( !pax::any_of ( str, str, []( auto c, auto d ){ return c != d; } ) );
		}
		{	// none_of
			DOCTEST_FAST_CHECK_UNARY(  none_of( "abcdefghijkl", []( auto c ){ return c == '\0'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  none_of( str, []( auto c ){ return c == 'x'; } ) );
			DOCTEST_FAST_CHECK_UNARY( !none_of( str, []( auto c ){ return c == 'd'; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::none_of ( "abcdefghijkl", str, []( auto c, auto d ){ return c != d; } ) );
			DOCTEST_FAST_CHECK_UNARY(  pax::none_of ( str, str, []( auto c, auto d ){ return c != d; } ) );
			DOCTEST_FAST_CHECK_UNARY( !none_of ( str, abd, []( auto c, auto d ){ return c != d; } ) );
		}
		{	// identify_newline
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcd\n\refgh"   ),	"\n\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( ""               ),	"\n" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcdefgh"       ),	"\n" );

			DOCTEST_ASCII_CHECK_EQ( identify_newline( "\nabcdefgh"     ),	"\n" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "\n\r\nabcdefgh" ),	"\n\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "\rabcdefgh"     ),	"\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "\r\nabcdefgh"   ),	"\r\n" );

			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcd\nefgh"     ),	"\n" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcd\n\refgh"   ),	"\n\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcd\refgh"     ),	"\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcd\r\nefgh"   ),	"\r\n" );

			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcdefgh\n"     ),	"\n" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcdefgh\n\r"   ),	"\n\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcdefgh\r"     ),	"\r" );
			DOCTEST_ASCII_CHECK_EQ( identify_newline( "abcdefgh\r\n"   ),	"\r\n" );
		}
		{	// luhn_sum
			DOCTEST_FAST_CHECK_EQ( luhn_sum( "6112161457" ),	30 );
			DOCTEST_FAST_CHECK_EQ( luhn_sum( "6212161457" ),	31 );
			DOCTEST_FAST_CHECK_EQ( luhn_sum( "7112161457" ),	32 );
		}
	}
	DOCTEST_TEST_CASE( "std::span in general" ) {
		{	// comparisons
			const auto					abc2{ "abcdffghijkl" };

			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::less,    not_last( abc ) <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::less,    abc  <=> abc2 );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::equal,   abc  <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::greater, abc2 <=> abc );
			DOCTEST_FAST_CHECK_EQ( std::strong_ordering::greater, abc  <=> not_last( abc ) );

			DOCTEST_FAST_CHECK_LT( abc, abc2 );
			DOCTEST_FAST_CHECK_LE( abc, abc2 );
			DOCTEST_FAST_CHECK_GT( abc2, abc );
			DOCTEST_FAST_CHECK_GE( abc2, abc );
			DOCTEST_FAST_CHECK_NE( abc2, abc );

            DOCTEST_FAST_CHECK_LT( first( str,  9 ), str );
            DOCTEST_FAST_CHECK_LE( first( str, 10 ), str );
            DOCTEST_FAST_CHECK_LE( first( str,  9 ), str );
            DOCTEST_FAST_CHECK_EQ( first( str, 12 ), str );

			DOCTEST_FAST_CHECK_NE( str, first( str,  9 ) );
			DOCTEST_FAST_CHECK_GT( str, first( str,  9 ) );
			DOCTEST_FAST_CHECK_GE( str, first( str, 10 ) );
			DOCTEST_FAST_CHECK_GE( str, first( str,  8 ) );
		}
		{	// operator<<, view
			{	// with int
				{
					std::ostringstream		os;
					os << std::span< int >{};
					DOCTEST_FAST_CHECK_EQ( os.str(), "[]" );
					DOCTEST_FAST_CHECK_EQ( os.str().size(), 2 );
				} {
					const int				hej[ 3 ] = { 0, 6, 7 };
					std::ostringstream		os;
					os << std::span( hej );
					DOCTEST_FAST_CHECK_EQ( os.str(), "[0, 6, 7]" );
					DOCTEST_FAST_CHECK_EQ( os.str().size(), 9 );
				}
			}
			{	// with text
				std::ostringstream		os;
				os << std::span( "abcd" ).first( 3 );
				DOCTEST_FAST_CHECK_EQ( os.str().size(), 3 );
				DOCTEST_FAST_CHECK_EQ( os.str(), "abc" );
			}
			{	// with texts
				std::ostringstream		os;
				const char *			strings[ 5 ] = { "Hej", " ", "hela", " ", "varlden" };
				os << std::span( strings );
				DOCTEST_FAST_CHECK_EQ( os.str().size(), 36 );
				DOCTEST_FAST_CHECK_EQ( os.str(), "[\"Hej\", \" \", \"hela\", \" \", \"varlden\"]" );
			}
			{	// with text technical
#if !defined( PAX_ASCII_TEST_UNUSABLE )
				std::ostringstream		os;
				constexpr auto			v0 = view( ">\0\a\b\t\n\v\f\r\"'\x18\x7f <" );
				constexpr auto			vr = view( "\">\\0\\a\\b\\t\\n\\v\\f\\r\\\"'<CAN><DEL> <\"" );

				os << as_ascii( v0 );
				const auto 				res0{ os.str() };
				const std::span 		res{ res0.data(), res0.size() };

				DOCTEST_FAST_CHECK_EQ( v0.size(),   15 );
				DOCTEST_ASCII_CHECK_EQ( res0, vr );
				DOCTEST_FAST_CHECK_EQ( res0.size(), vr.size() );
				DOCTEST_FAST_CHECK_EQ( res.size(),  vr.size() );
				DOCTEST_FAST_CHECK_EQ( res, vr );
#endif
			}
		}
	}
	DOCTEST_TEST_CASE( "String_view_splitter" ) { 
		// for( const auto s : String_view_splitter( "ett två tre", ' ' ) )	Debug{} << s;

		constexpr String_view_splitter	split( "ett två tre", ' ' );
		auto							itr = split.begin();
		DOCTEST_FAST_CHECK_EQ( *    itr,   "ett" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ), "två" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ), "tre" );
		DOCTEST_FAST_CHECK_NE(   itr, split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr, split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr, split.end() );
	}

}	// namespace pax
