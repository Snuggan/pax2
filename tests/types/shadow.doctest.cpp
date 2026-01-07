//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/shadow.hpp>
#include <pax/doctest.hpp>
#include <span>
#include <print>


namespace pax {

	template< auto V >	struct litteral_test	{	static constexpr decltype( V ) v = V;	};
	
	template< typename Sh >
	void text_test( const Sh & sh_, const char * const name_ ) {
		static_assert( std::ranges::contiguous_range< Sh > );

		std::cout << "---- Text:    " << name_ << " -------------------------\n";
		DOCTEST_FAST_CHECK_EQ ( std::format( "{}", sh_ ), "['t', 'e', 'x', 't']" );
		DOCTEST_FAST_CHECK_EQ ( std::format( "{:s}", sh_ ), "text" );

		DOCTEST_FAST_CHECK_EQ( sh_					, "text" );
		DOCTEST_FAST_CHECK_EQ( sh_.size()			, 4u );
		DOCTEST_FAST_CHECK_GT( sh_					, "test" );
		DOCTEST_FAST_CHECK_GE( sh_					, "test" );
		DOCTEST_FAST_CHECK_EQ( sh_[ 3 ]				, 't' );
		DOCTEST_FAST_CHECK_EQ( *( sh_.begin() + 1 ) , 'e' );
		DOCTEST_FAST_CHECK_EQ( *( sh_.end()   - 2 ) , 'x' );
		DOCTEST_FAST_CHECK_EQ( sh_.front() 			, 't' );
		DOCTEST_FAST_CHECK_EQ( sh_.back() 			, 't' );
		DOCTEST_FAST_CHECK_EQ( sh_.first( 2 )		, "te" );
		DOCTEST_FAST_CHECK_EQ( sh_.first( 9 )		, "text" );
		DOCTEST_FAST_CHECK_EQ( sh_.not_first( 2 )	, "xt" );
		DOCTEST_FAST_CHECK_EQ( sh_.not_first( 9 )	, "" );
		DOCTEST_FAST_CHECK_EQ( sh_.last( 2 )		, "xt" );
		DOCTEST_FAST_CHECK_EQ( sh_.last( 9 )		, "text" );
		DOCTEST_FAST_CHECK_EQ( sh_.not_last( 2 )	, "te" );
		DOCTEST_FAST_CHECK_EQ( sh_.not_last( 9 )	, "" );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  1, 2 )	, "ex" );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  9, 2 )	, "" );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  1, 9 )	, "ext" );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -3, 2 )	, "ex" );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -9, 2 )	, "te" );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -3, 9 )	, "ext" );
		DOCTEST_FAST_CHECK_UNARY( sh_.starts_with( "tex" ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.ends_with( "ext" ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.contains( 'x' ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.contains( "xt" ) );
	}
	
	template< typename Sh >
	void num_test( const Sh & sh_, const char * const name_ ) {
		static_assert( std::ranges::contiguous_range< Sh > );

		static constexpr int						  nums[] = { 0, 1, 2, 3, 4, 5, 0 };
		static constexpr std::span< int >			  empty{};

		std::cout << "---- Numbers: " << name_ << " -------------------------\n";
		DOCTEST_FAST_CHECK_EQ ( std::format( "{}", sh_ ), "[0, 1, 2, 3, 4, 5, 0]" );
		DOCTEST_FAST_CHECK_EQ ( std::format( "{::4}", sh_ ), "[   0,    1,    2,    3,    4,    5,    0]" );

		DOCTEST_FAST_CHECK_EQ( sh_					, nums );
		DOCTEST_FAST_CHECK_EQ( sh_.size()			, 7u );
		DOCTEST_FAST_CHECK_GT( sh_					, std::array{ 0, 1, 2, 3, 3, 5, 0 } );
		DOCTEST_FAST_CHECK_GE( sh_					, std::array{ 0, 1, 2, 3, 3, 5, 0 } );
		DOCTEST_FAST_CHECK_EQ( sh_[ 3 ]				, 3 );
		DOCTEST_FAST_CHECK_EQ( *( sh_.begin() + 1 ) , 1 );
		DOCTEST_FAST_CHECK_EQ( *( sh_.end()   - 2 ) , 5 );
		DOCTEST_FAST_CHECK_EQ( sh_.front() 			, 0 );
		DOCTEST_FAST_CHECK_EQ( sh_.back() 			, 0 );
		DOCTEST_FAST_CHECK_EQ( sh_.first( 2 )		, std::array{ 0, 1 } );
		DOCTEST_FAST_CHECK_EQ( sh_.first( 9 )		, nums );
		DOCTEST_FAST_CHECK_EQ( sh_.not_first( 2 )	, std::array{ 2, 3, 4, 5, 0 } );
		DOCTEST_FAST_CHECK_EQ( sh_.not_first( 9 )	, empty );
		DOCTEST_FAST_CHECK_EQ( sh_.last( 2 )		, std::array{ 5, 0 } );
		DOCTEST_FAST_CHECK_EQ( sh_.last( 9 )		, nums );
		DOCTEST_FAST_CHECK_EQ( sh_.not_last( 2 )	, std::array{ 0, 1, 2, 3, 4 } );
		DOCTEST_FAST_CHECK_EQ( sh_.not_last( 9 )	, empty );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  1, 2 )	, std::array{ 1, 2 } );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  9, 2 )	, empty );
		DOCTEST_FAST_CHECK_EQ( sh_.part(  1, 9 )	, std::array{ 1, 2, 3, 4, 5, 0 } );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -3, 2 )	, std::array{ 4, 5 } );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -9, 2 )	, std::array{ 0, 1 } );
		DOCTEST_FAST_CHECK_EQ( sh_.part( -3, 9 )	, std::array{ 4, 5, 0 } );
		DOCTEST_FAST_CHECK_UNARY( sh_.starts_with( std::array{ 0, 1, 2 } ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.ends_with( std::array{ 4, 5, 0 } ) );
#if defined( __clang__ )	// Not constexpr in gcc:
		DOCTEST_FAST_CHECK_UNARY( sh_.contains( 5 ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.contains( std::array{ 2, 3, 4 } ) );
#endif
	}

	DOCTEST_TEST_CASE( "shadow text" ) {
		static_assert( shadow( "text" ).last( 2 ) == "xt" );
		DOCTEST_FAST_CHECK_EQ ( std::format( "{:?s}", shadow( "1\t2\n3\"4" ) ), "\"1\\t2\\n3\\\"4\"" );
		text_test( shadow( "text" ), "shadow( \"text\" )" );
	}
	DOCTEST_TEST_CASE( "shadow numbers" ) {
		static constexpr int				nums[] = { 0, 1, 2, 3, 4, 5, 0 };
		// static constexpr std::array			nums{ 0, 1, 2, 3, 4, 5, 0 };
		static_assert( shadow( nums ).last( 2 ) == std::array{ 5, 0 } );
		num_test( shadow( nums ), "shadow( nums )" );
	}
	DOCTEST_TEST_CASE( "litteral text" ) {
		static_assert( litt( "text" ).last( 2 ) == "xt" );
		DOCTEST_FAST_CHECK_EQ ( std::format( "{:?s}", shadow( "1\t2\n3\"4" ) ), "\"1\\t2\\n3\\\"4\"" );
		text_test( litt( "text" ), "litt( \"text\" )" );

		// Test template usage.
		static_assert( litteral_test< litt( "text" ) >::v	== "text" );
	}

}
