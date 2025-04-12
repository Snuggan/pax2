//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/concepts.hpp>

#include <utility>
#include <string_view>
#include <span>


namespace pax {

	template< typename T, typename ...U > 
	class class_meta {
		static constexpr auto 	N			  = sizeof ...( U );
		using 					View		  = std::string_view;
		std::array< View, N >	m_names;

	public:
		using 					value_type	  = T;
		using 					types		  = std::tuple< U... >;
		static constexpr auto 	Rank		  = N;

		template< String ...Strs > requires( sizeof ...( Strs ) == Rank )
		constexpr class_meta( Strs && ...names_ ) : m_names{ View( std::forward< Strs >( names_ ) )... } {}

		constexpr auto names()	const noexcept	{
			return std::span< const View, Rank >{ m_names.data(), Rank };
		}
	};

}	// namespace pax
