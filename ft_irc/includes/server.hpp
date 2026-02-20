/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachofluxa <nachofluxa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:45:12 by nachofluxa        #+#    #+#             */
/*   Updated: 2026/02/20 10:46:21 by nachofluxa       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <poll.h>

class Server
{
	private:
		int						_serverSocket;
		int						_port;
		int						_timeout;
		std::vector<pollfd>		_fds;
		bool	createSocket();
		bool	configureSocket();
		bool	startListening();
		void	handleNewConnection();
		void	handleClient(size_t index);
		void	defragment();

	public:
		Server(int port, int timeout);
		~Server();

		bool	init();
		void	run();
};

#endif