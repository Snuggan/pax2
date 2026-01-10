//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include "../external/type_name_rt.hpp"		// type_name_rt
#include "../concepts.hpp"

#include <string>
#include <source_location>
#include <iostream>			// std::cerr
#include <sstream>
#include <exception>		// std::terminate


namespace pax {

	/// A simple class that outputs itself + '\n' to watever stream passed to its constructor.
	class Debug {
		std::ostream						  & m_out;
		
	public:
		template< typename T >
		class Object_info;
		
		Debug( const Debug & )				  = delete;
		Debug & operator=( const Debug & )	  = delete;
		Debug( Debug && )					  = delete;
		Debug & operator=( Debug && )		  = delete;
		
		Debug( 
			std::ostream					  & out_		  = std::cerr, 
			const std::source_location			location_	  = std::source_location::current()
		) : m_out{ out_ } {
			( m_out << location_.file_name() << ':' << location_.line() << ": " ).flush();
		}
		
		template< typename T >
		Debug( 
			std::ostream					  & out_		  = std::cerr, 
			const std::source_location			location_	  = std::source_location::current()
		) : Debug{ out_, location_ } {
			*this << Object_info< T >();
		}
		
		template< typename T >
		Debug( 
			T								 && t_,
			std::ostream					  & out_		  = std::cerr, 
			const std::source_location			location_	  = std::source_location::current()
		) : Debug{ out_, location_ } {
			*this << Object_info( t_ );
		}

        ~Debug() {	try {	m_out << std::endl;		} catch( ... ) {}	}	// '\n' and flush.

        template< typename T >
		std::ostream & operator<<( const T & t_ ) const {
			( m_out << t_ ).flush();
			return m_out;
		}
	};


	template< typename T >
	class Debug::Object_info {
		std::string		m_value{};
		using norefT = std::remove_reference_t< T >;
		
		template< typename U >
		static void named_output(
			std::ostream			  & out_,
			const char				  * name_,
			const U					  & value_
		) {
			const std::string			name( name_ );
			out_ << "\n\t\"" << name << "\": " 
				 << std::string( ( name.size() >= 17 ) ? 0 : 17 - name.size(), ' ' ) 
				 << '"' << value_ << '"';
		}
		
	public:
		Object_info()	noexcept = default;
		Object_info( T t_ ) {
			std::ostringstream		str{};
			str << t_;
			m_value = str.str();
		}

		static constexpr std::string_view as_string()	noexcept	{	return type_name_rt< T >;		}

		friend std::ostream & operator<<( std::ostream & out_, const Object_info oi_ ) {
			const auto typestr =	as_string();

			out_	<< "\n\"Object_info< " 
					<< typestr
					<< " >( " 
					<< oi_.m_value 
					<< " )\": {";
			if( oi_.m_value.size() ) {
				named_output( out_, "value",			oi_.m_value );
			}
			named_output( out_, "type",					typestr );
			named_output( out_, "size",					sizeof( T ) );
			if constexpr( traits::has_size< T > )
				named_output( out_, "extent",			traits::extent_v< T > );
			
			std::string		cats{};
			if( std::is_arithmetic_v< T > )				cats+= "arithmetic, ";
			if( std::is_floating_point_v< T > )			cats+= "floating point, ";
			if( std::is_integral_v< T > )				cats+= "integral, ";
			if( std::is_signed_v< T > )					cats+= "signed, ";
			if( std::is_unsigned_v< T > )				cats+= "unsigned, ";
			if( std::is_void_v< T > )					cats+= "void, ";
			if( std::is_array_v< T > )					cats+= "array, ";
			if( std::is_enum_v< T > )					cats+= "enum, ";
			if( std::is_union_v< T > )					cats+= "union, ";
			if( std::is_class_v< T > )					cats+= "class, ";
			if( std::is_function_v< T > )				cats+= "function, ";
			if( std::is_pointer_v< T > )				cats+= "pointer, ";
			if( std::is_rvalue_reference_v< T > )		cats+= "rvalue ref, ";
			if( std::is_lvalue_reference_v< T > )		cats+= "lvalue ref, ";
			if( std::is_member_object_pointer_v< T > )	cats+= "member object ptr, ";
			if( std::is_member_function_pointer_v< T > )cats+= "member function ptr, ";
			cats.pop_back();	cats.pop_back();
			named_output( out_, "categories", 			cats );
			cats.clear();

			if( std::is_fundamental_v< T > )			cats+= "fundamental, ";
			if( std::is_scalar_v< T > )					cats+= "scalar, ";
			if( std::is_object_v< T > )					cats+= "object, ";
			if( std::is_compound_v< T > )				cats+= "compound, ";
			if( std::is_reference_v< T > )				cats+= "reference, ";
			if( std::is_member_pointer_v< T > )			cats+= "member ptr, ";
			cats.pop_back();	cats.pop_back();
			named_output( out_, "composite", 			cats );
			cats.clear();

			if( std::is_const_v< T > )					cats+= "const, ";
			if( std::is_volatile_v< T > )				cats+= "volatile, ";
			if( std::is_trivially_default_constructible_v< T > )	cats+= "trivially default constructable, ";
			if( std::is_trivially_copyable_v< T > )		cats+= "trivially copyable, ";
			if( std::is_standard_layout_v< T > )		cats+= "standard layout, ";
			if( std::is_empty_v< T > )					cats+= "empty, ";
			if( std::is_polymorphic_v< T > )			cats+= "polymorphic, ";
			if( std::is_abstract_v< T > )				cats+= "abstract, ";
			if( std::is_final_v< T > )					cats+= "final, ";
			if( std::is_aggregate_v< T > )				cats+= "aggregate, ";
			cats.pop_back();
			cats.pop_back();
			named_output( out_, "properties", 			cats );
			cats.clear();

			out_ << "\n}";
			return out_;
		}
	};


	/// A simple class that outputs itself + "crashed" + '\n' to watever stream passed to its constructor.
	struct Crash : public Debug {
		Crash( 
			std::ostream				  & out_		  = std::cerr, 
			const std::source_location		location_	  = std::source_location::current()
		)	: Debug{ out_, location_ }	{}

	    ~Crash() 						{
			*this << " -- Crashed!\n";
			std::terminate( /* EXIT_FAILURE */ );
		}
	};

}		// namespace pax
