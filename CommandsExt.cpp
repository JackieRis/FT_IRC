#include "Server.hpp"

void Server::QuitUserFromServer(User *user, const std::string& reason)
{
	Channels *chan;

	std::map<std::string, Channels *>::iterator it = _channel.begin();

	for (; it != _channel.end(); ++it)
	{
		chan = it->second;
		if (chan->HasUser(user))
		{
			const std::set<User *>& usrList = chan->GetUsers();
			std::set<User *>::const_iterator it = usrList.begin();
			std::stringstream ss;

			ss << ":" << user->GetNick() << " QUIT " << reason;

			for (; it != usrList.end(); ++it)
			{
				SocketIo	*tmpIo = _userToIoLookup[*it];

				(*tmpIo) << ss.str();
				tmpIo->Send();
			}

			chan->RemoveUser(user);
		}
	}
	removeAllChannels(true);
}

void Server::SendToAllInChannel(Channels *chan, const std::string& msg)
{
	const std::set<User *>& usrList = chan->GetUsers();
	std::set<User *>::const_iterator it = usrList.begin();

	for (; it != usrList.end(); ++it)
	{
		SocketIo	*tmpIo = _userToIoLookup[*it];

		(*tmpIo) << msg;
		tmpIo->Send();
	}
}

void Server::SendToAllJoinedChannel(User *user, const std::string& msg)
{
	std::map<std::string, Channels *>::iterator it = _channel.begin();

	for (; it != _channel.end(); ++it)
	{
		if (it->second->HasUser(user))
			SendToAllInChannel(it->second, msg);
	}
}

void Server::PartUserFromAllChannel(User *user, const std::string& msg)
{
	std::map<std::string, Channels *>::iterator it = _channel.begin();

	for (; it != _channel.end(); ++it)
	{
		if (it->second->HasUser(user))
			PartUserFromChannel(user, it->second, msg);
	}
}

void Server::PartUserFromChannel(User *user, Channels *chan, const std::string& msg)
{
	const std::set<User *>& usrList = chan->GetUsers();
	std::set<User *>::const_iterator it = usrList.begin();
	std::stringstream ss; // create once, copy everywhere

	ss << ":" << user->GetNick() << " PART " << chan->GetName() << " ";
	if (msg[0] != ':') /* Make sure the input has a : */
		ss << ':'; /* Might not even be necessary but oh well */
	ss << msg;
	for (; it != usrList.end(); ++it)
	{
		SocketIo	*tmpIo = _userToIoLookup[*it];

		(*tmpIo) << ss.str();
		tmpIo->Send();
	}

	chan->RemoveUser(user);
}

void Server::UserMode(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];

	if (input.size() < 3)
	{
		std::string modestring = Utils::GenerateModestring(user->GetMode(), true);
		Rep::R221(NR_IN, modestring);
		return ;
	}

	if (!Utils::ValidModeParam(input[2], true))
	{
		Rep::E501(NR_IN);
		return ;
	}

	UserModeE bit = UM_NONE;

	switch (input[2][1])
	{
		case 'i': bit = UM_INVISIBLE; break;
		case 's': bit = UM_NOTICE; break;
		case 'w': bit = UM_WALLOPS; break;
		case 'o': bit = UM_OPER; break;
	}

	if (bit == UM_OPER && (input[2][0] == '+'))
		return ; /* Cannot set op yourself through MODE, silently ignore */

	user->SetMode(bit, (input[2][0] == '+'));

	(*io) << ":" << user->GetNick() << " MODE " << user->GetNick() << " :" << input[2];
	io->Send();
	/* Only need to send to this user, channel is a different story tho */
}

void Server::ChannelMode(const std::vector<std::string>& input, int fd)
{
	User		*user = _user[fd];
	SocketIo	*io = _io[fd];
	Channels	*chan = _channel[input[1]]; /* Guaranteed by the caller to be valid */

	if (input.size() < 3)
	{
		/* MODE channel query */
		return ; /* DEBUG DON'T REPLY TO THE CLIENT YET AS CHANNEL QUERY ISN'T PROPERLY IMPLEMENTED YET */
		//Rep::R324(NR_IN, chan->GetModes(), "l", "10");
		return ;
	}
	(void)user; (void)io; (void)chan;
}