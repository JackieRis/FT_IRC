/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SocketIo.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/09/08 13:34:44 by aberneli          #+#    #+#             */
/*   Updated: 2022/12/07 16:26:32 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SOCKETIO_HPP
# define SOCKETIO_HPP

# include <sstream>
# include <string>
# include <sys/socket.h>
# include <sys/types.h>
# include <stdexcept>

// helper class to communicate with clients

class SocketIo
{
		private:
	std::ostringstream	os;
	std::ostringstream	is;
	int					fd;
	bool				consumable;

		public:
	static uint64_t		sentkb;
	static uint64_t		recvkb;
	static uint64_t		sentMsg;
	static uint64_t		recvMsg;

	SocketIo(int newFd);
	~SocketIo();

	// Send the buffer built with operator<< and appends \r\n at the end to the client
	int			Send();

	// Read data from the socket buffer, returns true if a message is available for "consume"
	bool		Receive();
	
	// Get the latest message read from the socket if consumable
	std::string	Consume();

	// Tells if we have a message available
	bool		CanConsume() const;

	// Exception throw by Receive() if the remote socket was closed
	class EConnectionClosed : public std::exception
	{
			public:
		virtual const char *what() const throw();
	};

	// Exception thrown by Receive() when recv returns < 0
	class ERecvError : public std::exception
	{
			public:
		virtual const char *what() const throw();
	};
	
	// Appends data to the output buffer
	template <typename T>
	SocketIo& operator<<(const T &val)
	{
		os << val;
		return (*this);
	}
};

# endif