/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachofluxa <nachofluxa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 10:46:42 by nachofluxa        #+#    #+#             */
/*   Updated: 2026/02/20 10:55:12 by nachofluxa       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <iostream>
#include <cstring>
#include <cerrno>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>

Server::Server(int port, int timeout) : _serverSocket(-1), _port(port), _timeout(timeout)
{}

Server::~Server()
{
	size_t i;
	
	i = 0;
	while (i < _fds.size())
	{
		if (_fds[i].fd != -1)
			close(_fds[i].fd);
		i++;
	}
}

bool Server::createSocket()
{
	_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (_serverSocket < 0)
	{
		std::cerr << "Failed to create socket." << std::endl;
		return false;
	}
	return true;
}

bool Server::configureSocket()
{
	sockaddr_in servAddr;
	std::memset(&servAddr, 0, sizeof(servAddr));

	servAddr.sin_family = AF_INET;
	servAddr.sin_port = htons(_port);
	servAddr.sin_addr.s_addr = INADDR_ANY;

	int opt = 1;

	if (setsockopt(_serverSocket, SOL_SOCKET, SO_REUSEADDR,
				&opt, sizeof(opt)) < 0)
		return false;

	if (fcntl(_serverSocket, F_SETFL, O_NONBLOCK) < 0)
		return false;

	if (bind(_serverSocket,
			(struct sockaddr*)&servAddr,
			sizeof(servAddr)) < 0)
		return false;

	return true;
}

bool Server::startListening()
{
	if (listen(_serverSocket, 42) < 0)
		return false;
	return true;
}

bool Server::init()
{
	if (!createSocket())
		return false;

	if (!configureSocket())
	{
		close(_serverSocket);
		return false;
	}

	if (!startListening())
	{
		close(_serverSocket);
		return false;
	}

	pollfd serverPoll;
	serverPoll.fd = _serverSocket;
	serverPoll.events = POLLIN;
	serverPoll.revents = 0;

	_fds.push_back(serverPoll);

	return true;
}

void Server::handleNewConnection()
{
	while (true)
	{
		int clientSocket = accept(_serverSocket, NULL, NULL);

		if (clientSocket < 0)
		{
			if (errno != EWOULDBLOCK)
				std::cerr << "Accept failed." << std::endl;
			break;
		}

		pollfd clientPoll;
		clientPoll.fd = clientSocket;
		clientPoll.events = POLLIN;
		clientPoll.revents = 0;

		_fds.push_back(clientPoll);

		std::cout << "New incoming connection ("
				<< clientSocket << ")." << std::endl;
	}
}

void Server::handleClient(size_t index)
{
	char buffer[80];
	std::memset(buffer, 0, sizeof(buffer));

	int bytes = recv(_fds[index].fd, buffer, sizeof(buffer), 0);

	if (bytes <= 0)
	{
		if (bytes < 0 && errno != EWOULDBLOCK)
			std::cerr << "Recv failed." << std::endl;

		close(_fds[index].fd);
		_fds[index].fd = -1;
		return;
	}

	std::cout << "(" << _fds[index].fd << ") "
			<< bytes << " bytes received." << std::endl;

	if (send(_fds[index].fd, buffer, bytes, 0) < 0)
	{
		std::cerr << "Send failed." << std::endl;
		close(_fds[index].fd);
		_fds[index].fd = -1;
	}
}

void Server::defragment()
{
	std::vector<pollfd> clean;

	size_t i = 0;
	while (i < _fds.size())
	{
		if (_fds[i].fd != -1)
			clean.push_back(_fds[i]);
		i++;
	}

	_fds = clean;
}

void Server::run()
{
	while (true)
	{
		int ret = poll(&_fds[0], _fds.size(), _timeout);

		if (ret <= 0)
		{
			if (ret == 0)
				std::cerr << "Timeout." << std::endl;
			else
				std::cerr << "Poll failed." << std::endl;
			break;
		}

		size_t i = 0;
		while (i < _fds.size())
		{
			if (_fds[i].revents == 0)
			{
				i++;
				continue;
			}

			if (_fds[i].fd == _serverSocket)
				handleNewConnection();
			else
				handleClient(i);

			i++;
		}
		defragment();
	}
}