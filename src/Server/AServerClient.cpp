#include "Server/AServerClient.hpp"
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

AServerClient::AServerClient( int socket ) : AClient(), _socket( socket ), _connected( true )
{}

AServerClient::~AServerClient( void )
{}

int	AServerClient::getSocket( void ) const
{ return ( this->_socket ); }

void	AServerClient::queueBuffer( const std::string &buffer )
{
	if ( !buffer.empty() )
		this->_writeBuffers.push( buffer );
}

std::string &AServerClient::getReadBuffer( void )
{ return ( this->_readBuffer ); }

void	AServerClient::receiveMessage( void )
{
	char	buffer[ BUFFER_SIZE ];
	ssize_t	bytesRead = recv( this->_socket, buffer, sizeof( buffer ) - 1, 0 );

	if ( bytesRead > 0 )
	{
		buffer[ bytesRead ] = '\0';
		this->_readBuffer.append( buffer, bytesRead );
	}
	else
		throw ComunicationFailureException();
}

void	AServerClient::sendMessage( void )
{
	if ( this->_writeBuffers.empty() )
		return ;

	ssize_t	bytesSent = send(
		this->_socket,
		this->_writeBuffers.front().c_str(),
		this->_writeBuffers.front().length(),
		0
	);

	if ( bytesSent > 0 )
	{
		if ( static_cast< std::size_t>( bytesSent ) < this->_writeBuffers.front().length() )
			this->_writeBuffers.front().erase( 0, bytesSent );
		else
			this->_writeBuffers.pop();
	}
	else
		throw ComunicationFailureException();
}

bool	AServerClient::isConnected( void ) const
{ return ( this->_connected ); }

void	AServerClient::setConnected( bool status )
{ this->_connected = status; }

bool	AServerClient::hasPendingWrites( void ) const
{ return ( !this->_writeBuffers.empty() ); }

const char	*AServerClient::ComunicationFailureException::what( void ) const throw()
{ return ( "Error: Failed to reach the client" ); }
