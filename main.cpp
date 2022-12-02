/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 03:43:20 by tnguyen-          #+#    #+#             */
/*   Updated: 2022/11/29 23:34:03 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <cstdlib>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <iostream> // For cout
#include <unistd.h> // For read

int	main(int argc, char **argv)
{
	if (argc != 3)
	{
		std::cerr << "usage: " << argv[0] << " <port> <password>" << std::endl;
		return (-1);
	}
	Server	s(std::atoi(argv[1]), argv[2]);
	if (s.init() == -1)
		return (-1);

	s.run();
	s.shutdown();
	
	return (0);
}