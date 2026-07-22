//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <array>
#include <string>


namespace pax {

	template< typename Object, typename ... Parts > 
	struct Table_meta {
		static constexpr auto 	rank			  = sizeof ... ( Parts );
		using 					object_type		  = Object;
		using 					types			  = std::tuple< Parts ... >;

		template< typename ... Str >				requires( sizeof ...( Str ) == rank )
		constexpr Table_meta( Str && ... ids_ )		noexcept 
			: m_ids{ std::string( std::forward< Str >( ids_ ) ) ... } {}

		constexpr const auto & names()				const noexcept	{	return m_ids;		}
		
	private:
		std::array< std::string, rank >				m_ids;
	};
	

	template< typename Object > struct Object_meta;	
	// {
	// 	constexpr auto value = Table_meta< A, int, unsigned, std::string >{ "A", "B", "C" };
	// }


	namespace atest {
		struct A {};
		constexpr Table_meta< A, int, unsigned, std::string > meta_A0( "A", "B", "C" );
		static_assert( meta_A0.rank == 3 );
		static_assert( meta_A0.names()[ 1 ] == "B" );
	}

}	// namespace pax
