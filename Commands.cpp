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
		Rep::E421(*(_io[fd]), _user[fd]->GetNick(), params[0]);
		std::cout << "send E421 Unknown Command " << params[0] << std::endl;
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
	SocketIo	*io = _io[fd];
	
	if (user->GetRegistered())
	{
		Rep::E462(*io, user->GetNick());
		return ;
	}

	if (input.size() < 5)
	{
		/* Make this a NumericReply utility */
		Rep::E461(NR_IN, input[0]); /* check if string is correct */
		return ;
	}
	user->SetName(input[1]);
	user->SetRealName(input[4]);

	/* set user's mode, on login, only two modes are available, such as invisible */
	int mode = std::atoi(input[2].c_str());
	mode &= (0x2 | 0x4);

	user->SetMode(mode);
	user->SetDidUser(true);

	if (user->GetDidNick())
		welcomeUser(fd);
}

void Server::cmdNick(const std::vector<std::string>& input, int fd) /* must check if function terminated */
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	if (input.size() < 2)
	{
		Rep::E431(NR_IN);
		return ;
	}
	std::map<int, User *>::iterator	it = _user.begin();
	for (; it != _user.end(); ++it)
	{
		if (it->second->GetNick() == input[1])
		{
			Rep::E433(NR_IN, input[1]);
			return ;
		}
	}
	user->SetNick(input[1]);
	user->SetDidNick(true);
	
	if (user->GetDidUser())
		welcomeUser(fd);
}

void Server::cmdPass(const std::vector<std::string>& input, int fd) /* must add if already registered */
{
	SocketIo	*io = _io[fd];

	if (_user[fd]->GetRegistered() == true)
	{
		Rep::E462(*io, _user[fd]->GetNick());
		return ;
	}
	if (input.size() < 2)
	{	
		Rep::E461(*io, _user[fd]->GetNick(), input[0]);
		disconnectClient(fd);
		return ;
	}

	if (input[1] != _password)	/* wrong password, then user get disconnected */
	{
		Rep::E464(*io, _user[fd]->GetNick());
		disconnectClient(fd);
		return ;
	}

	_user[fd]->SetPass(true);
}

void Server::cmdPing(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 2)
	{
		//idk what to put
		return ;
	}

	(*io) << "PONG " << input[1];
	io->Send();
}

void Server::cmdPong(const std::vector<std::string>& input, int fd)
{
	SocketIo	*io = _io[fd];

	(*io) << input[1];
}

void Server::cmdJoin(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later since we might have to create the channel on the fly */

	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 1)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	if (input[1] == "0")
	{
		PartUserFromAllChannel(user, std::string(":Leaving all channels"));
		removeAllChannels(true);
		return ;
	}

	std::vector<std::string> lst = Utils::ToList(input[1]);
	std::vector<std::string> keyLst;
	
	if (input.size() >= 3)
		keyLst = Utils::ToList(input[2]);
	
	std::vector<std::string>::const_iterator it = lst.begin();
	std::vector<std::string>::const_iterator keyIt = keyLst.begin();
	bool creator = false;

	for (; it != lst.end(); ++it)
	{
		// check if channel exist or can be created, validate channel name
		if (_channel.find(*it) == _channel.end())
		{
			creator = true;
			addChannel(*it, user);
		}
		chan = _channel[*it];

		if (!creator)
		{
			// check BAD KEY
			// check BANNED
			// check INVITE ONLY

			chan->AddUser(user);
		}

		/* I hate this I need to change this for later */
		const std::set<User *>& usrList = chan->GetUsers();

		for (std::set<User *>::const_iterator uit = usrList.begin(); uit != usrList.end(); ++uit)
		{
			(*(_userToIoLookup[*uit])) << ":" << user->GetNick() << " JOIN " << chan->GetName();
			(_userToIoLookup[*uit])->Send();
		}

		/* Send Topic*/
		Rep::R332(*io, chan->GetName(), user->GetNick(), chan->GetTopic());
		
		/* Send channel user list as nicks */
		for (std::set<User *>::const_iterator uit = usrList.begin(); uit != usrList.end(); ++uit)
			Rep::R353(NR_IN, user->GetNick(), (*uit)->GetNick());
		Rep::R366(NR_IN, user->GetNick());

		if (creator)
		{
			// give op and notify
		}


		if (keyIt != keyLst.end())
			++keyIt;
	}
}

/* Parameters: <target>{,<target>} <text to be sent> */

int	Server::checkChan(std::string name)
{
	if (name[0] == '#')
		return (1);
	if (name[0] == '%')
	{
		if (name.size() < 4 || name[1] != '#')
			return (-1);
		return (2);
	}
	if (name[0] == '@')
	{
		if (name.size() < 5 || (name[1] != '%' && name[2] != '#'))
			return (-1);
		return (3);
	}
	return (0);
}

void	Server::cmdPrivmsg(const std::vector<std::string>& input, int fd)
{
	SocketIo	*io = _io[fd];

	std::cerr << input[input.size() - 1] << std::endl;

	if (_user[fd]->GetRegistered() == false)
	{
		Rep::E451(*io, _user[fd]->GetNick());
		return ;
	}
	if (input.size() < 3)
	{
		Rep::E421(*io, _user[fd]->GetNick(), input[0]);
		return ;
	}
	int	i = checkChan(input[1]);
	switch (i)
	{
	case 3:
		;//send to admin ? wtf i don't understand cause my english is very bad
	case 2:
		;//send to admin
	case 1:
		{
			std::map<std::string, Channels *>::iterator	Sit = _channel.find(input[1]);
			if (Sit != _channel.end())
			{
				ChanMsg(fd, input[2], Sit->second);
				return ;
			}
			break ;
		}
	case -1:
		Rep::E404(*io, _user[fd]->GetNick(), input[1]);
		return ;
		break ;
	default:
		{	
			std::vector<std::string>			tmp = Utils::ToList(input[1]);
			size_t								nUser = tmp.size();
			std::vector<std::string>::iterator	it = tmp.begin();
			std::map<int,User *>::iterator		Mit = _user.begin();
			for (; Mit != _user.end() && nUser != 0; ++Mit)
			{
				it = std::find(tmp.begin(), tmp.end(), Mit->second->GetNick());
				if (it != tmp.end())
				{
					(*_io[Mit->first]) << ":" << _user[fd]->GetNick() << " PRIVMSG " << Mit->second->GetNick() << " " << input[2];
					(*_io[Mit->first]).Send();
					nUser--;
				}
			}
			break;
		}
	}
}

/* Same functionalities than PRIVMSG but server or client can't send auto replies and server can't send errors */

/* /NOTICE <msgTarget>, <text> */
void	Server::cmdNotice(const std::vector<std::string>& input, int fd)
{
	std::vector<std::string>	tmp = Utils::ToList(input[1]);
	std::vector<std::string>::iterator	it = tmp.begin();

	if (input.size() < 3)
		return ;
	int	i = checkChan(input[1]);
	switch (i)
	{
	case 3:
		;//idk
	case 2:
		;//ops
	case 1:
		{
			std::map<std::string, Channels *>::iterator	Sit = _channel.find(input[1]);
			if (Sit != _channel.end())
			{
				ChanMsg(fd, input[2], Sit->second);
				return ;
			}
			break ;
		}
	case -1:
		return ;

	default:
		{
			size_t								nUser = tmp.size();
			std::map<int,User *>::iterator		Mit = _user.begin();

			for (; Mit != _user.end() && nUser != 0; ++Mit)
			{
				it = std::find(tmp.begin(), tmp.end(), Mit->second->GetNick());
				if (it != tmp.end())
				{
					(*_io[Mit->first]) << ":" << _user[fd]->GetNick() << " PRIVMSG " << Mit->second->GetNick() << " " << input[2];
					(*_io[Mit->first]).Send();
					nUser--;
				}
			}
			break;
		}
	}
}

void Server::cmdMode(const std::vector<std::string>& input, int fd)
{
	(void)input;(void)fd;
}

void Server::cmdPart(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later */

	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 1)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	std::vector<std::string> chanLst = Utils::ToList(input[1]);
	std::vector<std::string>::iterator it = chanLst.begin();

	for (; it != chanLst.end(); ++it)
	{
		if (_channel.find(*it) == _channel.end())
		{
			Rep::E403(NR_IN, *it);
			continue ;
		}
		chan = _channel[*it];

		if (!chan->HasUser(user))
		{
			Rep::E442(NR_IN, chan->GetName());
			continue ;
		}
		/* send an empty message if there is no PART message */
		PartUserFromChannel(user, chan, (input.size() < 3) ? std::string(":") : input[2]);
	}
	removeAllChannels(true);
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
	_cmds.insert(std::make_pair(std::string("PART"), &Server::cmdPart));
}