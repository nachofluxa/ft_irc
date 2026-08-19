#include "IRC/IRCServer.hpp"
#include "IRC/Channel.hpp"
#include "IRC/IRCClient.hpp"
#include "IRC/IRCCommand.hpp"
#include "Server/AServerClient.hpp"
#include <cstddef>
#include <cstdlib>
#include <map>
#include <sstream>
#include <string>
#include <vector>

IRCServer::IRCServer( int port, const std::string &pass ) : AServer( port, pass )
{}

IRCServer::~IRCServer( void )
{
	for (
		std::map< std::string, Channel * >::iterator iter = this->_channels.begin();
		iter != this->_channels.end();
		++iter
	)
		delete iter->second;
}

void	IRCServer::_onClientConnection( AServerClient & )
{
	// Tiene que mandar un mensaje de bienvenida.
}

void	IRCServer::_onClientDisconnect( AServerClient &client )
{
	IRCClient &irc = dynamic_cast< IRCClient & >( client );
	this->_removeClientFromChannels( irc );
}

void	IRCServer::_executeProtocol( AServerClient &client )
{
	IRCClient	&irc = dynamic_cast< IRCClient & >( client );
	std::string	&buffer = irc.getReadBuffer();

	std::size_t	pos = buffer.find( "\n" );
	while ( pos != std::string::npos )
	{
		std::string	line = buffer.substr( 0, pos );

		buffer.erase( 0, pos + 1 );
		if ( !line.empty() && line[ line.size() - 1] == '\r' )
			line.erase( line.size() - 1 );
		if ( line.empty() )
			continue ;
		try
		{
			IRCCommand	cmd( line );
			if ( cmd._command == "PASS" )
				this->_handlePass( irc, cmd );
			else if ( cmd._command == "NICK" )
				this->_handleNick( irc, cmd );
			else if ( cmd._command == "USER" )
				this->_handleUser( irc, cmd );
			else if ( cmd._command == "JOIN" )
				this->_handleJoin( irc, cmd );
			else if ( cmd._command == "PART" )
				this->_handlePart( irc, cmd );
			else if ( cmd._command == "PRIVMSG" )
				this->_handlePrivMsg( irc, cmd );
			else if ( cmd._command == "KICK" )
				this->_handleKick( irc, cmd );
			else if ( cmd._command == "INVITE" )
				this->_handleInvite( irc, cmd );
			else if ( cmd._command == "TOPIC" )
				this->_handleTopic( irc, cmd );
			else if ( cmd._command == "MODE" )
				this->_handleMode( irc, cmd );
			else if ( cmd._command == "PING" )
				this->_handlePing( irc, cmd );
			else if ( cmd._command == "QUIT" )
				this->_handleQuit( irc, cmd );
			else
				this->_sendError( irc, "421", cmd._command + " :Unknown command" );
		} catch ( const IRCCommand::InvalidCommandException & )
		{
			this->_sendError( irc, "421", "Unknown command" );
		}
		pos = buffer.find( "\n" );
	}
}

void	IRCServer::_sendReply( IRCClient &client, const std::string &msg )
{
	client.queueBuffer( msg + "\r\n" );
	this->_enableWrite( client.getSocket() );
}

void	IRCServer::_sendError(
	IRCClient &client,
	const std::string &code,
	const std::string &msg
)
{
	std::string	nick = client.getNickName();
	if ( nick.empty() )
		nick = "*";
	this->_sendReply( client, ":ircserv " + code + " " + nick + " " + msg );
}

void	IRCServer::_sendWelcome( IRCClient &client )
{
	this->_sendReply(
		client,
		":ircserv 001 " +
		client.getNickName() +
		" :Welcome to the IRC Network " +
		client.getPrefix()
	);
	this->_sendReply(
		client,
		":ircserv 002 " +
		client.getNickName() +
		" :Your host is ircserv, running version 0.1"
	);
	this->_sendReply(
		client,
		":ircserv 003 " +
		client.getNickName() +
		" :This server was created today"
	);
	this->_sendReply(
		client,
		":ircserv 004 " +
		client.getNickName() +
		" :ircserv 1.0 o i,t,k,l,o"
	);
}

void	IRCServer::_broadcastToChannel( Channel *channel, IRCClient *sender, const std::string &msg )
{
	if ( !channel )
		return;
	channel->broadcast( sender, msg );
	const std::vector<IRCClient*> &users = channel->getUsers();
	for ( std::size_t i = 0; i < users.size(); ++i )
		if ( users[i] != sender )
			this->_enableWrite( users[i]->getSocket() );
}

Channel	*IRCServer::_getOrCreateChannel( const std::string &name )
{
	std::map< std::string, Channel * >::iterator	iter = this->_channels.find( name );

	if ( iter != this->_channels.end() )
		return ( iter->second );

	Channel	*channel = new Channel( name );
	this->_channels[ name ] = channel;
	return ( channel );
}

void	IRCServer::_removeClientFromChannels( IRCClient &client )
{
	std::map< std::string, Channel * >::iterator	iter = this->_channels.begin();

	while ( iter != this->_channels.end() )
	{
		Channel	*channel = iter->second;

		if ( channel->hasUser( &client ) )
		{
			const std::vector< IRCClient * >	&users = channel->getUsers();
			std::string	quitMsg = ":" + client.getPrefix() + " QUIT :Connection closed";

			for ( std::size_t pos = 0; pos < users.size(); ++pos )
			{
				if ( users[ pos ] != &client )
				{
					users[ pos ]->queueBuffer( quitMsg + "\r\n" );
					this->_enableWrite(users[pos]->getSocket());
				}
			}
			channel->removeUser( &client );
			if ( channel->getUserCount() == 0 )
			{
				this->_channels.erase( iter++ );
				delete channel;
			}
			else
				++iter;
		}
		else
			++iter;
	}
	client.clearChannels();
}

AServerClient	*IRCServer::_createClient( int socket, const std::string &hostname )
{ return ( new IRCClient( socket, hostname ) ); }

void	IRCServer::_handlePass( IRCClient &client, const IRCCommand &cmd )
{
	if ( client.isPassAccepted() )
	{
		this->_sendError( client, "462", ":You may not reregister" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "461", "PASS :Not enough parameters" );
		return ;
	}
	if ( cmd._params[ 0 ] == this->_pass )
		client.acceptPass();
	else
		this->_sendError( client, "464", ":Password incorrect" );
}

void	IRCServer::_handleNick( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isPassAccepted() )
	{
		this->_sendError( client, "464", ":Password incorrect or not sent" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "431", ":No nickname given" );
		return ;
	}
	std::string	newNick = cmd._params[ 0 ];
	std::map< int, AServerClient * >::iterator	iter = this->_clients.begin();

	while ( iter != this->_clients.end() )
	{
		IRCClient	*other = dynamic_cast< IRCClient * >( iter->second );

		if ( other && other != &client && other->getNickName() == newNick )
		{
			this->_sendError( client, "433", newNick + " :Nickname is already in use" );
			return ;
		}
		++iter;
	}
	if ( client.isRegistered() )
	{
		std::string	oldNick = client.getNickName();
		client.setNickName( newNick );

		const std::set< Channel * >	&channels = client.getChannels();
		std::string					nickMsg = ":" +
			oldNick +
			"!" +
			client.getUserName() +
			"@" +
			client.getHostName() +
			" NICK :" +
			newNick;
		std::set< Channel * >::const_iterator iter = channels.begin();
		while ( iter != channels.end() )
		{
			( *iter )->broadcast( NULL, nickMsg );
			++iter;
		}
		return ;
	}
	client.setNickName( newNick );
	if ( !client.isRegistered() && client.isPassAccepted() && !client.getUserName().empty() )
	{
		client.beRegistered();
		this->_sendWelcome( client );
	}
}

void	IRCServer::_handleUser( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isPassAccepted() )
	{
		this->_sendError( client, "464", ":Password incorrect or not sent" );
		return ;
	}
	if ( client.isRegistered() )
	{
		this->_sendError( client, "462", ":You may not reregister" );
		return ;
	}
	if ( cmd._params.size() < 3 )
	{
		this->_sendError( client, "461", "USER :Not enough parameters" );
		return ;
	}
	client.setUserName( cmd._params[ 0 ] );
	if ( !cmd._trailing.empty() )
		client.setRealName( cmd._trailing );
	else if ( cmd._params.size() >= 4 )
		client.setRealName( cmd._params[ 3 ] );
	else
	{
		this->_sendError( client, "461", "USER :Not enough parameters" );
		return ;
	}

	if ( !client.isRegistered() && client.isPassAccepted() && !client.getNickName().empty() )
	{
		client.beRegistered();
		this->_sendWelcome( client );
	}
}

void	IRCServer::_handleJoin( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "461", "JOIN :Not enough parameters" );
		return ;
	}

	std::string	channelName = cmd._params[ 0 ];
	std::string	key = ( cmd._params.size() > 1 ? cmd._params[ 1 ] : "" );

	if ( channelName.empty() || channelName[ 0 ] != '#' )
	{
		this->_sendError( client, "403", channelName + " :No such channel" );
		return ;
	}

	Channel	*channel = this->_getOrCreateChannel( channelName );
	if ( !channel->addUser( &client, key ) )
	{
		this->_sendError( client, "403", channelName + " :Cannot join channel" );
		return ;
	}

	client.joinChannel( *channel );

	std::string	joinMsg = ":" + client.getPrefix() + " JOIN " + channelName;
	this->_broadcastToChannel( channel, NULL, joinMsg );

	if ( !channel->getTopic().empty() )
	{
		this->_sendReply(
			client,
			":ircserv 332 " +
			client.getNickName() +
			" " +
			channelName +
			" :" +
			channel->getTopic()
		);
	}
	const	std::vector< IRCClient * >	&users = channel->getUsers();
	std::string	names;
	for ( std::size_t pos = 0; pos < users.size(); ++pos )
	{
		if ( pos > 0 )
			names += " ";
		if ( channel->isOperator( users[ pos ] ) )
			names += "@";
		names += users[ pos ]->getNickName();
	}
	this->_sendReply(
		client,
		":ircserv 353 " +
		client.getNickName() +
		" = " +
		channelName +
		" :" +
		names
	);
	this->_sendReply(
		client,
		":ircserv 366 " +
		client.getNickName() +
		" " +
		channelName +
		" :End of NAMES list"
	);
}

void	IRCServer::_handlePart( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "461", ":Not enough parameters" );
		return ;
	}

	std::string	channelName = cmd._params[ 0 ];
	std::string	reason = ( cmd._trailing.empty() ? "" : cmd._trailing );

	std::map<std::string, Channel*>::iterator	iter = _channels.find( channelName );
	if ( iter == _channels.end() )
	{
		this->_sendError( client, "403", channelName + " :No such channel" );
		return;
	}

	Channel	*ch = iter->second;

	if ( !ch->hasUser( &client ) )
	{
		this->_sendError( client, "442", channelName + " :You're not on that channel" );
		return;
	}
	std::string partMsg = ":" + client.getPrefix() + " PART " + channelName;
	if ( !reason.empty() )
		partMsg += " :" + reason;
	this->_broadcastToChannel( ch, NULL, partMsg );
	ch->removeUser( &client );
	client.leaveChannel( *ch );
	if ( ch->getUserCount() == 0 )
	{
		delete ch;
		_channels.erase( iter );
	}
}

void	IRCServer::_handlePrivMsg( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.empty() || ( cmd._params.size() == 1 && cmd._trailing.empty() ) )
	{
		this->_sendError( client, "411", ":No recipient given ( PRIVMSG )" );
		return ;
	}

	std::string	target = cmd._params[ 0 ];
	std::string	message = ( cmd._trailing.empty() ? "" : cmd._trailing );

	if ( message.empty() )
	{
		this->_sendError( client, "412", ":No text sent" );
		return ;
	}
	if ( target[ 0 ] == '#' )
	{
		std::map< std::string, Channel * >::iterator	iter = this->_channels.find( target );

		if ( iter == this->_channels.end() )
		{
			this->_sendError( client, "403", target + " :No such channel" );
			return ;
		}

		Channel	*channel = iter->second;

		if ( !channel->hasUser( &client ) )
		{
			this->_sendError( client, "404", target + " :Cannot send to channel" );
			return ;
		}

		std::string	msg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message;

		this->_broadcastToChannel( channel, &client, msg );
	}
	else
	{
		IRCClient									*targetClient = NULL;
		std::map< int, AServerClient * >::iterator	iter = this->_clients.begin();

		while ( iter != this->_clients.end() )
		{
			IRCClient	*c = dynamic_cast< IRCClient * >( iter->second );
			if ( c && c->getNickName() == target )
			{
				targetClient = c;
				break ;
			}
			++iter;
		}
		if ( !targetClient )
		{
			this->_sendError( client, "401", target + " :No such nick" );
			return ;
		}
		std::string	msg = ":" + client.getPrefix() + " PRIVMSG " + target + " :" + message;
		this->_sendReply( *targetClient, msg );
	}
}

void	IRCServer::_handleKick( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.size() < 2 )
	{
		this->_sendError( client, "461", "KICK :Not enough parameters" );
		return ;
	}

	std::string	channelName = cmd._params[ 0 ];
	std::string	targetNick = cmd._params[ 1 ];
	std::string	comment = cmd._trailing.empty() ? "" : cmd._trailing;

	std::map< std::string, Channel * >::iterator	iter = _channels.find( channelName );

	if ( iter == this->_channels.end() )
	{
		this->_sendError( client, "403", channelName + " :No such channel" );
		return ;
	}
	Channel	*channel = iter->second;
	if ( !channel->hasUser( &client ) )
	{
		this->_sendError( client, "442", channelName + " :You're not on that channel" );
		return ;
	}
	if ( !channel->isOperator( &client ) )
	{
		this->_sendError( client, "482", channelName + " :You're not channel operator" );
		return ;
	}

	IRCClient	*targetClient = NULL;
	std::map< int, AServerClient * >::iterator	iter2 = this->_clients.begin();

	while ( iter2 != this->_clients.end() )
	{
		IRCClient	*c = dynamic_cast< IRCClient * >( iter2->second );
		if ( c && c->getNickName() == targetNick )
		{
			targetClient = c;
			break ;
		}
		++iter2;
	}
	if ( !targetClient || !channel->hasUser( targetClient ) )
	{
		this->_sendError( client, "441", targetNick + " " + channelName + " :They arent on that channel" );
		return ;
	}

	std::string	kickMsg = ":" +
		client.getPrefix() +
		" KICK " +
		channelName +
		" " +
		targetNick +
		" :" +
		comment;
	this->_broadcastToChannel( channel, NULL, kickMsg );

	channel->removeUser( targetClient );
	targetClient->leaveChannel( *channel );

	if ( channel->getUserCount() == 0 )
	{
		this->_channels.erase( iter );
		delete channel;
	}
}

void	IRCServer::_handleInvite( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.size() < 2 )
	{
		this->_sendError( client, "461", "INVITE :Not enough parameters" );
		return ;
	}

	std::string	channelName = cmd._params[ 0 ];
	std::string	targetNick = cmd._params[ 1 ];

	IRCClient	*targetClient = NULL;
	std::map< int, AServerClient * >::iterator	iter = this->_clients.begin();

	while ( iter != this->_clients.end() )
	{
		IRCClient	*c = dynamic_cast< IRCClient * >( iter->second );
		if ( c && c->getNickName() == targetNick )
		{
			targetClient = c;
			break ;
		}
		++iter;
	}
	if ( !targetClient )
	{
		this->_sendError( client, "401", targetNick + " :No such nick" );
		return ;
	}

	std::map< std::string, Channel * >::iterator	cIter = this->_channels.find( channelName );

	if ( cIter == this->_channels.end() )
	{
		this->_sendError( client, "403", channelName + " :No such channel" );
		return ;
	}
	Channel	*channel = cIter->second;
	if ( !channel->hasUser( &client ) )
	{
		this->_sendError( client, "442", channelName + " :You're not on that channel" );
		return ;
	}
	if ( !channel->isOperator( &client ) )
	{
		this->_sendError( client, "482", channelName + " :You're not channel operator" );
		return ;
	}

	if ( channel->hasUser( targetClient ) )
	{
		this->_sendError( client, "443", targetNick + " " + channelName + " :is already on channel" );
		return ;
	}

	channel->inviteClient( &client, targetClient );
	this->_sendReply(
		client,
		":ircserv 341 " +
		client.getNickName() +
		" " +
		targetNick +
		" " +
		channelName
	);
	this->_sendReply(
		*targetClient,
		":" +
		client.getPrefix() +
		" INVITE " +
		targetNick +
		" :" +
		channelName
	);
}

void	IRCServer::_handleTopic( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "461", "TOPIC :Not enough parameters" );
		return ;
	}

	std::string	channelName = cmd._params[ 0 ];
	std::map< std::string, Channel * >::iterator	iter = this->_channels.find( channelName );

	if ( iter == this->_channels.end() )
	{
		this->_sendError( client, "403", channelName + " :No such channel" );
		return ;
	}

	Channel	*channel = iter->second;

	if ( !channel->hasUser( &client ) )
	{
		this->_sendError( client, "442", channelName + " :You're not on that channel" );
		return ;
	}
	if ( cmd._params.size() == 1 && cmd._trailing.empty() )
	{
		if ( channel->getTopic().empty() )
		{
			this->_sendReply(
				client,
				":ircserv 331 " +
				client.getNickName() +
				" " +
				channelName +
				" :No topic is set"
			);
		}
		else
		{
			this->_sendReply(
				client,
				":ircserv 332 " +
				client.getNickName() +
				" " +
				channelName +
				" :" +
				channel->getTopic()
			);
		}
		return ;
	}

	std::string	newTopic = ( cmd._trailing.empty() ? cmd._params[ 1 ] : cmd._trailing );

	if ( !channel->setTopic( &client, newTopic ) )
	{
		this->_sendError( client, "482", channelName + " :You're not channel operator" );
		return ;
	}
	std::string	topicMsg = ":" +
		client.getPrefix() +
		" TOPIC " +
		channelName +
		" :" +
		newTopic;
	this->_broadcastToChannel( channel, NULL, topicMsg );
}

void	IRCServer::_handleMode( IRCClient &client, const IRCCommand &cmd )
{
	if ( !client.isRegistered() )
	{
		this->_sendError( client, "451", ":You have not registered" );
		return ;
	}
	if ( cmd._params.empty() )
	{
		this->_sendError( client, "461", "MODE :Not enough parameters" );
		return ;
	}

	std::string target = cmd._params[0];

	if ( target[0] == '#' )
	{
		std::map< std::string, Channel * >::iterator it = _channels.find( target );
		if ( it == _channels.end() )
		{
			this->_sendError( client, "403", target + " :No such channel" );
			return ;
		}
		Channel *ch = it->second;
		if ( !ch->hasUser( &client ) )
		{
			this->_sendError( client, "442", target + " :You're not on that channel" );
			return ;
		}
		if ( !ch->isOperator( &client ) )
		{
			this->_sendError( client, "482", target + " :You're not channel operator" );
			return ;
		}

		if ( cmd._params.size() < 2 )
		{
			std::string	modes;
			std::string	params;

			if ( ch->isInviteOnly() )
				modes += "i";
			if ( ch->isTopicRestricted() )
				modes += "t";
			if ( !ch->getKey().empty() )
			{
				modes += "k";
				params += " " + ch->getKey();
			}
			if ( ch->hasUserLimit() )
			{
				modes += "l";

				std::ostringstream	oss;
				oss << ch->getUserLimit();
				params += " " + oss.str();
			}
			this->_sendReply(
				client,
				":ircserv 324 " +
				client.getNickName() +
				" " +
				target +
				" +" +
				modes +
				params
			);
			return ;
		}

		std::string modeStr = cmd._params[1];
		bool add = true;
		std::size_t paramIndex = 2;

		for ( std::size_t i = 0; i < modeStr.size(); ++i )
		{
			char c = modeStr[i];
			if ( c == '+' )
				add = true;
			else if ( c == '-' )
				add = false;
			else
			{
				if ( c == 'i' )
					ch->setInviteOnly( add );
				else if ( c == 't' )
					ch->setTopicRestriction( add );
				else if ( c == 'k' )
				{
					if ( add )
					{
						if ( paramIndex < cmd._params.size() )
							ch->setKey( cmd._params[ paramIndex++ ] );
						else
						{
							this->_sendError( client, "461",
								"MODE :Not enough parameters for key" );
							return ;
						}
					}
					else
						ch->removeKey();
				}
				else if ( c == 'l' )
				{
					if ( add )
					{
						if ( paramIndex < cmd._params.size() )
						{
							int limit = std::atoi( cmd._params[ paramIndex++ ].c_str() );
							if ( limit > 0 )
								ch->setUserLimit( static_cast< std::size_t >( limit ) );
							else
							{
								this->_sendError(
									client,
									"696",
									target +
									" :Invalid limit"
								);
								return ;
							}
						}
						else
						{
							this->_sendError(
								client,
								"461",
								"MODE :Not enough parameters for limit"
							);
							return ;
						}
					}
					else
						ch->removeUserLimit();
				}
				else if ( c == 'o' )
				{
					if ( paramIndex < cmd._params.size() )
					{
						std::string nick = cmd._params[ paramIndex++ ];
						IRCClient *targetClient = NULL;
						std::map< int, AServerClient * >::iterator it2 = _clients.begin();

						while ( it2 != _clients.end() )
						{
							IRCClient *c2 = dynamic_cast< IRCClient * >( it2->second );
							if ( c2 && c2->getNickName() == nick )
							{
								targetClient = c2;
								break ;
							}
							++it2;
						}
						if ( !targetClient || !ch->hasUser( targetClient ) )
						{
							this->_sendError(
								client,
								"401",
								nick +
								" :No such nick on channel"
							);
							return ;
						}
						if ( add )
							ch->addOperator( targetClient );
						else
							ch->removeOperator( targetClient );
					}
					else
					{
						this->_sendError(
							client,
							"461",
							"MODE :Not enough parameters for operator"
						);
						return ;
					}
				}
				else
				{
					this->_sendError(
						client,
						"472",
						std::string( 1, c ) +
						" :is unknown mode char to me"
					);
				}
			}
		}

		std::string modeMsg = ":" +
			client.getPrefix() +
			" MODE " +
			target +
			" " +
			modeStr +
			( paramIndex != 2 ? " " + cmd._params[ 2 ] : "" );
		this->_broadcastToChannel( ch, NULL, modeMsg );
	}
	else
		this->_sendError( client, "502", ":Cannot change mode for other users" );
}

void	IRCServer::_handlePing( IRCClient &client, const IRCCommand &cmd )
{
	std::string token;

	if ( !cmd._trailing.empty() )
		token = cmd._trailing;
	else if ( !cmd._params.empty() )
		token = cmd._params[0];
	else
	{
		this->_sendError( client, "409", ":No origin specified" );
		return;
	}

	std::string reply = ":ircserv PONG ircserv :" + token + "\r\n";
	this->_sendReply( client, reply );
}

void	IRCServer::_handleQuit( IRCClient &client, const IRCCommand &cmd )
{
	std::string	quitMsg;

	{
		std::string	reason;

		reason = cmd._trailing.empty() ? "Leaving" : cmd._trailing;
		quitMsg = ":" + client.getPrefix() + " QUIT :" + reason;
	}

	const std::set< Channel * >				&channels = client.getChannels();
	std::set< Channel * >::const_iterator	iter = channels.begin();

	while ( iter != channels.end() )
	{
		( *iter )->broadcast( NULL, quitMsg );
		++iter;
	}
	client.setConnected( false );
}
