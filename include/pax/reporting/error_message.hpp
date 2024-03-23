//	Copyright (c) 2014-2021, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "../std/format.hpp"
#include <string_view>
#include <source_location>
#include <stdexcept>
#include <iostream>


namespace pax {
	
	using Runtime_exception = std::runtime_error;

	/// Helper: nice std::source_location string.
	inline std::string to_string( const std::source_location & sl_ ) {
		const bool ok = sl_.file_name() && *sl_.file_name();
		return std20::format( "{}:{}", ok ? sl_.file_name() : "<unspecified source location>", sl_.line() );
	}

	/// Helper: nice error report from a std::error_code.
	inline std::string to_string( 
		const std::error_code 			ec_, 
		const char					  * prefix_	= "" 
	) {	return ec_ ? std20::format( "{}error {} ({})", prefix_, ec_.value(), ec_.message() ) : std::string{};		}

	/// Helper: nice error report from a std::error_condition.
	inline std::string to_string( 
		const std::error_condition 		ec_, 
		const char					  * prefix_	= "" 
	) {	return ec_ ? std20::format( "{}error {} ({})", prefix_, ec_.value(), ec_.message() ) : std::string{};		}


	/// Helper: a std::error_code from an int errno value.
	inline std::error_code make_error_code( const int errno_ ) 
	{	return std::error_code( errno_, std::system_category() );													}


	/* Error message without source location. */
	inline auto user_error_message(
		const std::string_view			message_,
		const std::error_code			ec_ = std::error_code{}
	) {	return Runtime_exception( std20::format( "{}{}", message_, to_string( ec_, "\nPossibly due to\t" ) ) );	}

	/* Error message with error code and source location. */
	inline auto error_message( 
		const std::string_view			message_,
		const std::error_code			ec_ = std::error_code{},
		const std::source_location	  & sl_ = std::source_location::current()
	) {	return user_error_message( std20::format( "{}: {}", to_string( sl_ ), message_ ), ec_ );					}


	/// At destruction this class will write a message to std::cerr unless previously deactivated.
	class Final_error_message {
		std::string						m_message;

	public:
		Final_error_message( const Final_error_message & )				= delete;
		Final_error_message( Final_error_message && )					= delete;
		Final_error_message & operator=( const Final_error_message & )	= delete;
		Final_error_message & operator=( Final_error_message && )		= delete;

		Final_error_message( const std::source_location sl_ = std::source_location::current() ) 
			: Final_error_message{ "Something went wrong...", sl_ } {}

		Final_error_message(
			const std::string_view		message_, 
			const std::source_location	sl_ = std::source_location::current()
		) :	m_message{ std::format( "{}: error: {}\n", to_string( sl_ ), message_ ) } {}
		
		~Final_error_message()	{	if( m_message.size() )	std::cerr << m_message;		}
		void deactivate()		{	m_message.clear();									}
	};



	/// Add a second string in a formatted way.
	/** - After the first '\t' of a line, text is output starting from a fixed character position.
		- Each line except the first is prefixed with a tab.			**/
	inline std::string format_message(
		std::string						result,
		std::string_view				message_
	) {
		static constexpr auto split = []( std::string_view v, char c ) {
			auto i					  = v.find( c );
			i						  = ( i >= v.size() ) ? v.size() : i;
			return make_pair( v.substr( 0, i ), ( i >= v.size() ) ? "" : v.substr( i+1 ) );
		};
		while( message_.size() ) {
			const auto lines		  = split( message_, '\n' );
			const auto line1		  = split( lines.first, '\t' );
			result = line1.second.empty()
				? std20::format( "{}\n\t{}", result, line1.first )
				: std20::format( "{}\n\t{: <20}{}", result, std20::format( "{}:", line1.first ), line1.second );
			message_				  = lines.second;
		}
		return result;
	}

	/// Output the exception string. 
	template< typename Ch, typename Tr >
	auto & operator<<(
		std::basic_ostream< Ch, Tr >  & out_,
		const Runtime_exception		  & exception_
	) {	return out_ << exception_.what();															}


	/// Add textual lines to the exception string.
	inline Runtime_exception & operator<<(
		Runtime_exception			  & exception_,
		const std::string_view			message_
	) {	return exception_ = Runtime_exception{ format_message( exception_.what(), message_ ) };	}

	/// Add textual lines to the exception string. 
	inline decltype( auto ) operator<<(
		Runtime_exception			 && exception_,
		const std::string_view			message_
	) {	return exception_ = Runtime_exception{ format_message( exception_.what(), message_ ) };	}

}	// namespace pax



#define PAX_REPORT_IF( _act_, _test_ )		\
	if(  static_cast< bool >( _test_ ) )	_act_ pax::error_message( "Not allowed: " #_test_ ) 

#define PAX_REPORT_UNLESS( _act_, _test_ )	\
	if( !static_cast< bool >( _test_ ) )	_act_ pax::error_message( "Failed: " #_test_ ) 


/// Unless the condition is met, throw an exception.
/** @{ **/
#define PAX_THROW_IF( _test_ )				PAX_REPORT_IF    ( throw, _test_ )
#define PAX_THROW_UNLESS( _test_ )			PAX_REPORT_UNLESS( throw, _test_ )
/** @} **/

/// Unless the condition is met, display a warning. 
/** @{ **/
#define PAX_WARN_IF( _test_ )				PAX_REPORT_IF    ( std::cerr <<, _test_ )
#define PAX_WARN_UNLESS( _test_ )			PAX_REPORT_UNLESS( std::cerr <<, _test_ )
/** @} **/
