/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tnguyen- <tnguyen-@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 05:14:00 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/01 03:56:33 by tnguyen-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"

void Server::command(const std::string& str, int fd)
{
	const std::vector<std::string> params = Utils::ToParamList(str);

	if (params.empty())
	{
		std::cerr << "Invalid client input/no commands sent" << std::endl;
		return ;
	}

	std::map<std::string, cmdHandler>::iterator it = _cmds.find(params[0]);

	/* Unknown command, ignore or reply to user */
	if (it == _cmds.end())
	{
		std::cerr << "Invalid input command \"" << params[0] << "\" (full message: " << str << ")" << std::endl;
		return ;
	}

	std::cout << "Command called: " << params[0] << std::endl;

	/* Call the appropriate command handler */
	cmdHandler cmd = (*it).second;
	(this->*cmd)(params, fd);
}

void Server::cmdCap(const std::vector<std::string>&, int)
{
	/* We do not support CAP, silently ignore, don't even bother with ERR_NEEDMOREPARAM*/
}

void Server::cmdUser(const std::vector<std::string>& input, int fd) /* must add check name and realname */
{
	User		*user = _user[fd];
	//SocketIo	*io = _io[fd];
	
	/* USER <username> <flags> <> <realname> */
	if (input.size() < 5)
	{
		/* Make this a NumericReply utility */
		(*_io[fd]) << 461 << " :Not enough parameters";
		(*_io[fd]).Send();
		return ;
	}
	std::map<int, User *>::iterator	it = _user.begin();	/* let's search if there isn't a same name connected */
	for (; it != _user.end(); ++it)
	{
		if (it->second->GetName() == input[1])
		{
			(*_io[fd]) << 462 << " :User already exist";
			(*_io[fd]).Send();
			return ;
		}
	}
	user->SetName(input[1]);
	user->SetRealName(input[4]);
}

void Server::cmdNick(const std::vector<std::string>& input, int fd) /* must check if function terminated */
{
	User	*user = _user[fd];

	if (input.size() < 2)
	{
		(*_io[fd]) << 431 << " :No nickname given";
		(*_io[fd]).Send();
		return ;
	}
	std::map<int, User *>::iterator	it = _user.begin();
	for (; it != _user.end(); ++it)
	{
		if (it->second->GetNick() == input[1])
		{
			(*_io[fd]) << 433 << " :Nickname already taken";
			(*_io[fd]).Send();
			return ;
		}
	}
	user->SetNick(input[1]);
}

void Server::cmdPass(const std::vector<std::string>& input, int fd) /* must add if already registered */
{
	if (input.size() < 2)
	{	
		(*_io[fd]) << 461 << " :Not enough parameters";
		(*_io[fd]).Send();
		disconnectClient(fd);
		return ;
	}
	if (input[1] == _password)	/* wrong passwordm then user get disconnected */
	{
		(*_io[fd]) << 464 << " :Wrong password";
		(*_io[fd]).Send();
		disconnectClient(fd);
		return ;
	}
}

void Server::cmdPing(const std::vector<std::string>& input, int fd)
{
	//std::cout << input;
	//(*_io[fd]) << input;
	//(*_io[fd]).Send();
	(void)input;(void)fd;
}

void Server::cmdPong(const std::vector<std::string>& input, int fd)
{
	(void)input;(void)fd;
}

void Server::cmdJoin(const std::vector<std::string>& input, int fd)
{
	(void)input;(void)fd;
}

void Server::cmdPrivmsg(const std::vector<std::string>& input, int fd)
{
	(void)input;(void)fd;
}

void Server::cmdMode(const std::vector<std::string>& input, int fd)
{
	(void)input;(void)fd;
}

/* 
	We're using a map as a lookup to member function
*/
void Server::initCmds()
{
	_cmds.insert(std::make_pair(std::string("CAP"), &Server::cmdCap));
	_cmds.insert(std::make_pair(std::string("USER"), &Server::cmdUser));
	_cmds.insert(std::make_pair(std::string("NICK"), &Server::cmdNick));
	_cmds.insert(std::make_pair(std::string("PASS"), &Server::cmdPass));
	_cmds.insert(std::make_pair(std::string("PING"), &Server::cmdPing));
	_cmds.insert(std::make_pair(std::string("PONG"), &Server::cmdPong));
	_cmds.insert(std::make_pair(std::string("JOIN"), &Server::cmdJoin));
	_cmds.insert(std::make_pair(std::string("PRIVMSG"), &Server::cmdPrivmsg));
	_cmds.insert(std::make_pair(std::string("MODE"), &Server::cmdMode));
}