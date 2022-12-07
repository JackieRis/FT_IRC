/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:49:09 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/07 12:21:33 by aberneli         ###   ########.fr       */
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
	std::vector<User *> opped;
	std::vector<User *> banned;

	public:

	Channels();
	Channels(const std::string& chanName, User *firstUser);
	~Channels();

	void	RemoveUser(User *user);
	void	AddUser(User *user);

	void	SetTopic(const std::string& t, const std::string& author);

	// Getters
	const std::set<User *>			&GetUsers() const;
	const std::string&				GetName() const;
	const std::vector<std::string>	GetUserNickList() const;
	const std::vector<std::string>	GetUserNameList() const;
	bool							HasUser(User *user) const;
	//const std::vector<int>			GetUserFd() const;
	const std::string&				GetTopic() const;
	const std::string&				GetLastTopicEditor() const;
	time_t							GetLastTopicChangeDate() const;
	int GetSize() const;
	// GetUserList() ?
};

#endif