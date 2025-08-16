//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/span.hpp>
#include <pax/doctest.hpp>

#include <sstream>
#include <array>
#include <vector>


namespace pax {

	constexpr const int							ints0[] = { 0, 1, 2 };
	constexpr double							doubles0[] = { 0, 1, 2 };

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
		{	// std::format output
			{
				constexpr std::span< const int >		ints( ints0 );

				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( ints, 0 ) ) ),		"[]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( ints, 1 ) ) ),		"[0]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( ints, 3 ) ) ),		"[0, 1, 2]" );
			}
			{
				constexpr std::span< const double >		doubles( doubles0 );

				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( doubles, 0 ) ) ),		"[]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( doubles, 1 ) ) ),		"[0]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( doubles, 3 ) ) ),		"[0, 1, 2]" );

				DOCTEST_FAST_CHECK_EQ( std20::format( "{:.3f}", 3.14 ),	"3.140" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{:.3f}", span2( first( doubles, 0 ) ) ),	"[]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{:.3f}", span2( first( doubles, 1 ) ) ),	"[0.000]" );
				DOCTEST_FAST_CHECK_EQ( std20::format( "{:.3f}", span2( first( doubles, 3 ) ) ),	"[0.000, 1.000, 2.000]" );
			}
			{
				constexpr char const	  * strs[] = { "abc", "def", "ghi" };
				DOCTEST_FAST_CHECK_EQ( std20::format( "{}", span2( first( strs, 3 ) ) ),		"[\"abc\", \"def\", \"ghi\"]" );
			}
		}
	}

}	// namespace pax
