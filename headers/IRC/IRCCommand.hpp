#ifndef IRC_COMMAND_HPP
# define IRC_COMMAND_HPP

# include <exception>
# include <string>
# include <ostream>
# include <vector>

struct IRCCommand
{
	std::string					_prefix;
	std::string					_command;
	std::vector< std::string >	_params;
	std::string					_trailing;

	IRCCommand( void );
	IRCCommand( const std::string & );
	IRCCommand( const IRCCommand & );
	~IRCCommand( void );

	IRCCommand	&operator=( const IRCCommand & );

	friend std::ostream	&operator<<( std::ostream &, const IRCCommand & );

	class InvalidCommandException : public std::exception
	{
		public:
			virtual const char	*what( void ) const throw();
	};
};

#endif
