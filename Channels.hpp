/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:49:09 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/07 14:40:13 by aberneli         ###   ########.fr       */
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

# include "Modes.hpp"

class Channels
{
	protected:
	std::string channelName;
	std::string topic;
	std::string lastTopicEditor;
	time_t		lastTopicChangeDate;

	std::set<User *> users;
	std::set<User *> opped;
	std::set<User *> banned;

	int					modes;
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

	// Getters
	const std::set<User *>			&GetUsers() const;
	const std::string&				GetName() const;
	const std::vector<std::string>	GetUserNickList() const;
	const std::vector<std::string>	GetUserNameList() const;
	int								GetModes() const;
	bool							HasUser(User *user) const;
	bool							IsBanned(User *user) const;
	bool							ValidateKey(const std::string& userKey) const;
	//const std::vector<int>			GetUserFd() const;
	const std::string&				GetTopic() const;
	const std::string&				GetLastTopicEditor() const;
	time_t							GetLastTopicChangeDate() const;
	int GetSize() const;
	// GetUserList() ?
};

#endif