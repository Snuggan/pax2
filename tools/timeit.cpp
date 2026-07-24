//	Copyright (c) 2014-2016, Peder Axensten, all rights reserved.
//	Contact: peder ( at ) axensten.se


#include <pax/reporting/timers.hpp>

#include <vector>
#include <cstring>
#include <unistd.h>			// fork, execvp, exit
#include <sys/wait.h>		// Linux: waitpid


int main( const int argc, char * argv[] ) {
	int								status{};
	// const pax::Time_local			local( true );
	// const pax::Time_utc				utc ( true );
	// const pax::Clock_duration		cpu2( "clock", true );
	const pax::Usr_sys_duration		cpu ( "cpu",   true );
	const pax::Wall_duration		wall( "wall",  true );

	if( argc > 1 ) {
		// Build args for execvp (need char* array)
		std::vector< char* > 	cmd( argv + 1, argv + argc );
		cmd.push_back( nullptr );

		// Try to fork. 
		pid_t pid = fork();
		if( pid < 0 ) {
			std::perror( "fork" );
			return 1;
		}

		if( pid == 0 ) {
			// Child: execute the requested command
			execvp( cmd[0], cmd.data() );
			std::cerr << "execvp failed: " << std::strerror( errno ) << '\n';
			_exit( 127 );
		}

		// Parent: wait for child
		while( true ) {
			pid_t w = waitpid( pid, &status, 0 );
			if( w < 0 ) {
				if( errno == EINTR ) continue;
				std::perror( "waitpid" );
				return 1;
			}
			break;
		}
	}

	// Exit with the child's exit code (like time usually doesn’t change success)
	if( argc < 2 )				return status;
	if( WIFEXITED  ( status ) ) return WEXITSTATUS( status );
	if( WIFSIGNALED( status ) ) return WTERMSIG   ( status ) + 128;
	return 1;
}
