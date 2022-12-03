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
	std::string	_RealName;
	std::string	_Name;
	bool		_Pass;
	int			_Mode;
	bool		_Registered;
	
	// to be moved in a map<> insde Server
	in_addr_t	IpAddress;
	int			Tries;

public:
	User();
	User(in_addr_t ip);
	~User();

	const std::string& GetNick() const;
	const std::string& GetName() const;
	bool	GetPass() const;
	bool	GetRegistered() const;
	int		GetMode() const;

	void	SetNick(const std::string &Nick);
	void	SetName(const std::string &Name);
	void	SetRealName(const std::string &RealName);
	void	SetPass(bool pass);
	void	SetRegistered(bool Registered);
	void	SetMode(int mode);
};

#endif
