/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channels.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:49:09 by aberneli          #+#    #+#             */
/*   Updated: 2022/11/28 00:45:19 by aberneli         ###   ########.fr       */
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
	const std::vector<std::string> GetUserNickList() const;
	const std::vector<std::string> GetUserNameList() const;
	int GetSize() const;
	// GetUserList() ?
};

#endif