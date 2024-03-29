/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 22:59:27 by tnguyen-          #+#    #+#             */
/*   Updated: 2023/01/18 10:00:19 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef USER_HPP
# define USER_HPP

# include <netinet/in.h>
# include <string>

# include "Modes.hpp"

class User
{
protected:

	// what the user is
	std::string	_Nick;
	std::string	_RealName;
	std::string	_Name;
	bool		_Pass;
	int			_Mode;
	int			_fd;
	bool		_Registered;
	bool		_DidUser;
	bool		_DidNick;
	bool		_HasDisconnected;
	
	// to be moved in a map<> insde Server
	in_addr_t	IpAddress;
	int			Tries;

public:
	User();
	User(in_addr_t ip, int fd);
	~User();

	const std::string& GetNick() const;
	const std::string& GetName() const;
	const std::string& GetRealName() const;
	int		GetFd() const;
	bool	GetPass() const;
	bool	GetRegistered() const;
	int		GetMode() const;
	bool	GetDidUser() const;
	bool	GetDidNick() const;
	bool	HasDisconnected() const;
	bool	IsServerOpper() const;

	void	SetNick(const std::string &Nick);
	void	SetName(const std::string &Name);
	void	SetRealName(const std::string &RealName);
	void	SetPass(bool pass);
	void	SetRegistered(bool Registered);
	void	SetMode(UserModeE mode, int state);
	void	SetDidUser(bool b);
	void	SetDidNick(bool b);
	void	SetHasDisconnected();
	void	BecomeServerOper();
};

#endif
