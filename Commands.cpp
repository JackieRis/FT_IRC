/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Commands.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 05:14:00 by aberneli          #+#    #+#             */
/*   Updated: 2023/01/16 15:24:38 by aberneli         ###   ########.fr       */
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
	_cmdsCalled["CAP"]++;
}

/* USER <username> <flags> <> <realname> */
void Server::cmdUser(const std::vector<std::string>& input, int fd) /* must add check name and realname */
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	
	_cmdsCalled["USER"]++;
	if (user->GetRegistered())
	{
		Rep::E462(*io, user->GetNick());
		return ;
	}

	if (!user->GetPass())
	{
		Rep::E464(NR_IN);
		user->SetHasDisconnected();
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

	user->SetDidUser(true);

	if (user->GetDidNick())
		welcomeUser(fd);
}

void Server::cmdNick(const std::vector<std::string>& input, int fd) /* must check if function terminated */
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["NICK"]++;
	if (input.size() < 2)
	{
		Rep::E431(NR_IN);
		return ;
	}

	if (!user->GetPass())
	{
		Rep::E464(NR_IN);
		user->SetHasDisconnected();
		return ;
	}
	
	if (_nickToUserLookup.find(input[1]) != _nickToUserLookup.end())
	{
		Rep::E433(NR_IN, input[1]);
		return ;
	}

	if (!Utils::IsValidNick(input[1]))
	{
		Rep::E432(NR_IN, input[1]);
		return ;
	}
	
	/* This is the re-nick case */
	if (user->GetRegistered())
	{
		std::string oldNick = user->GetNick();
		
		_nickToUserLookup.erase(oldNick);
		user->SetNick(input[1]);
		_nickToUserLookup.insert(std::make_pair(user->GetNick(), user));
		
		std::stringstream ss;
		ss << ":" << oldNick << " NICK " << user->GetNick();
		
		/* make sure the user receives it, in case they're in no channels */
		(*io) << ss.str();
		io->Send();

		SendToAllJoinedChannel(user, ss.str());
		return ;
	}

	user->SetNick(input[1]);
	user->SetDidNick(true);
	_nickToUserLookup.insert(std::make_pair(user->GetNick(), user));
	
	if (user->GetDidUser())
		welcomeUser(fd);
}

void Server::cmdPass(const std::vector<std::string>& input, int fd) /* must add if already registered */
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	
	_cmdsCalled["PASS"]++;
	if (_user[fd]->GetRegistered() == true)
	{
		Rep::E462(NR_IN);
		return ;
	}
	if (input.size() < 2)
	{	
		Rep::E461(NR_IN, input[0]);
		user->SetHasDisconnected();
		return ;
	}

	if (input[1] != _password)	/* wrong password, then user get disconnected */
	{
		Rep::E464(NR_IN);
		user->SetHasDisconnected();
		return ;
	}

	_user[fd]->SetPass(true);
}

void Server::cmdQuit(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	std::string quitMsg = ":Quit: ";

	if (input.size() >= 2) /* we need the text without the : as we have our own space in the quit message */
		quitMsg += input[1].substr((input[1][0] == ':'));

	QuitUserFromServer(user, quitMsg);

	(*io) << ":" << user->GetNick() << " ERROR :answer to QUIT";
	io->Send();

	user->SetHasDisconnected();
}

void Server::cmdPing(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["PING"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 2)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	(*io) << "PONG " << input[1];
	io->Send();
}

void Server::cmdPong(const std::vector<std::string>& input, int fd)
{
	(void)input; (void)fd;
	_cmdsCalled["PONG"]++;
	/* Nothing to answer to PONG, silently ignore */
}

void Server::cmdJoin(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later since we might have to create the channel on the fly */

	_cmdsCalled["JOIN"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 2)
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
	bool creator = false;
	size_t n = 0;

	for (; it != lst.end(); ++it, ++n)
	{
		if (!Utils::IsValidChannelName(*it))
		{
			Rep::E476(*io, *it); /* Don't use NR_IN here ! */
			continue ;
		}
		// check if channel exist or can be created, validate channel name
		if (_channel.find(*it) == _channel.end())
		{
			creator = true;
			addChannel(*it, user);
		}
		chan = _channel[*it];

		if (!creator)
		{
			int flags = chan->GetModes();
			if (flags & CM_LIMIT) /* Channel is limited and full */
			{
				if (chan->GetSize() >= chan->GetLimit())
				{
					Rep::E471(NR_IN, chan->GetName());
					continue ;
				}
			}

			if (flags & CM_INVITEONLY) /* Invite only, don't accept */
			{
				Rep::E473(NR_IN, chan->GetName());
				continue ;
			}
			
			if (flags & CM_BAN && chan->IsBanned(user)) /* Banned, don't accept */
			{
				Rep::E474(NR_IN, chan->GetName());
				continue ;
			}

			if (flags & CM_KEY) 
			{
				std::string inputKey = std::string("");

				if (n < keyLst.size())
					inputKey = keyLst[n];
				if (!chan->ValidateKey(inputKey)) /* invalid key, don't accept */
				{
					Rep::E475(NR_IN, chan->GetName());
					continue ;
				}
			}

			chan->AddUser(user);
		}

		std::stringstream msg;
		msg << ":" << user->GetNick() << " JOIN " << chan->GetName();
		SendToAllInChannel(chan, msg.str());

		/* Send Topic if not empty */
		if (!chan->GetTopic().empty())
			Rep::R332(NR_IN, chan->GetName(), chan->GetTopic());
		
		/* Send channel user list as nicks */
		const std::set<User *>& usrList = chan->GetUsers();
		for (std::set<User *>::const_iterator uit = usrList.begin(); uit != usrList.end(); ++uit)
			Rep::R353(NR_IN, chan->GetName(), (*uit)->GetNick(), chan->GetChanPrefix(), chan->GetUserPrefix((*uit)));
		Rep::R366(NR_IN, chan->GetName());
	}
}

void	Server::cmdPrivmsg(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["PRIVMSG"]++;

	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 2)
	{
		Rep::E411(NR_IN, input[0]); /* ERR_NORECIPIENT instead of the usual not enough arg */
		return ;
	}
	if (input.size() < 3)
	{
		Rep::E412(NR_IN); /* ERR_NOTEXTTOSEND */
		return ;
	}
	
		/* (chan->GetModes() & CM_VOICE && chan->GetModes() & CM_MODERATED) ) && !(chan->IsOpped(user) || chan->IsVoice(user) ) */
	
	std::vector<std::string>			tmp = Utils::ToList(input[1]);
	std::vector<std::string>::iterator	it = tmp.begin();
	std::map<int,User *>::iterator		Mit = _user.begin();

	for (; it != tmp.end(); ++it)
	{
		std::cout << *it << std::endl;
		std::map<std::string, Channels *>::iterator	Sit = _channel.find(*it);
		if (!(Utils::IsChannel(*it) == false || Sit == _channel.end()))
		{
			if (Sit->second->GetModes() & CM_NOEXTERNAL && !Sit->second->HasUser(user))
			{
				Rep::E404(NR_IN, Sit->first);
				continue ;
			}
			if (Sit->second->IsBanned(user))
			{
				Rep::E404(NR_IN, Sit->first);
				continue ;
			}
			if (Sit->second->GetModes() & CM_MODERATED && !(Sit->second->IsOpped(user) || Sit->second->IsVoice(user)) )
			{
				Rep::E404(NR_IN, Sit->first);
				continue ;
			}
			ChanMsg(user, input[2], Sit->second, " PRIVMSG ");
		}
		else
		{
			std::vector<std::string>::iterator	userIt;
			for (; Mit != _user.end(); ++Mit)
			{
				userIt = std::find(tmp.begin(), tmp.end(), Mit->second->GetNick());
				if (userIt != tmp.end())
				{
					(*_io[Mit->first]) << ":" << user->GetNick() << " PRIVMSG " << Mit->second->GetNick() << " " << input[2];
					(*_io[Mit->first]).Send();
				}
			}
		}
	}

}

/* Same functionalities than PRIVMSG but server or client can't send auto replies and server can't send errors */

/* /NOTICE <msgTarget>, <text> */
void	Server::cmdNotice(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	//SocketIo	*io = _io[fd];

	_cmdsCalled["NOTICE"]++;
	if (!user->GetRegistered())
		return ;
	if (input.size() < 3)
		return ;
	
	std::vector<std::string>			tmp = Utils::ToList(input[1]);
	std::vector<std::string>::iterator	it = tmp.begin();
	std::map<int,User *>::iterator		Mit = _user.begin();

	for (; it != tmp.end(); ++it)
	{
		std::cout << *it << std::endl;
		std::map<std::string, Channels *>::iterator	Sit = _channel.find(*it);
		if (!(Utils::IsChannel(*it) == false || Sit == _channel.end()))
		{	
			if (Sit->second->GetModes() & CM_NOEXTERNAL && !Sit->second->HasUser(user))
				continue ;
			if (Sit->second->IsBanned(user))
				continue ;
			if (Sit->second->GetModes() & CM_MODERATED && !(Sit->second->IsOpped(user) || Sit->second->IsVoice(user)) )
				continue ;
			ChanMsg(user, input[2], Sit->second, " NOTICE ");
		}
		else
		{
			std::vector<std::string>::iterator	userIt;
			for (; Mit != _user.end(); ++Mit)
			{
				userIt = std::find(tmp.begin(), tmp.end(), Mit->second->GetNick());
				if (userIt != tmp.end())
				{
					(*_io[Mit->first]) << ":" << user->GetNick() << " NOTICE " << Mit->second->GetNick() << " " << input[2];
					(*_io[Mit->first]).Send();
				}
			}
		}
	}
}

void Server::cmdMode(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["MODE"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 2)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	/* target == user */
	if (!Utils::IsChannel(input[1]))
	{
		if (!_nickToUserLookup.count(input[1]))
		{
			Rep::E401(NR_IN, input[1]);
			return ;
		}
		if (input[1] != user->GetNick())
		{
			Rep::E502(NR_IN);
			return ;
		}
		UserMode(input, fd);
		return ;
	}

	if (!_channel.count(input[1]))
	{
		Rep::E403(NR_IN, input[1]);
		return ;
	}
	/* target == channel */
	ChannelMode(input, fd);
}

void Server::cmdTopic(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later */

	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 2)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	if (_channel.find(input[1]) == _channel.end())
	{
		Rep::E403(NR_IN, input[1]);
		return ;
	}

	chan = _channel[input[1]];

	if (!chan->HasUser(user))
	{
		Rep::E442(NR_IN, chan->GetName());
		return ;
	}

	/* Topic Query */
	if (input.size() == 2)
	{
		if (chan->GetTopic().empty())
			Rep::R331(NR_IN, chan->GetName());
		else
		{
			Rep::R332(NR_IN, chan->GetName(), chan->GetTopic());
			Rep::R333(NR_IN, chan->GetName(), chan->GetLastTopicEditor(), chan->GetLastTopicChangeDate());
		}
		return ;
	}

	/* Topic Change */
	if (chan->GetModes() & CM_PROTECTEDTOPIC)
	{
		/* Chan explicitely requires channel permissions to change topic */
		if (!chan->IsOpped(user))
		{
			Rep::E482(NR_IN, chan->GetName());
			return ;
		}
	}

	std::string topic = input[2].substr((input[2][0] == ':'));

	chan->SetTopic(topic, user->GetNick());

	std::stringstream ss;
	ss << ":" << user->GetNick() << " TOPIC " << chan->GetName() << " :" << chan->GetTopic();
	
	SendToAllInChannel(chan, ss.str());
}

void Server::cmdTime(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["TIME"]++;
	Rep::R391(NR_IN, _servName);
	
	(void)input;
}

void Server::cmdStats(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["STATS"]++;
	if (input.size() < 2)
	{
		Rep::R219(NR_IN, "");
		return ;
	}
	
	std::string toDo;
	if (input[1].find('l') != std::string::npos)
	{
		std::stringstream ss;

		ss << _servName << " 0 ";
		ss << SocketIo::sentMsg << " " << (SocketIo::sentkb / 1024.0f) << " ";
		ss << SocketIo::recvMsg << " " << (SocketIo::recvkb / 1024.0f) << " ";
		ss << (time(0) - _startupTimestamp);
		
		Rep::R211(NR_IN, ss.str());
		toDo += 'l';
	}
	if (input[1].find('m') != std::string::npos)
	{
		std::map<std::string, int>::iterator it = _cmdsCalled.begin();
		for (; it != _cmdsCalled.end(); ++it)
		{
			std::stringstream ss;

			ss << it->first << " " << it->second;
			Rep::R212(NR_IN, ss.str());
		}
		toDo += 'm';
	}
	if (input[1].find('o') != std::string::npos)
	{
		/* may put the server ip in first, later*/
		Rep::R243(NR_IN, "O * * *");
		toDo += 'o';
	}
	if (input[1].find('u') != std::string::npos)
	{
		time_t uptime = time(0) - _startupTimestamp;
		char buffer[128];

		/* This is a LOT more convenient than using std::cout with <iomanip>, pardon my C */
		snprintf(buffer, 128, "Server Up for %d days and %d:%02d:%02d",
				(int)(uptime / (3600 * 24)),
				(int)(uptime / 3600) % 24,
				(int)(uptime / 60) % 60,
				(int)(uptime % 60));

		Rep::R242(NR_IN, std::string(buffer));
		toDo += 'u';
	}

	Rep::R219(NR_IN, toDo);
}

void Server::cmdNames(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later */

	_cmdsCalled["NAMES"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 2)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	const std::vector<std::string> chanList = Utils::ToList(input[1]);
	std::vector<std::string>::const_iterator it = chanList.begin();

	for (; it != chanList.end(); ++it)
	{
		if (!_channel.count(*it)) /* Channel doesn't exist */
		{
			Rep::R366(NR_IN, *it);
			continue ;
		}

		chan = _channel[*it];
		bool isOnChannel = chan->HasUser(user);

		if ((chan->GetModes() & CM_SECRET || chan->GetModes() & CM_PRIVATE) && !isOnChannel) /* Channel is secret and user isn't part of it */
		{
			Rep::R366(NR_IN, chan->GetName());
			continue ;
		}

		const std::set<User *>& chanUsers = chan->GetUsers();
		std::set<User *>::const_iterator uit = chanUsers.begin();

		for (; uit != chanUsers.end(); ++uit)
		{
			if (!isOnChannel && ((*uit)->GetMode() & UM_INVISIBLE)) /* Querying user is not on channel and this specific user is invisible ? don't list */
				continue ;
			Rep::R353(NR_IN, chan->GetName(), (*uit)->GetNick(), chan->GetChanPrefix(), chan->GetUserPrefix(*uit));
		}
		Rep::R366(NR_IN, chan->GetName());
	}
}

void Server::cmdLusers(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	(void)input;

	_cmdsCalled["LUSERS"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	int clientNb = _user.size();
	int invisibleNb = 0;
	int	opped = 0;

	std::map<int, User *>::const_iterator it = _user.begin();
	for (; it != _user.end(); ++it)
	{
		invisibleNb += (it->second->GetMode() & UM_INVISIBLE);
		opped += (it->second->GetMode() & UM_OPER);
	}

	std::stringstream ss;

	ss << ":There are " << clientNb << " users and " << invisibleNb << " invisible on 1 server(s)";

	Rep::R251(NR_IN, ss.str());
	Rep::R252(NR_IN, opped);
	Rep::R253(NR_IN, 0);
	Rep::R254(NR_IN, _channel.size());

	ss.str("");
	ss.clear();
	ss << ":I have " << clientNb << " clients and 0 servers";

	Rep::R255(NR_IN, ss.str());
	Rep::R265(NR_IN, clientNb);
	Rep::R266(NR_IN, clientNb);
}

void	Server::cmdWho(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	/* TODO */
	_cmdsCalled["WHO"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 2 || input[1] == "0") /* Both have the same behavior */
	{
		Rep::R315(NR_IN);
		return ;
	}
	Rep::R315(NR_IN);
}

void	Server::cmdWhois(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["WHOIS"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 2)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	std::vector<std::string> nickList = Utils::ToList(input[1]);
	std::vector<std::string>::iterator lit = nickList.begin();

	for (; lit != nickList.end(); ++lit)
	{
		if (!_nickToUserLookup.count(*lit))
			continue ;
		
		User *queryed = _nickToUserLookup[*lit];
		
		Rep::R311(NR_IN, queryed->GetNick(), queryed->GetName(), queryed->GetNick()); /* last one *should* be realname, oops */
		if (queryed->IsServerOpper())
			Rep::R313(NR_IN, queryed->GetNick());
		std::map<std::string, Channels *>::iterator cit = _channel.begin();
		for (; cit != _channel.end(); ++cit)
		{
			if (!cit->second->HasUser(queryed))
				continue ;
			Channels *chan = cit->second;
			Rep::R319(NR_IN, queryed->GetNick(), chan->GetUserPrefix(queryed), chan->GetName());
		}
	}
	Rep::R318(NR_IN, input[1]); /* End of list is for everything at once */
}

void	Server::cmdMotd(const std::vector<std::string>& input, int fd)
{
	(void)input;
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	_cmdsCalled["MOTD"]++;
	std::map<std::string, std::string>::iterator	Name = _cfg.servConfig.find("name");
	std::map<std::string, std::string>::iterator	Motd = _cfg.servConfig.find("motd");

	if (Motd != _cfg.servConfig.end())
	{
		Rep::R375(NR_IN, Name->second);
		Rep::R372(NR_IN, Motd->second);
		Rep::R376(NR_IN);
		return ;
	}
	Rep::E422(NR_IN);
}

void Server::cmdPart(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later */

	_cmdsCalled["PART"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}

	if (input.size() < 2)
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

void	Server::cmdInvite(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan; /* Get the pointer later */

	_cmdsCalled["INVITE"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 3)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}

	if (_channel.count(input[2]) == 0)
	{
		Rep::E403(NR_IN, input[2]);
		return ;
	}

	chan = _channel[input[2]];

	if (!chan->HasUser(user))
	{
		Rep::E442(NR_IN, chan->GetName());
		return ;
	}

	if (chan->GetModes() & CM_INVITEONLY)
	{
		/* Documentation recommands to make InviteOnly channels require op to INVITE */
		if (!chan->IsOpped(user))
		{
			Rep::E482(NR_IN, chan->GetName());
			return ;
		}
	}

	if (_nickToUserLookup.count(input[1]) == 0)
	{
		/* Invited user doesn't exist */
		Rep::E401(NR_IN, input[1]);
		return ;
	}

	User		*otherUser = _nickToUserLookup[input[1]];

	if (chan->HasUser(otherUser))
	{
		Rep::E443(NR_IN, chan->GetName(), otherUser->GetNick());
		return ;
	}

	/* Every checks have been done, invite the user over */

	SocketIo	*otherIo = _userToIoLookup[otherUser];

	chan->AddUser(otherUser);

	Rep::R341(NR_IN, otherUser->GetNick(), chan->GetName());

	(*otherIo) << ":" << user->GetNick() << " INVITE " << otherUser->GetNick() << " " << chan->GetName();
	otherIo->Send();

	/* Invite does not notify other channel users of this user's arrival  */
}

void	Server::cmdOper(const std::vector<std::string>& input, int fd)
{
	SocketIo*	io = _io[fd];
	User		*user = _user[fd];
	std::map<std::string, std::string>::iterator Mit =  _cfg.opperAccounts.begin();

	_cmdsCalled["OPER"]++;
	if (input.size() != 3)
	{
		Rep::E461(*io, user->GetNick(), "OPER");
		return ;
	}
	
	Mit = _cfg.opperAccounts.find(input[1]);
	if (Mit == _cfg.opperAccounts.end()) /* no OP username matching */
	{
		Rep::E464(NR_IN);
		return ;
	}
	if (Mit->second != input[2]) /* Provided password doesn't match the OP username */
	{
		Rep::E464(NR_IN);
		return ;
	}
	
	user->BecomeServerOper();
	Rep::R381(NR_IN);

	/* Notify everyone of the new OP user */
	std::map<int, SocketIo *>::iterator sit = this->_io.begin();
	std::stringstream ss;
	ss << "MODE +o " << user->GetNick();
	for (; sit != _io.end(); ++sit)
	{
		(*sit->second) << ss.str();
		(*sit->second).Send();
	}
}

/*	
	list: sans arguments, donne une liste de tout les channels du server avec leur topic
	Les channels en PRV sont affichés mais le topic n'est pas précisé
	Les channels en SECRET n'affichent rien

	Si arguments, affiches les channels en arguments uniquement, tout en suivant les règles juste au dessus
*/

void	Server::cmdList(const std::vector<std::string>& input, int fd)
{
	SocketIo*	io = _io[fd];
	User*		user = _user[fd];
	std::map<std::string, Channels *>::iterator Mit = _channel.begin();
	std::map<std::string, Channels *>::iterator	Mend = _channel.end();
	int			nuser = 0;

	_cmdsCalled["LIST"]++;
	if (input.size() == 1)
	{
		for (; Mit != Mend; ++Mit)
		{
			nuser = Mit->second->GetVisibleUsers(user);
			if (Mit->second->HasUser(user))
			{	
				Rep::R322(NR_IN, nuser, Mit->second->GetTopic(), Mit->second->GetName());
				continue ;
			}
			
			if (Mit->second->GetModes() & CM_PRIVATE && !(Mit->second->GetModes() & CM_SECRET))
				Rep::R322(NR_IN, nuser, " ", "Prv");
			else if (!(Mit->second->GetModes() & CM_SECRET))
				Rep::R322(NR_IN, nuser, Mit->second->GetTopic(), Mit->second->GetName());
		}
		Rep::R323(NR_IN);
		return ;
	}
	std::vector<std::string>			tmp = Utils::ToList(input[1]);
	std::vector<std::string>::iterator	it = tmp.begin();
	std::vector<std::string>::iterator	itend = tmp.end();

	for (; it != itend; ++it)
	{
		Mit = _channel.find(*it);
		if (Mit != Mend)
		{
			nuser = Mit->second->GetVisibleUsers(user);
			if (Mit->second->HasUser(user))
			{	
				Rep::R322(NR_IN, nuser, Mit->second->GetTopic(), Mit->second->GetName());
				continue ;
			}
			
			if (Mit->second->GetModes() & CM_PRIVATE && !(Mit->second->GetModes() & CM_SECRET))
				Rep::R322(NR_IN, nuser, " ", "Prv");
			else if (!(Mit->second->GetModes() & CM_SECRET))
				Rep::R322(NR_IN, nuser, Mit->second->GetTopic(), Mit->second->GetName());
		}
	}
	Rep::R323(NR_IN);
}

void	Server::cmdKick(const std::vector<std::string>& input, int fd)
{
	SocketIo*	io = _io[fd];
	User*		user = _user[fd];
	Channels	*chan;

	_cmdsCalled["KICK"]++;
	if (!user->GetRegistered())
	{
		Rep::E451(NR_IN);
		return ;
	}
	if (input.size() < 3)
	{
		Rep::E461(NR_IN, input[0]);
		return ;
	}
	if (!_channel.count(input[1]))
	{
		Rep::E403(NR_IN, input[1]);
		return ;
	}

	chan = _channel[input[1]];

	if (!chan->HasUser(user))
	{
		Rep::E442(NR_IN, chan->GetName());
		return ;
	}

	if (!chan->IsOpped(user))
	{
		Rep::E482(NR_IN, chan->GetName());
		return ;
	}

	if (!chan->HasUser(input[2]))
	{
		// NOT ON CHANNEL
		return ;
	}

	std::string msg = ":";
	msg += user->GetNick() + " KICK " + chan->GetName() + " " + input[2];
	if (input.size() >= 4)
		msg += " " + input[3];

	SendToAllInChannel(chan, msg);
	chan->RemoveUser(_nickToUserLookup[input[2]]);
	removeAllChannels(true);
}

/* 
	We're using a map as a lookup to member function
*/
void Server::initCmds()
{
	_cmds.insert(std::make_pair(std::string("CAP"), &Server::cmdCap));
	_cmdsCalled.insert(std::make_pair(std::string("CAP"), 0));
	_cmds.insert(std::make_pair(std::string("USER"), &Server::cmdUser));
	_cmdsCalled.insert(std::make_pair(std::string("USER"), 0));
	_cmds.insert(std::make_pair(std::string("NICK"), &Server::cmdNick));
	_cmdsCalled.insert(std::make_pair(std::string("NICK"), 0));
	_cmds.insert(std::make_pair(std::string("PASS"), &Server::cmdPass));
	_cmdsCalled.insert(std::make_pair(std::string("PASS"), 0));
	_cmds.insert(std::make_pair(std::string("QUIT"), &Server::cmdQuit));
	_cmdsCalled.insert(std::make_pair(std::string("QUIT"), 0));
	_cmds.insert(std::make_pair(std::string("PING"), &Server::cmdPing));
	_cmdsCalled.insert(std::make_pair(std::string("PING"), 0));
	_cmds.insert(std::make_pair(std::string("PONG"), &Server::cmdPong));
	_cmdsCalled.insert(std::make_pair(std::string("PONG"), 0));
	_cmds.insert(std::make_pair(std::string("OPER"), &Server::cmdOper));
	_cmdsCalled.insert(std::make_pair(std::string("OPER"), 0));
	_cmds.insert(std::make_pair(std::string("JOIN"), &Server::cmdJoin));
	_cmdsCalled.insert(std::make_pair(std::string("JOIN"), 0));
	_cmds.insert(std::make_pair(std::string("PRIVMSG"), &Server::cmdPrivmsg));
	_cmdsCalled.insert(std::make_pair(std::string("PRIVMSG"), 0));
	_cmds.insert(std::make_pair(std::string("NOTICE"), &Server::cmdNotice));
	_cmdsCalled.insert(std::make_pair(std::string("NOTICE"), 0));
	_cmds.insert(std::make_pair(std::string("MODE"), &Server::cmdMode));
	_cmdsCalled.insert(std::make_pair(std::string("MODE"), 0));
	_cmds.insert(std::make_pair(std::string("TOPIC"), &Server::cmdTopic));
	_cmdsCalled.insert(std::make_pair(std::string("TOPIC"), 0));
	_cmds.insert(std::make_pair(std::string("TIME"), &Server::cmdTime));
	_cmdsCalled.insert(std::make_pair(std::string("TIME"), 0));
	_cmds.insert(std::make_pair(std::string("STATS"), &Server::cmdStats));
	_cmdsCalled.insert(std::make_pair(std::string("STATS"), 0));
	_cmds.insert(std::make_pair(std::string("NAMES"), &Server::cmdNames));
	_cmdsCalled.insert(std::make_pair(std::string("NAMES"), 0));
	_cmds.insert(std::make_pair(std::string("LUSERS"), &Server::cmdLusers));
	_cmdsCalled.insert(std::make_pair(std::string("LUSERS"), 0));
	_cmds.insert(std::make_pair(std::string("WHO"), &Server::cmdWho));
	_cmdsCalled.insert(std::make_pair(std::string("WHO"), 0));
	_cmds.insert(std::make_pair(std::string("WHOIS"), &Server::cmdWhois));
	_cmdsCalled.insert(std::make_pair(std::string("WHOIS"), 0));
	_cmds.insert(std::make_pair(std::string("INVITE"), &Server::cmdInvite));
	_cmdsCalled.insert(std::make_pair(std::string("INVITE"), 0));
	_cmds.insert(std::make_pair(std::string("PART"), &Server::cmdPart));
	_cmdsCalled.insert(std::make_pair(std::string("PART"), 0));
	_cmds.insert(std::make_pair(std::string("MOTD"), &Server::cmdMotd));
	_cmdsCalled.insert(std::make_pair(std::string("MOTD"), 0));
	_cmds.insert(std::make_pair(std::string("LIST"), &Server::cmdList));
	_cmdsCalled.insert(std::make_pair(std::string("LIST"), 0));
	_cmds.insert(std::make_pair(std::string("KICK"), &Server::cmdKick));
	_cmdsCalled.insert(std::make_pair(std::string("KICK"), 0));
}