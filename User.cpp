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

User::User(std::string Nick, std::string Name): _Nick(Nick), _Name(Name) {}

User::~User() {}

const std::string& User::GetNick() const
{
	return (_Nick);
}

const std::string& User::GetName() const
{
	return (_Name);
}

const std::string& User::GetPass() const
{
	return (_Pass);
}

bool User::GetRegistered() const
{
	return (_Registered);
}

int User::GetUserFlags() const
{
	return (_ModeFlags);
}

void User::SetPass(const std::string& pass)
{
	_Pass = pass;
}