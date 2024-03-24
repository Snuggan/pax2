//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "../../concepts.hpp"
#include "../../reporting/error_message.hpp"

#include <span>
#include <string_view>
#include <vector>
#include <map>
#include <sstream>


namespace pax { namespace cmd_args {
	
	
	constexpr std::string_view id_flag_on	  = "true";
	constexpr std::string_view id_flag_off	  = "false";
	constexpr std::string_view id_anonymous   = "";

	template< Character Char >
	constexpr bool is_flag_on ( std::basic_string_view< Char > tv_ )	noexcept	{	return tv_ == id_flag_on;		}

	template< Character Char >
	constexpr bool is_flag_off( std::basic_string_view< Char > tv_ )	noexcept	{	return tv_ == id_flag_off;		}

	template< Character Char >
	constexpr bool is_anonymous( std::basic_string_view< Char > tv_ )	noexcept	{	return tv_ == id_anonymous;		}

	

	template< Character Char = char >
	class Parameter_set;
	
	using Parameters = Parameter_set< char >;
	
	
	/// Convert from String_view to T. 
	/** Override this function for other types than the basic ones.		**/
	template< typename T >
	struct Convert {
		template< String Str >
		static auto convert( const Str & str_ ) {
			T		result;
			std::istringstream( str_ ) >> result;
			return result;
		}
	};
	
	template< String Str >
	struct Convert< Str > {
		static constexpr auto convert( Str && str_ )		noexcept	{	return std::forward< Str >( str_ );		}
		static constexpr auto convert( const Str & str_ )	noexcept	{	return str_;							}
	};



	/// An Argument is an ensamble of arguments, addressable by parameter name. 
	/** This is what you get from the Parameters::parse. It contains all the argument values, so you can
		destroy the Parameters, Group, and Param varaiables and only keep Arguments. 
		You may declare it const, as all public member functions are const. 
	**/
	template< Character Char >
	class Arguments {
		using String								  = std::basic_string< Char >;
		using String_view							  = std::basic_string_view< Char >;
		using Container								  = std::vector< String >;
		std::map< String, Container >					m_map;
		
	public:
		using value_type							  = String;

		/// Constant access to parameter param_'s arguments. If no argument is given, the anonymous arguments are returned. 
		std::span< const value_type > operator()( const String_view param_ = id_anonymous ) const {
			const auto itr							  = m_map.find( String( param_ ) );
			if( itr == m_map.end() )					throw user_error_message( std20::format( "Unknown parameter '--{}'.", param_ ) );
			return { itr->second.data(), itr->second.size() };
		}

		/// Return parameter param_'s argument as type T. (Throws if there is not just one argument.)
		template< typename T >
		T cast( const String_view param_ ) 				const			{
			const auto v							  = operator()( param_ );
			switch( v.size() ) {
				case 0: 	throw  user_error_message( std20::format( "Parameter '--{}' has no value.", param_ ) );
				case 1: 	return Convert< T >::convert( v.front() );
				default: 	throw  user_error_message( std20::format( "Parameter '--{}' has multiple values.", param_ ) );
			}
		}
		
		/// Return the argument for param_ if it is any of one_of_. (Throws, otherwise.)
		template< typename Container >
		value_type one_of( const String_view param_, const Container & one_of_ ) const {
			const auto v							  = cast< String_view >( param_ );
			for( const String_view one : one_of_ )		if( v == one )		return String( v );
			throw user_error_message( std20::format( "'{}' (value of '--{}') is not one of the required values.", v, param_ ) );
		}
		
		/// Is the flag argument 'on'? (Throws for non-flag parameters.)
		bool flag( const String_view param_ )			const			{
			const auto v							  = operator()( param_ );
			if( v.size() == 1 ) {
				if( is_flag_on ( String_view( v.front() ) ) )	return true;
				if( is_flag_off( String_view( v.front() ) ) )	return false;
			}
			throw user_error_message( std20::format( "Parameter '--{}' is not a flag.", param_ ) );
		}
		
		/// const begin() iterator.
		auto begin()									const noexcept	{	return m_map.cbegin();				}
		
		/// const end() iterator.
		auto end()										const noexcept	{	return m_map.cend();				}

	private:
		friend Parameter_set< Char >;

		Arguments()									  = default;
		Arguments( const Arguments & )				  = default;
		Arguments( Arguments && ) 					  = default;
		Arguments & operator=( const Arguments & )	  = default;
		Arguments & operator=( Arguments && ) 		  = default;



		// Insert a new parameter. Used only by Parameters::parse.
		// Param = Param< Char >;
		template< typename Param >
		void insert( const Param & param_ ) {
			const auto result = m_map.insert( std::pair( param_.id(), Container{} ) );
			if( !result.second )	throw user_error_message( std20::format( "Multiple parameters with the same name: '--{}'.", param_.id() ) );
			if     ( param_.is_on_flag()  )	result.first->second.push_back( value_type( id_flag_on  ) );
			else if( param_.is_off_flag() )	result.first->second.push_back( value_type( id_flag_off ) );
		}



		// Set the value of existing parameter param_ to value_. Used only by Parameters::parse.
		// Param_id = std::pair< Param< Char > *, String_view >;
		template< typename Param >
		void set( 
			const Param				  & param_,			// Parameter definition. 
			const String_view 			arg_id_, 		// Parameter id, as stated in the argument list -- coul bed pre-/postfixed. 
			const String_view 			value_ 			// The argument value (or "" ).
		) {
			Container & vals						  = get( param_.id() );
			if( param_.is_flag() ) {
				vals.front()						  = String( ( arg_id_ == param_.id() ) ? id_flag_on : id_flag_off );
				
				// The value following a flag will go to anonymous. 
				if( !value_.empty() )					get( id_anonymous ).emplace_back( value_ );
			} else if( arg_id_ != param_.id() ) {
				throw user_error_message( std20::format( "Unknown parameter '--{}'.", arg_id_ ) );
			} else if( param_.is_multiple() ) {
				// The parameter can handle multiple values -- check for value delimiter.
				auto									next{ value_.begin() };
				auto									itr{ next };
				while( itr < value_.end() ) {
					while( ( next != value_.end() ) && ( *next != Param::template delimiter< Char > ) )
						++next;
					vals.emplace_back( itr, next );
					itr = ++next;
				}
			} else {
				vals.emplace_back( value_ );
			}
			param_.check( vals );
		}



		// Find the mapped value of parameter param_ (param_ might have prefix 'no-').
		Container & get( const String_view param_ ) {
			const auto itr							  = m_map.find( String( param_ ) );
			if( itr == m_map.end() )					throw user_error_message( std20::format( "Unknown parameter '--{}'.", param_ ) );
			return itr->second;
		}



		// Check the parameter/values pair and complement the values if necessary (with default value etc.). 
		template< typename Param >
		void finalize( const Param & param_ ) 		{	param_.finalize( get( param_.id() ) );					}
	};



	/// Stream the contents of an Arguments.
	template< Character Char >
	auto & operator<<( std::basic_ostream< Char > & out_, const Arguments< Char > & arguments_ ) {
		arguments_.stream( out_ );
		return out_;
	}

	
}	// namespace cmd_args


	template< Character Char >
	auto to_string( const cmd_args::Arguments< Char > & arguments_ ) {
		std::ostringstream		result;
		result << arguments_;
		return result;
	}


	template< typename T > struct Serialize;

	template< Character Char >
	struct Serialize< cmd_args::Arguments< Char > > {
		using Args	  = cmd_args::Arguments< Char >;
		using Sview	  = std::basic_string_view< Char >;

		template< typename J >
		static void assign( J & j_, const Args & arguments_ ) {	
			// Output named arguments.
			for( const auto & ref : arguments_ ) if( ref.first.size() ) {
				Sview	id = ref.first;
						id = cmd_args::is_anonymous( id ) ? "<anonymous>" : id;
				if( ref.second.size() == 1 ) {
					const Sview											item	 = ref.second.front();
					if( ( id == "help" ) || ( id == "version" ) )		{}		   // Ignore these.
					else if( cmd_args::is_flag_on ( item ) )			j_[ id ] = true;
					else if( cmd_args::is_flag_off( item ) )			j_[ id ] = false;
					else												j_[ id ] = item;
				} else													j_[ id ] = ref.second;		
			}
		}
	};



}	// namespace pax
