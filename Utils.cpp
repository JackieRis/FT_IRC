/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 18:53:43 by aberneli          #+#    #+#             */
/*   Updated: 2023/01/06 00:15:43 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

// void	Utils::ChanMsg(Channels *chan, std::string msg, std::map<int,SocketIo *> io, User& Sender)
// {
// 	std::set<User *>	tmp = chan->GetUsers();
// 	std::set<User *>::iterator	it = tmp.begin();

// 	for (; it != tmp.end(); ++it)
// 	{
// 		(*io[(*it)->GetFd()]) << ":" << /* userNICK */ << " PRIVMSG " << chan->GetName() <<  " " << msg;
// 		(*io[(*it)->GetFd()]).Send();
// 	}
// }

std::vector<std::string>	Utils::ToParamList(const std::string& input)
{
	std::vector<std::string>	res;
	std::string tmp = "";
	std::string::const_iterator it = input.begin();
	bool textFlag = false;

	/* if the input starts with a space, discard it right away */
	if (*it == ' ')
		return (res);
	for (; it != input.end(); ++it)
	{
		if (*it == ' ' && !textFlag)
		{
			res.push_back(tmp);
			tmp = "";
			continue ;
		}
		if (*it == ':')
		{
			if (!tmp.empty()) /* Protocol tells us that : shall be preceeded by SPACE, guess I should throw an exception instead*/
			{
				res.push_back(tmp);
				tmp = "";
			}
			textFlag = true;
		}
		tmp += *it;
	}
	if (!tmp.empty())
		res.push_back(tmp);
	return (res);
}

std::vector<std::string>	Utils::ToList(const std::string& input)
{
	std::vector<std::string>	res;
	std::string tmp = "";
	std::string::const_iterator it = input.begin();

	for (; it != input.end(); ++it)
	{
		if (*it == ',')
		{
			res.push_back(tmp);
			tmp = "";
			continue ;
		}
		tmp += *it;
	}
	if (!tmp.empty())
		res.push_back(tmp);
	return (res);
}

bool	Utils::IsValidNick(const std::string& input)
{
	const std::string badChar(" ,*?!@#%:$.");

	for (std::string::const_iterator it = badChar.begin(); it != badChar.end(); ++it)
	{
		if (input.find(*it) != std::string::npos)
			return (false);
	}
	return (true);
}

bool	Utils::IsChannel(const std::string& target)
{
	return (target[0] == '#' || target[0] == '&');
}

bool Utils::IsValidChannelName(const std::string& target)
{
	if (!IsChannel(target) || target.size() < 2)
		return (false);
	return (target.find(0x07) == std::string::npos); /* Not found, returns true */
}

bool Utils::IsValidNumber(const std::string& target)
{
	if (target.length() >= 9) /* Let's not bother with numbers too big */
		return (false);
	for (std::string::const_iterator it = target.begin(); it != target.end(); ++it)
	{
		if (!(*it >= '0' && *it <= '9'))
			return (false);
	}
	return (true);
}

std::string Utils::GenerateModestring(int modes, bool forUser)
{
	std::string str = "+";

	if (forUser)
	{
		if (modes & UM_INVISIBLE) str += "i";
		if (modes & UM_NOTICE) str += "s";
		if (modes & UM_WALLOPS) str += "w";
		if (modes & UM_OPER) str += "o";
		return (str);
	}
	if (modes & CM_PRIVATE) str += "p";
	if (modes & CM_SECRET) str += "s";
	if (modes & CM_INVITEONLY) str += "i";
	if (modes & CM_PROTECTEDTOPIC) str += "t";
	if (modes & CM_NOEXTERNAL) str += "n";
	if (modes & CM_MODERATED) str += "m";
	if (modes & CM_LIMIT) str += "l";
	if (modes & CM_BAN) str += "b";
	if (modes & CM_VOICE) str += "v";
	if (modes & CM_KEY) str += "k";
	return (str);
}

std::string Utils::GenerateArgstring(Channels *chan)
{
	std::stringstream res;
	int mode = chan->GetModes();

	if (mode & CM_LIMIT)
	{
		res << chan->GetLimit();
	}
	return (res.str());
}

bool Utils::ValidModeParam(const std::string& str, bool forUser)
{
	if (str.size() != 2)
		return (false);
	if (str[0] != '+' && str[0] != '-')
		return (false);
	if (forUser)
		return (str.find_first_of(USERMODE_CHARLIST, 0) != std::string::npos);
	return (str.find_first_of(CHANNELMODE_CHARLIST, 0) != std::string::npos);
}

ChannelModeE Utils::ChanModeParamToFlag(char c)
{
	switch (c)
	{
		case 'o': return (CM_CHANGEOPPER); break;
		case 'p': return (CM_PRIVATE); break;
		case 's': return (CM_SECRET); break;
		case 'i': return (CM_INVITEONLY); break;
		case 't': return (CM_PROTECTEDTOPIC); break;
		case 'n': return (CM_NOEXTERNAL); break;
		case 'm': return (CM_MODERATED); break;
		case 'v': return (CM_VOICE); break;
		case 'l': return (CM_LIMIT); break;
		case 'b': return (CM_BAN); break;
		case 'k': return (CM_KEY); break;
	}
	return (CM_NONE);
}