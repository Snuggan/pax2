//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>

#include <pax/std/format.hpp>
#include <pax/doctest.hpp>

#include <sstream>
#include <array>
#include <vector>


namespace pax {

	/// What std::span or std::string_view is suitable for a type?
	/// Candidates are std::span< T >, std::span< T, N >, or std::string_view< T >.
	template< traits::contiguous C, bool Dynamic = false >
	struct view_type {
		using type = std::span< traits::element_type_t< C >, Dynamic ? std::dynamic_extent : traits::extent_v< C > >;
	};

	template< typename T, std::size_t N, bool Dynamic >
	struct view_type< std::span< T, N >, Dynamic > {
		using type = std::span< T, Dynamic ? std::dynamic_extent : N >;
	};

	template< traits::string S, bool Dynamic >
	struct view_type< S, Dynamic > {
		using type = std::basic_string_view< traits::value_type_t< S > >;
	};

	template< traits::string S, bool Dynamic >
		requires( std::remove_cvref_t< S >::traits_type )
	struct view_type< S, Dynamic > {
		using Ch = traits::value_type_t< S >;
		using Tr = typename std::remove_cvref_t< S >::traits_type;
		using type = std::basic_string_view< Ch, Tr >;
	};

	template< typename T, bool Dynamic >
	struct view_type< const T, Dynamic > : view_type< T, Dynamic > {};

	template< traits::contiguous C, bool Dynamic = false >
	using view_type_t = typename view_type< C, Dynamic >::type;




	constexpr auto								str		= "abcdefghijkl";
	constexpr auto	 							strN	= make_span( "abcdefghijkl" );
	constexpr const std::string_view			e( "" );

	constexpr const std::size_t 				N = 12;
	constexpr const int							ints0[ N ] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };
	constexpr const std::span< const int >		ints ( ints0 );
	constexpr const std::span< const int, N >	intsN( ints0 );
	constexpr const auto						abc		= std::span< const char >{ str, 12 };
	
	template< typename T >
	concept is_value_const = std::is_const_v< std::remove_reference_t< traits::element_type_t< T > > >;
	
	template< typename T >
	constexpr auto safe_first( T && t_, std::size_t sz_ ) {
		using std::data, std::size;
		return view_type_t< T, true >( data( t_ ), ( sz_ < size( t_ ) ? sz_ : size( t_ ) ) );
	}
	
	template< typename T >
	constexpr auto safe_last( T && t_, std::size_t sz_ ) {
		using std::data, std::size;
		if( sz_ > size( t_ ) )		sz_ = size( t_ );
		return view_type_t< T, true >( data( t_ ) + size( t_ ) - sz_, sz_ );
	}
	
	template< typename T >
	constexpr auto safe_subview( T && t_, int i_, int sz_ ) {
		using std::data, std::size;
		if( i_ < 0 )							i_	+= size( t_ );
		if( i_ < 0 )							i_	 = 0;
		if( i_ > int( size( t_ ) ) )			i_	 = size( t_ );
		if( sz_ + i_ > int( size( t_ ) ) )		sz_	 = size( t_ ) - i_;
		return safe_first( safe_last( t_, size( t_ ) - i_ ), sz_ );
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
		{	// is_newline
			static_assert(  is_newline( '\n' ) );
			static_assert(  is_newline( '\r' ) );
			static_assert( !is_newline( '\a' ) );
			static_assert( !is_newline( ' '  ) );
		}
		{	// newlines
			static_assert( newlines( '\n', '\r' ) == 2 );
			static_assert( newlines( '\r', '\n' ) == 2 );
			static_assert( newlines( '\n', '\n' ) == 1 );
			static_assert( newlines( '\r', '\r' ) == 1 );
			static_assert( newlines( '\n', '\a' ) == 1 );
			static_assert( newlines( '\r', '\a' ) == 1 );
			static_assert( newlines( '\a', '\n' ) == 0 );
			static_assert( newlines( '\a', '\r' ) == 0 );
		}
		{	// std::format output
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 0 ) ),	"[]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 1 ) ),	"[0]" );
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", first( ints, 3 ) ),	"[0, 1, 2]" );

			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", make_span( first( abc, 3 ) ) ),	"['a', 'b', 'c']" );
			
			DOCTEST_FAST_CHECK_EQ( std::string_view( "abc" ),	"abc" );
			DOCTEST_FAST_CHECK_EQ( first( abc, 3 ),				"abc" );

			constexpr char const	  * strs[] = { "abc", "def", "ghi" };
			DOCTEST_FAST_CHECK_EQ( std20::format( "{}", std::span( strs ) ),	"[\"abc\", \"def\", \"ghi\"]" );
		}
	}
	DOCTEST_TEST_CASE( "view_type, make_span" ) {
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
			static_assert(  empty( e ) );

			static_assert(  empty( std::span< int >{} ) );
			static_assert(  empty( std::vector< int >{} ) );
			static_assert(  empty( std::array< int, 0 >{} ) );
			static_assert( !empty( ints ) );
			static_assert( !empty( intsN ) );
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
		{	// make_const_span
			int					sp0[ 3 ] = { 0, 1, 2 };
			std::span			sp( sp0 );

			static_assert(  is_value_const < std::string_view > );
			static_assert( !is_value_const < decltype( sp ) > );
			static_assert( !std::is_const_v< decltype( sp.front() ) > );
			static_assert(  is_value_const < decltype( make_const_span( sp ) ) > );
			static_assert(  std::is_const_v< std::remove_reference_t< decltype( make_const_span( sp ).front() ) > > );
		}
		{	// make_dynamic_span
			static_assert( make_dynamic_span( std::string_view{} ).extent	== traits::dynamic_extent );
			static_assert( make_dynamic_span( e ).extent					== traits::dynamic_extent );
			static_assert( make_dynamic_span( str ).extent					== traits::dynamic_extent );

			static_assert( make_dynamic_span( ints ).extent					== traits::dynamic_extent );
			static_assert( make_dynamic_span( intsN ).extent				== traits::dynamic_extent );

			static_assert( std::span< int >{}.extent						== traits::dynamic_extent );
			static_assert( ints.extent										== traits::dynamic_extent );
			static_assert( intsN.extent										== N );
		}
		{	// identic
			DOCTEST_FAST_CHECK_UNARY(  identic( "abc", "abc" ) );
			DOCTEST_FAST_CHECK_UNARY( !identic( "abc", first( "abcdef", 3 ) ) );

			constexpr std::string_view	str2( str );
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
		constexpr const std::string_view	null_{};
		
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

			static_assert( identic( first( str,   0 ), safe_first( str,   0 ) ) );
			static_assert( identic( first( str,   5 ), safe_first( str,   5 ) ) );
			static_assert( identic( first( str,  12 ), safe_first( str,  12 ) ) );
			static_assert( identic( first( str,  22 ), safe_first( str,  22 ) ) );

			static_assert( identic( first( strN,  0 ), safe_first( strN,  0 ) ) );
			static_assert( identic( first( strN,  5 ), safe_first( strN,  5 ) ) );
			static_assert( identic( first( strN, 12 ), safe_first( strN, 12 ) ) );
			static_assert( identic( first( strN, 22 ), safe_first( strN, 22 ) ) );

			static_assert( identic( first<  0 >( strN ), safe_first( strN,  0 ) ) );
			static_assert( identic( first<  5 >( strN ), safe_first( strN,  5 ) ) );
			static_assert( identic( first< 12 >( strN ), safe_first( strN, 12 ) ) );
			static_assert( identic( first< 22 >( strN ), safe_first( strN, 22 ) ) );

			static_assert( identic( first( strN, Stat<  0u > ), safe_first( strN,  0 ) ) );
			static_assert( identic( first( strN, Stat<  5u > ), safe_first( strN,  5 ) ) );
			static_assert( identic( first( strN, Stat< 12u > ), safe_first( strN, 12 ) ) );
			static_assert( identic( first( strN, Stat< 22u > ), safe_first( strN, 22 ) ) );
		}
		{	// first (int)
			static_assert( identic( first( ints,   0 ), safe_first( ints,   0 ) ) );
			static_assert( identic( first( ints,   5 ), safe_first( ints,   5 ) ) );
			static_assert( identic( first( ints,  12 ), safe_first( ints,  12 ) ) );
			static_assert( identic( first( ints,  22 ), safe_first( ints,  22 ) ) );

			static_assert( identic( first( intsN,  0 ), safe_first( intsN,  0 ) ) );
			static_assert( identic( first( intsN,  5 ), safe_first( intsN,  5 ) ) );
			static_assert( identic( first( intsN, 12 ), safe_first( intsN, 12 ) ) );
			static_assert( identic( first( intsN, 22 ), safe_first( intsN, 22 ) ) );

			static_assert( identic( first<  0 >( ints  ), safe_first( ints,   0 ) ) );
			static_assert( identic( first<  5 >( ints  ), safe_first( ints,   5 ) ) );
			static_assert( identic( first< 12 >( ints  ), safe_first( ints,  12 ) ) );
			static_assert( first<  5 >( ints  ).extent	==	5 );

			static_assert( identic( first<  0 >( intsN ), safe_first( intsN,  0 ) ) );
			static_assert( identic( first<  5 >( intsN ), safe_first( intsN,  5 ) ) );
			static_assert( identic( first< 12 >( intsN ), safe_first( intsN, 12 ) ) );
			static_assert( identic( first< 22 >( intsN ), safe_first( intsN, 22 ) ) );
			static_assert( first<  5 >( intsN ).extent	==	5 );
		}
		{	// last (character)
			static_assert( last( "abcdefghijkl", 22 )		==	str );

			static_assert( identic( last( str,   0 ), safe_last( str,   0 ) ) );
			static_assert( identic( last( str,   5 ), safe_last( str,   5 ) ) );
			static_assert( identic( last( str,  12 ), safe_last( str,  12 ) ) );
			static_assert( identic( last( str,  22 ), safe_last( str,  22 ) ) );

			static_assert( identic( last( strN,  0 ), safe_last( strN,  0 ) ) );
			static_assert( identic( last( strN,  5 ), safe_last( strN,  5 ) ) );
			static_assert( identic( last( strN, 12 ), safe_last( strN, 12 ) ) );
			static_assert( identic( last( strN, 22 ), safe_last( strN, 22 ) ) );

			static_assert( identic( last<  0 >( strN ), safe_last( strN,  0 ) ) );
			static_assert( identic( last<  5 >( strN ), safe_last( strN,  5 ) ) );
			static_assert( identic( last< 12 >( strN ), safe_last( strN, 12 ) ) );
			static_assert( identic( last< 22 >( strN ), safe_last( strN, 22 ) ) );
		}
		{	// last (int)
			static_assert( identic( last( ints,   0 ), safe_last( ints,   0 ) ) );
			static_assert( identic( last( ints,   5 ), safe_last( ints,   5 ) ) );
			static_assert( identic( last( ints,  12 ), safe_last( ints,  12 ) ) );
			static_assert( identic( last( ints,  22 ), safe_last( ints,  22 ) ) );

			static_assert( identic( last( intsN,  0 ), safe_last( intsN,  0 ) ) );
			static_assert( identic( last( intsN,  5 ), safe_last( intsN,  5 ) ) );
			static_assert( identic( last( intsN, 12 ), safe_last( intsN, 12 ) ) );
			static_assert( identic( last( intsN, 22 ), safe_last( intsN, 22 ) ) );

			static_assert( identic( last<  0 >( ints  ), safe_last( ints,   0 ) ) );
			static_assert( identic( last<  5 >( ints  ), safe_last( ints,   5 ) ) );
			static_assert( identic( last< 12 >( ints  ), safe_last( ints,  12 ) ) );
			static_assert( last<  5 >( ints  ).extent	==	5 );

			static_assert( identic( last<  0 >( intsN ), safe_last( intsN,  0 ) ) );
			static_assert( identic( last<  5 >( intsN ), safe_last( intsN,  5 ) ) );
			static_assert( identic( last< 12 >( intsN ), safe_last( intsN, 12 ) ) );
			static_assert( last<  5 >( intsN ).extent	==	5 );
		}
		{	// not_first (character)
			static_assert( not_first( "abcdefghijkl", 0 )		==	str );

			static_assert( identic( not_first( str,   0 ), safe_subview( str,  0, 99 ) ) );
			static_assert( identic( not_first( str,   5 ), safe_subview( str,  5, 99 ) ) );
			static_assert( identic( not_first( str,  12 ), safe_subview( str, 12, 99 ) ) );
			static_assert( identic( not_first( str,  22 ), safe_subview( str, 22, 99 ) ) );

			static_assert( identic( not_first( strN,  0 ), safe_subview( strN,  0, 99 ) ) );
			static_assert( identic( not_first( strN,  5 ), safe_subview( strN,  5, 99 ) ) );
			static_assert( identic( not_first( strN, 12 ), safe_subview( strN, 12, 99 ) ) );
			static_assert( identic( not_first( strN, 22 ), safe_subview( strN, 22, 99 ) ) );

			static_assert( identic( not_first<  0 >( strN ), safe_subview( strN,  0, 99 ) ) );
			static_assert( identic( not_first<  5 >( strN ), safe_subview( strN,  5, 99 ) ) );
			static_assert( identic( not_first< 12 >( strN ), safe_subview( strN, 12, 99 ) ) );
			static_assert( identic( not_first< 22 >( strN ), safe_subview( strN, 22, 99 ) ) );
		}
		{	// not_first (int)
			static_assert( identic( not_first( ints,   0 ), safe_subview( ints,  0, 99 ) ) );
			static_assert( identic( not_first( ints,   5 ), safe_subview( ints,  5, 99 ) ) );
			static_assert( identic( not_first( ints,  12 ), safe_subview( ints, 12, 99 ) ) );
			static_assert( identic( not_first( ints,  22 ), safe_subview( ints, 22, 99 ) ) );

			static_assert( identic( not_first( intsN,  0 ), safe_subview( intsN,  0, 99 ) ) );
			static_assert( identic( not_first( intsN,  5 ), safe_subview( intsN,  5, 99 ) ) );
			static_assert( identic( not_first( intsN, 12 ), safe_subview( intsN, 12, 99 ) ) );
			static_assert( identic( not_first( intsN, 22 ), safe_subview( intsN, 22, 99 ) ) );

			static_assert( identic( not_first<  0 >( intsN ), safe_subview( intsN,  0, 99 ) ) );
			static_assert( identic( not_first<  5 >( intsN ), safe_subview( intsN,  5, 99 ) ) );
			static_assert( identic( not_first< 12 >( intsN ), safe_subview( intsN, 12, 99 ) ) );
			static_assert( identic( not_first< 22 >( intsN ), safe_subview( intsN, 22, 99 ) ) );
		}
		{	// not_last (character)
			static_assert( not_last( "abcdefghijkl", 0 )		==	str );

			static_assert( identic( not_last( str,   0 ), str ) );
			static_assert( identic( not_last( str,   5 ), safe_first( str, 7 ) ) );
			static_assert( identic( not_last( str,  12 ), safe_first( str, 0 ) ) );
			static_assert( identic( not_last( str,  22 ), safe_first( str, 0 ) ) );

			static_assert( identic( not_last( strN,  0 ), strN ) );
			static_assert( identic( not_last( strN,  5 ), safe_first( strN, 7 ) ) );
			static_assert( identic( not_last( strN, 12 ), safe_first( strN, 0 ) ) );
			static_assert( identic( not_last( strN, 22 ), safe_first( strN, 0 ) ) );
			static_assert( identic( not_last( null_, 22 ), null_ ) );

			static_assert( identic( not_last<  0 >( strN ), strN ) );
			static_assert( identic( not_last<  5 >( strN ), safe_first( strN, 7 ) ) );
			static_assert( identic( not_last< 12 >( strN ), safe_first( strN, 0 ) ) );
			static_assert( identic( not_last< 22 >( strN ), safe_first( strN, 0 ) ) );
		}
		{	// not_last (int)
			static_assert( identic( not_last( ints,   0 ), ints ) );
			static_assert( identic( not_last( ints,   5 ), safe_first( ints, 7 ) ) );
			static_assert( identic( not_last( ints,  12 ), safe_first( ints, 0 ) ) );
			static_assert( identic( not_last( ints,  22 ), safe_first( ints, 0 ) ) );

			static_assert( identic( not_last( intsN,  0 ), intsN ) );
			static_assert( identic( not_last( intsN,  5 ), safe_first( intsN, 7 ) ) );
			static_assert( identic( not_last( intsN, 12 ), safe_first( intsN, 0 ) ) );
			static_assert( identic( not_last( intsN, 22 ), safe_first( intsN, 0 ) ) );

			static_assert( identic( not_last<  0 >( intsN ), intsN ) );
			static_assert( identic( not_last<  5 >( intsN ), safe_first( intsN, 7 ) ) );
			static_assert( identic( not_last< 12 >( intsN ), safe_first( intsN, 0 ) ) );
			static_assert( identic( not_last< 22 >( intsN ), safe_first( intsN, 0 ) ) );
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
			static_assert( find( null_, 0 )		==	 0 );
			static_assert( find( null_, f )		==	 0 );
			static_assert( find( abd,  f  )		==	 5 );
			static_assert( find( abd, 'f' )		==	 5 );
			static_assert( find( abd,  x  )		==	12 );
			static_assert( find( abd, 'x' )		==	12 );

			static_assert( find( null_, str )	==	 0 );
			static_assert( find( str, null_ )	==	 0 );
			static_assert( find( null_, null_ )	==	 0 );

			static_assert( find( str, "cde" )	==	 2 );
			static_assert( find( str, "cdf" )	==	 std::string_view( str ).size() );
			static_assert( find( "abcdefghijkl", "cde" )		==	 2 );
			static_assert( find( str, first( str, 5 ) )			==	 0 );
			static_assert( find( str, last ( str, 7 ) )			==	 5 );

			static_assert( find( null_, Newline{} )				==	 0 );
			static_assert( find( "abcd\r\nefgh", Newline{} )	==	 4 );
		}
		{	// rfind
			static_assert( rfind( null_, f )	==	 0 );
			static_assert( rfind( abd,  f  )	==	 7 );
			static_assert( rfind( abd, 'a' )	==	 0 );
			static_assert( rfind( null_, 'f' )	==	 0 );
			static_assert( rfind( abd, 'f' )	==	 7 );
			static_assert( rfind( abd,  x  )	==	12 );
			static_assert( rfind( abd, 'x' )	==	12 );
			// 			static_assert( rfind( null_, str )		==	 0 );
			// static_assert( rfind( str, first( str, 5 ) )	==	 0 );
			// static_assert( rfind( str, last ( str, 7 ) )	==	 5 );
			// static_assert( rfind( null_, Newline{} )		==	 0 );
			// static_assert( rfind( "abcd\r\nefgh", Newline{} )	==	4 );
		}
		{	// contains
			static_assert(  contains( str, 'd' ) );
			static_assert( !contains( str, '8' ) );
			static_assert(  contains( str, first( str, 5 ) ) );
			static_assert(  contains( str, last ( str, 5 ) ) );
			static_assert(  contains( "abcdefghijkl", first( str, 5 ) ) );
			static_assert(  contains( "abcdefghijkl", "cdef" ) );
			static_assert(  contains( "abcd\r\nefgh", Newline{} ) );
		}
		{	// until
			using std::data, std::size;
			constexpr auto base = subview( abc, 3, 4 );
			static_assert( base			==	"defg" );

			static_assert( until( base, 'a' )	==	base );				// None
			static_assert( until( base, 'd' )	==	first( base, 0 ) );	// First
			static_assert( until( base, 'f' )	==	first( base, 2 ) );	// Middle
			static_assert( until( base, 'g' )	==	first( base, 3 ) );	// Last

			static_assert( until( "abcdefgh", 'g' )		==	"abcdef" );	// Last
			static_assert( until( "abcdefgh", "fg" )	==	"abcde" );	// Last

			// Newline 
			static_assert( until( "abcd\r\nefgh"	, Newline{} ).size() == 4 );
			static_assert( until( ""				, Newline{} )	==	"" );
			static_assert( until( "abcdefgh"		, Newline{} )	==	"abcdefgh" );

			static_assert( until( "\nabcdefgh"		, Newline{} )	==	"" );
			static_assert( until( "\n\r\nabcdefgh"	, Newline{} )	==	"" );
			static_assert( until( "\rabcdefgh"		, Newline{} )	==	"" );
			static_assert( until( "\r\nabcdefgh"	, Newline{} )	==	"" );

			static_assert( until( "abcd\nefgh"		, Newline{} )	==	"abcd" );
			static_assert( until( "abcd\n\refgh"	, Newline{} )	==	"abcd" );
			static_assert( until( "abcd\refgh"		, Newline{} )	==	"abcd" );
			static_assert( until( "abcd\r\nefgh"	, Newline{} )	==	"abcd" );

			static_assert( until( "abcdefgh\n"		, Newline{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\n\r"	, Newline{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\r"		, Newline{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\r\n"	, Newline{} )	==	"abcdefgh" );
		}
		{	// equal
			static_assert(  equal( "", "" ) );
			static_assert(  equal( str, str ) );
			static_assert(  equal( "abcdefghijkl", "abcdefghijkl" ) );
			static_assert(  equal( str, "abcdefghijkl" ) );
			static_assert(  equal( "abcdefghijkl", str ) );
			static_assert( !equal( "abcdef", "abc" ) );
			static_assert( !equal( "abc", "abcdef" ) );
		}
		{	// starts_with
			static_assert( starts_with( "abcdef", "abc" )	==	3 );
			static_assert( starts_with( "abcdef", "abd" )	==	0 );
			static_assert( starts_with( str, ""    )		==	0 );
			static_assert( starts_with( null_, "123" )		==	0 );
			static_assert( starts_with( str, "abc" )		==	3 );
			static_assert( starts_with( str, "def" )		==	0 );
			static_assert( starts_with( null_, ' ' )		==	0 );
			static_assert( starts_with( str, 'a' )			==	1 );
			static_assert( starts_with( str, 'b' )			==	0 );

			static_assert( starts_with( ""            ,	Newline{} )	==	0 );
			static_assert( starts_with( "abcd\r\nefgh",	Newline{} )	==	0 );
			static_assert( starts_with( "\nabcdefgh"  ,	Newline{} )	==	1 );
			static_assert( starts_with( "\n\rabcdefgh",	Newline{} )	==	2 );
			static_assert( starts_with( "\rabcdefgh"  ,	Newline{} )	==	1 );
			static_assert( starts_with( "\r\nabcdefgh",	Newline{} )	==	2 );
		}
		{	// ends_with
			static_assert( ends_with( "abcdef", "def" )					==	3 );
			static_assert( ends_with( "abcdef", "deg" )					==	0 );
			static_assert( ends_with( str, ""    )						==	0 );
			static_assert( ends_with( null_, "jkl" )					==	0 );
			static_assert( ends_with( "abcdefghijkl", last( str, 3 ) )	==	3 );
			static_assert( ends_with( str, "jkl" )						==	3 );
			static_assert( ends_with( str, "ijk" )						==	0 );
			static_assert( ends_with( null_, ' ' )						==	0 );
			static_assert( ends_with( str, 'l' )						==	1 );
			static_assert( ends_with( str, 'k' )						==	0 );

			static_assert( ends_with( ""            ,	Newline{} )		==	0 );
			static_assert( ends_with( "abcd\r\nefgh",	Newline{} )		==	0 );
			static_assert( ends_with( "abcdefgh\n"  ,	Newline{} )		==	1 );
			static_assert( ends_with( "abcdefgh\n\r",	Newline{} )		==	2 );
			static_assert( ends_with( "abcdefgh\r"  ,	Newline{} )		==	1 );
			static_assert( ends_with( "abcdefgh\r\n",	Newline{} )		==	2 );
		}
		{	// split_at, spolit_by
			{	// at index
				{
					constexpr auto div = split_at( "abcdefghijkl", 24 );
					static_assert( div.first	==	"abcdefghijkl" );
					static_assert( div.second	==	"" );
				} {
					constexpr auto div = split_at( "abcdefghijkl", 12 );
					static_assert( div.first 	==	"abcdefghijkl" );
					static_assert( div.second	==	"" );
				} {
					constexpr auto div = split_at( "abcdefghijkl", 4 );
					static_assert( div.first 	==	"abcd" );
					static_assert( div.second	==	"fghijkl" );
				} {
					constexpr auto div = split_at( abc, 0 );
					static_assert( div.first	==	first( abc, 0 ) );
					static_assert( div.second	==	not_first( abc, 1 ) );
				} {
					constexpr auto div = split_at( abc, 2 );
					static_assert( div.first	==	first( abc, 2 ) );
					static_assert( div.second	==	not_first( abc, 3 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() - 1 );
					static_assert( div.first	==	first( abc, abc.size() - 1 ) );
					static_assert( div.second	==	last( abc, 0 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() );
					static_assert( div.first	==	abc );
					static_assert( div.second	==	last( abc, 0 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() + 1 );
					static_assert( div.first	==	abc );
					static_assert( div.second	==	last( abc, 0 ) );
				}
			}
			{	// by value
				constexpr auto base = subview( abc, 3, 4 );
				static_assert( base				==	"defg" );
				{
					constexpr auto div = split_by( "abcdefghijkl", 'e' );
					static_assert( div.first 	==	"abcd" );
					static_assert( div.second	==	"fghijkl" );
				} {
					constexpr auto div = split_by( base, 'a' );					// None
					static_assert( div.first	==	base );
					static_assert( div.second	==	last( base, 0 ) );
				} {
					constexpr auto div = split_by( base, 'd' );					// First
					static_assert( div.first	==	first( base, 0 ) );
					static_assert( div.second	==	not_first( base, 1 ) );
				} {
					constexpr auto div = split_by( base, 'f' );					// Middle
					static_assert( div.first	==	first( base, 2 ) );
					static_assert( div.second	==	not_first( base, 3 ) );
				} {
					constexpr auto div = split_by( base, 'g' );					// Last
					static_assert( div.first	==	first( base, 3 ) );
					static_assert( div.second	==	not_first( base, 4 ) );
				}
			}
			{	// by string_view
				constexpr auto base = subview( abc, 3, 4 );
				static_assert( base			==	"defg" );
				{
					constexpr auto div = split_by( "defg", "ef" );
					static_assert( div.first 	==	"d" );
					static_assert( div.second	==	"g" );
				} {
					constexpr auto div = split_by( str, first( str, 2 ) );		// First
					static_assert( find( str, first( str, 2 ) )	==	0 );
					static_assert( div.first			==	first( str,  0 ) );
					static_assert( div.second			==	last ( str, 10 ) );
					static_assert( div.first.data() 	==	std::string_view( str ).data() );
				} {
					constexpr auto div = split_by( str, subview( str, 4, 2 ) );	// Middle
					static_assert( div.first			==	first( str,  4 ) );
					static_assert( div.second			==	last ( str,  6 ) );
				} {
					constexpr auto div = split_by( str, last( str, 2 ) );		// End
					static_assert( div.first			==	first( str, 10 ) );
					static_assert( div.second			==	last ( str,  0 ) );
					static_assert ( div.second.data()	==	std::string_view( str ).data() + std::string_view( str ).size() );
				} {
					constexpr auto div = split_by( str, "aaa" );				// None
					static_assert( div.first			==	str );
					static_assert( div.second			==	last( str, 0 ) );
				}
			}
			{	// by Newline
				// None
				{
					constexpr auto div = split_by( std::string_view( "bcdefgh" ), Newline{} );
					static_assert( div.first 		==	"bcdefgh" );
					static_assert( div.second		==	"" );
				} {
					constexpr auto div = split_by( std::string_view( "" ), Newline{} );
					static_assert( div.first		==	"" );
					static_assert( div.second		==	"" );
				}

				{	// Beginning
					{
						constexpr auto div = split_by( "\nbcdefgh", Newline{} );
						static_assert( div.first 	==	"" );
						static_assert( div.second	==	"bcdefgh" );
					} {
						constexpr auto div = split_by( "\n\rbcdefgh", Newline{} );
						static_assert( div.first	==	"" );
						static_assert( div.second	==	"bcdefgh" );
					} {
						constexpr auto div = split_by( "\rbcdefgh", Newline{} );
						static_assert( div.first	==	"" );
						static_assert( div.second	==	"bcdefgh" );
					} {
						constexpr auto div = split_by( "\r\nbcdefgh", Newline{} );
						static_assert( div.first	==	"" );
						static_assert( div.second	==	"bcdefgh" );
					}
				}
				{	// Middle
					{
						constexpr auto div = split_by( "bcd\nefgh", Newline{} );
						static_assert( div.first 	==	"bcd" );
						static_assert( div.second	==	"efgh" );
					} {
						constexpr auto div = split_by( "bcd\n\refgh", Newline{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.second	==	"efgh" );
					} {
						constexpr auto div = split_by( "bcd\refgh", Newline{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.second	==	"efgh" );
					} {
						constexpr auto div = split_by( "bcd\r\nefgh", Newline{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.second	==	"efgh" );
					}
				}
				{	// Ending
					{
						constexpr auto div = split_by( "bcdefgh\n", Newline{} );
						static_assert( div.first 	==	"bcdefgh" );
						static_assert( div.second	==	"" );
					} {
						constexpr auto div = split_by( "bcdefgh\n\r", Newline{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.second	==	"" );
					} {
						constexpr auto div = split_by( "bcdefgh\r", Newline{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.second	==	"" );
					} {
						constexpr auto div = split_by( "bcdefgh\r\n", Newline{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.second	==	"" );
					}
				}
			}
		}
		{	// trim
			constexpr auto					text = "++++abcdef++";
			constexpr auto					pred = []( char c ) { return c <= 'b'; };
			constexpr auto 					is_plus = []( auto x_ ){	return x_ == '+'; };

			{	// trim_front
				static_assert( trim_front( "++++abcdef++", '+' )==	"+++abcdef++" );
				static_assert( trim_front( text, '+' )			==	not_first( text, 1 ) );
				static_assert( trim_front( text, '-' )			==	text );
			}
			{	// trim_back
				static_assert( trim_back( "++++abcdef++", '+' )	==	"++++abcdef+" );
				static_assert( trim_back( text, '+' )			==	not_last( text, 1 ) );
				static_assert( trim_back( text, '-' )			==	text );
			}
			{	// trim_first
				{	// traits::character
					static_assert( trim_first( "abc", '+' )		==	"abc" );
					static_assert( trim_first( text, '+' )		==	not_first( text, 4 ) );
				}
				{	// Predicate
					static_assert( trim_first( text, is_plus )	==	not_first( text, 4 ) );
					static_assert( trim_first( text, pred )		==	not_first( text, 6 ) );
				}
				{	// Newline
					static_assert( trim_first( "",					Newline{} )	==	"" );
					static_assert( trim_first( "\n\r",				Newline{} )	==	"" );
					static_assert( trim_first( "abcdefgh\n\r",		Newline{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\nabcdefgh\n\r",	Newline{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\n\rabcdefgh\n\r",	Newline{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\rabcdefgh\n\r",	Newline{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\r\nabcdefgh\n\r",	Newline{} )	==	"abcdefgh\n\r" );
				}
			}
			{	// trim_last
				{	// traits::character
					static_assert( trim_last( "abc", '+' )		==	"abc" );
					static_assert( trim_last( text, '+' )		==	not_last( text, 2 ) );
				}
				{	// Predicate
					static_assert( trim_last( text, is_plus )	==	not_last( text, 2 ) );
					static_assert( trim_last( text, pred )		==	not_last( text, 2 ) );
				}
				{	// Newline
					static_assert( trim_last( "",					Newline{} )	==	"" );
					static_assert( trim_last( "\n\r",				Newline{} )	==	"" );
					static_assert( trim_last( "\n\rabcdefgh",		Newline{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\n",		Newline{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\n\r",	Newline{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\r",		Newline{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\r\n",	Newline{} )	==	"\n\rabcdefgh" );
				}
			}
			{	// trim
				static_assert( trim( text, is_plus )			==	subview( text, 4, 6 ) );
				static_assert( trim( text, pred )				==	subview( text, 6, 4 ) );
			}
		}
	}
	DOCTEST_TEST_CASE( "other..." ) {
		constexpr auto						abd = "abddefffijkl";

		{	// sort
			std::vector< int >				v{ 0, 1, 2, 33, 4, 5, 6, 7, 8, 9, 10, 11 };
			DOCTEST_FAST_CHECK_EQ( v[  3 ], 33 );
			sort( v );
			DOCTEST_FAST_CHECK_EQ( v[  3 ],  4 );
			DOCTEST_FAST_CHECK_EQ( v[ 11 ], 33 );
		}
		{	// all_of
			static_assert(  all_of ( "abcdefghijkl", []( auto c ){ return c != '\0'; } ) );
			static_assert(  all_of ( str, []( auto c ){ return c >= 'a'; } ) );
			static_assert( !all_of ( str, []( auto c ){ return c >= 'd'; } ) );
			static_assert(  pax::all_of ( str, "abcdefghijkl", []( auto c, auto d ){ return c == d; } ) );
			static_assert(  pax::all_of ( str, str, []( auto c, auto d ){ return c == d; } ) );
			static_assert( !all_of ( str, abd, []( auto c, auto d ){ return c == d; } ) );
		}
		{	// any_of
			static_assert( !any_of ( "abcdefghijkl", []( auto c ){ return c == '\0'; } ) );
			static_assert(  any_of ( str, []( auto c ){ return c >= 'a'; } ) );
			static_assert( !any_of ( str, []( auto c ){ return c == 'x'; } ) );
			static_assert(  pax::any_of ( str, "abcdefghijkl", []( auto c, auto d ){ return c == d; } ) );
			static_assert(  pax::any_of ( str, str, []( auto c, auto d ){ return c == d; } ) );
			static_assert(  any_of ( str, abd, []( auto c, auto d ){ return c != d; } ) );
			static_assert( !pax::any_of ( str, str, []( auto c, auto d ){ return c != d; } ) );
		}
		{	// none_of
			static_assert(  none_of( "abcdefghijkl", []( auto c ){ return c == '\0'; } ) );
			static_assert(  none_of( str, []( auto c ){ return c == 'x'; } ) );
			static_assert( !none_of( str, []( auto c ){ return c == 'd'; } ) );
			static_assert(  pax::none_of ( "abcdefghijkl", str, []( auto c, auto d ){ return c != d; } ) );
			static_assert(  pax::none_of ( str, str, []( auto c, auto d ){ return c != d; } ) );
			static_assert( !none_of ( str, abd, []( auto c, auto d ){ return c != d; } ) );
		}
		{	// identify_newline
			static_assert( identify_newline( "abcd\n\refgh"   )	==	"\n\r" );
			static_assert( identify_newline( ""               )	==	"\n" );
			static_assert( identify_newline( "abcdefgh"       )	==	"\n" );

			static_assert( identify_newline( "\nabcdefgh"     )	==	"\n" );
			static_assert( identify_newline( "\n\r\nabcdefgh" )	==	"\n\r" );
			static_assert( identify_newline( "\rabcdefgh"     )	==	"\r" );
			static_assert( identify_newline( "\r\nabcdefgh"   )	==	"\r\n" );

			static_assert( identify_newline( "abcd\nefgh"     )	==	"\n" );
			static_assert( identify_newline( "abcd\n\refgh"   )	==	"\n\r" );
			static_assert( identify_newline( "abcd\refgh"     )	==	"\r" );
			static_assert( identify_newline( "abcd\r\nefgh"   )	==	"\r\n" );

			static_assert( identify_newline( "abcdefgh\n"     )	==	"\n" );
			static_assert( identify_newline( "abcdefgh\n\r"   )	==	"\n\r" );
			static_assert( identify_newline( "abcdefgh\r"     )	==	"\r" );
			static_assert( identify_newline( "abcdefgh\r\n"   )	==	"\r\n" );
		}
		{	// luhn_sum
			static_assert( luhn_sum( "6112161457" )	==	30 );
			static_assert( luhn_sum( "6212161457" )	==	31 );
			static_assert( luhn_sum( "7112161457" )	==	32 );
		}
	}
	DOCTEST_TEST_CASE( "std::span in general" ) {
		{	// comparisons
			const auto					abc2{ "abcdffghijkl" };

			DOCTEST_FAST_CHECK_UNARY( std::strong_ordering::less	==	not_last( abc ) <=> abc );
			DOCTEST_FAST_CHECK_UNARY( std::strong_ordering::less	==	( abc  <=> abc2 ) );
			DOCTEST_FAST_CHECK_UNARY( std::strong_ordering::equal	==	( abc  <=> abc  ) );
			DOCTEST_FAST_CHECK_UNARY( std::strong_ordering::greater	==	( abc2 <=> abc  ) );
			DOCTEST_FAST_CHECK_UNARY( std::strong_ordering::greater	==	( abc  <=> not_last( abc ) ) );

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
					std::print( os, "{}", std::span< int >{} );
					DOCTEST_FAST_CHECK_EQ( os.str(), "[]" );
					DOCTEST_FAST_CHECK_EQ( os.str().size(), 2 );
				} {
					const int				hej[ 3 ] = { 0, 6, 7 };
					std::ostringstream		os;
					std::print( os, "{}", std::span( hej ) );
					DOCTEST_FAST_CHECK_EQ( os.str(), "[0, 6, 7]" );
					DOCTEST_FAST_CHECK_EQ( os.str().size(), 9 );
				}
			}
			{	// with text
				std::ostringstream		os;
				std::print( os, "{}", std::span( "abcd" ).first( 3 ) );
				DOCTEST_FAST_CHECK_EQ( os.str().size(), 15 );
				DOCTEST_FAST_CHECK_EQ( os.str(), "['a', 'b', 'c']" );
			}
			{	// with texts
				std::ostringstream		os;
				const char *			strings[ 5 ] = { "Hej", " ", "hela", " ", "varlden" };
				std::print( os, "{}", std::span( strings ) );
				DOCTEST_FAST_CHECK_EQ( os.str().size(), 36 );
				DOCTEST_FAST_CHECK_EQ( os.str(), "[\"Hej\", \" \", \"hela\", \" \", \"varlden\"]" );
			}
		}
	}
	DOCTEST_TEST_CASE( "String_view_splitter" ) { 
		// for( const auto s : String_view_splitter( "ett två tre", ' ' ) )	Debug{} << s;

		constexpr String_view_splitter	split( "ett två tre", ' ' );
		auto								itr = split.begin();
		DOCTEST_FAST_CHECK_EQ( *    itr,  	"ett" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ),	"två" );
		DOCTEST_FAST_CHECK_EQ( *( ++itr ),	"tre" );
		DOCTEST_FAST_CHECK_NE(   itr,		split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr,		split.end() );
		DOCTEST_FAST_CHECK_EQ( ++itr,		split.end() );
	}

}	// namespace pax
