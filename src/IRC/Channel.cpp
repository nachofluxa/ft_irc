#include "IRC/Channel.hpp"
#include "IRC/IRCClient.hpp"
#include <vector>

Channel::Channel( const std::string &name )
	: _name( name ),
	_topic( "" ),
	_key( "" ),
	_inviteOnly( false ),
	_topicRestricted( false ),
	_hasLimit( false ),
	_userLimit( 0 )
{}

Channel::~Channel( void )
{}

bool	Channel::hasUser( IRCClient *client ) const
{
	for ( std::size_t pos = 0; pos < this->_clients.size(); ++pos )
		if ( this->_clients[ pos ] == client )
			return ( true );
	return ( false );
}

bool	Channel::addUser( IRCClient *client, const std::string &key )
{
	if ( this->hasUser( client ) )
		return ( false );
	if ( this->_hasLimit && this->_clients.size() >= this->_userLimit )
		return ( false );
	if ( !this->_key.empty() && this->_key != key )
		return ( false );
	if ( this->_inviteOnly && !this->isInvited( client ) )
		return ( false );

	bool wasEmpty = this->_clients.empty();

	this->_clients.push_back( client );
	this->removeInvite( client );

	if ( wasEmpty )
		this->addOperator( client );
	return ( true );
}

bool	Channel::removeUser( IRCClient *client )
{
	std::vector< IRCClient * >::iterator iter = this->_clients.begin();
	while ( iter != this->_clients.end() )
	{
		if ( *iter == client )
		{
			this->_clients.erase( iter );
			removeOperator( client );
			return ( true );
		}
		++iter;
	}
	return ( false );
}

bool	Channel::isOperator( IRCClient *client ) const
{
	for ( std::size_t pos = 0; pos < this->_operators.size(); ++pos )
		if ( this->_operators[ pos ] == client )
			return ( true );
	return ( false );
}

void	Channel::addOperator( IRCClient *client )
{
	if ( !isOperator( client ) )
		this->_operators.push_back( client );
}

void	Channel::removeOperator( IRCClient *client )
{
	std::vector< IRCClient * >::iterator iter = this->_operators.begin();
	while ( iter != this->_operators.end() )
	{
		if ( *iter == client )
		{
			this->_operators.erase( iter );
			return ;
		}
		++iter;
	}
}

bool Channel::isInvited( IRCClient *client ) const
{
	for ( std::size_t pos = 0; pos < this->_invitedClients.size(); ++pos )
		if ( this->_invitedClients[ pos ] == client )
			return ( true );
	return ( false );
}

bool	Channel::inviteClient( IRCClient *op, IRCClient *target )
{
	if ( !this->isOperator( op ) )
		return ( false );

	if ( !isInvited( target ) )
		this->_invitedClients.push_back( target );
	return ( true );
}

void	Channel::removeInvite( IRCClient *client )
{
	std::vector< IRCClient * >::iterator iter = this->_invitedClients.begin();
	while ( iter != this->_invitedClients.end() )
	{
		if ( *iter == client )
		{
			this->_invitedClients.erase( iter );
			return ;
		}
		++iter;
	}
}

bool	Channel::kickClient( IRCClient *op, IRCClient *target )
{
	if ( !this->isOperator( op ) || !hasUser( target ) )
		return ( false );
	this->removeUser( target );
	return ( true );
}

bool	Channel::setTopic( IRCClient *client, const std::string &topic )
{
	if ( this->_topicRestricted && !isOperator( client ) )
		return ( false );
	this->_topic = topic;
	return ( true );
}

const std::string	&Channel::getTopic( void ) const
{ return ( this->_topic ); }

void	Channel::setInviteOnly( bool enabled )
{ this->_inviteOnly = enabled; }

void Channel::setTopicRestriction( bool enabled )
{ this->_topicRestricted = enabled; }

void	Channel::setKey( const std::string &key )
{ this->_key = key; }

void	Channel::removeKey( void )
{ this->_key.clear(); }

bool	Channel::checkKey( const std::string &key ) const
{ return ( this->_key == key ); }

void	Channel::setUserLimit( std::size_t limit )
{
	if ( limit == 0 )
	{
		this->removeUserLimit();
		return ;
	}
	this->_hasLimit = true;
	this->_userLimit = limit;
}

void	Channel::removeUserLimit( void )
{
	this->_hasLimit = false;
	this->_userLimit = 0;
}

const std::string	&Channel::getName( void ) const
{ return ( this->_name ); }

std::size_t	Channel::getUserCount( void ) const
{ return ( this->_clients.size() ); }

const std::vector< IRCClient * >	&Channel::getUsers( void ) const
{ return ( this->_clients ); }

bool	Channel::isInviteOnly( void ) const
{ return ( this->_inviteOnly ); }

bool	Channel::isTopicRestricted( void ) const
{ return ( this->_topicRestricted ); }

const std::string	&Channel::getKey( void ) const
{ return ( this->_key ); }

bool	Channel::hasUserLimit( void ) const
{ return ( this->_hasLimit ); }

std::size_t	Channel::getUserLimit( void ) const
{ return ( this->_userLimit ); }

void	Channel::broadcast( IRCClient *sender, const std::string &message )
{
	for ( std::size_t pos = 0; pos < this->_clients.size(); ++pos )
	{
		if ( this->_clients[ pos ] != sender )
			this->_clients[ pos ]->queueBuffer( message + "\r\n" );
	}
}
