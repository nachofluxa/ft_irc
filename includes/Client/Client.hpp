#ifndef CLIENT_HPP
# define  CLIENT_HPP

# include <string>

class	Client
{
	private:
		int				_fd;
		std::string		_nick;
		std::string		_user;
		std::string		_name;
		std::string 	_buffer;
		unsigned char	_range;
	public:
		Client( int fd );
		~Client( void );

		int					getFd( void ) const;
		const std::string	&getNick( void ) const;
		const std::string	&getUser( void ) const;
		const std::string	&getName( void ) const;
		const std::string	&getBuffer( void ) const;
		bool			isBufferComplete( void ) const;

		void				setNick( std::string nick );
		void				setUser( std::string user );

		void				addBuffer( const std::string &data );
		void				clearBuffer( void );
};

#endif
