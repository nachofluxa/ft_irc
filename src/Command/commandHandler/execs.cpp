#include "Command/Command.hpp"
#include "Command/Command.hpp"
#include "Server/Server.hpp"

static bool	validNick( std::string nick )
{
	if ( nick.length() >= 30 || nick.empty() )
		return ( false ); // Mensaje de error ( supongo )
	if ( nick[ 0 ] == '-' || ( '0' <= nick[ 0 ] && nick[ 0 ] <= '9' )
		|| nick.find_first_of( " \t\v\f\r\n@,.*?!$:" ) != std::string::npos )
		return ( false ); // Mensaje de error ( supongo )
	return ( true );
}

static bool	validUser( std::string user )
{
	if ( user.length() >= 30 || user.empty() )
		return ( false ); // Mensaje de error ( supongo )
	if ( user.find_first_of( " \t\v\f\r\n@!" ) != std::string::npos )
		return ( false ); // Mensaje de error ( supongo )
	return ( true );
}

void	execUSER( t_command& cmd, Client* client )
{
	if ( client->getRange() & ( 1 << 1 ) )
	{
		// lanzar error
		return ;
	}
	if ( validUser( cmd.params[ 0 ] ) == false )
	{
		// lanzar error
		return ;
	}
	client->setUser( cmd.params[ 0 ] );
	client->setName( cmd.trailing );
	client->raiseFlag( 1 );
}

void	execNICK( t_command& cmd, Client* client )
{
	if ( cmd.params.size() != 1 )
	{
		// lanzar error
		return ;
	}
	if ( validNick( cmd.params[ 0 ] ) == false )
	{
		// lanzar error
		return ;
	}
	client->setNick( cmd.params[ 0 ] );
	client->raiseFlag( 2 );
}

void	execPASS( t_command& cmd, Client* client, Server* server )
{ }
