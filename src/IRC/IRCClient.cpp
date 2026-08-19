#include "IRC/IRCClient.hpp"
#include <string>

IRCClient::IRCClient( int socket, const std::string &hostname )
	: AServerClient( socket ),
	_host( hostname ),
	_pass( false ),
	_registered( false ),
	_operator( false )
{}

IRCClient::~IRCClient( void )
{}

const std::string	&IRCClient::getNickName( void ) const
{ return ( this->_nick ); }

const std::string	&IRCClient::getUserName( void ) const
{ return ( this->_user ); }

const std::string	&IRCClient::getRealName( void ) const
{ return ( this->_name ); }

const std::string	&IRCClient::getHostName( void ) const
{ return ( this->_host ); }

std::string	IRCClient::getPrefix( void ) const
{
	std::string	nick = this->_nick.empty() ? "*" : this->_nick;
	std::string user = this->_user.empty() ? "*" : this->_user;

	return ( nick + "!" + user + "@" + this->_host );
}

const std::set< Channel * >	&IRCClient::getChannels( void ) const
{ return ( this->_channels ); }

bool	IRCClient::isPassAccepted( void ) const
{ return ( this->_pass ); }

bool	IRCClient::isRegistered( void ) const
{ return ( this->_registered ); }

bool	IRCClient::isOperator( void ) const
{ return ( this->_operator ); }

void	IRCClient::acceptPass( void )
{ this->_pass = true; }

void	IRCClient::beRegistered( void )
{ this->_registered = true; }

void	IRCClient::setOperator( bool status ) // He puesto estatus en vez de operator porque operator debe estár ya definido en algún lugar
{ this->_operator = status; }

void	IRCClient::setNickName( const std::string &nick )
{ this->_nick = nick; }

void	IRCClient::setUserName( const std::string &user )
{ this->_user = user; }

void	IRCClient::setRealName( const std::string &name)
{ this->_name = name; }

void	IRCClient::joinChannel( Channel &channel )
{ this->_channels.insert( &channel ); }

void	IRCClient::leaveChannel( Channel &channel)
{ this->_channels.erase( &channel ); }

void	IRCClient::clearChannels( void )
{ this->_channels.clear(); }
