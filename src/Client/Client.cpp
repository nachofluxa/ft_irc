#include "Client/Client.hpp"

Client::Client( int fd )
	:	_fd( fd ),
		_nick(""),
		_user(""),
		_buffer(""),
		_range( 0x0 ) // Bits ( 4 - Operator ) ( 3 - Pass ) ( 2 - Nick ) ( 1 - User ) ( 0 - Welcome )
{ /* Save a log message on the corresponding log file */ }

Client::~Client( void )
{}

int	Client::getFd( void ) const
{ return ( this->_fd ); }

const std::string	&Client::getNick( void ) const
{ return ( this->_nick ); }

const std::string	&Client::getUser( void ) const
{ return ( this->_user ); }

const std::string	&Client::getBuffer( void ) const
{ return ( this->_buffer ); }

const unsigned char	&Client::getRange( void ) const
{ return ( this->_range ); }

bool	Client::isBufferComplete( void ) const
{
	if ( this->_buffer[ this->_buffer.size() - 1 ] == '\n' )
		return ( true );
	return ( false );
}


void	Client::setNick( std::string nick )
{ this->_nick = nick; }

void	Client::setUser( std::string user )
{ this->_user = user; }

void	Client::setName( std::string name )
{ this->_name = name; }

void	Client::raiseFlag( unsigned int bitPos )
{
	if ( bitPos > 4 )
		throw std::out_of_range("Flag position out of range");
	this->_range |= ( 1 << bitPos );
}

void	Client::addBuffer( const std::string &data )
{ this->_buffer += data; }

void	Client::clearBuffer( void )
{ this->_buffer.clear(); }
