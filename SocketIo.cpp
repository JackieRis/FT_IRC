/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketIo.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/08 13:42:24 by aberneli          #+#    #+#             */
/*   Updated: 2023/01/18 16:10:07 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "SocketIo.hpp"

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

	tmp = os.str();
	os.clear();
	os.str(std::string());
	tmp = tmp.substr(0, 510) + "\r\n";
	SocketIo::sentkb += tmp.length();
	SocketIo::sentMsg++;
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
	return (tmp.substr(0, 510)); // cap the message length to 512 (510 + the \r\n we forced out of the message), discard the rest
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