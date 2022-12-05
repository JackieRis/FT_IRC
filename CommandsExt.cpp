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

			ss << ":" << user->GetNick() << " QUIT " << chan->GetName() << " " << reason;

			for (; it != usrList.end(); ++it)
			{
				SocketIo	*tmpIo = _userToIoLookup[*it];

				(*tmpIo) << ss.str();
				tmpIo->Send();
			}

			chan->RemoveUser(user);
		}
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
