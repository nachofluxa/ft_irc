#include "Server/Server.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
	// int				aux;
	unsigned int	port;
	if (argc != 3)
	{
		std::cout << "There must be 3 and only 3 args." << std::endl;
		return (1);
	}
	port = std::atoi( argv[ 1 ] );
	if ( port < 1024 )
	{
		std::cerr << "Invalid port. ( 1024 – 49151 )" << std::endl;
		return ( 1 );
	}
	Server serv( port );

	serv.init();
	serv.run();
	return ( 0 );
}
