
#include <pax/reporting/seconds_to_string.hpp>
#include <chrono>
#include <iostream>
#include <vector>
#include <unistd.h>


int main( const int argc, char * argv[] ) {
	const auto	start	  = std::chrono::steady_clock::now();
	int			status	  = 0;

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

	const auto end		  = std::chrono::steady_clock::now();
	const double duration = std::chrono::duration_cast< std::chrono::nanoseconds >( end - start ).count()*1e-9;
	std::cout	<< pax::seconds_to_string( duration ) << " (wall) ";

	// Exit with the child's exit code (like time usually doesn’t change success)
	if( argc < 2 )				return status;
	if( WIFEXITED  ( status ) ) return WEXITSTATUS( status );
	if( WIFSIGNALED( status ) ) return WTERMSIG   ( status ) + 128;
	return 1;
}
