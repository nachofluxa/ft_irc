#include "Server/Server.hpp"
#include "Client/Client.hpp"
#include <cstring>
#include <cerrno>
#include <iostream>
#include <map>
#include <string>
#include <sys/poll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <vector>
#include "Command/Command.hpp"

Server::Server( int port ) : _serverSocket( -1 ), _port( port ), _shouldDefragment( false )
{}

Server::~Server()
{
	std::map< int, Client * >::iterator	mapIter;
	std::vector< pollfd >::iterator		vecIter;

	for ( mapIter = _clients.begin(); mapIter != _clients.end(); ++mapIter )
		if ( mapIter->second != NULL )
			delete mapIter->second;
	_clients.clear();

	for ( vecIter = _fds.begin(); vecIter != _fds.end(); ++vecIter )
		if ( vecIter->fd != -1 )
			close( vecIter->fd );
}

bool Server::_createSocket()
{
	_serverSocket = socket( AF_INET, SOCK_STREAM, 0 );
	if ( _serverSocket < 0 )
	{
		std::cerr << "Failed to create socket." << std::endl;
		return false;
	}
	return true;
}

bool Server::_configureSocket()
{
	sockaddr_in servAddr;
	std::memset( &servAddr, 0, sizeof( servAddr ) );

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons( _port );
	servAddr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;

	if ( setsockopt( _serverSocket, SOL_SOCKET, SO_REUSEADDR,
				&opt, sizeof( opt ) ) < 0 )
		return false;

	if ( fcntl( _serverSocket, F_SETFL, O_NONBLOCK ) < 0 )
		return false;

	if ( bind( _serverSocket,
			( struct sockaddr* )&servAddr,
			sizeof( servAddr ) ) < 0 )
		return false;

	return true;
}

bool Server::_startListening()
{
	if ( listen( _serverSocket, 42 ) < 0 )
		return false;
	return true;
}

bool Server::init()
{
	if ( !_createSocket() )
		return false;

	if ( !_configureSocket() )
	{
		close( _serverSocket );
		return false;
	}

	if ( !_startListening() )
	{
		close( _serverSocket );
		return false;
	}

	pollfd serverPoll;
	serverPoll.fd = _serverSocket;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;

	_fds.push_back( serverPoll );

	return true;
}

void Server::_handleNewConnection()
{
	while ( true )
	{
		int clientSocket = accept( _serverSocket, NULL, NULL );

		if ( clientSocket < 0 )
		{
			if ( errno != EWOULDBLOCK )
				std::cerr << "( Server ) accept() failed." << std::endl;
			break;
		}


		if ( fcntl( clientSocket, F_SETFL, O_NONBLOCK ) < 0 )
		{
			std::cerr << "( Server ) fcntl() failed." << std::endl;
			close( clientSocket );
			continue ;
		}

		pollfd clientPoll;
		clientPoll.fd = clientSocket;
		clientPoll.events = POLLIN;
		clientPoll.revents = 0;

		_fds.push_back( clientPoll );
		_clients[ clientSocket ] = new Client( clientSocket );

		std::cout << "( Server ) New incoming connection fd [ "
				<< clientSocket << " ]." << std::endl;
	}
}

void Server::_handleClient( size_t index )
{
	const int	fd = _fds[ index ].fd;
	char		buffer[ 512 ];
	int			bytes;

	std::memset( buffer, 0, sizeof( buffer ) );

	bytes = recv( fd, buffer, sizeof( buffer ) - 1, 0 );

	if ( bytes <= 0 )
	{
		if ( bytes == 0 )
			std::cout << "( Server ) Client with fd [ " << fd << " ] disconnected." << std::endl;
		else
			std::cerr << "( Server ) recv() error on fd [ " << fd << " ]." << std::endl;

		std::map< int, Client * >::iterator	mapIter = _clients.find( fd );
		if ( mapIter != _clients.end() )
		{
			delete mapIter->second;
			_clients.erase( mapIter );
		}
		close( fd );
		_fds[ index ].fd = -1;
		_shouldDefragment = true;
		return ;
	}

	buffer[ bytes ] = '\0';
	if ( _clients.count( fd ) )
	{
		_clients[ fd ]->addBuffer( std::string( buffer ) );
		if ( _clients[ fd ]->isBufferComplete() )
		{
			std::string	buf= _clients[ fd ]->getBuffer();
			// std::vector<std::string>	splitBuf = split( buf );
			t_command	bufCommand = commandFactory( buf );

			std::cout << "( Server ) el mensaje del cliente [ " << fd << " ] es :" << std::endl;
			std::cout << " + " << buf;

			// Desde aquí hasta la siguiente marca se tiene que sustituir por la llamada
			//   a la función correspondiente
			send( fd, "Prefix - ", 9, MSG_NOSIGNAL );
			send( fd, bufCommand.prefix.c_str(), bufCommand.prefix.size(), MSG_NOSIGNAL );
			send( fd, "\n", 1, MSG_NOSIGNAL );
			send( fd, "Command - ", 10, MSG_NOSIGNAL );
			send( fd, bufCommand.type.c_str(), bufCommand.type.size(), MSG_NOSIGNAL );
			send( fd, "\n", 1, MSG_NOSIGNAL );
			send( fd, "Params - ", 9, MSG_NOSIGNAL );
			for ( std::vector< std::string >::iterator vecIter = bufCommand.params.begin(); vecIter != bufCommand.params.end(); ++vecIter )
				send( fd, vecIter->c_str(), vecIter->size(), MSG_NOSIGNAL );
			send( fd, "\n", 1, MSG_NOSIGNAL );
			send( fd, "Trailing - ", 11, MSG_NOSIGNAL );
			send( fd, bufCommand.trailing.c_str(), bufCommand.trailing.size(), MSG_NOSIGNAL );
			send( fd, "\n", 1, MSG_NOSIGNAL );
			// Auquí termina el segmente que hay que sustituri por la llamada a la función
			//   correspondiente

			// for ( std::vector<std::string>::iterator vecIter = splitBuf.begin(); vecIter != splitBuf.end(); ++vecIter )
			// {
			// 	/*
			// 	 * Modificar esto para parsear correctamente y enviar mensajes
			// 	 */
			// 	if ( vecIter == splitBuf.begin() && vecIter->find( ':' ) != std::string::npos )
			// 		send( fd, "Prefix - ", 10, MSG_NOSIGNAL );
			// 	else if ( vecIter->find( ':' ) != std::string::npos )
			// 		send( fd, "Trailing - ", 11, MSG_NOSIGNAL );
			// 	else
			// 		send( fd, "Command/Param - ", 16, MSG_NOSIGNAL );
			// 	send( fd, vecIter->c_str(), vecIter->size(), MSG_NOSIGNAL );
			// 	send( fd, "\n", 1, MSG_NOSIGNAL );
			// }

			_clients[ fd ]->clearBuffer();
		}
	}
}

void Server::_defragment()
{
	std::vector< pollfd > clean;

	size_t i = 0;
	while ( i < _fds.size() )
	{
		if ( _fds[ i ].fd != -1 )
			clean.push_back( _fds[ i ] );
		i++;
	}

	_fds = clean;
	_shouldDefragment = false;
}

void Server::run()
{
	while ( true )
	{
		int ret = poll( &_fds[ 0 ], _fds.size(), -1 );

		if ( ret <= 0 )
		{
			if ( ret == 0 )
				std::cerr << "( Server ) Timeout." << std::endl;
			else
				std::cerr << "( Server ) poll() failed." << std::endl;
			break;
		}

		size_t i = 0;
		while ( i < _fds.size() )
		{
			if ( _fds[ i ].revents == 0 )
			{
				i++;
				continue;
			}

			if ( _fds[ i ].fd == _serverSocket )
				_handleNewConnection();
			else
				_handleClient( i );

			i++;
		}
		_defragment();
	}
}
