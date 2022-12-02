/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 10:48:58 by tnguyen-          #+#    #+#             */
/*   Updated: 2022/11/30 02:37:02 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): _port(8080), _password(""){}

Server::Server(int port, std::string password): _port(port), _password(password){}

int	Server::security(int connection)
{
	/* Read from the connection */
	char	buffer[BUFFER_SIZE + 1];
	int	bytesRead = read(connection, buffer, BUFFER_SIZE); /* last arguments are flags */
	std::cout << "The message of size " << bytesRead << " was: " << buffer;
	return (0);
}

void	Server::disconnectClient(int fd)
{
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (_client_fd[i] == fd)
		{
			// loop through all channels to notify them of this user's disconnection
			_io.erase(fd);
			_user.erase(fd);
			close(fd);
			_client_fd[i] = -1;
			return ;
		}
	}
}

void	Server::acceptClient()
{
	sockaddr_in	c;
	socklen_t	clen = sizeof(sockaddr_in);
	int			fd;
	
	fd = accept(_client_fd[0], (sockaddr*)&c, &clen);
	if (fd < 0)
	{
		std::cerr << "Cannot accept client: " << strerror(errno) << std::endl;
		return ;
	}

	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (_client_fd[i] == -1)
		{
			_client_fd[i] = fd;
			_user.insert(std::make_pair(fd, new User(c.sin_addr.s_addr)));
			_io.insert(std::make_pair(fd, new SocketIo(fd)));
			std::cout << "Client successfully connected" << std::endl; /*debug message*/
			return ;
		}
	}
	std::cerr << "Cannot accept client: Server is full" << std::endl;
}

void Server::manageClient(int fd)
{
//	User		*user;
	SocketIo	*io;
	std::string msg;

	//user = _user[fd];
	io = _io[fd];

	std::cout << "\e[38;5;247mFD " << fd << " is sending data\e[0m" << std::endl;
		
	try
	{
		io->Receive();
	}
	catch (SocketIo::EConnectionClosed& e)
	{
		std::cerr << "Remote client disconnected" << std::endl;
		disconnectClient(fd);
		return ;
	}
	catch (SocketIo::ERecvError& e)
	{
		std::cerr << e.what() << std::endl;
		std::cerr << "baguette" << std::endl;
		return ;
	}
	if (io->CanConsume())
	{
		msg = io->Consume();
		std::cout << "\e[32m(" << fd << ") recv: " << msg << "\e[0m" << std::endl;

		command(msg, fd);
	}
}

int	Server::init()
{
	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Failed to create socket. Errno: " << strerror(errno) << std::endl;
		return (-1);
	}

	std::cout << "Server socket successfully created" << std::endl;
	
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(_port); //htons convert a number to network byte order
	
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port " << _port << " Errno: " << strerror(errno) << std::endl;
		return (-1);
	}

	std::cout << "Server socket successfully bound on port " << _port << std::endl;
	
	/* start listening. Hold at most 10 connections in the queue */
	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Failed to listen on socket. Errno: " << strerror(errno) << std::endl;
		return (-1);
	}

	std::cout << "Server socket successfully listening" << std::endl;

	/* Setting unused sockets to -1 */
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		_client_fd[i] = -1;
	}
	_client_fd[0] = sockfd;

	initCmds();

	return (0);
}

void Server::run()
{
	fd_set	rfds;
	
	while (1) // until we implement a way to close our server by listening to descriptor 0
	{
		FD_ZERO(&rfds);
		for (int i = 0; i < MAX_CLIENT; i++)
		{
			if (_client_fd[i] >= 0)
				FD_SET(_client_fd[i], &rfds);
		}

		/*
			select() allows us to await socket input without wasting CPU cycles.
			Whenever we are notified, bits within rfds will be set and we can check them with FD_ISSET
			_client_fd[0] represent our server socket, it wakes up upon receiving a connection
			Other array cells represent client sockets, they wake up upon receiving packets
		*/
		int	retSelect = select(FD_SETSIZE, &rfds, NULL, NULL, NULL);

		if (retSelect == -1)
		{
			std::cerr << strerror(errno) << std::endl;
			break ;
		}
		
		for (int i = 1; i < MAX_CLIENT; i++)
		{
			if (_client_fd[i] >= 0 && FD_ISSET(_client_fd[i], &rfds))
				manageClient(_client_fd[i]);
		}

		if (FD_ISSET(_client_fd[0], &rfds))
			acceptClient();
	}
}

void Server::shutdown()
{
	// destroy all channels

	for (std::map<int, User *>::iterator it = _user.begin(); it != _user.end(); ++it)
	{
		delete (it->second);
	}
	for (std::map<int, SocketIo *>::iterator it = _io.begin(); it != _io.end(); ++it)
	{
		delete (it->second);
	}
	
	/* First we close all client and then we close server */
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (_client_fd[i] >= 0)
			close(_client_fd[i]);
	}
	close(_client_fd[0]);
}

Server::~Server() {}