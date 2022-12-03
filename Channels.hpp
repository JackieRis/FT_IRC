/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <aberneli@student.42lyon.fr>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:49:09 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/03 20:14:50 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CHANNELS_HPP
# define CHANNELS_HPP

# include <string>
# include <map>
# include <vector>
# include <set>
# include "User.hpp"

class Channels
{
	protected:
	std::string channelName;
	std::set<User *> users;
	std::vector<User *> opped;
	std::vector<User *> banned;

	public:

	Channels();
	Channels(const std::string& chanName, User *firstUser);
	~Channels();

	void	RemoveUser(User *user);

	// Getters
	const std::string&				GetName() const;
	const std::vector<std::string>	GetUserNickList() const;
	const std::vector<std::string>	GetUserNameList() const;
	const std::vector<int>			GetUserFd() const;
	int GetSize() const;
	// GetUserList() ?
};

#endif