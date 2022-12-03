/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <aberneli@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 05:14:00 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/03 20:17:31 by aberneli         ###   ########.fr       */
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

// utilise un autre fichier, ici c'est réservé pour les trucs de la map de pointeur de fonction ouais aller adieu
void Server::cmdJoin(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later since we might have to create the channel on the fly */

	if (!user->GetRegistered())
	{
		(*io) << 451 << " :You are not registered";
		io->Send();
		return ;
	}

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
	bool creator = false;

	for (; it != lst.end(); ++it)
	{
		// check if channel exist or can be created
		if (_channel.find(*it) == _channel.end())
		{
			creator = true;
			addChannel(*it, user);
		}
		chan = _channel[*it];
		(void)chan;
		if (!creator)
		{
			// check BAD KEY
			// check BANNED
			// check INVITE ONLY
		}

		// if successful
		(*io) << ":" << user->GetNick() << " JOIN " << *it;
		io->Send();

		(*io) << 332 << " " << "TOPIC";
		io->Send();

		// for each user in list, send RPL_NAMES then RPL_ENDNAMES

		if (creator)
		{
			// give op and notify
		}

		if (keyIt != keyLst.end())
			++keyIt;
	}
}

/* Parameters: <target>{,<target>} <text to be sent> */
void Server::cmdPrivmsg(const std::vector<std::string>& input, int fd)
{
	SocketIo	*io = _io[fd];

	if (_user[fd]->GetRegistered() == false)
	{
		(*io) << 451 << " :You are not registered";
		(*io).Send();
		return ;
	}
	if (input.size() < 3)
	{
		(*io) << 412 << " :Not enough parameters";
		(*io).Send();
		return ;
	}
	if (input[1][0] == '#' || input[1][0] == '&') /* if target starts with '#'or '&' then it is a channel */ //edit because #& can be anywhere in the string
	{
		std::map<std::string, Channels *>::iterator	it = _channel.find(input[1]);
		if (it != _channel.end())
		{
			;/* send msg in channel  */
		}
		(*io) << 404 << " :Channel not found"; /* 404 -> ERR_CANNOTSENDTOCHAN */
		(*io).Send();
		return ;
	}
	std::vector<std::string>			tmp = Utils::ToList(input[1]);
	size_t								nUser = tmp.size();
	std::vector<std::string>::iterator	it = tmp.begin();
	std::map<int,User *>::iterator		Mit = _user.begin();
	for (; Mit != _user.end() && nUser != 0; ++Mit)
	{
		it = std::find(tmp.begin(), tmp.end(), Mit->second->GetNick());
		if (it != tmp.end())
		{
			// if (Mit->second->GetRegistered() == false) // waiting afk functionnality
			// {
			// 	(*io) << 301 << " :User disconnected";
			// 	(*io).Send();
			// }
			//send msg
			(*_io[Mit->first]) << input[2];
			(*_io[Mit->first]).Send();
			nUser--;
		}
	}
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