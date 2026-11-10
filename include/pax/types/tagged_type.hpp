//	Copyright (c) 2014-2025, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

#pragma once

#include <format>	// std::formatter


namespace pax {

	/// Base class for having semantically dedicated types for specific uses.
	template< typename Tag, typename T >
	struct Tagged {
		using				tag		= Tag;
		using				type	= T;
		type				value{};

		constexpr Tagged()									  noexcept	=	default;
		constexpr Tagged( const Tagged & )					  noexcept	=	default;
		constexpr Tagged & operator=( const Tagged & )		  noexcept	=	default;

		explicit constexpr Tagged( type t_ )				  noexcept	:	value{ t_ } {}
		constexpr Tagged & operator=( type t_ )				  noexcept	{	value = t_;		return *this;	}

		explicit constexpr operator type()				const noexcept	{	return value;					}
		constexpr type operator()()						const noexcept	{	return value;					}

		template< typename U >
		friend constexpr bool operator== ( Tagged tg0, Tagged< tag, U > tg1 ) noexcept	{	return tg0() ==  tg1();	}

		template< typename U >
		friend constexpr auto operator<=>( Tagged tg0, Tagged< tag, U > tg1 ) noexcept	{	return tg0() <=> tg1();	}

		template< typename Out >
		friend std::ostream & operator<<( Out & out_, const Tagged tg )					{	return out_ << tg();	}
	};

	/// Dedicated clas to semantically distinguish between flags in function calls. 
	template< typename Tag >
	using Bool = Tagged< Tag, bool >;


	/// Base class for having dedicated types for Col, Row, etc.
	/// So as not to mix their order when accessing tables etc. 
	template< typename Tag >
	struct Idx : Tagged< Tag, std::size_t > {
		using type		   = std::size_t;
		using Tagged< Tag, type >::value;
		using Tagged< Tag, type >::Tagged;

		constexpr Idx & operator++()						  noexcept	{	++value;		return *this;	}
		constexpr Idx & operator--()						  noexcept	{	--value;		return *this;	}

		constexpr Idx   operator++( int )				const noexcept	{
			const type	temp = value;
			++value;
			return Idx{ temp };
		}
		constexpr Idx   operator--( int )				const noexcept	{
			const type	temp = value;
			--value;
			return Idx{ temp };
		}

		constexpr Idx & operator+=( type t )				  noexcept	{	value += t;		return *this;	}
		constexpr Idx & operator-=( type t )				  noexcept	{	value -= t;		return *this;	}

		friend constexpr Idx operator+( Idx tg, type t )	  noexcept	{	return tg += t;					}
		friend constexpr Idx operator-( Idx tg, type t )	  noexcept	{	return tg -= t;					}
	};

	using Col = Idx< struct Col_tag >;		///< To symantically distiguish column index.
	using Row = Idx< struct Row_tag >;		///< To symantically distiguish row index.

}	// namespace pax

namespace std {
	template< typename Tag, typename T >
	struct formatter< pax::Tagged< Tag, T > > : formatter< T > {
	    constexpr auto format( const pax::Tagged< Tag, T > & tagged_, format_context & ctx_ ) const {
			return formatter< T >::format( tagged_(), ctx_ );
	    }
	};
}	// namespace std
