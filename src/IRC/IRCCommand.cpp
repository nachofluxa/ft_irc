#include "IRC/IRCCommand.hpp"
#include <algorithm>
#include <cctype>
#include <cstddef>
#include <string>
#include <vector>

static void	parse( IRCCommand &command, const std::string &msg )
{
	if ( msg.empty() )
		throw IRCCommand::InvalidCommandException();

	std::size_t	pos = 0;

	while ( pos < msg.length() && msg[ pos ] == ' ' )
		++pos;
	if ( pos >= msg.length() )
		throw IRCCommand::InvalidCommandException();

	// Prefijo ( si existe )
	if ( msg[ pos ] == ':' )
	{
		++pos;

		std::size_t	endPos = msg.find( ' ', pos );

		if ( endPos == std::string::npos )
			throw IRCCommand::InvalidCommandException();

		std::size_t	len = endPos - pos;

		command._prefix = msg.substr( pos, len );
		pos = endPos + 1;
	}
	// Comando ( tiene que haber )
	{
		while ( pos < msg.length() && msg[ pos ] == ' ' )
			++pos;
		if ( pos >= msg.length() )
			throw IRCCommand::InvalidCommandException();

		std::size_t	endPos = msg.find( ' ', pos );

		if ( endPos == std::string::npos )
		{
			command._command = msg.substr( pos );
			std::transform(
				command._command.begin(),
				command._command.end(),
				command._command.begin(),
				::toupper
			);
			return ;
		}

		std::size_t	len = endPos - pos;

		command._command = msg.substr( pos, len );
		std::transform(
			command._command.begin(),
			command._command.end(),
			command._command.begin(),
			::toupper
		);
		pos = endPos + 1;
	}
	// Parametros ( puede o no haber, tanto los intermedios como el final )
	while ( pos < msg.length() )
	{
		while ( pos < msg.length() && msg[ pos ] == ' ' )
			++pos;
		if ( pos >= msg.length() )
			break ;
		if ( msg[ pos ] == ':' )
		{
			command._trailing = msg.substr( pos + 1 );
			break ;
		}

		std::size_t	endPos = msg.find( ' ', pos );

		if ( endPos == std::string::npos )
		{
			command._params.push_back( msg.substr( pos ) );
			return ;
		}

		std::size_t	len = endPos - pos;

		command._params.push_back( msg.substr( pos, len ) );
		pos = endPos + 1;
	}
}

IRCCommand::IRCCommand( void )
{}

IRCCommand::IRCCommand( const std::string &msg )
{ parse( *this, msg ); }

IRCCommand::IRCCommand( const IRCCommand &org )
	: _prefix( org._prefix ),
	_command( org._command ),
	_params( org._params ),
	_trailing( org._trailing )
{}

IRCCommand::~IRCCommand( void )
{}

IRCCommand	&IRCCommand::operator=( const IRCCommand &org )
{
	if ( this == &org )
		return ( *this );
	this->_prefix = org._prefix;
	this->_command = org._command;
	this->_params = org._params;
	this->_trailing = org._trailing;
	return ( *this );
}

const char	*IRCCommand::InvalidCommandException::what( void ) const throw()
{ return ( "Error: Command structure is not valid" ); }

std::ostream	&operator<<( std::ostream &o, const IRCCommand &command )
{
	if ( !command._prefix.empty() )
		o << ":" << command._prefix << " ";
	o << command._command << " ";

	for (
		std::vector< std::string >::const_iterator iter = command._params.begin();
		iter != command._params.end();
		++iter
	)
		o << *iter;
	return ( o );
}
