//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#pragma once

#include "seconds_to_string.hpp"


namespace pax {

	class Progress;

	struct Reporter {
		virtual ~Reporter()						  = default;
		virtual void report( const Progress & )		const {};
		virtual void clear()						const {};
	};



	/// Progress reporter that gives many statistics on execution progress.
	/** The following statistics are reported:
		- #seconds(): how much time has elapsed.
		- #count(): how many items have been processed.
		- #speed() and #wall_speed(): speed, items/time-unit.
		- #target(): how many items are we to do in total. *
		- #etl(): estimated time left [to execute]. *
	**/
	class Progress {

		struct Timer {
			using clock				  = std::conditional_t< std::chrono::high_resolution_clock::is_steady, 
															std::chrono::high_resolution_clock, std::chrono::steady_clock >;
			using duration			  = typename clock::duration;

			Timer() 					noexcept : m_start( clock::now() ) {};
			duration operator()()		const noexcept {	return clock::now() - m_start;		}
	
		private:	
			typename clock::time_point	m_start;
		};


		std::size_t						m_end_count{}, m_now_count{};
		Timer							m_clock;
		mutable Timer::duration			m_next_time{ std::chrono::seconds( 10 ) };
		mutable std::size_t				m_next_update{};
		std::shared_ptr< Reporter >		m_reporter{};
		
		

	public:
		using count_type		  = std::size_t;
		
		/// Use this constructor to deactivate progress reporting.
		Progress() noexcept : m_next_update{ -1u } {}
		
		/// If end_ is zero, progress will be active but there will obviously be no ETL.
		template< typename Rep >
		Progress( const Rep & reporter_, const count_type end_ = 0 ) noexcept 
			: m_end_count( end_ ), m_reporter{ std::make_shared< Rep >( reporter_ ) } {}
		
		/// Add one to the counter, report progress if necessary.
		void operator++()							{	if( ( ++m_now_count      ) >= m_next_update )	update();	}
		
		/// Add argument to counter, report progress if necessary.
		void operator+=( const count_type & inc_ )	{	if( ( m_now_count+= inc_ ) >= m_next_update )	update();	}
		
		/// Present count.
		count_type count()	 		const noexcept	{	return m_now_count;		}

		/// The target count.
		count_type target() 		const noexcept	{	return m_end_count;		}
		
		/// Items per second (wall time).
		double speed()				const noexcept	{
			const auto wall		  = m_clock();
			return wall.count() ? ( count() / seconds_to_double( wall ) ) : 0.0;
		}
		
		/// Estimated Time Left, in seconds.
		double etl() 				const noexcept	{
			return ( ( count() <= 0 ) || ( target() <= count() ) )
					? 0
					: ( target() - count() ) / speed();
		}

		/// Update status report, if necessary.
		void update()								{
			constexpr Timer::duration				seconds1{ std::chrono::seconds( 1 ) };
			constexpr Timer::duration				seconds5{ std::chrono::seconds( 5 ) };
			const auto								wall{ m_clock() };

			// Don't show anything until after a delay and not even then, if the process is almost finished.
			if( ( wall < m_next_time ) && ( ( wall < seconds5 ) || ( count() > 0.75*target() ) ) )	return;
			m_next_time							  = wall + seconds1;
			
			m_reporter->report( *this );

			m_next_update						  = count() + 0.01*target();
			if( wall.count() ) {
				const count_type					next( count() + count()/seconds_to_double( wall ) );
				if( next < m_next_update )			m_next_update = next;
			}
			if( m_next_update <= count() )			m_next_update = count() + 1;
		}

		/// Print status message to stderr.
		std::string report()		const		{
			const auto sp{ speed() };
			std::string message =
				( sp > 2 )		? std20::format( "{:.1f}/s",   sp ) :
				( sp > 0.001 )	? std20::format( "{:.1f}/h",   sp*3600 ) :
				( sp > 5.5e-6 )	? std20::format( "{:.1f}/24h", sp*3600*24 ) :
								  std20::format( "<0.5/24h"  );

  			return ( ( count() < target() )
				?	std20::format( "{} of {} ({:.1f}%) processed doing {}, ETL: {}", 
						count(), target(), ( 1000*count()/target() )/10.0, message, 
						seconds_to_string( seconds_to_double( std::chrono::duration< double, std::ratio< 1, 1 > >( etl() ) ) ) )
				:	std20::format( "{} processed doing {}", count(), message )
			);
		}

		/// Progress report is removed and/or cleared.
		void clear() 				const		{	m_reporter->clear();	};
	};



	class Textual : public Reporter {
		std::string					m_text{};
		mutable unsigned			m_last_length{};

	public:
		/// if end_ is zero, progress will be active but there will obviously be no ETL.
		Textual( const std::string text_ = "" ) 
			: m_text{ std::string( " " ) + text_ + ( text_.empty() ? "" : ": " ) }, m_last_length{} {}

		Textual( const Textual & ) 								=	default;
		Textual( Textual && ) 						noexcept	=	default;
		Textual & operator=( const Textual & )					=	default;
		Textual & operator=( Textual && ) 			noexcept	=	default;
		~Textual()												{	try{ clear(); } catch( ... ) {}		}

		/// Print status message to stderr.
		void report( const Progress & pr_ )			const override	{
  			const std::string		message{ m_text + pr_.report() };
			const auto				diff{ ( m_last_length > message.size() ) ? ( m_last_length - message.size() ) : 0 };
			m_last_length		  = message.size();
			std::fprintf( stderr, "%s%s\r", message.c_str(), std::string( diff, ' ' ).c_str() );
		}
	
		/// Clear the status report from stderr.
		void clear()								const override	{
			std::fprintf( stderr, "%s\r", std::string( m_last_length, ' ' ).c_str() );
			m_last_length	  = 0;
		}
	};
	
	
	
	inline Progress textual_progress(
		const std::string 		  & text_	= "",	///< Prefix string.
		const std::size_t			end_	= 0		///< Total number of iterations.
	) {	return Progress{ Textual{ text_ }, end_ };					}

}	// namespace pax
