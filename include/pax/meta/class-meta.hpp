//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <array>
#include <string_view>
#include <span>


namespace pax {

	template< typename T, typename ...U > 
	struct class_meta {
		static constexpr auto 	Rank		  = sizeof ...( U );
		using 					value_type	  = T;
		using 					types		  = std::tuple< U... >;

		template< typename ...Strings >	requires( sizeof ...( Strings ) == Rank )
		constexpr class_meta( Strings && ...ids_ ) noexcept 
			: m_ids{ std::string_view( std::forward< Strings >( ids_ ) )... } {}

		constexpr auto names()	const noexcept	{
			return std::span< const std::string_view, Rank >{ m_ids.data(), Rank };
		}
		
	private:
		std::array< std::string_view, Rank >	m_ids;
	};

}	// namespace pax
