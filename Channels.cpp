/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:59:51 by aberneli          #+#    #+#             */
/*   Updated: 2023/01/06 03:01:11 by aberneli         ###   ########.fr       */
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
	userflags.insert(std::make_pair(firstUser, (ChanUserFlags){true, false}));

	channelCreationDate = time(0);

	lastTopicEditor = firstUser->GetNick();
	lastTopicChangeDate = channelCreationDate;

	modes = CM_NONE;
	limit = 1024;

	std::cerr << "Channel " << chanName << " created" << std::endl;
}

Channels::~Channels()
{
	// No internal allocations are done within the class

	std::cerr << "Channel " << channelName << " destroyed" << std::endl;
}

/*
	Setters
*/

void	Channels::RemoveUser(User *user)
{
	if (users.find(user) == users.end())
		return ;
	
	userflags.erase(user);
	users.erase(user);
}

void	Channels::AddUser(User *user)
{
	users.insert(user);
	userflags.insert(std::make_pair(user, (ChanUserFlags){false, false}));
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
void	Channels::Ban(const std::string& banName) {banned.insert(banName);}
void	Channels::Unban(const std::string& banName) {banned.erase(banName);}
void	Channels::ChangeUserOp(User *user, bool state) {userflags[user].op = state;}
void	Channels::ChangeUserVoice(User *user, bool state) {userflags[user].voice = state;}

/*
	Getters
*/

const std::set<User *>&		Channels::GetUsers() const {return(users);}
const std::string&			Channels::GetName() const {return (channelName);}

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

int		Channels::GetVisibleUsers(User *user) const
{
	if (HasUser(user))
		return (users.size());
		
	int visible = 0;
	std::set<User *>::const_iterator it = users.begin();
	for (; it != users.end(); ++it)
	{
		visible += ((*it)->GetMode() & UM_INVISIBLE);
	}
	return (visible);
}

bool	Channels::HasUser(User *user) const
{
	return (users.find(user) != users.end());
}

bool	Channels::HasUser(const std::string& user) const
{
	std::set<User *>::iterator it = users.begin();

	for (; it != users.end(); ++it)
	{
		if ((*it)->GetNick() == user)
			return (true);
	}
	return (false);
}

bool	Channels::IsBanned(User *user) const
{
	return (banned.count(user->GetNick()) || banned.count(user->GetName()));
}

bool	Channels::IsOpped(User *user) const
{
	std::map<User *, ChanUserFlags>::const_iterator it = userflags.find(user);
	return (it->second.op);
}

bool	Channels::IsVoice(User *user) const
{
	std::map<User *, ChanUserFlags>::const_iterator it = userflags.find(user);
	return (it->second.voice);
}

bool	Channels::ValidateKey(const std::string& userKey) const
{
	return (userKey == key);
}

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
	if (modes & CM_PRIVATE)
		return ('*');
	return ('=');
}

char	Channels::GetUserPrefix(User *user) const
{
	std::map<User *, ChanUserFlags>::const_iterator it = userflags.find(user);
	
	if (it->second.op)
		return ('@');
	if (it->second.voice)
		return ('+');
	return ('u');
}

int Channels::GetLimit() const
{
	return (limit);
}

int Channels::GetSize() const
{
	return (users.size());
}