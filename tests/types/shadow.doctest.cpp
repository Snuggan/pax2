//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/types/shadow.hpp>
#include <pax/std/string_view.hpp>
#include <pax/doctest.hpp>
#include <string>
#include <array>
#include <vector>
#include <span>
#include <print>


namespace pax { 
	
	DOCTEST_TEST_CASE( "concepts zero_suffix" ) {
		using std::data, std::size;
		{
			const char			str[] = "Hej!";
			DOCTEST_FAST_CHECK_EQ( size( str ), 5 );
			DOCTEST_FAST_CHECK_EQ( no_nullchar_end( str ) - str, 4 );
		} {
			const auto			str   = "Hej!";
			DOCTEST_FAST_CHECK_EQ( size( str ), 4 );
			DOCTEST_FAST_CHECK_EQ( no_nullchar_end( str ) - str, 4 );
		}
	}

	template< auto V >	struct litteral_test	{	static constexpr decltype( V ) v = V;	};

	template< typename T >
	concept is_value_const = std::is_const_v< std::remove_reference_t< traits::element_type_t< T > > >;
	
	template< typename Sh >
	void text_test( const Sh & sh_ ) {
		static_assert( std::ranges::contiguous_range< Sh > );

		DOCTEST_FAST_CHECK_EQ ( std::format( "{}", sh_ ), "['t', 'e', 'x', 't']" );
		DOCTEST_FAST_CHECK_EQ ( std::format( "{:s}", sh_ ), "text" );

		DOCTEST_ASCII_CHECK_EQ( sh_					, "text" );
		DOCTEST_FAST_CHECK_EQ( sh_.size()			, 4u );
		DOCTEST_FAST_CHECK_GT( sh_					, "test" );
		DOCTEST_FAST_CHECK_GE( sh_					, "test" );
		DOCTEST_ASCII_CHECK_EQ( sh_[ 3 ]			, 't' );
		DOCTEST_ASCII_CHECK_EQ( *( sh_.begin() + 1 ) , 'e' );
		DOCTEST_ASCII_CHECK_EQ( *( sh_.end()   - 2 ) , 'x' );
		DOCTEST_ASCII_CHECK_EQ( sh_.front() 		, 't' );
		DOCTEST_ASCII_CHECK_EQ( sh_.back() 			, 't' );
		DOCTEST_ASCII_CHECK_EQ( sh_.first( 2 )		, "te" );
		DOCTEST_ASCII_CHECK_EQ( sh_.first( 9 )		, "text" );
		DOCTEST_ASCII_CHECK_EQ( sh_.not_first( 2 )	, "xt" );
		DOCTEST_ASCII_CHECK_EQ( sh_.not_first( 9 )	, "" );
		DOCTEST_ASCII_CHECK_EQ( sh_.last( 2 )		, "xt" );
		DOCTEST_ASCII_CHECK_EQ( sh_.last( 9 )		, "text" );
		DOCTEST_ASCII_CHECK_EQ( sh_.not_last( 2 )	, "te" );
		DOCTEST_ASCII_CHECK_EQ( sh_.not_last( 9 )	, "" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part(  1, 2 )	, "ex" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part(  9, 2 )	, "" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part(  1, 9 )	, "ext" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part( -3, 2 )	, "ex" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part( -9, 2 )	, "te" );
		DOCTEST_ASCII_CHECK_EQ( sh_.part( -3, 9 )	, "ext" );
		DOCTEST_FAST_CHECK_UNARY( sh_.starts_with( "tex" ) );
		DOCTEST_FAST_CHECK_UNARY( sh_.ends_with( "ext" ) );
		DOCTEST_FAST_CHECK_EQ( sh_.find( 'x'  ).begin() - sh_.begin(), 2 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( '8'  ).begin() - sh_.begin(), 4 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( "ex" ).begin() - sh_.begin(), 1 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( "ax" ).begin() - sh_.begin(), 4 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( 'x'  ).size(), 1 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( '8'  ).size(), 0 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( "ex" ).size(), 2 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( "ax" ).size(), 0 );
		{
			const auto res = sh_.template first< 2 >();
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_FAST_CHECK_EQ( res, "te" );
			DOCTEST_ASCII_CHECK_EQ( res, "te" );
		} {
			const auto res = sh_.template last< 2 >();
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_ASCII_CHECK_EQ( res, "xt" );
		} {
			const auto res = sh_.template part< 2 >( 1 );
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_ASCII_CHECK_EQ( res, "ex" );
		} {
			const auto res = sh_.template part< 2 >( -3 );
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_ASCII_CHECK_EQ( res, "ex" );
		} {
			{
				const auto res = sh_.split( 0 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const auto res = sh_.split( 3 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const auto res = sh_.split( sh_.size() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			}
		} {
			{
				const shadow	gap{ sh_.begin() - 3u, sh_.begin() - 1u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() - 3u, sh_.end() + 2u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() - 3u, sh_.begin() + 2u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 2u );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() + 1u, sh_.begin() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 1u );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 3u );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() + 1u, sh_.end() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 1u );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.end() + 1u, sh_.end() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			}
		}

		if constexpr( traits::has_extent< Sh > ) {
			DOCTEST_ASCII_CHECK_EQ( get< 2 >( sh_ )	, sh_[ 2 ] );
			{
				const auto res = sh_.template first< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 4 );
				DOCTEST_ASCII_CHECK_EQ( res, "text" );
			} {
				const auto res = sh_.template not_first< 2 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
				DOCTEST_ASCII_CHECK_EQ( res, "xt" );
			} {
				const auto res = sh_.template not_first< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 0 );
				DOCTEST_ASCII_CHECK_EQ( res, "" );
			} {
				const auto res = sh_.template last< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 4 );
				DOCTEST_ASCII_CHECK_EQ( res, "text" );
			} {
				const auto res = sh_.template not_last< 2 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
				DOCTEST_ASCII_CHECK_EQ( res, "te" );
			} {
				const auto res = sh_.template not_last< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 0 );
				DOCTEST_ASCII_CHECK_EQ( res, "" );
			}
		}
	}
	
	template< typename Sh >
	void num_test( const Sh & sh_ ) {
		static_assert( std::ranges::contiguous_range< Sh > );

		static constexpr int						  nums[] = { 0, 1, 2, 3, 4, 5, 0 };
		static constexpr std::span< int >			  empty{};

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
		DOCTEST_FAST_CHECK_EQ( sh_.find( 3  ).begin() - sh_.begin(), 3 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( 9  ).begin() - sh_.begin(), sh_.size() );
		DOCTEST_FAST_CHECK_EQ( sh_.find( sh_.part( 2, 3 ) ).begin() - sh_.begin(), 2 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( std::array{ 1, 3, 4 } ).begin() - sh_.begin(), sh_.size() );
		DOCTEST_FAST_CHECK_EQ( sh_.find( 3  ).size(), 1 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( 9  ).size(), 0 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( sh_.part( 2, 3 ) ).size(), 3 );
		DOCTEST_FAST_CHECK_EQ( sh_.find( std::array{ 1, 3, 4 } ).size(), 0 );
		{
			const auto res = sh_.template first< 2 >();
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_FAST_CHECK_EQ( res, std::array{ 0, 1 } );
		} {
			const auto res = sh_.template last< 2 >();
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_FAST_CHECK_EQ( res, std::array{ 5, 0 } );
		} {
			const auto res = sh_.template part< 2 >( 1 );
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_FAST_CHECK_EQ( res, std::array{ 1, 2 } );
		} {
			const auto res = sh_.template part< 2 >( -6 );
			DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 2 );
			DOCTEST_FAST_CHECK_EQ( res, std::array{ 1, 2 } );
		} {
			{
				const auto res = sh_.split( 0 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const auto res = sh_.split( 3 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const auto res = sh_.split( sh_.size() + 3 );
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			}
		} {
			{
				const shadow	gap{ sh_.begin() - 3u, sh_.begin() - 1u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() - 3u, sh_.end() + 2u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() - 3u, sh_.begin() + 2u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 2u );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() + 1u, sh_.begin() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 1u );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.begin() + 3u );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.begin() + 1u, sh_.end() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.begin() + 1u );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			} {
				const shadow	gap{ sh_.end() + 1u, sh_.end() + 3u };
				const auto		res{ sh_.split( gap ) };
				DOCTEST_FAST_CHECK_EQ( res.first.begin(),  sh_.begin() );
				DOCTEST_FAST_CHECK_EQ( res.first.end(),    sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.begin(), sh_.end() );
				DOCTEST_FAST_CHECK_EQ( res.second.end(),   sh_.end() );				
			}
		}
		if constexpr( traits::has_extent< Sh > ) {
			DOCTEST_FAST_CHECK_EQ( get< 2 >( sh_ )	, sh_[ 2 ] );
			{
				const auto res = sh_.template first< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 7 );
				DOCTEST_FAST_CHECK_EQ( res, nums );
			} {
				const auto res = sh_.template not_first< 2 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 5 );
				DOCTEST_FAST_CHECK_EQ( res, std::array{ 2, 3, 4, 5, 0 } );
			} {
				const auto res = sh_.template not_first< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 0 );
				DOCTEST_FAST_CHECK_EQ( res, empty );
			} {
				const auto res = sh_.template last< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 7 );
				DOCTEST_FAST_CHECK_EQ( res, nums );
			} {
				const auto res = sh_.template not_last< 2 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 5 );
				DOCTEST_FAST_CHECK_EQ( res, std::array{ 0, 1, 2, 3, 4 } );
			} {
				const auto res = sh_.template not_last< 9 >();
				DOCTEST_FAST_CHECK_EQ( traits::extent_v< decltype( res ) >, 0 );
				DOCTEST_FAST_CHECK_EQ( res, empty );
			}
		}
	}

	DOCTEST_TEST_CASE( "shadow text static size" ) {
		static_assert( shadow( "text" ).last( 2 ) == "xt" );
		DOCTEST_FAST_CHECK_EQ( std::format( "{:?s}", shadow( "1\t2\n3\"4" ) ), "\"1\\t2\\n3\\\"4\"" );
		text_test( shadow( "text" ) );
		{
			const auto sh	  = shadow( "first\nsecond\rthird\n\rfourth\r\nfifth\n\nsixth" );
			auto res		  = sh.split( sh.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "first" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "second" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_ASCII_CHECK_EQ( res.first, "third" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "fourth" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "fifth" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "" );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_EQ( res.first, "sixth" );
			DOCTEST_FAST_CHECK_UNARY( res.second.empty() );
			res				  = res.second.split( res.second.find_line() );
			DOCTEST_FAST_CHECK_UNARY( res.first.empty() );
			DOCTEST_FAST_CHECK_UNARY( res.second.empty() );
		}
	}
	DOCTEST_TEST_CASE( "shadow text dynamic size" ) {
		const std::string		str0{ "1\t2\n3\"4" };
		DOCTEST_FAST_CHECK_EQ( std::format( "{:?s}", shadow( str0 ) ), "\"1\\t2\\n3\\\"4\"" );
		std::string				str{ "text" };
		text_test( shadow( str ) );
		const auto sh = shadow( str );
		sh[ 2 ]	= 'a';
		DOCTEST_FAST_CHECK_EQ( str[ 2 ], 'a' );
	}
	DOCTEST_TEST_CASE( "shadow numbers static size" ) {
		static constexpr std::array			nums0{ 0, 1, 2, 3, 4, 5, 0 };
		static_assert( shadow( nums0 ).last( 2 ) == std::array{ 5, 0 } );
		std::array							nums{ nums0 };
		num_test( shadow( nums ) );
		const auto sh = shadow( nums );
		sh[ 2 ]	= 99;
		DOCTEST_FAST_CHECK_EQ( nums[ 2 ], 99 );
	}
	DOCTEST_TEST_CASE( "shadow numbers dynamic size" ) {
		static constexpr std::array			nums0{ 0, 1, 2, 3, 4, 5, 0 };
		std::vector< int >					nums{ nums0.begin(), nums0.end() };

		static_assert( !is_value_const < decltype( shadow( nums ) ) > );
		static_assert( !std::is_const_v< std::remove_reference_t< decltype( shadow( nums ).front() ) > > );

		num_test( shadow( nums ) );
	}
	DOCTEST_TEST_CASE( "const_shadow" ) {
		static constexpr std::array			nums0{ 0, 1, 2, 3, 4, 5, 0 };
		std::vector< int >					nums{ nums0.begin(), nums0.end() };

		static_assert(  is_value_const < decltype( const_shadow( nums ) ) > );
		static_assert(  std::is_const_v< std::remove_reference_t< decltype( const_shadow( nums ).front() ) > > );

		num_test( const_shadow( nums ) );
	}
	DOCTEST_TEST_CASE( "litteral text" ) {
		DOCTEST_ASCII_CHECK_EQ( litt( "text" ).last( 2 ), "xt" );
		DOCTEST_ASCII_CHECK_EQ( std::format( "{:?s}", shadow( "1\t2\n3\"4" ) ), "\"1\\t2\\n3\\\"4\"" );
		text_test( litt( "text" ) );
	}
}
