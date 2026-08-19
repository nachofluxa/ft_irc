#ifndef A_SERVER_CLIENT_HPP
# define A_SERVER_CLIENT_HPP

# include "Client/AClient.hpp"
# include <queue>
# include <string>

# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 1024
# endif

class AServerClient : public AClient
{
	private:
		int							_socket;
		std::string					_readBuffer;
		std::queue< std::string >	_writeBuffers;
		bool						_connected;

		AServerClient( void );
	public:
		AServerClient( int );
		virtual ~AServerClient( void ) = 0;

		int				getSocket( void ) const;

		void			queueBuffer( const std::string & );

		std::string		&getReadBuffer( void );

		void			receiveMessage( void );
		void			sendMessage( void );

		bool			isConnected( void ) const;
		void			setConnected( bool );

		bool			hasPendingWrites( void ) const;

		class ComunicationFailureException : public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
};

#endif
