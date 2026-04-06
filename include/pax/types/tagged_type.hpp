//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/std/format.hpp>	// std20::formatter


namespace pax {

	/// A specific type to use as the base for compile time constants.
	/// Ignore Tag unless you want the value type to be unique. 
	template< auto N, typename Tag = struct general >
	struct Statique {
		using type									  = Statique;
		using tag_type								  = Tag;
		using value_type							  = decltype( N );
		static constexpr value_type						value{ N };

		// This function is conciously not explicit, as that would only be in the way. 
		constexpr operator value_type()					const noexcept	{	return value;			}

		static constexpr value_type operator()()		noexcept		{	return value;			}

		template< typename U >
	    constexpr bool operator== ( const U & v_ )		const noexcept	{   return value ==  v_;	}

		template< typename U >
	    constexpr auto operator<=>( const U & v_ )		const noexcept	{   return value <=> v_;	}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const Statique & stat_ ) {
			return out_ << stat_.value;
		}
	};

	/// Create compile time constants.
	/// Ignore Tag unless you want the value type to be unique. 
	template< auto N, typename Tag = struct general >
	constexpr Statique< N, Tag >	Stat{};

	static_assert( Stat< 5 > == Stat< 5.0 > );
	static_assert(       5   == Stat< 5.0 > );
	static_assert(       5   <  Stat< 5.1 > );



	/// Hold values in a type-specific way.
	template< typename Tag, typename T >
	struct Tagged {
		using type									  = Tagged;
		using tag_type								  = Tag;
		using value_type							  = T;
		value_type										value;

		constexpr Tagged()								noexcept = delete;
		constexpr Tagged( const Tagged & )				noexcept = default;
		constexpr Tagged & operator=( const Tagged & )	noexcept = default;
		constexpr Tagged( const T v_ )					noexcept		:	value{ v_ } {}
		explicit constexpr operator value_type()		const noexcept	{	return value;	}
		constexpr value_type operator()()				const noexcept	{	return value;	}

		template< typename U >
	    constexpr bool operator== ( const Tagged< Tag, U > & v_ ) const noexcept {
			return value ==  v_.value;
		}

		template< typename U >
	    constexpr auto operator<=>( const Tagged< Tag, U > & v_ ) const noexcept {
			return value <=> v_.value;
		}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const Tagged & v_ ) {
			return out_ << v_.value;
		}
	};

	template< typename T >
	constexpr Tagged< struct general, T > tagged( const T & t_ )		{	return { t_ };	}

	template< typename Tag, typename T >
	constexpr Tagged< Tag, T > tagged( const T & t_ )					{	return { t_ };	}

	static_assert( tagged( 5 ) == tagged( 5.0 ) );
	static_assert( tagged( 5 ) <  tagged( 5.1 ) );

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
