/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:49:09 by aberneli          #+#    #+#             */
/*   Updated: 2023/01/16 10:55:48 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <string>
# include <vector>
# include <set>
# include "User.hpp"
# include <sys/types.h>
# include <iostream>
# include <map>

# include "Modes.hpp"

struct ChanUserFlags
{
	bool	op;
	bool	voice;
};

class Channels
{
	protected:

	std::string channelName;
	std::string topic;
	std::string lastTopicEditor;
	time_t		lastTopicChangeDate;
	time_t		channelCreationDate;

	std::set<User *> users;
	std::set<std::string> banned;
	std::map<User *, ChanUserFlags> userflags;

	int					modes;
	int					limit;
	std::string			key;

	public:

	Channels();
	Channels(const std::string& chanName, User *firstUser);
	~Channels();

	void	RemoveUser(User *user);
	void	AddUser(User *user);

	void	SetTopic(const std::string& t, const std::string& author);
	void	SetMode(ChannelModeE mode, int state);
	void	SetKey(const std::string newKey);
	void	SetLimit(int userLimit);
	void	Ban(const std::string& banName);
	void	Unban(const std::string& banName);
	void	ChangeUserOp(User *user, bool state);
	void	ChangeUserVoice(User *user, bool state);

	// Getters
	const std::set<User *>&			GetUsers() const;
	const std::string&				GetName() const;
	const std::vector<std::string>	GetUserNickList() const;
	const std::vector<std::string>	GetUserNameList() const;
	const std::vector<std::string>	GetUserBanList() const;
	bool							HasBans() const;
	int								GetModes() const;
	int								GetVisibleUsers(User *user) const;
	bool							HasUser(User *user) const;
	bool							HasUser(const std::string& user) const;
	bool							IsBanned(User *user) const;
	bool							IsOpped(User *user) const;
	bool							IsVoice(User *user) const;
	bool							ValidateKey(const std::string& userKey) const;
	const std::string&				GetTopic() const;
	const std::string&				GetLastTopicEditor() const;
	time_t							GetLastTopicChangeDate() const;
	char							GetChanPrefix() const;
	char							GetUserPrefix(User *user) const;
	int								GetLimit() const;
	int								GetSize() const;
};

#endif