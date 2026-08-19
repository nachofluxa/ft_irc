#ifndef IRC_CLIENT_HPP
# define IRC_CLIENT_HPP

# include "Server/AServerClient.hpp"
# include <set>
#include <string>

class Channel;
struct IRCCommand;

class IRCClient : public AServerClient
{
	private:
		std::string				_nick;
		std::string				_user;
		std::string				_name;
		std::string				_host;
		bool					_pass;
		bool					_registered;
		bool					_operator;
		std::set< Channel * >	_channels;

		IRCClient( void );
	public:
		IRCClient( int, const std::string & );
		virtual ~IRCClient( void );

		const std::string			&getNickName( void ) const;
		const std::string			&getUserName( void ) const;
		const std::string			&getRealName( void ) const;
		const std::string			&getHostName( void ) const;
		std::string					getPrefix( void ) const;
		const std::set< Channel * >	&getChannels( void ) const;
		bool						isPassAccepted( void ) const;
		bool						isRegistered( void ) const;
		bool						isOperator( void ) const;

		void						acceptPass( void );
		void						beRegistered( void );
		void						setOperator( bool status );

		void						setNickName( const std::string & );
		void						setUserName( const std::string & );
		void						setRealName( const std::string & );
		void						joinChannel( Channel & );
		void						leaveChannel( Channel & );

		void						clearChannels( void );
};

#endif
