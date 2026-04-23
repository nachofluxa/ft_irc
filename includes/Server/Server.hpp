#ifndef SERVER_HPP
# define SERVER_HPP

# include <cstddef>
# include "Client/Client.hpp"
# include <map>
# include <vector>
# include <poll.h>

class Server
{
	private:
		int							_serverSocket;
		int							_port;
		std::vector< pollfd >		_fds;
		std::map< int, Client * >	_clients;
		bool						_shouldDefragment;

		bool	_createSocket();
		bool	_configureSocket();
		bool	_startListening();
		void	_handleNewConnection();
		void	_handleClient(std::size_t index);
		void	_defragment();

	public:
		Server( int port );
		~Server();

		bool	init();
		void	run();
};

#endif
