/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/30 18:53:13 by aberneli          #+#    #+#             */
/*   Updated: 2022/11/30 19:04:35 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <vector>
# include <string>

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
};

# endif