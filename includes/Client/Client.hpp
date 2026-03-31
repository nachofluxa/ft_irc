#ifndef CLIENT_HPP
# define  CLIENT_HPP

# include <string>

class	Client
{
	private:
		int			_fd;
		std::string	_nick;
		std::string	_user;
		std::string _buffer;
		bool		_isRegistered;
	public:
		Client( int fd );
		~Client( void );

		int					getFd( void ) const;
		const std::string	&getNick( void ) const;
		const std::string	&getUser( void ) const;
		const std::string	&getBuffer( void ) const;
		bool				isRegistered( void ) const;

		void				setNick( std::string nick );
		void				setUser( std::string user );

		void				confirmRegistration( void );

		void				addBuffer( const std::string &data );
		void				clearBuffer( void );
};

#endif
