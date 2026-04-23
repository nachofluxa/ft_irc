#include "Command/Command.hpp"
#include <algorithm>
#include <iterator>
#include <vector>

std::vector< std::string >	split( const std::string &str );

t_command	commandFactory( std::string buf )
{
	std::vector< std::string >	splitedBuf;
	t_command					bufCommand;
	const std::string			validTypes[] = { "USER", "NICK", "PASS" };

	if ( std::count(buf.begin(), buf.end(), ':' ) > 2 )
		throw std::invalid_argument("Formato invalido");
	splitedBuf = split( buf );
	if ( splitedBuf.size() == 0 )
		throw std::invalid_argument("Error al parsear");
	for ( std::vector< std::string >::iterator vecIter = splitedBuf.begin();
		vecIter != splitedBuf.end();
		++vecIter )
	{
		if ( vecIter->empty() )
			throw std::invalid_argument("Error al parsear");
		if ( vecIter == splitedBuf.begin() && ( *vecIter )[ 0 ] == ':' )
		{
			bufCommand.prefix = *vecIter;
			continue ;
		}
		if ( vecIter == splitedBuf.begin()
			|| ( std::distance( splitedBuf.begin(), vecIter) == 1
				&& ( *splitedBuf.begin() )[ 0 ] == ':' ) )
		{
			if ( std::find( validTypes, validTypes + 3, ( *vecIter ) ) == validTypes + 3 )
				throw std::invalid_argument("Comando no existe : " + ( *vecIter ) );
			bufCommand.type = *vecIter;
			continue ;
		}
		if ( ( *vecIter )[ 0 ] == ':' )
		{
			bufCommand.trailing = ( *( vecIter++ ) ).substr( 1 );
			while ( vecIter != splitedBuf.end() )
				bufCommand.trailing.append( " " + *( vecIter++ ) );
			break ;
		}
		bufCommand.params.push_back( *vecIter );
	}
	return ( bufCommand );
}
