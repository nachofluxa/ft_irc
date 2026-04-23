#include <string>

bool	validNick( std::string nick )
{
	if ( nick.length() >= 30 || nick.empty() )
		return ( false ); // Mensaje de error ( supongo )
	if ( nick[ 0 ] == '-' || ( '0' <= nick[ 0 ] && nick[ 0 ] <= '9' )
		|| nick.find_first_of( " \t\v\f\r\n@,.*?!$:" ) != std::string::npos )
		return ( false ); // Mensaje de error ( supongo )
	return ( true );
}

bool	validUser( std::string user )
{
	if ( user.length() >= 30 || user.empty() )
		return ( false ); // Mensaje de error ( supongo )
	if ( user.find_first_of( " \t\v\f\r\n@!" ) != std::string::npos )
		return ( false ); // Mensaje de error ( supongo )
	return ( true );
}
