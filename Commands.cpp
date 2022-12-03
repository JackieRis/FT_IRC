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

/* USER <username> <flags> <> <realname> */
void Server::cmdUser(const std::vector<std::string>& input, int fd) /* must add check name and realname */
{
	User		*user = _user[fd];
	//SocketIo	*io = _io[fd];
	
	if (input.size() < 5)
	{
		/* Make this a NumericReply utility */
		(*_io[fd]) << 461 << " :Not enough parameters";
		(*_io[fd]).Send();
		return ;
	}
	user->SetName(input[1]);
	user->SetRealName(input[4]);

	/* set user's mode, on login, only two modes are available, such as invisible */
	int mode = std::atoi(input[2].c_str());
	mode &= (0x2 | 0x4);

	user->SetMode(mode);
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
			(*_io[fd]) << 433 << " * :Nickname already taken";
			(*_io[fd]).Send();
			return ;
		}
	}
	user->SetNick(input[1]);
	
	// numericreply RPL_WELCOME
	// set registered
}

void Server::cmdPass(const std::vector<std::string>& input, int fd) /* must add if already registered */
{
	if (_user[fd]->GetRegistered() == true)
	{
		(*_io[fd]) << 462 << " * :Already registered";
		(*_io[fd]).Send();
		return ;
	}
	if (input.size() < 2)
	{	
		(*_io[fd]) << 461 << " :Not enough parameters";
		(*_io[fd]).Send();
		disconnectClient(fd);
		return ;
	}

	if (input[1] != _password)	/* wrong password, then user get disconnected */
	{
		/* "464 * :Wrong password" */
		(*_io[fd]) << 464 << " * :Wrong password";
		(*_io[fd]).Send();
		disconnectClient(fd);
		return ;
	}

	_user[fd]->SetPass(true);
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
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan;

	if (input.size() < 1)
	{
		(*io) << 461 << " " << user->GetNick() << " :Not enough parameters";
		io->Send();
		return ;
	}

	if (input[1] == "0")
	{
		// PART all channels
		return ;
	}

	std::vector<std::string> lst = Utils::ToList(input[1]);
	std::vector<std::string> keyLst;
	
	if (input.size() >= 2)
		keyLst = Utils::ToList(input[2]);
	
	std::vector<std::string>::const_iterator it = lst.begin();
	std::vector<std::string>::const_iterator keyIt = keyLst.begin();

	for (; it != lst.end(); ++it)
	{
		// check if channel exist or can be created


		// if successfull
		(*io) << ":" << user->GetNick() << " JOIN " << *it;
		io->Send();

		(*io) << 332 << " " << "TOPIC";
		io->Send();

		if (keyIt != keyLst.end())
			++keyIt;
	}

	// for each channels

	/*
		If a client’s JOIN command to the server is successful, the server MUST send, in this order:

		A JOIN message with the client as the message <source> and the channel they have joined as the first parameter of the message.
		:Nickname JOIN CHANNELNAME

		The channel’s topic (with RPL_TOPIC (332) and optionally RPL_TOPICWHOTIME (333)), and no message if the channel does not have a topic.
		332 TOPIC_TEXT

		A list of users currently joined to the channel (with one or more RPL_NAMREPLY (353) numerics followed by a single RPL_ENDOFNAMES (366) numeric). These RPL_NAMREPLY messages sent by the server MUST include the requesting client that has just joined the channel.
		535 SYMBOL*@= a_nick
		535 b_nick
		535 c_nick
		...
		366 CHANNEL_NAME :End of list
	*/

}

/* Parameters: <target>{,<target>} <text to be sent> */
void Server::cmdPrivmsg(const std::vector<std::string>& input, int fd)
{
	if (input.size() < 3)
	{
		(*_io[fd]) << 412 << " :Not enough parameters";
		(*_io[fd]).Send();
		return ;
	}
	if (input[1][0] == '#' || input[1][0] == '&') /* if target starts with '#'or '&'then it is a channel */
	{
		std::map<std::string, Channels *>::iterator	it = _channel.find(input[1]);
		if (it != _channel.end())
		{
			;/* send msg in channel  */
		}
		(*_io[fd]) << 404 << " :Channel not found"; /* 404 -> ERR_CANNOTSENDTOCHAN */
		(*_io[fd]).Send();
		return ;
	}
	std::map<int, User *>::iterator	it = _user.begin();
	for (; it != _user.end(); ++it)
	{
		/* code */
	}
	
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