//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/std/ascii.hpp>
#include <pax/std/string_view.hpp>
#include <pax/std/span.hpp>
#include <pax/doctest.hpp>

#include <sstream>
#include <array>
#include <vector>
#include <format>


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


	/// Return true iff both data() and size() are equal between the two std::spans.
	template< traits::contiguous V0, traits::contiguous V1 >
	[[nodiscard]] constexpr bool identic(
		const V0						  & v0_, 
		const V1						  & v1_ 
	) noexcept {
		using std::data, std::size;
		return ( data( v0_ ) == data( v1_ ) ) && ( size( v0_ ) == size( v1_ ) );
	}




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
	constexpr auto safe_mid( T && t_, int i_, int sz_ ) {
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
			constexpr std::span				subc{ mid( str, 3, 3 ) };	// "def"
			static_assert( !overlap( std::string_view{}, std::string_view{} ) );
			static_assert(  overlap( str, str ) );
			static_assert(  overlap( subc, subc ) );
			static_assert(  overlap( subc, str ) );
			static_assert(  overlap( str, subc ) );
			static_assert( !overlap( str, mid( str, 3, 0 ) ) );			// An emptry span cannot overlap
			static_assert( !overlap( mid( str, 3, 0 ), str ) );			// An emptry span cannot overlap
			static_assert( !overlap( subc, first( str, 3 ) ) );				// Kloss i kloss
			static_assert(  overlap( subc, first( str, 4 ) ) );				// First overlapion
			static_assert(  overlap( subc, not_first( str, 4 ) ) );			// Middle overlapion
			static_assert(  overlap( subc, not_first( str, 5 ) ) );			// Last overlapion
			static_assert( !overlap( subc, not_first( str, 6 ) ) );			// Kloss i kloss
			static_assert( !overlap( subc, mid( str, 2, 0 ) ) );
			static_assert( !overlap( subc, mid( str, 3, 0 ) ) );
			static_assert( !overlap( subc, mid( str, 4, 0 ) ) );
			static_assert( !overlap( subc, mid( str, 5, 0 ) ) );
			static_assert( !overlap( subc, mid( str, 6, 0 ) ) );

			constexpr std::span				subi{ mid( ints, 3, 3 ) };	// { 3, 4, 5 }
			static_assert( !overlap( std::span< int >{}, std::span< int >{} ) );
			static_assert(  overlap( ints, ints ) );
			static_assert(  overlap( subi, subi ) );
			static_assert(  overlap( subi, ints ) );
			static_assert(  overlap( ints, subi ) );
			static_assert( !overlap( ints, mid( ints, 3, 0 ) ) );		// An emptry span cannot overlap
			static_assert( !overlap( mid( ints, 3, 0 ), ints ) );		// An emptry span cannot overlap
			static_assert( !overlap( subi, first( ints, 3 ) ) );			// Kloss i kloss
			static_assert(  overlap( subi, first( ints, 4 ) ) );			// First overlapion
			static_assert(  overlap( subi, not_first( ints, 4 ) ) );		// Middle overlapion
			static_assert(  overlap( subi, not_first( ints, 5 ) ) );		// Last overlapion
			static_assert( !overlap( subi, not_first( ints, 6 ) ) );		// Kloss i kloss
			static_assert( !overlap( subi, mid( ints, 2, 0 ) ) );
			static_assert( !overlap( subi, mid( ints, 3, 0 ) ) );
			static_assert( !overlap( subi, mid( ints, 4, 0 ) ) );
			static_assert( !overlap( subi, mid( ints, 5, 0 ) ) );
			static_assert( !overlap( subi, mid( ints, 6, 0 ) ) );
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

			static_assert( identic( not_first( str,   0 ), safe_mid( str,  0, 99 ) ) );
			static_assert( identic( not_first( str,   5 ), safe_mid( str,  5, 99 ) ) );
			static_assert( identic( not_first( str,  12 ), safe_mid( str, 12, 99 ) ) );
			static_assert( identic( not_first( str,  22 ), safe_mid( str, 22, 99 ) ) );

			static_assert( identic( not_first( strN,  0 ), safe_mid( strN,  0, 99 ) ) );
			static_assert( identic( not_first( strN,  5 ), safe_mid( strN,  5, 99 ) ) );
			static_assert( identic( not_first( strN, 12 ), safe_mid( strN, 12, 99 ) ) );
			static_assert( identic( not_first( strN, 22 ), safe_mid( strN, 22, 99 ) ) );

			static_assert( identic( not_first<  0 >( strN ), safe_mid( strN,  0, 99 ) ) );
			static_assert( identic( not_first<  5 >( strN ), safe_mid( strN,  5, 99 ) ) );
			static_assert( identic( not_first< 12 >( strN ), safe_mid( strN, 12, 99 ) ) );
			static_assert( identic( not_first< 22 >( strN ), safe_mid( strN, 22, 99 ) ) );
		}
		{	// not_first (int)
			static_assert( identic( not_first( ints,   0 ), safe_mid( ints,  0, 99 ) ) );
			static_assert( identic( not_first( ints,   5 ), safe_mid( ints,  5, 99 ) ) );
			static_assert( identic( not_first( ints,  12 ), safe_mid( ints, 12, 99 ) ) );
			static_assert( identic( not_first( ints,  22 ), safe_mid( ints, 22, 99 ) ) );

			static_assert( identic( not_first( intsN,  0 ), safe_mid( intsN,  0, 99 ) ) );
			static_assert( identic( not_first( intsN,  5 ), safe_mid( intsN,  5, 99 ) ) );
			static_assert( identic( not_first( intsN, 12 ), safe_mid( intsN, 12, 99 ) ) );
			static_assert( identic( not_first( intsN, 22 ), safe_mid( intsN, 22, 99 ) ) );

			static_assert( identic( not_first<  0 >( intsN ), safe_mid( intsN,  0, 99 ) ) );
			static_assert( identic( not_first<  5 >( intsN ), safe_mid( intsN,  5, 99 ) ) );
			static_assert( identic( not_first< 12 >( intsN ), safe_mid( intsN, 12, 99 ) ) );
			static_assert( identic( not_first< 22 >( intsN ), safe_mid( intsN, 22, 99 ) ) );
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
		{	// mid (character)
			static_assert( identic( mid( null_,   0,  0 ),	safe_mid( null_,   0,  0 ) ) );
			static_assert( identic( mid( null_,   0,  3 ),	safe_mid( null_,   0,  3 ) ) );
			static_assert( identic( mid( null_,   0, 12 ),	safe_mid( null_,   0, 12 ) ) );
			static_assert( identic( mid( null_,   0, 22 ),	safe_mid( null_,   0, 22 ) ) );
			static_assert( identic( mid( null_,   5,  0 ),	safe_mid( null_,   5,  0 ) ) );
			static_assert( identic( mid( null_,   5,  3 ),	safe_mid( null_,   5,  3 ) ) );
			static_assert( identic( mid( null_,   5, 12 ),	safe_mid( null_,   5, 12 ) ) );
			static_assert( identic( mid( null_,   5, 22 ),	safe_mid( null_,   5, 22 ) ) );
			static_assert( identic( mid( null_,  -5,  0 ),	safe_mid( null_,  -5,  0 ) ) );
			static_assert( identic( mid( null_,  -5,  3 ),	safe_mid( null_,  -5,  3 ) ) );
			static_assert( identic( mid( null_,  -5, 12 ),	safe_mid( null_,  -5, 12 ) ) );
			static_assert( identic( mid( null_,  -5, 22 ),	safe_mid( null_,  -5, 22 ) ) );
			static_assert( identic( mid( null_,  12,  0 ),	safe_mid( null_,  12,  0 ) ) );
			static_assert( identic( mid( null_,  12,  3 ),	safe_mid( null_,  12,  3 ) ) );
			static_assert( identic( mid( null_,  12, 12 ),	safe_mid( null_,  12, 12 ) ) );
			static_assert( identic( mid( null_,  12, 22 ),	safe_mid( null_,  12, 22 ) ) );
			static_assert( identic( mid( null_, -12,  0 ),	safe_mid( null_, -12,  0 ) ) );
			static_assert( identic( mid( null_, -12,  3 ),	safe_mid( null_, -12,  3 ) ) );
			static_assert( identic( mid( null_, -12, 12 ),	safe_mid( null_, -12, 12 ) ) );
			static_assert( identic( mid( null_, -12, 22 ),	safe_mid( null_, -12, 22 ) ) );
			static_assert( identic( mid( null_,  22,  0 ),	safe_mid( null_,  22,  0 ) ) );
			static_assert( identic( mid( null_,  22,  5 ),	safe_mid( null_,  22,  3 ) ) );
			static_assert( identic( mid( null_,  22, 12 ),	safe_mid( null_,  22, 12 ) ) );
			static_assert( identic( mid( null_,  22, 22 ),	safe_mid( null_,  22, 22 ) ) );
			static_assert( identic( mid( null_, -22,  0 ),	safe_mid( null_, -22,  0 ) ) );
			static_assert( identic( mid( null_, -22,  5 ),	safe_mid( null_, -22,  3 ) ) );
			static_assert( identic( mid( null_, -22, 12 ),	safe_mid( null_, -22, 12 ) ) );
			static_assert( identic( mid( null_, -22, 22 ),	safe_mid( null_, -22, 22 ) ) );

			static_assert( identic( mid( str,   0,  0 ),	safe_mid( str,   0,  0 ) ) );
			static_assert( identic( mid( str,   0,  3 ),	safe_mid( str,   0,  3 ) ) );
			static_assert( identic( mid( str,   0, 12 ),	safe_mid( str,   0, 12 ) ) );
			static_assert( identic( mid( str,   0, 22 ),	safe_mid( str,   0, 22 ) ) );
			static_assert( identic( mid( str,   5,  0 ),	safe_mid( str,   5,  0 ) ) );
			static_assert( identic( mid( str,   5,  3 ),	safe_mid( str,   5,  3 ) ) );
			static_assert( identic( mid( str,   5, 12 ),	safe_mid( str,   5, 12 ) ) );
			static_assert( identic( mid( str,   5, 22 ),	safe_mid( str,   5, 22 ) ) );
			static_assert( identic( mid( str,  -5,  0 ),	safe_mid( str,  -5,  0 ) ) );
			static_assert( identic( mid( str,  -5,  3 ),	safe_mid( str,  -5,  3 ) ) );
			static_assert( identic( mid( str,  -5, 12 ),	safe_mid( str,  -5, 12 ) ) );
			static_assert( identic( mid( str,  -5, 22 ),	safe_mid( str,  -5, 22 ) ) );
			static_assert( identic( mid( str,  12,  0 ),	safe_mid( str,  12,  0 ) ) );
			static_assert( identic( mid( str,  12,  3 ),	safe_mid( str,  12,  3 ) ) );
			static_assert( identic( mid( str,  12, 12 ),	safe_mid( str,  12, 12 ) ) );
			static_assert( identic( mid( str,  12, 22 ),	safe_mid( str,  12, 22 ) ) );
			static_assert( identic( mid( str, -12,  0 ),	safe_mid( str, -12,  0 ) ) );
			static_assert( identic( mid( str, -12,  3 ),	safe_mid( str, -12,  3 ) ) );
			static_assert( identic( mid( str, -12, 12 ),	safe_mid( str, -12, 12 ) ) );
			static_assert( identic( mid( str, -12, 22 ),	safe_mid( str, -12, 22 ) ) );
			static_assert( identic( mid( str,  22,  0 ),	safe_mid( str,  22,  0 ) ) );
			static_assert( identic( mid( str,  22,  3 ),	safe_mid( str,  22,  3 ) ) );
			static_assert( identic( mid( str,  22, 12 ),	safe_mid( str,  22, 12 ) ) );
			static_assert( identic( mid( str,  22, 22 ),	safe_mid( str,  22, 22 ) ) );
			static_assert( identic( mid( str, -22,  0 ),	safe_mid( str, -22,  0 ) ) );
			static_assert( identic( mid( str, -22,  3 ),	safe_mid( str, -22,  3 ) ) );
			static_assert( identic( mid( str, -22, 12 ),	safe_mid( str, -22, 12 ) ) );
			static_assert( identic( mid( str, -22, 22 ),	safe_mid( str, -22, 22 ) ) );

			static_assert( identic( mid( strN,  0,  0 ),	safe_mid( strN,  0,  0 ) ) );
			static_assert( identic( mid( strN,  0,  3 ),	safe_mid( strN,  0,  3 ) ) );
			static_assert( identic( mid( strN,  0, 12 ),	safe_mid( strN,  0, 12 ) ) );
			static_assert( identic( mid( strN,  0, 22 ),	safe_mid( strN,  0, 22 ) ) );
			static_assert( identic( mid( strN,  5,  0 ),	safe_mid( strN,  5,  0 ) ) );
			static_assert( identic( mid( strN,  5,  3 ),	safe_mid( strN,  5,  3 ) ) );
			static_assert( identic( mid( strN,  5, 12 ),	safe_mid( strN,  5, 12 ) ) );
			static_assert( identic( mid( strN,  5, 22 ),	safe_mid( strN,  5, 22 ) ) );
			static_assert( identic( mid( strN, -5,  0 ),	safe_mid( strN, -5,  0 ) ) );
			static_assert( identic( mid( strN, -5,  3 ),	safe_mid( strN, -5,  3 ) ) );
			static_assert( identic( mid( strN, -5, 12 ),	safe_mid( strN, -5, 12 ) ) );
			static_assert( identic( mid( strN, -5, 22 ),	safe_mid( strN, -5, 22 ) ) );
			static_assert( identic( mid( strN, 12,  0 ),	safe_mid( strN, 12,  0 ) ) );
			static_assert( identic( mid( strN, 12,  3 ),	safe_mid( strN, 12,  3 ) ) );
			static_assert( identic( mid( strN, 12, 12 ),	safe_mid( strN, 12, 12 ) ) );
			static_assert( identic( mid( strN, 12, 22 ),	safe_mid( strN, 12, 22 ) ) );
			static_assert( identic( mid( strN,-12,  0 ),	safe_mid( strN,-12,  0 ) ) );
			static_assert( identic( mid( strN,-12,  3 ),	safe_mid( strN,-12,  3 ) ) );
			static_assert( identic( mid( strN,-12, 12 ),	safe_mid( strN,-12, 12 ) ) );
			static_assert( identic( mid( strN,-12, 22 ),	safe_mid( strN,-12, 22 ) ) );
			static_assert( identic( mid( strN, 22,  0 ),	safe_mid( strN, 22,  0 ) ) );
			static_assert( identic( mid( strN, 22,  3 ),	safe_mid( strN, 22,  3 ) ) );
			static_assert( identic( mid( strN, 22, 12 ),	safe_mid( strN, 22, 12 ) ) );
			static_assert( identic( mid( strN, 22, 22 ),	safe_mid( strN, 22, 22 ) ) );
			static_assert( identic( mid( strN,-22,  0 ),	safe_mid( strN,-22,  0 ) ) );
			static_assert( identic( mid( strN,-22,  3 ),	safe_mid( strN,-22,  3 ) ) );
			static_assert( identic( mid( strN,-22, 12 ),	safe_mid( strN,-22, 12 ) ) );
			static_assert( identic( mid( strN,-22, 22 ),	safe_mid( strN,-22, 22 ) ) );

			static_assert( identic( mid<  0 >( strN,  0 ),	safe_mid( strN,  0,  0 ) ) );
			static_assert( identic( mid<  0 >( strN,  5 ),	safe_mid( strN,  5,  0 ) ) );
			static_assert( identic( mid<  0 >( strN, -5 ),	safe_mid( strN, -5,  0 ) ) );
			static_assert( identic( mid<  0 >( strN, 12 ),	safe_mid( strN, 12,  0 ) ) );
			static_assert( identic( mid<  0 >( strN,-12 ),	safe_mid( strN,-12,  0 ) ) );
			static_assert( identic( mid<  0 >( strN, 22 ),	safe_mid( strN, 22,  0 ) ) );
			static_assert( identic( mid<  3 >( strN,  0 ),	safe_mid( strN,  0,  3 ) ) );
			static_assert( identic( mid<  3 >( strN,  5 ),	safe_mid( strN,  5,  3 ) ) );
			static_assert( identic( mid<  3 >( strN, -5 ),	safe_mid( strN, -5,  3 ) ) );
			static_assert( identic( mid< 12 >( strN,  0 ),	safe_mid( strN,  0, 12 ) ) );
		}
		{	// mid (int)
			static_assert( identic( mid( ints,   0,  0 ),	safe_mid( ints,   0,  0 ) ) );
			static_assert( identic( mid( ints,   0,  3 ),	safe_mid( ints,   0,  3 ) ) );
			static_assert( identic( mid( ints,   0, 12 ),	safe_mid( ints,   0, 12 ) ) );
			static_assert( identic( mid( ints,   0, 22 ),	safe_mid( ints,   0, 22 ) ) );
			static_assert( identic( mid( ints,   5,  0 ),	safe_mid( ints,   5,  0 ) ) );
			static_assert( identic( mid( ints,   5,  3 ),	safe_mid( ints,   5,  3 ) ) );
			static_assert( identic( mid( ints,   5, 12 ),	safe_mid( ints,   5, 12 ) ) );
			static_assert( identic( mid( ints,   5, 22 ),	safe_mid( ints,   5, 22 ) ) );
			static_assert( identic( mid( ints,  -5,  0 ),	safe_mid( ints,  -5,  0 ) ) );
			static_assert( identic( mid( ints,  -5,  3 ),	safe_mid( ints,  -5,  3 ) ) );
			static_assert( identic( mid( ints,  -5, 12 ),	safe_mid( ints,  -5, 12 ) ) );
			static_assert( identic( mid( ints,  -5, 22 ),	safe_mid( ints,  -5, 22 ) ) );
			static_assert( identic( mid( ints,  12,  0 ),	safe_mid( ints,  12,  0 ) ) );
			static_assert( identic( mid( ints,  12,  3 ),	safe_mid( ints,  12,  3 ) ) );
			static_assert( identic( mid( ints,  12, 12 ),	safe_mid( ints,  12, 12 ) ) );
			static_assert( identic( mid( ints,  12, 22 ),	safe_mid( ints,  12, 22 ) ) );
			static_assert( identic( mid( ints, -12,  0 ),	safe_mid( ints, -12,  0 ) ) );
			static_assert( identic( mid( ints, -12,  3 ),	safe_mid( ints, -12,  3 ) ) );
			static_assert( identic( mid( ints, -12, 12 ),	safe_mid( ints, -12, 12 ) ) );
			static_assert( identic( mid( ints, -12, 22 ),	safe_mid( ints, -12, 22 ) ) );
			static_assert( identic( mid( ints,  22,  0 ),	safe_mid( ints,  22,  0 ) ) );
			static_assert( identic( mid( ints,  22,  3 ),	safe_mid( ints,  22,  3 ) ) );
			static_assert( identic( mid( ints,  22, 12 ),	safe_mid( ints,  22, 12 ) ) );
			static_assert( identic( mid( ints,  22, 22 ),	safe_mid( ints,  22, 22 ) ) );
			static_assert( identic( mid( ints, -22,  0 ),	safe_mid( ints, -22,  0 ) ) );
			static_assert( identic( mid( ints, -22,  3 ),	safe_mid( ints, -22,  3 ) ) );
			static_assert( identic( mid( ints, -22, 12 ),	safe_mid( ints, -22, 12 ) ) );
			static_assert( identic( mid( ints, -22, 22 ),	safe_mid( ints, -22, 22 ) ) );

			static_assert( identic( mid( intsN,  0,  0 ),	safe_mid( intsN,  0,  0 ) ) );
			static_assert( identic( mid( intsN,  0,  3 ),	safe_mid( intsN,  0,  3 ) ) );
			static_assert( identic( mid( intsN,  0, 12 ),	safe_mid( intsN,  0, 12 ) ) );
			static_assert( identic( mid( intsN,  0, 22 ),	safe_mid( intsN,  0, 22 ) ) );
			static_assert( identic( mid( intsN,  5,  0 ),	safe_mid( intsN,  5,  0 ) ) );
			static_assert( identic( mid( intsN,  5,  3 ),	safe_mid( intsN,  5,  3 ) ) );
			static_assert( identic( mid( intsN,  5, 12 ),	safe_mid( intsN,  5, 12 ) ) );
			static_assert( identic( mid( intsN,  5, 22 ),	safe_mid( intsN,  5, 22 ) ) );
			static_assert( identic( mid( intsN, -5,  0 ),	safe_mid( intsN, -5,  0 ) ) );
			static_assert( identic( mid( intsN, -5,  3 ),	safe_mid( intsN, -5,  3 ) ) );
			static_assert( identic( mid( intsN, -5, 12 ),	safe_mid( intsN, -5, 12 ) ) );
			static_assert( identic( mid( intsN, -5, 22 ),	safe_mid( intsN, -5, 22 ) ) );
			static_assert( identic( mid( intsN, 12,  0 ),	safe_mid( intsN, 12,  0 ) ) );
			static_assert( identic( mid( intsN, 12,  3 ),	safe_mid( intsN, 12,  3 ) ) );
			static_assert( identic( mid( intsN, 12, 12 ),	safe_mid( intsN, 12, 12 ) ) );
			static_assert( identic( mid( intsN, 12, 22 ),	safe_mid( intsN, 12, 22 ) ) );
			static_assert( identic( mid( intsN,-12,  0 ),	safe_mid( intsN,-12,  0 ) ) );
			static_assert( identic( mid( intsN,-12,  3 ),	safe_mid( intsN,-12,  3 ) ) );
			static_assert( identic( mid( intsN,-12, 12 ),	safe_mid( intsN,-12, 12 ) ) );
			static_assert( identic( mid( intsN,-12, 22 ),	safe_mid( intsN,-12, 22 ) ) );
			static_assert( identic( mid( intsN, 22,  0 ),	safe_mid( intsN, 22,  0 ) ) );
			static_assert( identic( mid( intsN, 22,  3 ),	safe_mid( intsN, 22,  3 ) ) );
			static_assert( identic( mid( intsN, 22, 12 ),	safe_mid( intsN, 22, 12 ) ) );
			static_assert( identic( mid( intsN, 22, 22 ),	safe_mid( intsN, 22, 22 ) ) );
			static_assert( identic( mid( intsN,-22,  0 ),	safe_mid( intsN,-22,  0 ) ) );
			static_assert( identic( mid( intsN,-22,  3 ),	safe_mid( intsN,-22,  3 ) ) );
			static_assert( identic( mid( intsN,-22, 12 ),	safe_mid( intsN,-22, 12 ) ) );
			static_assert( identic( mid( intsN,-22, 22 ),	safe_mid( intsN,-22, 22 ) ) );

			static_assert( identic( mid<  0 >( intsN,  0 ),	safe_mid( intsN,  0,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN,  5 ),	safe_mid( intsN,  5,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN, -5 ),	safe_mid( intsN, -5,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN, 12 ),	safe_mid( intsN, 12,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN,-12 ),	safe_mid( intsN,-12,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN, 22 ),	safe_mid( intsN, 22,  0 ) ) );
			static_assert( identic( mid<  0 >( intsN,-22 ),	safe_mid( intsN,-22,  0 ) ) );
			static_assert( identic( mid<  3 >( intsN,  0 ),	safe_mid( intsN,  0,  3 ) ) );
			static_assert( identic( mid<  3 >( intsN,  5 ),	safe_mid( intsN,  5,  3 ) ) );
			static_assert( identic( mid<  3 >( intsN, -5 ),	safe_mid( intsN, -5,  3 ) ) );
			static_assert( identic( mid< 12 >( intsN,  0 ),	safe_mid( intsN,  0, 12 ) ) );
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
			DOCTEST_FAST_CHECK_EQ( find_span( str, first( str, 5 ) ),	 std::string_view( "abcde" ) );
			DOCTEST_FAST_CHECK_EQ( find_span( str, last ( str, 7 ) ),	 std::string_view( "fghijkl" ) );

			static_assert( find( null_, linebreak{} )				==	 0 );
			static_assert( find( "abcd\r\nefgh", linebreak{} )	==	 4 );
		}
		{	// contains
			static_assert(  contains( str, 'd' ) );
			static_assert( !contains( str, '8' ) );
			static_assert(  contains( str, first( str, 5 ) ) );
			static_assert(  contains( str, last ( str, 5 ) ) );
			static_assert(  contains( "abcdefghijkl", first( str, 5 ) ) );
			static_assert(  contains( "abcdefghijkl", "cdef" ) );
		}
		{	// until
			using std::data, std::size;
			constexpr auto base = mid( abc, 3, 4 );
			DOCTEST_FAST_CHECK_EQ( base,	std::string_view( "defg" ) );

			static_assert( until( base, 'a' )	==	base );				// None
			static_assert( until( base, 'd' )	==	first( base, 0 ) );	// First
			static_assert( until( base, 'f' )	==	first( base, 2 ) );	// Middle
			static_assert( until( base, 'g' )	==	first( base, 3 ) );	// Last

			static_assert( until( "abcdefgh", 'g' )		==	"abcdef" );	// Last
			static_assert( until( "abcdefgh", "fg" )	==	"abcde" );	// Last

			// linebreak 
			static_assert( until( "abcd\r\nefgh"	, linebreak{} ).size() == 4 );
			static_assert( until( ""				, linebreak{} )	==	"" );
			static_assert( until( "abcdefgh"		, linebreak{} )	==	"abcdefgh" );

			static_assert( until( "\nabcdefgh"		, linebreak{} )	==	"" );
			static_assert( until( "\n\r\nabcdefgh"	, linebreak{} )	==	"" );
			static_assert( until( "\rabcdefgh"		, linebreak{} )	==	"" );
			static_assert( until( "\r\nabcdefgh"	, linebreak{} )	==	"" );

			static_assert( until( "abcd\nefgh"		, linebreak{} )	==	"abcd" );
			static_assert( until( "abcd\n\refgh"	, linebreak{} )	==	"abcd" );
			static_assert( until( "abcd\refgh"		, linebreak{} )	==	"abcd" );
			static_assert( until( "abcd\r\nefgh"	, linebreak{} )	==	"abcd" );

			static_assert( until( "abcdefgh\n"		, linebreak{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\n\r"	, linebreak{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\r"		, linebreak{} )	==	"abcdefgh" );
			static_assert( until( "abcdefgh\r\n"	, linebreak{} )	==	"abcdefgh" );
		}
		{	// starts_with
			static_assert( starts_with( "abcdef", "abc" )	==	true );
			static_assert( starts_with( "abcdef", "abd" )	==	false );
			static_assert( starts_with( str, ""    )		==	true );
			static_assert( starts_with( null_, "123" )		==	false );
			static_assert( starts_with( str, "abc" )		==	true );
			static_assert( starts_with( str, "def" )		==	false );
		}
		{	// ends_with
			static_assert( ends_with( "abcdef", "def" )					==	true );
			static_assert( ends_with( "abcdef", "deg" )					==	false );
			static_assert( ends_with( str, ""    )						==	true );
			static_assert( ends_with( null_, "jkl" )					==	false );
			static_assert( ends_with( "abcdefghijkl", last( str, 3 ) )	==	true );
			static_assert( ends_with( str, "jkl" )						==	true );
			static_assert( ends_with( str, "ijk" )						==	false );
		}
		{	// split_at, spolit_by
			{	// at index
				{
					constexpr auto div = split_at( "abcdefghijkl", 24 );
					static_assert( div.first	==	"abcdefghijkl" );
					static_assert( div.rest		==	"" );
				} {
					constexpr auto div = split_at( "abcdefghijkl", 12 );
					static_assert( div.first 	==	"abcdefghijkl" );
					static_assert( div.rest		==	"" );
				} {
					constexpr auto div = split_at( "abcdefghijkl", 4 );
					static_assert( div.first 	==	"abcd" );
					static_assert( div.rest		==	"efghijkl" );
				} {
					constexpr auto div = split_at( abc, 0 );
					static_assert( div.first	==	first( abc, 0 ) );
					static_assert( div.rest		==	not_first( abc, 0 ) );
				} {
					constexpr auto div = split_at( abc, 2 );
					static_assert( div.first	==	first( abc, 2 ) );
					static_assert( div.rest		==	not_first( abc, 2 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() - 1 );
					static_assert( div.first	==	first( abc, abc.size() - 1 ) );
					static_assert( div.rest		==	last( abc, 1 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() );
					static_assert( div.first	==	abc );
					static_assert( div.rest		==	last( abc, 0 ) );
				} {
					constexpr auto div = split_at( abc, abc.size() + 1 );
					static_assert( div.first	==	abc );
					static_assert( div.rest		==	last( abc, 0 ) );
				}
			}
			{	// by value
				constexpr auto base = mid( abc, 3, 4 );
				DOCTEST_FAST_CHECK_EQ( base,	std::string_view( "defg" ) );
				{
					constexpr auto div = split( "abcdefghijkl", 'e' );
					static_assert( div.first 	==	"abcd" );
					static_assert( div.rest		==	"fghijkl" );
				} {
					constexpr auto div = split( base, 'a' );					// None
					static_assert( div.first	==	base );
					static_assert( div.rest		==	last( base, 0 ) );
				} {
					constexpr auto div = split( base, 'd' );					// First
					static_assert( div.first	==	first( base, 0 ) );
					static_assert( div.rest		==	not_first( base, 1 ) );
				} {
					constexpr auto div = split( base, 'f' );					// Middle
					static_assert( div.first	==	first( base, 2 ) );
					static_assert( div.rest		==	not_first( base, 3 ) );
				} {
					constexpr auto div = split( base, 'g' );					// Last
					static_assert( div.first	==	first( base, 3 ) );
					static_assert( div.rest		==	not_first( base, 4 ) );
				}
			}
			{	// by string_view
				constexpr auto base = mid( abc, 3, 4 );
				DOCTEST_FAST_CHECK_EQ( base,	std::string_view( "defg" ) );
				{
					constexpr auto div = split( "defg", "ef" );
					static_assert( div.first 	==	"d" );
					static_assert( div.rest		==	"g" );
				} {
					constexpr auto div = split( str, first( str, 2 ) );		// First
					DOCTEST_FAST_CHECK_EQ( find_span( str, first( str, 2 ) ),	std::string_view( "ab" ) );
					static_assert( div.first			==	first( str,  0 ) );
					static_assert( div.rest				==	last ( str, 10 ) );
					static_assert( div.first.data() 	==	std::string_view( str ).data() );
				} {
					constexpr auto div = split( str, mid( str, 4, 2 ) );	// Middle
					static_assert( div.first			==	first( str,  4 ) );
					static_assert( div.rest				==	last ( str,  6 ) );
				} {
					constexpr auto div = split( str, last( str, 2 ) );		// End
					static_assert( div.first			==	first( str, 10 ) );
					static_assert( div.rest				==	last ( str,  0 ) );
					static_assert ( div.rest.data()		==	std::string_view( str ).data() + std::string_view( str ).size() );
				} {
					constexpr auto div = split( str, "aaa" );				// None
					static_assert( div.first			==	str );
					static_assert( div.rest				==	last( str, 0 ) );
				}
			}
			{	// by linebreak
				// None
				{
					constexpr auto div = split( std::string_view( "bcdefgh" ), linebreak{} );
					static_assert( div.first 		==	"bcdefgh" );
					static_assert( div.rest			==	"" );
				} {
					constexpr auto div = split( std::string_view( "" ), linebreak{} );
					static_assert( div.first		==	"" );
					static_assert( div.rest			==	"" );
				}

				{	// Beginning
					{
						constexpr auto div = split( "\nbcdefgh", linebreak{} );
						static_assert( div.first 	==	"" );
						static_assert( div.rest		==	"bcdefgh" );
					} {
						constexpr auto div = split( "\n\rbcdefgh", linebreak{} );
						static_assert( div.first	==	"" );
						static_assert( div.rest		==	"bcdefgh" );
					} {
						constexpr auto div = split( "\rbcdefgh", linebreak{} );
						static_assert( div.first	==	"" );
						static_assert( div.rest		==	"bcdefgh" );
					} {
						constexpr auto div = split( "\r\nbcdefgh", linebreak{} );
						static_assert( div.first	==	"" );
						static_assert( div.rest		==	"bcdefgh" );
					}
				}
				{	// Middle
					{
						constexpr auto div = split( "bcd\nefgh", linebreak{} );
						static_assert( div.first 	==	"bcd" );
						static_assert( div.rest		==	"efgh" );
					} {
						constexpr auto div = split( "bcd\n\refgh", linebreak{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.rest		==	"efgh" );
					} {
						constexpr auto div = split( "bcd\refgh", linebreak{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.rest		==	"efgh" );
					} {
						constexpr auto div = split( "bcd\r\nefgh", linebreak{} );
						static_assert( div.first	==	"bcd" );
						static_assert( div.rest		==	"efgh" );
					}
				}
				{	// Ending
					{
						constexpr auto div = split( "bcdefgh\n", linebreak{} );
						static_assert( div.first 	==	"bcdefgh" );
						static_assert( div.rest		==	"" );
					} {
						constexpr auto div = split( "bcdefgh\n\r", linebreak{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.rest		==	"" );
					} {
						constexpr auto div = split( "bcdefgh\r", linebreak{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.rest		==	"" );
					} {
						constexpr auto div = split( "bcdefgh\r\n", linebreak{} );
						static_assert( div.first	==	"bcdefgh" );
						static_assert( div.rest		==	"" );
					}
				}
			}
		}
		{	// trim
			constexpr auto					text = "++++abcdef++";
			constexpr auto					pred = []( char c ) { return c <= 'b'; };
			constexpr auto 					is_plus = []( auto x_ ){	return x_ == '+'; };

			{	// trim_first
				{	// traits::character
					static_assert( trim_first( "abc", '+' )		==	"abc" );
					static_assert( trim_first( text, '+' )		==	not_first( text, 4 ) );
				}
				{	// Predicate
					static_assert( trim_first( text, is_plus )	==	not_first( text, 4 ) );
					static_assert( trim_first( text, pred )		==	not_first( text, 6 ) );
				}
				{	// linebreak
					static_assert( trim_first( "",					linebreak{} )	==	"" );
					static_assert( trim_first( "\n\r",				linebreak{} )	==	"" );
					static_assert( trim_first( "abcdefgh\n\r",		linebreak{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\nabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\n\rabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\rabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );
					static_assert( trim_first( "\r\nabcdefgh\n\r",	linebreak{} )	==	"abcdefgh\n\r" );
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
				{	// linebreak
					static_assert( trim_last( "",					linebreak{} )	==	"" );
					static_assert( trim_last( "\n\r",				linebreak{} )	==	"" );
					static_assert( trim_last( "\n\rabcdefgh",		linebreak{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\n",		linebreak{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\n\r",	linebreak{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\r",		linebreak{} )	==	"\n\rabcdefgh" );
					static_assert( trim_last( "\n\rabcdefgh\r\n",	linebreak{} )	==	"\n\rabcdefgh" );
				}
			}
			{	// trim
				static_assert( trim( text, is_plus )			==	mid( text, 4, 6 ) );
				static_assert( trim( text, pred )				==	mid( text, 6, 4 ) );
			}
		}
	}
	DOCTEST_TEST_CASE( "other..." ) {
		{	// identify_linebreak
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
