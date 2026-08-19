#ifndef A_SERVER_HPP
# define A_SERVER_HPP

# include <exception>
# include <map>
# include <poll.h>
# include <string>
# include <vector>

class AServerClient;

class AServer
{
	private:
		AServer( void );
		AServer( const AServer & );

		AServer	&operator=( const AServer & );

		/// Acepta la conexión entrante, almacena el socket, prepara el pollfd y ejecuta la acción correspondiente del servidor.
		void	_acceptNewConnection( void );
		/// Recoge y almacena la entrada en el cliente para despues procesarla de acuerdo a las necesidades del servidor.
		void	_handleClientActivity( std::size_t );
		/// Borra la conexión con el cliente y borrar todo rastro de esta del servidor.
		void	_closeConnection( std::size_t );
	protected:
		int									_socket;
		int									_port;
		std::string							_pass;
		std::vector< pollfd >				_fds;
		std::map< int, AServerClient * >	_clients;
		std::vector< int >					_pendingWrites;

		/// Función especifica que se ejecuta tras lograr la correcta conexión de un cliente al servidor.
		virtual void			_onClientConnection( AServerClient & ) = 0;
		/// Función especifica para procesamiento de los paquetes enviados por un cliente.
		virtual void			_executeProtocol( AServerClient & ) = 0;
		/// Función especifica previa al borrado de la conexión del servidor con un cliente.
		virtual void			_onClientDisconnect( AServerClient & ) = 0;

		virtual AServerClient	*_createClient( int, const std::string & ) = 0;
		void					_enableWrite( int fd );
	public:
		AServer( int, const std::string & );
		virtual	~AServer( void );

		/// Función para iniciar la ejecición del servidor
		void	run( void );

		class ClientConnectionException: public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
		class ListenException: public std::exception
		{
			public:
				virtual const char	*what( void ) const throw();
		};
};

#endif
