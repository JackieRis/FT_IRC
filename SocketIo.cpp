/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketIo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/08 13:42:24 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/07 16:28:57 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketIo.hpp"

#include <iostream>
# include <sys/errno.h>

SocketIo::SocketIo(int newFd) : os(), is(), fd(newFd)
{
	
}

SocketIo::~SocketIo()
{

}

int	SocketIo::Send()
{
	std::string tmp;

	os << "\r\n";
	SocketIo::sentkb += os.str().length();
	SocketIo::sentMsg++;
	tmp = os.str();
	os.clear();
	os.str(std::string());
	return (send(fd, tmp.c_str(), tmp.size(), 0));
}

bool	SocketIo::Receive()
{
	char	buffer[4096 + 1];
	int		rd;
	
	rd = recv(fd, (void *)buffer, 4096, 0);
	
	if (rd < 0)
		throw ERecvError();
	if (rd == 0)
		throw EConnectionClosed();
	
	SocketIo::recvkb += rd;
	
	buffer[rd] = '\0';
	is << buffer;
	consumable = (is.str().find("\r\n") != std::string::npos);
	return (consumable);
}

std::string	SocketIo::Consume()
{
	std::string tmp;
	size_t		pos;

	if (!consumable)
		return (""); // bad consume, throw exception maybe ?

	SocketIo::recvMsg++;
	tmp = is.str();
	pos = tmp.find("\r\n");
	is.str(std::string(tmp.substr(pos + 2))); // make sure to keep older content if a packet contains multiple lines
	is.clear();
	tmp = tmp.substr(0, pos); // removed +2 because we don't want \r\n in the parsing
	consumable = (is.str().find("\r\n") != std::string::npos); // check if we can consume again
	return (tmp);
}

bool	SocketIo::CanConsume() const
{
	return (consumable);
}

const char *SocketIo::EConnectionClosed::what() const throw()
{
	return ("Client connection dropped");
}

const char *SocketIo::ERecvError::what() const throw()
{
	return (strerror(errno));
}

/* Necessary for static members */
uint64_t SocketIo::sentkb;
uint64_t SocketIo::sentMsg;
uint64_t SocketIo::recvkb;
uint64_t SocketIo::recvMsg;