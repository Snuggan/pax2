//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/format.hpp>
#include <pax/std/algorithm.hpp>
#include <pax/external/doctest/doctest-pax.hpp>

#include <pax/reporting/as_ascii.hpp>
#include <pax/reporting/debug.hpp>

#include <sstream>
#include <array>
#include <vector>


namespace pax {
	
	constexpr const string_view2				str		= view( "abcdefghijkl" );
	const auto 									strN	= span2( "abcdefghijkl" );
	constexpr const string_view2				e		= view( "" );

	constexpr const std::size_t 				N = 12;
	constexpr const int							ints0[ N ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	constexpr const std::span< const int >		ints	= dview( ints0 );
	constexpr const std::span< const int, N >	intsN	=  view( ints0 );
	constexpr const std::span					abc( str );
	
	template< typename T >
	constexpr bool is_value_const = std::is_const_v< std::remove_reference_t< Value_type_t< T > > >;
	
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
		// string_view2
			using View = string_view2;
			static_assert(!std::is_aggregate_v< View > );
			static_assert( std::is_standard_layout_v< View > );
			static_assert( std::is_trivially_copyable_v< View > );
			static_assert(!std::has_virtual_destructor_v< View > );
			static_assert( std::is_nothrow_destructible_v< View > );
			static_assert( std::is_trivially_destructible_v< View > );
			static_assert( std::is_nothrow_default_constructible_v< View > == ( View::extent == std::dynamic_extent ) );
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
		{	// std::format output
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", std::string_view( "abc" ) ),	"abc" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", pax::string_view2( "abc" ) ),	"abc" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( abc, 3 ) ),				"abc" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", view( first( abc, 3 ) ) ),		"abc" );

			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( abc, 3 ) ) ),	"['a', 'b', 'c']" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 0 ) ),	"[]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 1 ) ),	"[0]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 3 ) ),	"[0, 1, 2]" );
		}
	}
	DOCTEST_TEST_CASE( "view_type, view" ) {
		{	// view, dview, view_type, view_type
			static_assert( std::is_same_v< view_type_t< string_view2 >,					string_view2 > );
			static_assert( std::is_same_v< view_type_t< std::string_view >,				string_view2 > );
			static_assert( std::is_same_v< view_type_t< std::string >,					string_view2 > );
			static_assert( std::is_same_v< view_type_t< span2< int > >,					span2< int > > );
			static_assert( std::is_same_v< view_type_t< span2< int, 4 > >,				span2< int, 4 > > );
			static_assert( std::is_same_v< view_type_t< std::vector< int > >,			span2< int > > );
			static_assert( std::is_same_v< view_type_t< std::array < int, 4 > >,		span2< int, 4 > > );

			static_assert( std::is_same_v< view_type_t< string_view2, true >,			string_view2 > );
			static_assert( std::is_same_v< view_type_t< std::string_view, true >,		string_view2 > );
			static_assert( std::is_same_v< view_type_t< std::string, true >,			string_view2 > );
			static_assert( std::is_same_v< view_type_t< span2< int >, true >,			span2< int > > );
			static_assert( std::is_same_v< view_type_t< span2< int, 4 >, true >,		span2< int > > );
			static_assert( std::is_same_v< view_type_t< std::vector< int >, true >,		span2< int > > );
			static_assert( std::is_same_v< view_type_t< std::array < int, 4 >, true >,	span2< int > > );

			DOCTEST_ASCII_CHECK_EQ( span2( "abcde" ),			"abcde" );
			DOCTEST_ASCII_CHECK_EQ( string_view2( "abcde" ),	"abcde" );

			DOCTEST_FAST_CHECK_LE ( span2( "abcde" ),			"abcde" );
			DOCTEST_FAST_CHECK_LE ( string_view2( "abcde" ),	"abcde" );

			DOCTEST_FAST_CHECK_GE ( span2( "abcde" ),			"abcde" );
			DOCTEST_FAST_CHECK_GE ( string_view2( "abcde" ),	"abcde" );

			{
				static constexpr const char			 	res[] = { '\n', '\r', '\n' };
				const auto								v  = view( res );
				const auto								dv = dview( res );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				DOCTEST_FAST_CHECK_EQ( v.extent,		dynamic_extent );
				DOCTEST_FAST_CHECK_EQ( size( dv ),		3 );
				DOCTEST_FAST_CHECK_EQ( dv.extent,		dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const auto								v  = view( span2< int >{} );
				const auto								dv = dview( span2< int >{} );
				DOCTEST_FAST_CHECK_EQ( size( v ),		0 );
				DOCTEST_FAST_CHECK_EQ( v.extent,		dynamic_extent );
				DOCTEST_FAST_CHECK_EQ( size( dv ),		0 );
				DOCTEST_FAST_CHECK_EQ( dv.extent,		dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		span2< int > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	span2< int > > );
			} {
				const auto								v  = view( span2< int, 0 >{} );
				const auto								dv = dview( span2< int, 0 >{} );
				DOCTEST_FAST_CHECK_EQ( size( v ),		0 );
				DOCTEST_FAST_CHECK_EQ( v.extent,		0 );
				DOCTEST_FAST_CHECK_EQ( size( dv ),		0 );
				DOCTEST_FAST_CHECK_EQ( dv.extent,		dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		span2< int, 0 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	span2< int > > );
			} {
				const auto								v  = view( string_view2{} );
				const auto								dv = dview( string_view2{} );
				DOCTEST_FAST_CHECK_EQ( size( v ),		0 );
				DOCTEST_FAST_CHECK_EQ( v.extent,		dynamic_extent );
				DOCTEST_FAST_CHECK_EQ( size( dv ),		0 );
				DOCTEST_FAST_CHECK_EQ( dv.extent,		dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const auto								v  = view( "" );
				const auto								dv = dview( "" );
				DOCTEST_FAST_CHECK_EQ( size( v ),		0 );
				DOCTEST_FAST_CHECK_EQ( v.extent,		dynamic_extent );
				DOCTEST_FAST_CHECK_EQ( size( dv ),		0 );
				DOCTEST_FAST_CHECK_EQ( dv.extent,		dynamic_extent );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const std::vector< int >				arr{ 1, 2, 3 };
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		span2< const int > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	span2< const int > > );
			} {
				const std::array< int, 3 >				arr{ 1, 2, 3 };
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		span2< const int, 3 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	span2< const int > > );
			} {
				const int								arr[ 3 ] = { 1, 2, 3 };
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		span2< const int, 3 > > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	span2< const int > > );
			} {
				const std::string						arr = "abc";
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const char							  * arr = "abc";
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const char								arr[ 4 ] = "abc";
				const auto								v  = view( arr );
				const auto								dv = dview( arr );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			} {
				const auto								v = view( "abc" );
				const auto								dv = dview( "abc" );
				DOCTEST_FAST_CHECK_EQ( size( v ),		3 );
				static_assert( std::is_same_v< view_type_t< decltype( v ) >,		string_view2 > );
				static_assert( std::is_same_v< view_type_t< decltype( dv ), true >,	string_view2 > );
			}
		}
		{	// span2
			{
				const std::vector< int >				arr{ 1, 2, 3 };
				const auto								sp = span2( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,		std::dynamic_extent );
			} {
				const std::array< int, 3 >				arr{ 1, 2, 3 };
				const auto								sp = span2( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );
			} {
				const int								arr[ 3 ] = { 1, 2, 3 };
				const auto								sp = span2( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );
			} {
				const auto								sp = span2( "abc" );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );	// Includes the final '\0'!!!!
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );	// Includes the final '\0'!!!!
			} {
				const char								arr[ 4 ] = "abc";
				const auto								sp = span2( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );	// Includes the final '\0'!!!!
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );	// Includes the final '\0'!!!!
			} {
				// const char							  * arr = "abc";
				// const auto								sp = span2( arr );
				// DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				// DOCTEST_FAST_CHECK_EQ( sp.extent,		std::dynamic_extent );
			} {
				const char								arr[ 4 ] = "abc";
				const auto								sp = span2( arr );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );
			} {
				const auto								sp = span2( "abc" );
				DOCTEST_FAST_CHECK_EQ( size( sp ),		3 );
				DOCTEST_FAST_CHECK_EQ( sp.extent,		3 );
			}
		}
	}
	DOCTEST_TEST_CASE( "specials" ) {
		{	// valid
			DOCTEST_FAST_CHECK_EQ( valid( std::string_view{} ),		false );
			DOCTEST_FAST_CHECK_EQ( valid( e ),						true );
			DOCTEST_FAST_CHECK_EQ( valid( str ),					true );

			DOCTEST_FAST_CHECK_EQ( valid( std::span< int >{} ),		false );
			DOCTEST_FAST_CHECK_EQ( valid( ints ),					true );
			DOCTEST_FAST_CHECK_EQ( valid( intsN ),					true );
		}
		{	// empty
			DOCTEST_FAST_CHECK_EQ( empty( std::string_view{} ),		true );
			DOCTEST_FAST_CHECK_EQ( empty( e ),						true );
			DOCTEST_FAST_CHECK_EQ( empty( str ),					false );

			DOCTEST_FAST_CHECK_EQ( empty( std::span< int >{} ),		true );
			DOCTEST_FAST_CHECK_EQ( empty( ints ),					false );
			DOCTEST_FAST_CHECK_EQ( empty( intsN ),					false );
		}
		{	// as_const
			int					sp0[ 3 ] = { 0, 1, 2 };
			const std::span		sp	= dview( sp0 );

			static_assert(  is_value_const< std::string_view > );
			static_assert( !is_value_const< decltype( sp.front() ) > );
//			static_assert(  is_value_const< decltype( as_const( sp ).front() ) > );
		}
		{	// as_dynamic
			DOCTEST_FAST_CHECK_EQ( as_dynamic( std::string_view{} ).extent,		dynamic_extent );
			DOCTEST_FAST_CHECK_EQ( as_dynamic( e ).extent,						dynamic_extent );
			DOCTEST_FAST_CHECK_EQ( as_dynamic( str ).extent,					dynamic_extent );

			DOCTEST_FAST_CHECK_EQ( as_dynamic( ints ).extent,					dynamic_extent );
			DOCTEST_FAST_CHECK_EQ( as_dynamic( intsN ).extent,					dynamic_extent );

			DOCTEST_FAST_CHECK_EQ( std::span< int >{}.extent,					dynamic_extent );
			DOCTEST_FAST_CHECK_EQ( ints.extent,									dynamic_extent );
			DOCTEST_FAST_CHECK_EQ( intsN.extent,								N );
		}
		{	// identic
			DOCTEST_FAST_CHECK_UNARY(  identic( str, str ) );
			DOCTEST_FAST_CHECK_UNARY( !identic( str, str.first( 2 ) ) );
			DOCTEST_FAST_CHECK_UNARY(  identic( str, str.first( str.size() ) ) );

			DOCTEST_FAST_CHECK_UNARY(  identic( ints, ints ) );
			DOCTEST_FAST_CHECK_UNARY(  identic( ints, intsN ) );
			DOCTEST_FAST_CHECK_UNARY( !identic( ints, intsN. first( 2 ) ) );
			DOCTEST_FAST_CHECK_UNARY(  identic( ints, intsN. first( intsN.size() ) ) );
		}
		{	// overlap
			const std::span					subc{ subview( str, 3, 3 ) };				// "def"
			DOCTEST_FAST_CHECK_UNARY( !overlap( std::string_view{}, std::string_view{} ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( str, str ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( subc, subc ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( subc, str ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( str, subc ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( str, subview( str, 3, 0 ) ) );		// An emptry span cannot overlap
			DOCTEST_FAST_CHECK_UNARY( !overlap( subview( str, 3, 0 ), str ) );		// An emptry span cannot overlap
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, first( str, 3 ) ) );			// Kloss i kloss
			DOCTEST_FAST_CHECK_UNARY(  overlap( subc, first( str, 4 ) ) );			// First overlapion
			DOCTEST_FAST_CHECK_UNARY(  overlap( subc, not_first( str, 4 ) ) );		// Middle overlapion
			DOCTEST_FAST_CHECK_UNARY(  overlap( subc, not_first( str, 5 ) ) );		// Last overlapion
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, not_first( str, 6 ) ) );		// Kloss i kloss
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, subview( str, 2, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, subview( str, 3, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, subview( str, 4, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, subview( str, 5, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subc, subview( str, 6, 0 ) ) );

			const std::span					subi{ subview( ints, 3, 3 ) };			// { 3, 4, 5 }
			DOCTEST_FAST_CHECK_UNARY( !overlap( std::span< int >{}, std::span< int >{} ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( ints, ints ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( subi, subi ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( subi, ints ) );
			DOCTEST_FAST_CHECK_UNARY(  overlap( ints, subi ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( ints, subview( ints, 3, 0 ) ) );	// An emptry span cannot overlap
			DOCTEST_FAST_CHECK_UNARY( !overlap( subview( ints, 3, 0 ), ints ) );	// An emptry span cannot overlap
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, first( ints, 3 ) ) );			// Kloss i kloss
			DOCTEST_FAST_CHECK_UNARY(  overlap( subi, first( ints, 4 ) ) );			// First overlapion
			DOCTEST_FAST_CHECK_UNARY(  overlap( subi, not_first( ints, 4 ) ) );		// Middle overlapion
			DOCTEST_FAST_CHECK_UNARY(  overlap( subi, not_first( ints, 5 ) ) );		// Last overlapion
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, not_first( ints, 6 ) ) );		// Kloss i kloss
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, subview( ints, 2, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, subview( ints, 3, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, subview( ints, 4, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, subview( ints, 5, 0 ) ) );
			DOCTEST_FAST_CHECK_UNARY( !overlap( subi, subview( ints, 6, 0 ) ) );
		}
		{	// as_bytes, as_writable_bytes
			// DOCTEST_FAST_CHECK_EQ( as_bytes( std::string_view{} ),			0 );
			// DOCTEST_FAST_CHECK_EQ( as_bytes( e ),							0 );
			// DOCTEST_FAST_CHECK_EQ( as_bytes( str ),							str.size() );
			//
			// DOCTEST_FAST_CHECK_EQ( as_bytes( std::span< int >{} ),			0 );
			// DOCTEST_FAST_CHECK_EQ( as_bytes( ints ),						3*sizeof( int ) );
			// DOCTEST_FAST_CHECK_EQ( as_bytes( intsN ),						3*sizeof( int ) );
			//
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( std::string_view{} ),	0 );
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( e ),					0 );
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( str ),				str.size() );
			//
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( std::span< int >{} ),	0 );
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( ints ),				3*sizeof( int ) );
			// DOCTEST_FAST_CHECK_EQ( as_writable_bytes( intsN ),				3*sizeof( int ) );
		}
	}
	DOCTEST_TEST_CASE( "parts" ) {
		constexpr const string_view2				first_	= view( "abcde" );
		constexpr const string_view2				last_	= view( "jkl" );
		constexpr const string_view2				null_{};
		constexpr const string_view2				empty_	= view( "" );
		
		{	// front, back
			char			str[ 10 ] = { '1','2','3','4','5','6','7','8','9','0' };
			const std::span	v( str );
			
			DOCTEST_FAST_CHECK_EQ( front( "abc" ),  'a' );
			DOCTEST_FAST_CHECK_EQ( back ( "abc" ),  'c' );

			v[ 0 ]			= 'a';		DOCTEST_FAST_CHECK_EQ( front( v ), 'a' );
			front( v )		= 'c';		DOCTEST_FAST_CHECK_EQ( front( v ), 'c' );
			back( v )		= 'd';		DOCTEST_FAST_CHECK_EQ( back( v ),  'd' );
			*v.begin()		= 'e';		DOCTEST_FAST_CHECK_EQ( front( v ), 'e' );
			*v.rbegin()		= 'f';		DOCTEST_FAST_CHECK_EQ( back( v ),  'f' );
		}
		{	// first
			DOCTEST_ASCII_CHECK_EQ( first( "abcdefghijkl", 22 ),  	str );
			// Should assert:	    first< 13 >( "abcdefghijkl" )

			DOCTEST_FAST_CHECK_EQ( first( str,  0 ), 				empty_ );
			DOCTEST_FAST_CHECK_EQ( first( str,  5 ), 				first_ );
			DOCTEST_FAST_CHECK_EQ( first( str, 22 ),  				str );
			DOCTEST_FAST_CHECK_EQ( first( str, 22 ).data(),  		str.data() );
			DOCTEST_FAST_CHECK_EQ( first( null_, 22 ),  			empty_ );
			DOCTEST_FAST_CHECK_EQ( first( null_, 22 ).data(),  		nullptr );
			DOCTEST_FAST_CHECK_EQ( first<  5 >( str ), 				first_ );
			DOCTEST_FAST_CHECK_EQ( first<  5 >( strN ), 			first_ );
			DOCTEST_FAST_CHECK_EQ( first< 22 >( strN ),				str );
			DOCTEST_FAST_CHECK_EQ( first< 22 >( strN ).data(),		strN.data() );
			DOCTEST_FAST_CHECK_EQ( first<  0 >( null_ ), 			empty_ );
			DOCTEST_FAST_CHECK_EQ( first<  0 >( null_ ).data(), 	nullptr );
			// Should assert:	   first< 22 >( empty_ )
		}
		{	// last
			DOCTEST_ASCII_CHECK_EQ( last( "abcdefghijkl", 22 ),  	str );
			DOCTEST_ASCII_CHECK_EQ( last< 12 >( "abcdefghijkl" ),	str );

			DOCTEST_FAST_CHECK_EQ( last( str,  0 ), 				empty_ );
			DOCTEST_FAST_CHECK_EQ( last( str,  3 ), 				last_ );
			DOCTEST_FAST_CHECK_EQ( last( str, 22 ), 				str );
			DOCTEST_FAST_CHECK_EQ( last( str, 22 ).data(), 			str.data() );
			DOCTEST_FAST_CHECK_EQ( last( null_, 22 ),  				empty_ );
			DOCTEST_FAST_CHECK_EQ( last( null_, 22 ).data(),  		nullptr );
			DOCTEST_FAST_CHECK_EQ( last < 3 >( str ), 				last_ );
			DOCTEST_FAST_CHECK_EQ( last < 3 >( strN ), 				last_ );
			DOCTEST_FAST_CHECK_EQ( last< 22 >( strN ), 				str );
			DOCTEST_FAST_CHECK_EQ( last< 22 >( strN ).data(), 		strN.data() );
			DOCTEST_FAST_CHECK_EQ( last<  0 >( null_ ), 			empty_ );
			DOCTEST_FAST_CHECK_EQ( last<  0 >( null_ ).data(), 		nullptr );
			// Should assert:	   last< 22 >( empty_ )
		}
		{	// not_first
			DOCTEST_ASCII_CHECK_EQ( not_first( "abcdefghijkl", 0 ), str );
			DOCTEST_FAST_CHECK_EQ( not_first( str,  0 ), 			str );
			DOCTEST_FAST_CHECK_EQ( not_first( str,  9 ), 			last_ );
			DOCTEST_FAST_CHECK_EQ( not_first( str, 22 ), 			empty_ );
			DOCTEST_FAST_CHECK_EQ( not_first( str, 22 ).data(), 	str.data() + str.size() );
			DOCTEST_FAST_CHECK_EQ( not_first( null_, 22 ),  		empty_ );
			DOCTEST_FAST_CHECK_EQ( not_first( null_, 22 ).data(),  	nullptr );
			DOCTEST_FAST_CHECK_EQ( not_first<  0 >( strN ),			str );
			DOCTEST_FAST_CHECK_EQ( not_first<  9 >( strN ), 		last_ );
			DOCTEST_FAST_CHECK_EQ( not_first< 22 >( strN ), 		empty_ );
			DOCTEST_FAST_CHECK_EQ( not_first< 22 >( strN ).data(), 	strN.data() + strN.size() );
		}
		{	// not_last
			DOCTEST_ASCII_CHECK_EQ( not_last( "abcdefghijkl", 0 ), 	str );
			DOCTEST_FAST_CHECK_EQ( not_last( str,  0 ), 			str );
			DOCTEST_FAST_CHECK_EQ( not_last( str,  7 ), 			first_ );
			DOCTEST_FAST_CHECK_EQ( not_last( str, 22 ), 			empty_ );
			DOCTEST_FAST_CHECK_EQ( not_last( str, 22 ).data(), 		str.data() );
			DOCTEST_FAST_CHECK_EQ( not_last( null_, 22 ),  			empty_ );
			DOCTEST_FAST_CHECK_EQ( not_last( null_, 22 ).data(),  	nullptr );
			DOCTEST_FAST_CHECK_EQ( not_last <  0 >( strN ),			str );
			DOCTEST_FAST_CHECK_EQ( not_last <  7 >( strN ), 		first_ );
			DOCTEST_FAST_CHECK_EQ( not_last < 22 >( strN ), 		empty_ );
			DOCTEST_FAST_CHECK_EQ( not_last < 22 >( strN ).data(), 	strN.data() );
		}
		{	// subview
			DOCTEST_ASCII_CHECK_EQ( subview( "abcdefghijkl",   0, 22 ), str );
			DOCTEST_ASCII_CHECK_EQ( subview( "abcdefghijkl", -22, 22 ), str );

			DOCTEST_ASCII_CHECK_EQ((subview( null_, 0, 0 )),			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( null_, 0, 0 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ((subview( null_, 0, 2 )),			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( null_, 0, 2 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ((subview( null_, 2, 2 )),			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( null_, 2, 2 ).data(),		nullptr );
			DOCTEST_ASCII_CHECK_EQ((subview( null_, 2, 0 )),			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( null_, 2, 0 ).data(),		nullptr );

			DOCTEST_ASCII_CHECK_EQ((subview( str,   0, 22 )),			str );
			DOCTEST_ASCII_CHECK_EQ((subview( str,   0,  5 )), 			first( str, 5 ) );
			DOCTEST_ASCII_CHECK_EQ((subview( str,   3,  2 )), 			"de" );
			DOCTEST_ASCII_CHECK_EQ((subview( str,   7, 22 )), 			not_first( str, 7 ) );
			DOCTEST_ASCII_CHECK_EQ((subview( str,  22,  0 )), 			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( str,  22,  0 ).data(), 	str.data() + str.size() );
			DOCTEST_ASCII_CHECK_EQ((subview( str,  -7, 22 )), 			last( str, 7 ) );
			DOCTEST_ASCII_CHECK_EQ((subview( str,  -9,  2 )), 			"de" );
			DOCTEST_ASCII_CHECK_EQ((subview( str, -22,  0 )), 			empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview( str, -22,  0 ).data(), 	str.data() );
			DOCTEST_ASCII_CHECK_EQ((subview( str, -22, 22 )), 			str );

			DOCTEST_ASCII_CHECK_EQ((subview<   2 >( "abcde", -2 )),		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   2 >( str,   3 )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   2 >( strN,  3 )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   2 >( str,  -9 )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   2 >( strN, -9 )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<  12 >( strN,  0 )),		str );

			DOCTEST_ASCII_CHECK_EQ((subview<  -2,  2 >( "abcde" )), 	"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   0,  5 >( str  )), 		first( str, 5 ) );
			DOCTEST_ASCII_CHECK_EQ((subview<   0,  5 >( strN )), 		first( str, 5 ) );
			DOCTEST_ASCII_CHECK_EQ((subview<   0, 12 >( strN )),		str );
			DOCTEST_ASCII_CHECK_EQ((subview<   0, 22 >( strN )),		str );
			DOCTEST_ASCII_CHECK_EQ((subview<   3,  2 >( str  )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<   3,  2 >( strN )), 		"de" );
			// Should assert:	    subview<   7, 22 >( str  )
			DOCTEST_ASCII_CHECK_EQ((subview<   7, 22 >( strN )), 		not_first( str, 7 ) );
			DOCTEST_ASCII_CHECK_EQ((subview<  22,  0 >( str  )), 		empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview<  22,  0 >( str  ).data(), 	str.data() + strN.size() );
			DOCTEST_ASCII_CHECK_EQ((subview<  22,  0 >( strN )), 		empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview<  22,  0 >( strN ).data(), 	strN.data() + strN.size() );
			// Should assert:	    subview<  22, 22 >( str  )
			DOCTEST_ASCII_CHECK_EQ((subview<  22, 22 >( strN )), 		empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview<  22, 22 >( strN ).data(), 	strN.data() + strN.size() );
			// Should assert:	   subview<  -7, 22 >( str  )
			DOCTEST_ASCII_CHECK_EQ((subview<  -7, 22 >( strN )), 		last( str, 7 ) );
			DOCTEST_ASCII_CHECK_EQ((subview<  -9,  2 >( strN )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview<  -9,  2 >( str  )), 		"de" );
			DOCTEST_ASCII_CHECK_EQ((subview< -22,  0 >( str  )),		empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview< -22,  0 >( str  ).data(), 	str.data() );
			DOCTEST_ASCII_CHECK_EQ((subview< -22,  0 >( strN )), 		empty_ );
			DOCTEST_FAST_CHECK_EQ ( subview< -22,  0 >( strN ).data(), 	strN.data() );
			DOCTEST_ASCII_CHECK_EQ((subview< -22, 22 >( strN )), 		str );
		}
	}
	DOCTEST_TEST_CASE( "searching-related" ) {
		constexpr const string_view2				null_{};
		constexpr const std::span					abd( view( "abddefffijkl" ) );
		constexpr auto f = []( auto c ){ return c == 'f'; };
		constexpr auto x = []( auto c ){ return c == 'x'; };

		{	// find
			DOCTEST_FAST_CHECK_EQ( find( null_, 0 ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( null_, f ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( abd,  f  ),					5 );
			DOCTEST_FAST_CHECK_EQ( find( abd, 'f' ),					5 );
			DOCTEST_FAST_CHECK_EQ( find( abd,  x  ),					abd.size() );
			DOCTEST_FAST_CHECK_EQ( find( abd, 'x' ),					abd.size() );

			DOCTEST_FAST_CHECK_EQ( find( null_, str ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( str, null_ ),					0 );
			DOCTEST_FAST_CHECK_EQ( find( null_, null_ ),				0 );

			DOCTEST_FAST_CHECK_EQ( find( str, "cde" ), 					2 );
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
			DOCTEST_FAST_CHECK_EQ( rfind( abd,  x  ),					abd.size() );
			DOCTEST_FAST_CHECK_EQ( rfind( abd, 'x' ),					abd.size() );
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
				auto div = split_at( "abcdefghijkl", 4 );
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
			{	// by std::string_view
				constexpr auto base = subview( abc, 3, 4 );
				DOCTEST_FAST_CHECK_EQ( base, "defg" );
				
				auto div = split_by( "defg", "ef" );
				DOCTEST_ASCII_CHECK_EQ( div.first , view( "d" ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, view( "g" ) );

				div = split_by( str, first( str, 2 ) );			// First
				DOCTEST_FAST_CHECK_EQ ( find( str, first( str, 2 ) ), 0 );
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str,  0 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str, 10 ) );
				DOCTEST_FAST_CHECK_EQ ( div.first.data() , str.data() );

				div = split_by( str, subview( str, 4, 2 ) );	// Middle
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str,  4 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str,  6 ) );

				div = split_by( str, last( str, 2 ) );			// End
				DOCTEST_ASCII_CHECK_EQ( div.first , first( str, 10 ) );
				DOCTEST_ASCII_CHECK_EQ( div.second, last ( str,  0 ) );
				DOCTEST_FAST_CHECK_EQ ( div.second.data(), str.data() + str.size() );

				div = split_by( str, "aaa" );					// None
				DOCTEST_ASCII_CHECK_EQ( div.first , str );
				DOCTEST_ASCII_CHECK_EQ( div.second, last( str, 0 ) );
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
			constexpr std::string_view		text{ "++++abcdef++" };
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
				DOCTEST_ASCII_CHECK_EQ( trim_first( text, is_plus ),	not_first( text, 4 ) );
				DOCTEST_ASCII_CHECK_EQ( trim_first( text, pred ),		not_first( text, 6 ) );

				DOCTEST_ASCII_CHECK_EQ( trim_first( "",					Newline{} ),	"" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "\n\r",				Newline{} ),	"" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "abcdefgh\n\r",		Newline{} ),	"abcdefgh\n\r" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "\nabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "\n\rabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "\rabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
				DOCTEST_ASCII_CHECK_EQ( trim_first( "\r\nabcdefgh\n\r",	Newline{} ),	"abcdefgh\n\r" );
			}
			{	// trim_last
				DOCTEST_ASCII_CHECK_EQ( trim_last( text, is_plus ),		not_last( text, 2 ) );
				DOCTEST_ASCII_CHECK_EQ( trim_last( text, pred ),		not_last( text, 2 ) );

				DOCTEST_ASCII_CHECK_EQ( trim_last( "",					Newline{} ),	"" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\r",				Newline{} ),	"" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh",		Newline{} ),	"\n\rabcdefgh" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\n",	Newline{} ),	"\n\rabcdefgh" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\n\r",	Newline{} ),	"\n\rabcdefgh" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\r",	Newline{} ),	"\n\rabcdefgh" );
				DOCTEST_ASCII_CHECK_EQ( trim_last( "\n\rabcdefgh\r\n",	Newline{} ),	"\n\rabcdefgh" );
			}
			{	// trim
				DOCTEST_ASCII_CHECK_EQ( trim( text, is_plus ),	subview( text, 4, 6 ) );
				DOCTEST_ASCII_CHECK_EQ( trim( text, pred ),		subview( text, 6, 4 ) );
			}
		}
	}
	DOCTEST_TEST_CASE( "other..." ) {
		constexpr const std::span					abd( view( "abddefffijkl" ) );

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
				std::ostringstream		os;
				constexpr auto			v0 = view( ">\0\a\b\t\n\v\f\r\"'\x18\x7f <" );
				constexpr auto			vr = view( ">\\0\\a\\b\\t\\n\\v\\f\\r\\\"\\'^X^?\u00B7<" );

				os << as_ascii( v0 );
				const auto 				res0{ os.str() };
				const std::span 		res{ res0.data(), res0.size() };

				DOCTEST_FAST_CHECK_EQ( v0.size(),   15 );
				DOCTEST_FAST_CHECK_EQ( vr.size(),   28 );
				DOCTEST_FAST_CHECK_EQ( res0.size(), vr.size() );
				DOCTEST_FAST_CHECK_EQ( res.size(),  vr.size() );
				DOCTEST_FAST_CHECK_EQ( res, vr );
			}
		}
	}
	DOCTEST_TEST_CASE( "String_splitter" ) { 
		// for( auto s : String_splitter( "ett två tre", ' ' ) )	Debug{} << s;

		constexpr String_splitter	split( "ett två tre", ' ' );
		auto						itr = split.begin();
		DOCTEST_FAST_CHECK_EQ( *    itr,   "ett" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ), "två" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ), "tre" );
		DOCTEST_FAST_CHECK_NE(   itr, split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr, split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr, split.end() );
	}

}	// namespace pax
