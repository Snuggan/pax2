//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include <pax/concepts.hpp>

#include <utility>
#include <string_view>
#include <span>


namespace pax {

	template< typename T, typename ...U > 
	struct class_meta {
		static constexpr auto 		Rank		  = sizeof ...( U );
		using 						value_type	  = T;
		using 						types		  = std::tuple< U... >;
		using 						View		  = std::string_view;

		template< String ...Strs > requires( sizeof ...( Strs ) == Rank )
		constexpr class_meta( Strs && ...names_ ) : m_names{ View( std::forward< Strs >( names_ ) )... } {}

		constexpr auto names()	const noexcept	{
			return std::span< const View, Rank >{ m_names.data(), Rank };
		}
		
	private:
		std::array< View, Rank >	m_names;
	};

}	// namespace pax
