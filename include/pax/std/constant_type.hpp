//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <cstddef>		// std::size_t, std::ptrdiff_t


namespace pax {

	/// Define a specific type of a value that is known at compile time.
	/// Tag is to be used if you want the value type to be unique. Use Const_type, otherwise.
	template< auto N, typename Tag >
	struct Const_type_base {
		using value_type						  = decltype( N );
		using tag_type							  = Tag;
		using type								  = Const_type_base;
		static constexpr value_type					value{ N };
		constexpr operator value_type()				const noexcept	{	return value;	}
		static constexpr value_type operator()()	noexcept		{	return value;	}
	};

	/// Define the general type of a value that is known at compile time.
	template< auto N >
	using Const_type			  = Const_type_base< N, struct general >;

	/// Define the value that is known at compile time.
	template< auto N >
	constexpr Const_type< N >		Const{};

	constinit Const_type< true  >	True{};
	constinit Const_type< false >	False{};




	/// Hold values in a type specific way.
	template< typename T, typename Tag >
	struct Tagged {
		using value_type						  = T;
		using tag_type							  = Tag;
		using type								  = Tagged;
		value_type									value;
		constexpr Tagged( T v_ )			noexcept		:	value{ v_ } {}
		constexpr operator value_type()				const noexcept	{	return value;	}
		constexpr value_type operator()()			const noexcept	{	return value;	}
	};

	template< typename Tag >
	using Bool		= Tagged< bool, Tag >;

	template< typename Tag >
	using Unsigned	= Tagged< std::size_t, Tag >;

	template< typename Tag >
	using Signed	= Tagged< std::ptrdiff_t, Tag >;

	template< typename Tag >
	using Double	= Tagged< double, Tag >;

}	// namespace pax
