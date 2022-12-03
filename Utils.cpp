/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 18:53:43 by aberneli          #+#    #+#             */
/*   Updated: 2022/11/30 19:11:44 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Utils.hpp"

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

void	Utils::SendList(SocketIo *io, const std::vector<std::string>& lst)
{
	std::vector<std::string>::const_iterator it;

	for (it = lst.begin(); it != lst.end(); ++it)
	{
		(*io) << 
		io->Send();
	}
	(*io) << 
	io->Send();
}