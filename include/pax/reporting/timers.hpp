//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "seconds_to_string.hpp"	// seconds_to_string

#if __has_include( <boost/timer/timer.hpp> )
#	include <boost/timer/timer.hpp>
#	define __has_boost_timer__	true
#else
#	define __has_boost_timer__	false
#endif

#include <iostream>
#include <chrono>
#include <sys/resource.h>			// Linux: rusage, getrusage, RUSAGE_SELF


namespace pax {


	class Time_local {
		bool										m_auto;

	public:
		Time_local( const bool autoout_ = false ) : m_auto{ autoout_ } {}
		~Time_local() 							{	if( m_auto )	std::cout << now_string();		}
	
		static std::string now_string() 		{
			// const auto now				  = std::chrono::system_clock::now();
			// const auto seconds			  = std::chrono::time_point_cast< std::chrono::seconds >( now );
			// const auto local_time		  = std::chrono::zoned_time{ std::chrono::current_zone(), seconds };
			// return std::format( "%T", local_time );
			return std::string( "<local time not implemented>" );
		}
	};


	class Time_utc {
		bool										m_auto;

	public:
		Time_utc( const bool autoout_ = false ) :	m_auto{ autoout_ } {}
		~Time_utc() 							{	if( m_auto )	std::cout << now_string();		}
	
		static std::string now_string() 		{
			const auto now						  = std::chrono::system_clock::now();
			const auto seconds					  = std::chrono::time_point_cast< std::chrono::seconds >( now );
			return std::vformat( "{:%T} (utc) ", std::make_format_args( seconds ) );
		}
	};


	/// Wrapper for the timers.
	template< typename Timer >
	class Duration : public Timer {
		bool										m_auto;

	public:
		Duration( const bool autoout_ = false )	:	m_auto{ autoout_ } {}

		~Duration() 							{	if( m_auto )	std::cout << now_string();		}
	
		std::string now_string()		const	{
			return Timer::valid()
				?	std::format( "{} ({}) ", pax::seconds_to_string( Timer::seconds() ), Timer::suffix() )
				:	std::format( "not_valid ({}) ", Timer::suffix() );
		}
	};


	/// Based on std::chrono::steady_clock.
	class Wall_timer {
		std::chrono::time_point< std::chrono::steady_clock >	m_start;

	public:
		Wall_timer() : m_start{ std::chrono::steady_clock::now() } {}
		double seconds() 				const	{
			return std::chrono::duration_cast< std::chrono::nanoseconds >( 
				std::chrono::steady_clock::now() - m_start 
			).count()*1e-9;
		}	
		static auto suffix()					{	return "wall";									}
		static constexpr bool valid()			{	return true;									}
	};
	using Wall_duration							  = Duration< Wall_timer >;


	/// Based on rusage.
	/// Needs #include <sys/resource.h>.
	///
	/// Measurement Method	Includes I/O	Includes			Measures Only
	/// Measurement Method	Operations		Other Processes		Program Time
	/// ---------------------------------------------------------------------
	/// C++ Timer			No				No					Yes
	/// Sh Utility time		Yes				Yes					No
	///
	/// - I/O Operations: The shell utility time accounts for the time spent on input/output operations,
	///   which can significantly increase the reported time.
	/// - Other Processes: The shell utility also considers the time consumed by other processes running
	///   on the system, which can lead to longer reported times.
	/// - Program-Specific Time: Your C++ timer likely measures only the time spent executing the program's
	///   code, excluding any delays caused by system-level operations or other running programs.
	class Usr_sys_timer {
		rusage 										m_ru;
		bool										m_ok;

		static double ru_seconds( const timeval tv_ ) {
			return tv_.tv_sec + tv_.tv_usec*1e-6;
		}

	public:
		Usr_sys_timer() 						:	m_ok{ !getrusage( RUSAGE_SELF, &m_ru ) }	   {}
		double user_seconds()			const	{	return ru_seconds( m_ru.ru_utime );				}
		double  sys_seconds()			const	{	return ru_seconds( m_ru.ru_stime );				}
		double seconds() 				const	{	return user_seconds() + sys_seconds();			}
		constexpr bool valid()			const	{	return m_ok;									}
		static auto suffix()					{	return "cpu";									}
	};
	using Usr_sys_duration						  = Duration< Usr_sys_timer >;
	
	
	class Boost_timer {
		static_assert( __has_boost_timer__, "Boost timer not available on this system" );
		boost::timer::cpu_timer						m_timer;

	public:
		Boost_timer() 							:	m_timer{}									   {}
		double wall_seconds()			const	{	return m_timer.elapsed().wall;					}
		double user_seconds()			const	{	return m_timer.elapsed().user;					}
		double  sys_seconds()			const	{	return m_timer.elapsed().system;				}
		double  cpu_seconds()			const	{	return user_seconds() + sys_seconds();			}
		double seconds() 				const	{	return cpu_seconds();							}
		static constexpr bool valid()			{	return true;									}
		static auto suffix()					{	return "b-cpu";									}
	};
	using Boost_duration						  = Duration< Boost_timer >;


	/// Cpu timer ased on clock_t and clock().
	class Clock_timer {
		clock_t 									m_start;

	public:
		Clock_timer() : m_start{ clock() } {}
		double seconds() 				const	{	return ( clock() - m_start )/CLOCKS_PER_SEC;	}	
		static constexpr bool valid()			{	return true;									}
		static auto suffix()					{	return "clock";									}
	};
	using Clock_duration						  = Duration< Clock_timer >;


}	// namespace pax