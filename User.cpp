/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:02:44 by tnguyen-          #+#    #+#             */
/*   Updated: 2023/01/18 10:00:40 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User() {}

User::User(in_addr_t ip, int fd) : _fd(fd), IpAddress(ip) {_Nick = "*"; _Mode = 0;}

User::~User() {}

void	User::SetNick(const std::string &Nick) {_Nick = Nick;}
void	User::SetName(const std::string &Name) {_Name = Name;}
void	User::SetRealName(const std::string &RealName) {_RealName = RealName;}
void	User::SetPass(bool pass) {_Pass = pass;}
void	User::SetRegistered(bool Registered) {_Registered = Registered;}
void	User::SetMode(UserModeE mode, int state)
{
	if (!state)
		_Mode &= ~mode;
	else 
		_Mode |= mode;
}

void	User::SetDidUser(bool b) {_DidUser = b;}
void	User::SetDidNick(bool b) {_DidNick = b;}
void	User::SetHasDisconnected() {_HasDisconnected = true;}
void	User::BecomeServerOper() {SetMode(UM_OPER, true);}

int		User::GetFd() const {return (_fd);}

const std::string& User::GetNick() const
{
	return (_Nick);
}

const std::string& User::GetName() const
{
	return (_Name);
}

const std::string& User::GetRealName() const
{
	return (_RealName);
}

bool User::GetPass() const
{
	return (_Pass);
}

bool User::GetRegistered() const
{
	return (_Registered);
}

int User::GetMode() const
{
	return (_Mode);
}

bool User::GetDidUser() const
{
	return (_DidUser);
}

bool User::GetDidNick() const
{
	return (_DidNick);
}

bool	User::HasDisconnected() const
{
	return (_HasDisconnected);
}

bool	User::IsServerOpper() const
{
	return (_Mode & UM_OPER);
}