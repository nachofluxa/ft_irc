#ifndef COMMAND_HPP
# define COMMAND_HPP

# include <string>
#include <vector>

struct s_command
{
	std::string					prefix;
	std::string					type;
	std::vector< std::string >	params;
	std::string					trailing;
};

typedef struct s_command	t_command;

t_command	commandFactory( std::string buf );

#endif
