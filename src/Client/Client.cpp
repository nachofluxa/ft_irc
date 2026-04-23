#include "Client/Client.hpp"

bool	validNick( std::string nick );
bool	validUser( std::string user );

Client::Client( int fd )
	:	_fd( fd ),
		_nick(""),
		_user(""),
		_buffer(""),
		_range( 0x0 ) // Bits ( 4 - Operator ) ( 3 - Pass ) ( 2 - Nick ) ( 1 - User ) ( 0 - Welcome )
{ /* Save a log message on the corresponding log file */ }

Client::~Client( void )
{}

int			Client::getFd( void ) const
{ return ( this->_fd ); }

const std::string	&Client::getNick( void ) const
{ return ( this->_nick ); }

const std::string	&Client::getUser( void ) const
{ return ( this->_user ); }

const std::string	&Client::getBuffer( void ) const
{ return ( this->_buffer ); }

bool			Client::isBufferComplete( void ) const
{
	if ( this->_buffer[ this->_buffer.size() - 1 ] == '\n' )
		return ( true );
	return ( false );
}


void	Client::setNick( std::string nick )
{
	if ( validNick( nick ) == false )
	{
		// lanzar error
		return ;
	}
	this->_nick = nick;
	this->_range |= 1 << 2;
}

void	Client::setUser( std::string user )
{
	if ( _range & ( 1 << 1 ) )
	{
		// lanzar error
		return ;
	}
	if ( validUser( user ) == false )
	{
		// lanzar error
		return ;
	}
	this->_user = user;
	this->_range |= ( 1 << 1 );
}

void	Client::addBuffer( const std::string &data )
{ this->_buffer += data; }

void	Client::clearBuffer( void )
{ this->_buffer.clear(); }
