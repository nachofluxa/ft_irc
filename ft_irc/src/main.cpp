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

#include "check_argv.hpp"
#include "server.hpp"

void	start_ft_irc(void)
{
	std::cout << "WELL DONE, YOU ARE IN." << std::endl;
}

int main(int argc, char **argv)
{
	int	aux;
	if (argc != 3)
	{
		std::cout << "There must be 3 and only 3 args." << std::endl;
		return (1);
	}
	aux = check_argv(argv);
	if (aux == 1)
		start_ft_irc();
	else if (aux == 2)
		std::cout << "There is something wrong with the executable." << std::endl;
	else if (aux == 3)
		std::cout << "There is something wrong with the port." << std::endl;
	else if (aux == 4)
		std::cout << "There is something wrong with the password." << std::endl;
	else
		std::cout << "Something went wrong." << std::endl;
	return (1);
}