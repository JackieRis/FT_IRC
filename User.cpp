/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/27 23:02:44 by tnguyen-          #+#    #+#             */
/*   Updated: 2022/11/30 03:24:20 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "User.hpp"

User::User() {}

User::User(in_addr_t ip) : IpAddress(ip) {}

User::~User() {}

void	User::SetNick(const std::string &Nick) {_Nick = Nick;}
void	User::SetName(const std::string &Name) {_Name = Name;}
void	User::SetRealName(const std::string &RealName) {_RealName = RealName;}
void	User::SetPass(bool pass) {_Pass = pass;}
void	User::SetRegistered(bool Registered) {_Registered = Registered;}
void	User::SetMode(int mode) {_Mode = mode;}

const std::string& User::GetNick() const
{
	return (_Nick);
}

const std::string& User::GetName() const
{
	return (_Name);
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