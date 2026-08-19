#include "Socket/Socket.hpp"
#include <cstring>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <arpa/inet.h>

Socket::~Socket( void )
{}

int	Socket::createSocket( int port, unsigned int conf )
{
	int	sck = socket( AF_INET, SOCK_STREAM, 0 );

	if ( sck < 0 )
		throw CreationException();
	if ( conf != 0 )
	{
		try
		{
			configureSocket( sck, conf );
		} catch ( std::exception & )
		{
			close( sck );
			throw ;
		}
	}

	struct sockaddr_in	address;

	std::memset( &address, 0, sizeof( address ) );
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons( port );
	if ( bind( sck, ( struct sockaddr * )&address, sizeof( address ) ) < 0 )
	{
		close( sck );
		throw BindException();
	}
	return ( sck );
}

void	Socket::configureSocket( int fd, unsigned int conf )
{
	int	opt = 1;

	if ( conf & OPT_REUSE_ADDR )
		if ( setsockopt( fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof( opt ) ) < 0 )
			throw OptionSettingException( "REUSE_ADDR" );
#ifdef SO_NOSIGPIPE
	if ( conf & OPT_NO_SIGPIPE )
		if ( setsockopt( fd, SOL_SOCKET, SO_NOSIGPIPE, &opt, sizeof( opt ) ) < 0 )
			throw OptionSettingException( "NO_SIGPIPE" );
#endif
	if ( conf & OPT_NODELAY )
		if ( setsockopt( fd, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof( opt ) ) < 0 )
			throw OptionSettingException( "TCP_NODELAY" );
	if ( conf & OPT_NON_BLOCKING )
	{
		if ( fcntl( fd, F_SETFL, O_NONBLOCK ) < 0 )
			throw OperatingModeException();
	}
}

/* *** * *** * *** * *** * *** * Exceptions * *** * *** * *** * *** * *** */

const char	*Socket::CreationException::what( void ) const throw()
{ return ( "Error: Failed to initialize network endpoint" ); }

const char	*Socket::BindException::what( void ) const throw()
{ return ( "Error: Failed to bind to the requested port ( address conflict )" ); }

Socket::OptionSettingException::OptionSettingException( const std::string &opt ) : _msg( "Error: Failed to configure network property: " + opt ) {}

Socket::OptionSettingException::~OptionSettingException( void ) throw() {}

const char	*Socket::OptionSettingException::what( void ) const throw()
{ return ( _msg.c_str() ); }

const char	*Socket::OperatingModeException::what( void ) const throw()
{ return ( "Error: Failed to change channel operating mode" ); }
