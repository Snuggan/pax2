//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

// Include files from https://github.com/nlohmann/json
#include "../external/nlohmann-json/json.hpp"

#include <fstream>
#include <sstream>


#define PAX_NLOHMANN_JSON
#define PAX_AS_JSON( __x__ ) pretty_json( Debug{}, ( __x__ ) )


namespace pax {
	
	using Json_value  = nlohmann::json;
	
	
	template< typename T >
	struct Serialize : nlohmann::adl_serializer< T > {
		static auto as( const Json_value & j_ ) {
			return j_.template get< T >();
		}
	};


	template< typename T >
	T from_json( const Json_value & j_ ) {
		return Serialize< T >::as( j_ );
	}

	template< typename T >
	Json_value to_json( const T & t_ ) {
		Json_value			j;
		Serialize< T >::assign( j, t_ );
		return j;
	}

	inline Json_value to_json( const Json_value & j_ )	{	return j_;			}

	template< typename Out, typename T >
	inline void pretty_json(
		Out					 && out_,
		const T 			  & t_ 
	) {	out_ << to_json( t_ ).dump( 2 );										}

	template< typename Out, typename T >
	inline Out & pretty_json(
		Out					  & out_,
		const T 			  & t_ 
	) {	return out_ << to_json( t_ ).dump( 2 );									}

	
	inline Json_value read_json( const std::filesystem::path & path_ ) {
		std::ifstream				f( path_ );
		return Json_value::parse( f );
	}
	

	inline std::string to_string( const Json_value & j_, const bool pretty_ = true ) {
		switch( j_.type() ) {
			case nlohmann::detail::value_t::array: 
			case nlohmann::detail::value_t::object: 
				{
					std::ostringstream			stream;
					if( pretty_ )				stream.width( 2 );
					stream << j_;
					return stream.str();
				}
				break;
			default: 
				return j_.get< std::string >();
				break;
		}
	}


	inline void save_json( 
		std::filesystem::path		path_,
		const Json_value		  & meta_, 
		const bool 					pretty_ = true
	) {
		path_.replace_extension( ".json" );
		std::ofstream 				dest( path_ );
		if( pretty_ )				dest.width( 2 );
		dest << meta_;
	}
 
}	// namespace pax
