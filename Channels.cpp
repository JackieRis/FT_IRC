/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <aberneli@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:59:51 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/03 20:18:43 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Channels.hpp"

Channels::Channels()
{
	// Don't use
}

Channels::Channels(const std::string& chanName, User *firstUser) : channelName(chanName)
{
	users.insert(firstUser);
	opped.push_back(firstUser);
}

Channels::~Channels()
{
	// No internal allocations are done within the class
}

// Removes the user from the channel
// If the channel becomes empty, it has to be deleted externally
void	Channels::RemoveUser(User *user)
{
	if (users.find(user) != users.end())
		return ; // return false;
	
	std::vector<User *>::iterator it;
	
	// remove the user from opped list if present
	it = std::find(opped.begin(), opped.end(), user);
	if (it != opped.end())
		opped.erase(it);
	
	users.erase(user);
	// Don't forget to delete the channel from Server if channel.GetSize() == 0
}

void	Channels::AddUser(User *user)
{
	users.insert(user);
}

void	Channels::SetTopic(const std::string& t) {topic = t;}

const std::set<User *>	&Channels::GetUsers() const {return(users);}
const std::string&	Channels::GetName() const {return (channelName);}

const std::vector<std::string> Channels::GetUserNickList() const
{
	std::vector<std::string> lst;

	lst.resize(users.size());
	for (std::set<User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		lst.push_back((*it)->GetNick());
	}
	return (lst);
}

const std::vector<std::string> Channels::GetUserNameList() const
{
	std::vector<std::string> lst;

	lst.resize(users.size());
	for (std::set<User *>::iterator it = users.begin(); it != users.end(); ++it)
	{
		lst.push_back((*it)->GetName());
	}
	return (lst);
}

// const std::vector<int>	Channels::GetUserFd() const
// {
// 	std::vector<int>			ret;
// 	std::set<User *>::iterator	Sit = users.begin();
// 	for (; Sit != users.end(); ++Sit)
// 	{
// 		ret.push_back()
// 	}
	
// }

const std::string&	Channels::GetTopic() const
{
	return (topic);
}

int Channels::GetSize() const
{
	return (users.size());
}