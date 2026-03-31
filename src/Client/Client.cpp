#include "Client/Client.hpp"
#include <iostream>


Client::Client( int fd )
	:	_fd( fd ),
		_nick(""),
		_user(""),
		_buffer(""),
		_isRegistered( false )
{ }

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

bool		Client::isRegistered( void ) const
{ return ( this->_isRegistered ); }

void	Client::setNick( std::string nick )
{ this->_nick = nick; }

void	Client::setUser( std::string user )
{ this->_user = user; }

void	Client::confirmRegistration( void )
{ this->_isRegistered = true; }

void	Client::addBuffer( const std::string &data )
{ this->_buffer += data; }

void	Client::clearBuffer( void )
{ this->_buffer.clear(); }
