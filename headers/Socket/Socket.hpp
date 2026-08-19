#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <string>
# include <exception>

class Socket
{
	private:
		Socket( void );
		Socket( const Socket & );

		Socket	&operator=( const Socket & );
	public:
		~Socket( void );

		enum flags_e
		{
			OPT_REUSE_ADDR = 1 << 0,
			OPT_NON_BLOCKING = 1 << 1,
			OPT_NO_SIGPIPE = 1 << 2,
			OPT_NODELAY = 1 << 3,
			OPT_KEEP_ALIVE = 1 << 4,
			OPT_REUSE_PORT = 1 << 5,
			OPT_BROADCAST = 1 << 6
		};

		static int	createSocket( int, unsigned int );
		static void	configureSocket( int, unsigned int );

		class CreationException : public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
		class BindException : public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
		class OptionSettingException : public std::exception
		{
			private:
				std::string	_msg;
			public:
				OptionSettingException( const std::string & );
				virtual ~OptionSettingException( void ) throw();
				virtual const char	*what( void ) const throw();
		};
		class OperatingModeException : public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
};

#endif
