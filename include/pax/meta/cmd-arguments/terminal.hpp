//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se

//	Comments are formatted for Doxygen (http://www.doxygen.nl) to read and create documentation.


#pragma once

#include <string_view>

#if defined _WIN32
#	include <windows.h>
#else
#	include <cstdio>
#	include <sys/ioctl.h>
#	include <unistd.h>
#endif


// From https://en.wikipedia.org/wiki/ANSI_escape_code
#define ANSI_RESET			"\033[0m"	// All attributes off.
#define ANSI_BOLD			"\033[1m"	// Increased intensity.


namespace pax::cmd_args {
	
	class Terminal {
		uint16_t 	m_chars, m_lines;

	public:
		Terminal( const uint16_t default_chars_ = 80, const uint16_t default_lines_ = 24 ) noexcept 
		: m_chars{ default_chars_ }, m_lines{ default_lines_ } {
#			if defined _WIN32
				CONSOLE_SCREEN_BUFFER_INFO	csbi;
				GetConsoleScreenBufferInfo( GetStdHandle( STD_OUTPUT_HANDLE ), &csbi );
				m_chars	  = csbi.srWindow.Right  - csbi.srWindow.Left + 1;
				m_lines	  = csbi.srWindow.Bottom - csbi.srWindow.Top  + 1;
#			elif defined(TIOCGSIZE)
				struct ttysize ts;
				ioctl(STDIN_FILENO, TIOCGSIZE, &ts);
				m_chars	  = ts.ts_cols;
				m_lines	  = ts.ts_lines;
#			elif defined(TIOCGWINSZ)
				struct winsize ts;
				ioctl(STDIN_FILENO, TIOCGWINSZ, &ts);
				m_chars	  = ts.ws_col;
				m_lines	  = ts.ws_row;
#			endif // TIOCGSIZE
		}
		
		Terminal( const Terminal & )			  =	default;
		Terminal( Terminal && )					  =	default;
		Terminal & operator=( const Terminal & )  =	default;
		Terminal & operator=( Terminal && )		  =	default;

		auto chars()			const noexcept	{	return m_chars;		}
		auto lines()			const noexcept	{	return m_lines;		}
		
		template< typename Out >
		Out & wrap(
			Out						  & out_,
			const uint16_t 				tab_,
			std::string_view			text_		
		) const noexcept {
			const auto 					prefix = std::string( tab_, ' ' );
			auto 						width = chars();
			while( !text_.empty() ) {
				std::size_t 			last = 0;
				for( std::size_t i=0; i<width; ++i ) {
					if( ( text_[ i ] == ' ' ) || ( text_[ i ] == '\n' ) ) {
						last = i;
						if( text_[ i ] == '\n' )	break;
					}
				}
				if( last == 0 )						last = width;
				out_ << text_.substr( 0, last ) << '\n';
				text_ = text_.substr( last + 1 );
				if( !text_.empty() )				out_ << prefix;
				width = chars() - tab_;
			}
			return out_;
		}
	};


	template< typename Out >
	Out & operator<<( Out & out_, const Terminal & terminal_ ) {
		return out_	<< ANSI_BOLD << terminal_.chars() << ANSI_RESET" chars by " 
					<< ANSI_BOLD << terminal_.lines() << ANSI_RESET" lines";
	}
	
}	// namespace pax::cmd_args
