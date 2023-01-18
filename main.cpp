/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 03:43:20 by tnguyen-          #+#    #+#             */
/*   Updated: 2023/01/18 16:09:50 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

int	main(int argc, char **argv)
{
	Server	*serv;
	SocketIo::sentkb = 0;
	SocketIo::sentMsg = 0;
	SocketIo::recvkb = 0;
	SocketIo::recvMsg = 0;

	if (argc != 3)
	{
		std::cerr << "usage: " << argv[0] << " <port> <password>" << std::endl;
		return (-1);
	}

	try
	{
		serv = new Server(std::atoi(argv[1]), argv[2]);
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return (-1);
	}
	
	if (serv->init() == -1)
	{
		delete serv;
		return (-1);
	}
		
	serv->run();
	serv->shutdown();

	delete serv;
	
	return (0);
}