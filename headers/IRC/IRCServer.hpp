#ifndef IRC_SERVER_HPP
# define IRC_SERVER_HPP

# include "Server/AServer.hpp"
# include "IRC/IRCClient.hpp"
# include "IRC/Channel.hpp"
#include "Server/AServerClient.hpp"
# include <map>
# include <string>

class IRCServer : public AServer
{
	private:
		std::map< std::string, Channel * >	_channels;

		void	_handlePass( IRCClient &, const IRCCommand & );
		void	_handleNick( IRCClient &, const IRCCommand & );
		void	_handleUser( IRCClient &, const IRCCommand & );
		void	_handleJoin( IRCClient &, const IRCCommand & );
		void	_handlePart( IRCClient &, const IRCCommand & );
		void	_handlePrivMsg( IRCClient &, const IRCCommand & );
		void	_handleKick( IRCClient &, const IRCCommand & );
		void	_handleInvite( IRCClient &, const IRCCommand & );
		void	_handleTopic( IRCClient &, const IRCCommand & );
		void	_handleMode( IRCClient &, const IRCCommand & );
		void	_handlePing( IRCClient &, const IRCCommand & );
		void	_handleQuit( IRCClient &, const IRCCommand & );

		void	_sendReply( IRCClient &, const std::string & );
		void	_sendError( IRCClient &, const std::string &, const std::string &);
		void	_sendWelcome( IRCClient & );
		void	_broadcastToChannel( Channel *, IRCClient *, const std::string & );
		Channel	*_getOrCreateChannel( const std::string & );
		void	_removeClientFromChannels( IRCClient & );

		virtual AServerClient	*_createClient( int, const std::string & );
	protected:
		virtual void	_onClientConnection( AServerClient & );
		virtual void	_executeProtocol( AServerClient & );
		virtual void	_onClientDisconnect( AServerClient & );
	public:
		IRCServer( int, const std::string & );
		virtual ~IRCServer( void );
};

#endif
