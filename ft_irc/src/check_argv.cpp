/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check_argv.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachofluxa <nachofluxa@student.42.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/10 19:10:57 by nachofluxa        #+#    #+#             */
/*   Updated: 2026/02/20 10:54:36 by nachofluxa       ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "check_argv.hpp"

int	check_char(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;

	if (!s1 || !s2)
		return (-1);
	while (s1[i] && s2[i])
	{
		if (s1[i] != s2[i])
			return (-1);
		i++;
	}
	return (1);
}

int	check_argv(char **argv)
{
	int	solution;

	solution = 1;
	if (!check_char(argv[1], "443"/*PUERTO*/))
	{
		solution = -1;
		return (solution);
	}
	if (!check_char(argv[2], "contrasena"/*CONTRASEÑA*/))
	{
		solution = -1;
		return (solution);
	}
	return (solution);
}
