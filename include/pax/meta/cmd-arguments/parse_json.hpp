//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "../../textual/json.hpp"


namespace pax::cmd_args {


	/// Parses a json file indo a vector of cmd arguments. 
	/** For example, 
		{
			"parameter-name": "argument1",
			"group-name": {
				"parameter-name": "argument2",
				"subname": {
					"name-id": "name"
				}
			}
		}
		will be converted into a vector of strings:
		[ parameter-name, argument2, name-id, name, parameter-name, argument1 ]
	**/
	class Parse_json{
		struct Meta {
			template< typename String_view >
			bool exists( const String_view ) const noexcept {	return true;	}
		};
		
	public:
		using String					  = std::string;
		using Vector					  = std::vector< String >;

		template< typename Meta_ = Meta >
		static Vector value(
			const Json_value				value_,
			const Meta_					  & meta_ = Meta{}
		) {
			Vector							args{};
			args.push_back( "from json" );
			parse( args, "", value_, meta_ );
			return args;
		}

		template< typename Meta_ = Meta >
		static Vector string(
			const String					json_,
			const Meta_					  & meta_ = Meta{}
		) {	return value( Json_value::parse( json_ ), meta_ );					}

		template< typename Meta_ = Meta >
		static Vector file(
			const String					path_,
			const Meta_					  & meta_ = Meta{}
		) {	return value( read_json( path_ ), meta_ );							}
		
	private:
		template< typename Meta_ >
		static void parse(
			Vector						  & args_, 
			const String					id_, 
			const Json_value			  & j_,
			const Meta_					  & meta_
		) {
			switch( j_.type() ) {
				// Add only id:
				case nlohmann::detail::value_t::null:
					args_.push_back( id_ );
					break;

				// Multiple values, repeat id:
				case nlohmann::detail::value_t::array:
					if( meta_.exists( id_ ) ) for( const auto & item : j_ ) {
						if( id_.size() )	args_.push_back( id_ );
						args_.push_back( to_string( item ) );
					}
					break;

				// New collection, 
				case nlohmann::detail::value_t::object:
					for( const auto & [ key, val ] : j_.items() )
						parse( args_, key, val, meta_ );
					break;

				// Ignore the rest:
				default:
				if( meta_.exists( id_ ) ) {
					if( id_.size() )	args_.push_back( id_ );
					args_.push_back( j_.get< std::string >() );
					// args_.push_back( to_string( j_ ) );
				}
				break;
			}
		}
	};


}	// namespace pax::cmd_args
