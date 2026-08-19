#include "Server/AServer.hpp"
#include "Server/AServerClient.hpp"
#include "Socket/Socket.hpp"
#include <cerrno>
#include <exception>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <unistd.h>

AServer::AServer( int port, const std::string &pass ) : _port( port ), _pass( pass )
{ this->_socket = Socket::createSocket( port, Socket::OPT_REUSE_ADDR | Socket::OPT_NON_BLOCKING ); }

AServer::~AServer( void )
{
	close( this->_socket );

	std::map< int, AServerClient * >::iterator	iter = this->_clients.begin();

	while ( iter != this->_clients.end() )
	{
		delete iter->second;
		++iter;
	}
	this->_clients.clear();
}

void	AServer::_acceptNewConnection( void )
{
	struct sockaddr_in	clientAddr;
	socklen_t			clientLen = sizeof( clientAddr );
	int					clientFD;

	clientFD = accept( this->_socket, ( struct sockaddr * )&clientAddr, &clientLen );
	if ( clientFD < 0 )
		throw ClientConnectionException();

	try
	{
		Socket::configureSocket(
			clientFD,
			Socket::OPT_NON_BLOCKING | Socket::OPT_NO_SIGPIPE | Socket::OPT_NODELAY
		);
	} catch ( const std::exception & )
	{
		close( clientFD );
		throw ;
	}

	char	ip[ INET_ADDRSTRLEN ];
	inet_ntop( AF_INET, &( clientAddr.sin_addr), ip, INET_ADDRSTRLEN );
	std::string hostname( ip );

	AServerClient	*client = this->_createClient( clientFD, hostname );

	this->_clients[ clientFD ] = client;

	pollfd	clientPoll;

	clientPoll.fd = clientFD;
	clientPoll.events = POLLIN;
	clientPoll.revents = 0;
	this->_fds.push_back( clientPoll );
	this->_onClientConnection( *client );
}

void	AServer::_handleClientActivity( std::size_t pos )
{
	int				fd = this->_fds[ pos ].fd;
	AServerClient	*client = this->_clients[ fd ];
	// bool			needPollOut = false;

	if ( this->_fds[ pos ].revents & POLLIN )
	{
		try
		{
			client->receiveMessage();
		} catch ( const AServerClient::ComunicationFailureException &e )
		{
			std::cerr << e.what() << "\n";
			client->setConnected( false );
			this->_fds[ pos ].events &= ~POLLOUT;
			return ;
		}

		try
		{
			this->_executeProtocol( *client );
		} catch ( const std::exception &e )
		{
			std::cerr << e.what() << "\n";
			client->setConnected( false );
			this->_fds[ pos ].events &= ~POLLOUT;
			return ;
		}
	}

	if ( this->_fds[ pos ].revents & POLLOUT )
	{
		try
		{
			client->sendMessage();
		} catch ( const AServerClient::ComunicationFailureException &e )
		{
			std::cerr << e.what() << "\n";
			client->setConnected( false );
			this->_fds[ pos ].events &= ~POLLOUT;
			return ;
		}
	}

	if ( client->hasPendingWrites() )
		this->_fds[ pos ].events |= POLLOUT;
	else
		this->_fds[ pos ].events &= ~POLLOUT;
}

void	AServer::_closeConnection( std::size_t pos )
{
	int	fd = this->_fds[ pos ].fd;
	std::map< int, AServerClient * >::iterator	iter = this->_clients.find( fd );

	if ( iter != this->_clients.end() )
	{
		AServerClient	*client = iter->second;

		this->_onClientDisconnect( *client );
		this->_clients.erase( iter );
		delete client;
	}
	this->_fds.erase( this->_fds.begin() + pos );
	close( fd );
}

void	AServer::_enableWrite( int fd )
{ this->_pendingWrites.push_back( fd ); }

void	AServer::run( void )
{
	if ( listen( this->_socket, 128 ) < 0 )
		throw ListenException();

	{
		pollfd	serverPoll;

		serverPoll.fd = this->_socket;
		serverPoll.events = POLLIN;
		serverPoll.revents = 0;
		this->_fds.push_back( serverPoll );
	}

	while ( true )
	{
		int	ready = poll( &( this->_fds[ 0 ] ), this->_fds.size(), -1 );

		if ( ready <= 0 )
		{
			if ( ready == 0 )
				std::cerr << "Error: poll timeout." << "\n";
			else if ( errno == EINTR )
				continue ;
			else
				std::cerr << "Error: poll error." << "\n";
			break ;
		}

		for ( std::size_t position = 0; position < this->_fds.size(); ++position )
		{
			if ( this->_fds[ position ].revents == 0 )
				continue ;

			if ( this->_fds[ position ].fd == this->_socket )
			{
				try
				{
					this->_acceptNewConnection();
				} catch ( const ClientConnectionException &e )
				{
					std::cerr << e.what() << std::endl;
				}
			}
			else
			{
				_handleClientActivity( position );

				if ( !this->_clients[ this->_fds[ position ].fd ]->isConnected() )
					_closeConnection( position-- );
			}
		}

		for (std::size_t i = 0; i < this->_pendingWrites.size(); ++i)
		{
			int fd = this->_pendingWrites[ i ];
			for ( std::size_t j = 0; j < this->_fds.size(); ++j )
			{
				if (this->_fds[ j ].fd == fd)
				{
					this->_fds[ j ].events |= POLLOUT;
					break;
				}
			}
		}
		this->_pendingWrites.clear();
	}
}

const char	*AServer::ClientConnectionException::what( void ) const throw()
{ return ( "Error: connection couldn't be accepted" ); }

const char	*AServer::ListenException::what( void ) const throw()
{ return ( "Error: failed to listen connections" ); }
