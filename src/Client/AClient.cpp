#include "Client/AClient.hpp"

AClient::AClient( void ) : _id( AClient::_idGenerator() )
{
}

AClient::~AClient( void )
{
}

std::size_t	AClient::_idGenerator( void )
{
	static std::size_t	id = 0;

	return ( id++ );
}

std::size_t	AClient::getID( void ) const
{
	return ( this->_id );
}

bool			AClient::operator==( const AClient &other ) const
{ return ( this->_id == other._id ); }

bool			AClient::operator!=( const AClient &other ) const
{ return ( this->_id != other._id ); }

bool			AClient::operator<( const AClient &other ) const
{ return ( this->_id < other._id ); }

bool			AClient::operator<=( const AClient &other ) const
{ return ( this->_id <= other._id ); }

bool			AClient::operator>( const AClient &other ) const
{ return ( this->_id > other._id ); }

bool			AClient::operator>=( const AClient &other ) const
{ return ( this->_id >= other._id ); }
