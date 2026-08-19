#ifndef CHANNEL_HPP
# define CHANNEL_HPP

# include <string>
# include <vector>

class IRCClient;

class Channel
{
	private:
		std::string					_name;
		std::string					_topic;
		std::string					_key;
		std::vector< IRCClient * >	_clients;
		std::vector< IRCClient * >	_operators;
		std::vector< IRCClient * >	_invitedClients;
		bool						_inviteOnly;
		bool						_topicRestricted;
		bool						_hasLimit;
		std::size_t					_userLimit;
	public:
		Channel( const std::string & );
		~Channel( void );

		bool								hasUser( IRCClient * ) const;
		bool								addUser( IRCClient *, const std::string & );

		bool								removeUser( IRCClient * );

		bool								isOperator( IRCClient * ) const;
		void								addOperator( IRCClient * );
		void								removeOperator( IRCClient *client );

		bool								isInvited( IRCClient * ) const;
		bool								inviteClient( IRCClient *, IRCClient * );
		void								removeInvite( IRCClient * );

		bool								kickClient( IRCClient *, IRCClient * );

		bool								setTopic( IRCClient *, const std::string & );

		const std::string					&getTopic( void ) const;

		void								setInviteOnly( bool );
		void								setTopicRestriction( bool );
		void								setKey( const std::string & );
		void								removeKey( void );
		bool								checkKey( const std::string & ) const;
		void								setUserLimit( std::size_t );
		void								removeUserLimit( void );

		const std::string					&getName( void ) const;
		std::size_t							getUserCount( void ) const;
		const std::vector< IRCClient * >	&getUsers( void ) const;
		bool								isInviteOnly( void ) const;
		bool								isTopicRestricted( void ) const;
		const std::string					&getKey( void ) const;
		bool								hasUserLimit( void ) const;
		std::size_t							getUserLimit( void ) const;

		void								broadcast( IRCClient *, const std::string & );
};

#endif
