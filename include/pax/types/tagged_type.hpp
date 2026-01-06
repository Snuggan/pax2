//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/std/format.hpp>	// std20::formatter


namespace pax {

	/// Define a specific type of a value that is known at compile time.
	/// Tag is to be used if you want the value type to be unique. Use Statique, otherwise.
	template< auto N, typename Tag = struct general >
	struct Statique {
		using tag_type							  = Tag;
		using value_type						  = decltype( N );
		using type								  = Statique;
		static constexpr value_type					value{ N };
		constexpr operator value_type()				const noexcept	{	return value;		}
		static constexpr value_type operator()()	noexcept		{	return value;		}
	    constexpr bool operator==( const value_type & v_ )	const	{   return value == v_;	}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const Statique & stat_ ) 
		{	return out_ << stat_.value;				}
	};

	/// Define the value [of Statique] that is usable at compile time.
	template< auto N, typename Tag = struct general >
	constexpr Statique< N, Tag >	Stat{};

	static_assert(     5 == Stat< 5 > );



	/// Hold values in a type-specific way.
	template< typename Tag, typename T >
	struct Tagged {
		using tag_type							  = Tag;
		constexpr Tagged( const T v_ )				noexcept		:	value{ v_ } {}
		using value_type						  = T;
		using type								  = Tagged;
		value_type									value;
		constexpr operator value_type()				const noexcept	{	return value;		}
		constexpr value_type operator()()			const noexcept	{	return value;		}
	    constexpr bool operator==( const value_type & v_ )	const	{   return value == v_;	}
		
		template< typename T2, typename Tag2 >
	    constexpr bool operator==( const Tagged< T2, Tag2 > & v_ )	const
		{   return value == v_.value;				}
	};

	template< typename T >
	constexpr Tagged< struct general, T > tagged( const T & t_ )
	{	return { t_ };								}

	template< typename Tag, typename T >
	constexpr Tagged< Tag, T > tagged( const T & t_ )
	{	return { t_ };								}

	static_assert(     5 == tagged( 5 ) );
	
}	// namespace pax

namespace std20 {
	template< auto N, typename Tag >
	struct formatter< pax::Statique< N, Tag > > : formatter< decltype( N ) > {
	    constexpr auto format( const pax::Statique< N, Tag > & stat_, format_context & ctx_ )	const {
			return formatter< decltype( N ) >::format( stat_.value, ctx_ );
	    }
	};

	template< typename Tag, typename T >
	struct formatter< pax::Tagged< Tag, T > > : formatter< T > {
	    constexpr auto format( const pax::Tagged< Tag, T > & tagged_, format_context & ctx_ ) const {
			return formatter< T >::format( tagged_.value, ctx_ );
	    }
	};
}	// namespace std
