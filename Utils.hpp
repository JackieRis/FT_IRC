/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 18:53:13 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/07 10:53:47 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>
# include "SocketIo.hpp"
# include "Channels.hpp"

class Utils
{
public:

	/*
		Converts a client input into a vector of parameters
		Whenever a ':' character is encountered, the rest of the string is copied into the last argument
		This is required by the irc protocol to distinguish plain text
		
		If vector.size() results in 0, an invalid input was sent.
		In this case, discard the input and send an error to the client
	*/
	static std::vector<std::string>	ToParamList(const std::string& input);

	/*
		Converts a comma separated list of names from a string to a vector
	*/
	static std::vector<std::string>	ToList(const std::string& input);

	static int	checkNick(std::string input);

	/*
		Checks the first character to identify if target is a channel
	*/
	static bool	IsChannel(const std::string& target);
};

# endif