#include <sstream>
#include <vector>
#include <string>

std::vector< std::string >	split( const std::string &str ) {
	std::vector< std::string >	tokens;
	std::string					token;
	std::istringstream			tokenStream(str);

	while ( std::getline( tokenStream, token, ' ' ) )
		if ( !token.empty() )
			tokens.push_back( token );
	return ( tokens );
}
