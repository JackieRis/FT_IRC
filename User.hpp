/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 22:59:27 by tnguyen-          #+#    #+#             */
/*   Updated: 2022/11/30 03:24:13 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <netinet/in.h>
# include <string>

class User
{
protected:

	// what the user is
	std::string	_Nick;
	std::string	_Name;
	std::string _Pass;
	int			_ModeFlags;
	bool		_Registered;
	
	// to be moved in a map<> insde Server
	in_addr_t	IpAddress;
	int			Tries;

public:
	User();
	User(in_addr_t ip);
	User(std::string Nick, std::string _Name);
	~User();

	const std::string& GetNick() const;
	const std::string& GetName() const;
	const std::string&	GetPass() const;
	bool GetRegistered() const;
	int GetUserFlags() const;

	void SetPass(const std::string& pass);
};

#endif
