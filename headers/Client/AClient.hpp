#ifndef A_CLIENT_HPP
# define A_CLIENT_HPP

# include <cstddef>

class AClient
{
	private:
		std::size_t			_id;
		static std::size_t	_idGenerator( void );

		AClient( const AClient & );
		AClient	&operator=( const AClient & );
	public:
		AClient( void );
		virtual ~AClient( void ) = 0;

		std::size_t		getID( void ) const;

		bool			operator==( const AClient & ) const;
		bool			operator!=( const AClient & ) const;
		bool			operator<( const AClient & ) const;
		bool			operator<=( const AClient & ) const;
		bool			operator>( const AClient & ) const;
		bool			operator>=( const AClient & ) const;
};

#endif
