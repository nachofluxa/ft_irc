/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachofluxa <nachofluxa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 18:57:04 by nachofluxa        #+#    #+#             */
/*   Updated: 2026/02/20 10:54:16 by nachofluxa       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "server.hpp"
#include <cstdlib>
#include <iostream>

int main(int argc, char **argv)
{
	// int				aux;
	unsigned int	port;
	if (argc != 3)
	{
		std::cout << "There must be 3 and only 3 args." << std::endl;
		return (1);
	}
	port = std::atoi( argv[ 2 ] );
	if ( port < 1024 )
	{
		std::cerr << "Invalid port. ( 1024 – 49151 )" << std::endl;
		return ( 1 );
	}
	Server serv( port, -1 );

	serv.init();
	serv.run();
	return ( 0 );
}
