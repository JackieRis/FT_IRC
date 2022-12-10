/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 18:53:43 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/10 15:15:47 by aberneli         ###   ########.fr       */
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
	if (!IsChannel(target))
		return (false);
	return (target.find(0x07) == std::string::npos); /* Not found, returns true */
}