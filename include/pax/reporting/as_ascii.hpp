//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../concepts.hpp"
#include <sstream>


#define DOCTEST_ASCII_CHECK_EQ( __1__, __2__ )	DOCTEST_FAST_CHECK_EQ( pax::ascii( __1__ ), pax::ascii( __2__ ) )
#define DOCTEST_ASCII_CHECK_NE( __1__, __2__ )	DOCTEST_FAST_CHECK_NE( pax::ascii( __1__ ), pax::ascii( __2__ ) )

#define DOCTEST_ASCII_WARN_EQ( __1__, __2__ )	DOCTEST_FAST_WARN_EQ( pax::ascii( __1__ ), pax::ascii( __2__ ) )
#define DOCTEST_ASCII_WARN_NE( __1__, __2__ )	DOCTEST_FAST_WARN_NE( pax::ascii( __1__ ), pax::ascii( __2__ ) )


namespace pax {

	template< Character Ch >
	class Ascii_t {
		Ch					  * m_begin;
		Ch					  * m_end;
		
	public:
		constexpr Ascii_t( Ch * data_, std::size_t sz_ ) noexcept : m_begin{ data_ }, m_end{ data_ + sz_ }	{}

		template< typename Dest >
		friend constexpr Dest & operator<<(
			Dest			  & dest_,
			const Ascii_t		v_
		) {
			constexpr Ch specials[ 67 ] = {
				"\\0^A^B^C^D^E^F\\a\\b\\t\\n\\v\\f\\r^N^O" 
				"^P^Q^R^S^T^U^V^W^X^Y^Z\\e^\\^]^^^_\u00B7"	// mid dot for space
			};

			for( auto c=v_.m_begin; c<v_.m_end; ++c ) 
				( *c <= 0x20 )	? ( dest_ << specials[ 2*(int)*c ] << specials[ 2*(int)*c + 1 ] )	: 
				( *c == '\'' )	? ( dest_ << '\\' << '\'' )		: 
				( *c == '"'  )	? ( dest_ << '\\' << '"' )		: 
				( *c == '\\' )	? ( dest_ << '\\' << '\\' )		: 
				( *c == 0x7f )	? ( dest_ << '^' << '?' )		: 
				( *c == 0xa0 )	? ( dest_ << '^' << '!' )		: 
								  ( dest_ << *c );
			return dest_;
		}

		constexpr auto ascii() const {
			std::basic_ostringstream< std::remove_cv_t< Ch > >	os;
			os << *this;
			return os.str();
		}
	};



	template< typename T >
	constexpr auto as_ascii( T && t_ )						noexcept	{
		using std::data, std::size;
		return Ascii_t{ data( t_ ), size( t_ ) };
	}

	template< pax::Character C, std::size_t N >	
	constexpr auto as_ascii( C( & c_ )[ N ] )				noexcept	{
		constexpr std::size_t 	n = ( N == 0 ) ? 0u : N-1u;
		return Ascii_t{ c_, n };
	}

	template< Character Ch >
	constexpr auto as_ascii( Ch * &c_ )						noexcept	{
		auto				itr = c_;
		if( itr && *itr )	while( *( ++itr ) );
		return Ascii_t{ c_, std::size_t( itr - c_ ) };
	}

	template< Character Ch >
	constexpr auto as_ascii( Ch * const &c_ )				noexcept	{
		auto				itr = c_;
		if( itr && *itr )	while( *( ++itr ) );
		return Ascii_t{ c_, std::size_t( itr - c_ ) };
	}



	template< typename ...Args >
	[[nodiscard]] constexpr auto ascii( Args && ...args_ )	noexcept	{
		return as_ascii( std::forward< Args >( args_ )... ).ascii();
	}


}	// namespace pax
