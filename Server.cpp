/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 10:48:58 by tnguyen-          #+#    #+#             */
/*   Updated: 2023/01/18 16:11:23 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

Server::Server(): _cfg(), _port(8080), _password("") {}

Server::Server(int port, std::string password): _cfg(), _port(port), _password(password) {}

void	Server::ChanMsg(User *sender, const std::string& msg, Channels* chan, std::string type)
{
	const std::set<User *>&				tmp = chan->GetUsers();
	std::set<User *>::const_iterator	it = tmp.begin();
	
	for (; it != tmp.end(); ++it)
	{
		if (sender == *it)
			continue ;
		
		SocketIo& io = *_userToIoLookup[*it];
		io << ":" << sender->GetNick() << type << chan->GetName() <<  " " << msg;
		io.Send();
	}
}

int	Server::security(int connection)
{
	/* Read from the connection */
	char	buffer[BUFFER_SIZE + 1];
	int	bytesRead = read(connection, buffer, BUFFER_SIZE); /* last arguments are flags */
	std::cout << "The message of size " << bytesRead << " was: " << buffer;
	return (0);
}

void Server::welcomeUser(int fd)
{
	SocketIo	*io = _io[fd];
	User		*user = _user[fd];

	user->SetRegistered(true);
	Rep::R001(NR_IN);
	Rep::R002(NR_IN, _servName, "1.0");
	Rep::R003(NR_IN, _startupDateFormatted);
	Rep::R004(NR_IN);
	//_nickToUserLookup.insert(std::make_pair(user->GetNick(), user));
}

void Server::removeAllChannels(bool emptyOnly)
{
	std::map<std::string, Channels *>::iterator it;

	for (it = _channel.begin(); it != _channel.end(); )
	{
		if (!emptyOnly || it->second->GetSize() == 0)
		{
			delete it->second;
			_channel.erase(it++);
		}
		else
			++it;
	}
}

void Server::addChannel(const std::string& cName, User *creator)
{
	Channels *chan = new Channels(cName, creator);
	_channel.insert(std::make_pair(cName, chan));
}

void Server::removeChannel(std::map<std::string, Channels *>::iterator it)
{
	if (it == _channel.end())
		return ;
	delete it->second;
	_channel.erase(it);
}

void Server::removeFromChannel(const std::string& cName, User *user)
{
	std::map<std::string, Channels *>::iterator it = _channel.find(cName);

	if (it == _channel.end())
		return ;

	Channels *chan = it->second;

	chan->RemoveUser(user);
	if (chan->GetSize() == 0)
		removeChannel(it);
}

void	Server::disconnectClient(int fd)
{
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (_client_fd[i] == fd)
		{
			User		*user = _user[fd];
			SocketIo	*io = _io[fd];

			_nickToUserLookup.erase(user->GetNick());
			_userToIoLookup.erase(user);
			_io.erase(fd);
			_user.erase(fd);

			delete io;
			delete user;

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
			_user.insert(std::make_pair(fd, new User(c.sin_addr.s_addr, fd)));
			_io.insert(std::make_pair(fd, new SocketIo(fd)));
			_userToIoLookup.insert(std::make_pair(_user[fd], _io[fd]));
			std::cout << "Client successfully connected" << std::endl; /*debug message*/
			return ;
		}
	}
	std::cerr << "Cannot accept client: Server is full" << std::endl;
}

void Server::manageClient(int fd)
{
	User		*user;
	SocketIo	*io;
	std::string msg;

	user = _user[fd];
	io = _io[fd];

	std::cout << "\e[38;5;247mFD " << fd << " is sending data\e[0m" << std::endl;
		
	try
	{
		io->Receive();
	}
	catch (SocketIo::EConnectionClosed& e)
	{
		std::cerr << "Remote client disconnected" << std::endl;
		QuitUserFromServer(user, ":Connection lost");
		disconnectClient(fd);
		return ;
	}
	catch (SocketIo::ERecvError& e)
	{
		std::cerr << e.what() << std::endl;
		return ;
	}
	while (io->CanConsume())
	{
		msg = io->Consume();
		std::cout << "\e[32m(" << fd << ") recv: " << msg << "\e[0m" << std::endl;

		command(msg, fd);
		if (user->HasDisconnected())
		{
			disconnectClient(fd);
			break ;
		}
	}
}

int	Server::init()
{
	_servName = _cfg.servConfig["name"];

	int	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1)
	{
		std::cerr << "Failed to create socket. Errno: " << strerror(errno) << std::endl;
		return (-1);
	}
	
	sockaddr_in sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_addr.s_addr = INADDR_ANY;
	sockaddr.sin_port = htons(_port); //htons convert a number to network byte order
	
	if (bind(sockfd, (struct sockaddr*)&sockaddr, sizeof(sockaddr)) < 0)
	{
		std::cerr << "Failed to bind to port " << _port << " Errno: " << strerror(errno) << std::endl;
		close(sockfd);
		return (-1);
	}
	
	/* start listening. Hold at most 10 connections in the queue */
	if (listen(sockfd, 10) < 0)
	{
		std::cerr << "Failed to listen on socket. Errno: " << strerror(errno) << std::endl;
		close(sockfd);
		return (-1);
	}

	/* Setting unused sockets to -1 */
	for (int i = 0; i < MAX_CLIENT; i++)
	{
		_client_fd[i] = -1;
	}
	_client_fd[0] = sockfd;

	initCmds();

	char date_string[128];
	time_t curr_time;
	tm *curr_tm;
	_startupTimestamp = std::time(&curr_time);
	curr_tm = std::localtime(&curr_time);

	std::strftime(date_string, 50, "%c", curr_tm);

	_startupDateFormatted = date_string;

	std::cout << "Server Ready on port " << _port << std::endl;

	return (0);
}

void Server::run()
{
	fd_set	rfds;
	
	while (true) // Server can be stopped with CTRL+D or "EXIT\n"
	{
		FD_ZERO(&rfds);
		for (int i = 0; i < MAX_CLIENT; i++)
		{
			if (_client_fd[i] >= 0)
				FD_SET(_client_fd[i], &rfds);
		}
		FD_SET(0, &rfds);

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
		
		if (FD_ISSET(0, &rfds))
		{
			char buffer[128 + 1];
			int rd;
			
			rd = read(0, buffer, 128);
			if (rd == 0)
				break ; /* close server, ignore other input */
		}
	}
}

void Server::shutdown()
{
	std::cout << "Shutting down server..." << std::endl;

	removeAllChannels(false);

	for (std::map<int, User *>::iterator it = _user.begin(); it != _user.end(); ++it)
	{
		delete (it->second);
	}
	for (std::map<int, SocketIo *>::iterator it = _io.begin(); it != _io.end(); ++it)
	{
		delete (it->second);
	}
	
	/*
		First we close all client and then we close server
		This effectively kicks everyone without warning
	*/
	for (int i = 1; i < MAX_CLIENT; i++)
	{
		if (_client_fd[i] >= 0)
			close(_client_fd[i]);
	}
	close(_client_fd[0]);
}

Server::~Server()
{

}
