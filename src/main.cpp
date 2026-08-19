#include <csignal>
#include <exception>
#include <signal.h>
#include <iostream>
#include <cstdlib>
#include <IRC/IRCServer.hpp>

int	main( int argc, char **argv )
{
	if ( argc != 3 )
	{
		std::cerr << "Usage: ./ircserv <port> <password>" << "\n";
		return ( 1 );
	}

	signal( SIGPIPE, SIG_IGN );

	try
	{
		int			port = std::atoi( argv[ 1 ] );
		std::string	pass = argv[ 2 ];

		IRCServer	server( port, pass );
		server.run();
	}
	catch ( const std::exception &e )
	{
		std::cerr << e.what() << "\n";
		return ( 1 );
	}
	return ( 0 );
}
