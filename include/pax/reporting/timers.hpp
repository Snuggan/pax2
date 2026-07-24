//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "seconds_to_string.hpp"	// seconds_to_string

#include <iostream>
#include <chrono>
#include <sys/resource.h>			// Linux: rusage, getrusage, RUSAGE_SELF


namespace pax {

	class Time_local {
		bool								m_auto;

	public:
		Time_local( const bool autoout_ = false ) : m_auto{ autoout_ } {}
		~Time_local() 					{	if( m_auto )	std::cout << now_string();		}
	
		std::string now_string() const	{
			// const auto now				  = std::chrono::system_clock::now();
			// const auto seconds			  = std::chrono::time_point_cast< std::chrono::seconds >( now );
			// const auto local_time		  = std::chrono::zoned_time{ std::chrono::current_zone(), seconds };
			// return std::format( "%T", local_time );
			return std::string( "<local time not implemented>" );
		}
	};


	class Time_utc {
		bool								m_auto;

	public:
		Time_utc( const bool autoout_ = false ) : m_auto{ autoout_ } {}
		~Time_utc() 					{	if( m_auto )	std::cout << now_string();		}
	
		std::string now_string() const	{
			const auto now				  = std::chrono::system_clock::now();
			const auto seconds			  = std::chrono::time_point_cast< std::chrono::seconds >( now );
			return std::vformat( "{:%T} (utc) ", std::make_format_args( seconds ) );
		}
	};


	/// Wrapper for the timers.
	template< typename Timer >
	class Duration : public Timer {
		std::string							m_suffix;
		bool								m_auto;

	public:
		Duration( const std::string & suffix_ = "", const bool autoout_ = false ) 
			: m_suffix{ suffix_ }, m_auto{ autoout_ } {}

		~Duration() 					{	if( m_auto )	std::cout << now_string();		}
	
		std::string now_string() const	{
			return Timer::valid()		?	std::format( "{} ({}) ", pax::seconds_to_string( Timer::seconds() ), 
																	 Timer::suffix() )
										:	std::format( "not_valid ({}) ", Timer::suffix() );
		}
	};


	/// Based on std::chrono::steady_clock.
	class Wall_timer {
		std::chrono::time_point< std::chrono::steady_clock >	m_start;

	public:
		Wall_timer() : m_start{ std::chrono::steady_clock::now() } {}
		double seconds() 		const	{
			return std::chrono::duration_cast< std::chrono::nanoseconds >( 
				std::chrono::steady_clock::now() - m_start 
			).count()*1e-9;
		}	
		static auto suffix()			{	return "wall";									}
		static constexpr bool valid()	{	return true;									}
	};
	using Wall_duration					  = Duration< Wall_timer >;


	/// Based on rusage.
	/// Needs #include <sys/resource.h>.
	class Usr_sys_timer {
		rusage 								m_ru;
		bool								m_ok;

		static double ru_seconds( const timeval tv_ ) {
			return tv_.tv_sec + tv_.tv_usec*1e-6;
		}

	public:
		Usr_sys_timer() 				:	m_ok{ !getrusage( RUSAGE_SELF, &m_ru ) }	   {}
		double user_seconds()	const	{	return ru_seconds( m_ru.ru_utime );				}
		double  sys_seconds()	const	{	return ru_seconds( m_ru.ru_stime );				}
		double seconds() 		const	{	return user_seconds() + sys_seconds();			}
		constexpr bool valid()	const	{	return m_ok;									}
		static auto suffix()			{	return "cpu";									}
	};
	using Usr_sys_duration				  = Duration< Usr_sys_timer >;


	/// Cpu timer ased on clock_t and clock().
	class Clock_timer {
		clock_t 							m_start;

	public:
		Clock_timer() : m_start{ clock() } {}
		double seconds() 		const	{	return ( clock() - m_start )/CLOCKS_PER_SEC;	}	
		static constexpr bool valid()	{	return true;									}
		static auto suffix()			{	return "clock";									}
	};
	using Clock_duration				  = Duration< Clock_timer >;


}	// namespace pax