//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <pax/std/format.hpp>
#include <algorithm>		// std::copy_n, std::ranges::equal


namespace pax {

	/// A class to simlify string use at compile time, i.e. as template arguments.
	template< typename Char, size_t N >
	struct strlit {
		using value_type		  = std::remove_cv_t< Char >;

	    constexpr strlit( value_type const ( & str_ )[ N ] )
		{	std::copy_n( str_, N, value );						}
 
 	    template< std::size_t M >
	    constexpr bool operator==( value_type const ( & str_ )[ M ] )		const
		{   return std::ranges::equal( value, str_ );			}
 
 	    template< std::size_t M >
	    constexpr bool operator==( const strlit< value_type, M > & str_ )	const
		{   return std::ranges::equal( value, str_.value );		}

		value_type value[ N ];
		static constexpr std::size_t size()		{	return N;	}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const strlit & str_ ) 
		{	return out_.write( str_.value, N - 1 );				}
	};

	template< typename Char, size_t N >
	strlit( Char const ( & )[ N ] ) -> strlit< Char, N >;



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

	/// Define the value that is known at compile time.
	template< auto N, typename Tag = struct general >
	constexpr Statique< N, Tag >	Stat{};

	static_assert(     5 == Stat< 5 > );
	static_assert( "Hej" == Stat< strlit{ "Hej" } > );



	/// Hold values in a type specific way.
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

	template< typename Char, size_t N >
	constexpr Tagged< struct general, strlit< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}

	template< typename Tag, typename Char, size_t N >
	constexpr Tagged< Tag, strlit< Char, N > > tagged( Char const ( & str_ )[ N ] )
	{	return { str_ };							}

	static_assert(     5 == tagged( 5 ) );
	static_assert( "Hej" == tagged( "Hej" ) );
	static_assert( "Hej" == tagged( strlit{ "Hej" } ) );
	
}	// namespace pax

namespace std20 {
	template< typename Char, size_t N >
	struct formatter< pax::strlit< Char, N > > : formatter< Char const * > {
	    auto format( const pax::strlit< Char, N > & str_, format_context & ctx_ )	const {
			return formatter< Char const * >::format( str_.value, ctx_ );
	    }
	};
	template< auto N, typename Tag >
	struct formatter< pax::Statique< N, Tag > > : formatter< decltype( N ) > {
	    auto format( const pax::Statique< N, Tag > & stat_, format_context & ctx_ )	const {
			return formatter< decltype( N ) >::format( stat_.value, ctx_ );
	    }
	};
	template< typename Tag, typename T >
	struct formatter< pax::Tagged< Tag, T > > : formatter< T > {
		using Tagged = pax::Tagged< Tag, T >;
	    auto format( const pax::Tagged< Tag, T > & tagged_, format_context & ctx_ ) const {
			return formatter< T >::format( tagged_.value, ctx_ );
	    }
	};
}	// namespace std
