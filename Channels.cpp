/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:59:51 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/12 12:38:21 by aberneli         ###   ########.fr       */
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
	opped.insert(firstUser);
	prefix.insert(std::make_pair(firstUser, '@'));

	channelCreationDate = time(0);

	lastTopicEditor = firstUser->GetNick();
	lastTopicChangeDate = channelCreationDate;

	std::cerr << "Channel " << chanName << " created" << std::endl;
}

Channels::~Channels()
{
	// No internal allocations are done within the class

	std::cerr << "Channel " << channelName << " destroyed" << std::endl;
}

// Removes the user from the channel
// If the channel becomes empty, it has to be deleted externally
void	Channels::RemoveUser(User *user)
{
	if (users.find(user) == users.end())
		return ;
	
	// remove the user from opped list if present
	if (opped.count(user))
		opped.erase(user);
	
	prefix.erase(user);
	users.erase(user);
}

void	Channels::AddUser(User *user)
{
	users.insert(user);
	prefix.insert(std::make_pair(user, 'u'));
}

void	Channels::SetTopic(const std::string& t, const std::string& author) {topic = t; lastTopicEditor = author; lastTopicChangeDate = time(0);}
void	Channels::SetKey(const std::string newKey) {key = newKey;}

void	Channels::SetMode(ChannelModeE mode, int state)
{
	if (!state)
		modes &= ~mode;
	else 
		modes |= mode;
}

void	Channels::SetLimit(int userLimit) {limit = userLimit;}

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

int		Channels::GetModes() const
{
	return (modes);
}

bool	Channels::HasUser(User *user) const
{
	return (users.find(user) != users.end());
}

bool	Channels::IsBanned(User *user) const
{
	return (banned.count(user));
}

bool	Channels::IsOpped(User *user) const
{
	return (opped.count(user));
}

bool	Channels::ValidateKey(const std::string& userKey) const
{
	return (userKey == key);
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

const std::string&	Channels::GetLastTopicEditor() const
{
	return (lastTopicEditor);
}

time_t 	Channels::GetLastTopicChangeDate() const
{
	return (lastTopicChangeDate);
}

char	Channels::GetChanPrefix() const
{
	if (modes & CM_SECRET)
		return ('@');
	return ('=');
}

char	Channels::GetUserPrefix(User *user) const
{
	return (prefix.find(user)->second);
}

int Channels::GetLimit() const
{
	return (limit);
}

int Channels::GetSize() const
{
	return (users.size());
}