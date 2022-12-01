/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 10:47:18 by tnguyen-          #+#    #+#             */
/*   Updated: 2022/11/30 19:13:42 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BUFFER_SIZE 4096
# define MAX_CLIENT 1024

# include "User.hpp"
# include "Channels.hpp"
# include "SocketIo.hpp"
# include "Utils.hpp"
# include <string>
# include <cstdlib>
# include <sys/socket.h> // For socket functions
# include <sys/select.h> // for select functions
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/in.h> // For sockaddr_in
# include <iostream> // For cout
# include <unistd.h> // For read
# include <map>


class Server
{
protected:

	//std::map<std::string, Channel> _channels;
	/*
		auto c = _channels.find("#fromage");
		
		// (list des utilisateurs, )
		SendToAll(c.GetUserList(), sender);

		{
			for (iterateur)
			{
				if (it != sender)
				{
					_user
				}
			}
		}
	*/

	int									_client_fd[MAX_CLIENT];
	std::map<int, User *>				_user;
	std::map<int, SocketIo *>			_io;
	std::map<std::string, Channels *>	_channel;
	int									_port;
	std::string							_password;

	void acceptClient();
	void manageClient(int fd);
	void disconnectClient(int fd);
	/* Add security function */
	int	security(int connection);

	void command(const std::string& str, int fd);

	/*
		Command handlers
		void f(const std::vector<std::string>& input, int fd);
	*/

	typedef void (Server::*cmdHandler)(const std::vector<std::string>&, int);

	void cmdCap(const std::vector<std::string>& input, int fd);
	void cmdUser(const std::vector<std::string>& input, int fd);
	void cmdNick(const std::vector<std::string>& input, int fd);
	void cmdPass(const std::vector<std::string>& input, int fd);
	void cmdPing(const std::vector<std::string>& input, int fd);
	void cmdPong(const std::vector<std::string>& input, int fd);
	void cmdJoin(const std::vector<std::string>& input, int fd);
	void cmdPrivmsg(const std::vector<std::string>& input, int fd);
	void cmdMode(const std::vector<std::string>& input, int fd);
	// kick ban 
	
	std::map<std::string, cmdHandler>	_cmds;

	void initCmds();
	
public:
	Server();
	Server(int port, std::string password);
	~Server();
	
	int		init();
	void	run();
	void	shutdown();
};

#endif