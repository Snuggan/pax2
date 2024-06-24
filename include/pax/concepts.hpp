//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <iterator>		// std::begin, std::end, std::size, std::data
#include <type_traits>


namespace pax {

	constexpr std::size_t dynamic_extent( -1 );

	
	template< std::size_t I >
	using size_constant = std::integral_constant< std::size_t, I >;

	template< std::size_t I >
	using diff_constant = std::integral_constant< std::ptrdiff_t, I >;


	using std::begin, std::end, std::size, std::data;


	/// A concept to match structs that "handles" contigous elements, i.e. std::array, std::string, std::span etc.
	template< typename T >
	concept Iterable = requires( T t ) {
		requires	std::input_iterator< decltype( begin( t ) ) >;
		requires	std::is_same_v< decltype( begin( t ) ), decltype( end( t ) ) >;
		requires	std::is_same_v< decltype( begin( t ) == end( t ) ), bool >;
	};


	/// A concept to match structs that "handles" contigous elements, i.e. std::array, std::string, std::span etc.
	template< typename T >
	concept Contiguous_elements_object	= 
			Iterable< T > 
		&&	requires( T t ) {
				{ size( t ) } -> std::convertible_to< std::size_t >;
				std::is_pointer_v< decltype( data( t ) ) >;
				std::contiguous_iterator< decltype( begin( t ) ) >;
			};

	/// A concept to match "handlers" of contigous elements, i.e. pointer, Contiguous_elements_object etc.
	template< typename T >
	concept Contiguous_elements		= 
			Contiguous_elements_object< T >						// Containers, viewers.
		||	std::is_pointer_v< std::remove_cvref_t< T > >		// Pointers.
		||	std::rank_v< std::remove_cvref_t< T > > == 1		// We need to recognize T[N] and T(&)[N].
	;

	/// A statically sized Contiguous_elements, e.g. std::array.
	template< typename T >
	concept Has_static_size		  = 
			( std::tuple_size< T >::value >= 0 ) 
		||	( std::is_bounded_array_v< std::remove_cvref_t< T > > && std::rank_v< std::remove_cvref_t< T > > == 1 )	// T[N], T(&)[N]
		||	( T::extent != dynamic_extent )						// std::span< T, N > doesn't have tuple_size...
	;

	/// A dynamically sized contiguous elements, e.g. std::vector.
	template< typename T >
	concept Has_dynamic_size	  = Contiguous_elements_object< T > && not Has_static_size< T >;

	/// A statically or dynamically sized contiguous elements, e.g. *not* pointers.
	template< typename T >
	concept Has_size			  = Has_static_size< T > || Has_dynamic_size< T >;
	
	
	static_assert( Has_static_size< int[4] > );
	static_assert( Has_static_size< int(&)[4] > );
	static_assert( !Has_size< int(&)[] > );
	static_assert( !Has_size< int[] > );
	static_assert( !Has_size< int* > );



	template< typename T >
	constexpr std::size_t extent_default() {
		if constexpr( requires{ T::extent; } )				return T::extent;
		if constexpr( requires{ std::rank_v< T > == 1; } )	return std::extent_v< T >;
		return 0; 
	}

	/// The "size" of an object. 
	/**	- dynamic_extent: for dynamically sized containers with contigous elements.
		- N: for statically sized containers with N contigous elements.
		- 0: for all other objects.
	**/
	template< typename T >	struct extent : size_constant< extent_default< T >() > {};
	template< typename T >	struct extent< T & >		: extent< T > {};
	template< typename T >	struct extent< T && >		: extent< T > {};
	template< typename T >	struct extent< const T >	: extent< T > {};
	template< typename T >	struct extent< volatile T >	: extent< T > {};
	
	template< typename T >				requires( std::tuple_size< T >::value >= 0 )
	struct extent< T > : std::tuple_size< T > {};
	
	template< Has_dynamic_size T >
	struct extent< T > : size_constant< dynamic_extent > {};

	template< typename T >
	constexpr std::size_t extent_v{ extent< T >::value };



	template< typename T >
	concept has_element_type	  = requires { typename T::element_type; };

	template< typename T >	struct Value_type					{	using type = T;								};
	template< typename T >	using  Value_type_t					=	typename Value_type< T >::type;
	template< typename T >	struct Value_type< T & >			:	Value_type< T > {};
	template< typename T >	struct Value_type< T && >			:	Value_type< T > {};
	template< typename T >	struct Value_type< const T >		{	using type = const Value_type_t< T >;		};
	template< typename T >	struct Value_type< volatile T >		{	using type = volatile Value_type_t< T >;	};

	template< Contiguous_elements_object T >
		requires( has_element_type< T > )
	struct Value_type< T >	{	using type = typename T::element_type;											};

	template< Contiguous_elements_object T >
		requires( !has_element_type< T > )
	struct Value_type< T >	{
		using type = std::conditional_t< std::is_same_v< typename T::iterator, typename T::const_iterator >,
			std::add_const_t< typename T::value_type >,		// std::string_view needs it...
			typename T::value_type
		>;
	};
	
	template< typename T >
		requires( std::is_pointer_v< T > )
	struct Value_type< T >	{	using type = std::remove_pointer_t< T >;										};
	
	template< typename T >
		requires( std::is_array_v< T > )
	struct Value_type< T >	{	using type = std::remove_all_extents_t< T >;									};



	template< typename T >	struct is_character					:	std::false_type {};
	template< typename T >	constexpr bool is_character_v		=	is_character< T >::value;
	template< typename T >	struct is_character< T & >			:	is_character< T > {};
	template< typename T >	struct is_character< T && >			:	is_character< T > {};
	template< typename T >	struct is_character< const T >		:	is_character< T > {};
	template< typename T >	struct is_character< volatile T >	:	is_character< T > {};
	template<>				struct is_character< char >			:	std::true_type {};
	template<>				struct is_character< signed char >	:	std::true_type {};
	template<>				struct is_character< unsigned char >:	std::true_type {};
	template<>				struct is_character< wchar_t >		:	std::true_type {};
	template<>				struct is_character< char8_t >		:	std::true_type {};
	template<>				struct is_character< char16_t >		:	std::true_type {};
	template<>				struct is_character< char32_t >		:	std::true_type {};

	/// Is T a character type. 
	template< typename T >
	concept Character	  = is_character_v< T >;

	template< typename T >
	concept String		  = Contiguous_elements< T > && Character< Value_type_t< T > >;

	/// A concept to match character arrays.
	template< typename T >
	concept Character_array	= 
			Contiguous_elements< T >
		&&	(		std::is_pointer_v< std::remove_cvref_t< T > > 
				||	std::rank_v< std::remove_cvref_t< T > > == 1	// T[N] and T(&)[N].
			);


}	// namespace pax
