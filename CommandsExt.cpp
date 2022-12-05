#include "Server.hpp"

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

	for (; it != usrList.end(); ++it)
	{
		SocketIo	*tmpIo = _userToIoLookup[*it];

		(*tmpIo) << ":" << user->GetNick() << " PART " << chan->GetName() << " ";
		if (msg[0] != ':') /* Make sure the input has a : */
			(*tmpIo) << ':'; /* Might not even be necessary but oh well */
		(*tmpIo) << msg;
		tmpIo->Send();
	}

	chan->RemoveUser(user);
}
