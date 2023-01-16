/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aberneli <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/11/26 10:47:18 by tnguyen-          #+#    #+#             */
/*   Updated: 2023/01/06 00:40:15 by aberneli         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# define BUFFER_SIZE 4096
# define MAX_CLIENT 1000

# include <algorithm>
# include "Config.hpp"
# include "User.hpp"
# include "Channels.hpp"
# include "SocketIo.hpp"
# include "NumericReplies.hpp"
# include "Utils.hpp"
# include <string>
# include <cstdlib>
# include <sys/socket.h> // For socket functions
# include <sys/select.h> // for select functions
# include <sys/types.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <netinet/in.h> // For sockaddr_in
# include <iostream> // For cout
# include <unistd.h> // For read
# include <map>


class Server
{
protected:

	Config								_cfg;
	int									_client_fd[MAX_CLIENT];
	std::map<int, User *>				_user;
	std::map<int, SocketIo *>			_io;
	std::map<User *, SocketIo *>		_userToIoLookup;
	std::map<std::string, User *>		_nickToUserLookup;
	std::map<std::string, Channels *>	_channel;
	int									_port;
	std::string							_password;
	time_t								_startupTimestamp;
	std::string							_servName;

	/*
		Command handlers
		void f(const std::vector<std::string>& input, int fd);
	*/
	typedef void (Server::*cmdHandler)(const std::vector<std::string>&, int);
	std::map<std::string, cmdHandler>	_cmds;
	std::map<std::string, int>			_cmdsCalled;

	/** Server.cpp **/
	void	ChanMsg(User *sender, const std::string& msg, Channels* chan, std::string type);
	void	acceptClient();
	void	manageClient(int fd);
	void	disconnectClient(int fd);
	void	addChannel(const std::string& cName, User *creator);
	void	removeAllChannels(bool emptyOnly);
	void	removeChannel(std::map<std::string, Channels *>::iterator it);
	void	removeFromChannel(const std::string& cname, User *user);
	void	welcomeUser(int fd);
	/* Add security function */
	int		security(int connection);

	/** Commands.cpp **/
	void	command(const std::string& str, int fd);

	// Auth
	void	cmdCap(const std::vector<std::string>& input, int fd);
	void	cmdUser(const std::vector<std::string>& input, int fd);
	void	cmdNick(const std::vector<std::string>& input, int fd);
	void	cmdPass(const std::vector<std::string>& input, int fd);
	void	cmdQuit(const std::vector<std::string>& input, int fd);

	//  Misc
	void	cmdPing(const std::vector<std::string>& input, int fd);
	void	cmdPong(const std::vector<std::string>& input, int fd);
	void	cmdMode(const std::vector<std::string>& input, int fd);
	void	cmdTopic(const std::vector<std::string>& input, int fd);
	void	cmdTime(const std::vector<std::string>& input, int fd);
	void	cmdStats(const std::vector<std::string>& input, int fd);
	void	cmdNames(const std::vector<std::string>& input, int fd);
	void	cmdLusers(const std::vector<std::string>& input, int fd);
	void	cmdWho(const std::vector<std::string>& input, int fd);
	void	cmdWhois(const std::vector<std::string>& input, int fd);
	void	cmdMotd(const std::vector<std::string>& input, int fd);
	void	cmdList(const std::vector<std::string>& input, int fd);

	// Communication
	void	cmdJoin(const std::vector<std::string>& input, int fd);
	void	cmdPrivmsg(const std::vector<std::string>& input, int fd);
	void	cmdNotice(const std::vector<std::string>& input, int fd);
	void	cmdPart(const std::vector<std::string>& input, int fd);
	void	cmdInvite(const std::vector<std::string>& input, int fd);
	void	cmdOper(const std::vector<std::string>& input, int fd);

	void	initCmds();

	/** CommandsExt.cpp **/
	void	SendToAllInChannel(Channels *chan, const std::string& msg);
	void	SendToAllJoinedChannel(User *user, const std::string& msg);
	void	QuitUserFromServer(User *user, const std::string& reason);
	void	PartUserFromAllChannel(User *user, const std::string& msg);
	void	PartUserFromChannel(User *user, Channels *chan, const std::string& msg);
	void	UserMode(const std::vector<std::string>& input, int fd);
	void	ChannelMode(const std::vector<std::string>& input, int fd);
	

public:
	Server();
	Server(int port, std::string password);
	~Server();
	
	int		init();
	void	run();
	void	shutdown();
};

#endif