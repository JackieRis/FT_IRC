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

	if (input.size() < 3) /* MODE channel query */
	{
		std::string modestring = Utils::GenerateModestring(chan->GetModes(), false);
		std::string argstring = Utils::GenerateArgstring(chan);
		const std::vector<std::string> banList = chan->GetUserBanList();
		for (std::vector<std::string>::const_iterator bit = banList.begin(); bit != banList.end(); ++bit)
		{
			if (!(*bit).empty()) /* The channel bans query code may return empty strings, so skip them */
				Rep::R367(NR_IN, chan->GetName(), *bit);
		}
		Rep::R324(NR_IN, chan->GetName(), modestring, argstring);
		return ;
	}

	/* user is not on channel or isn't a channel op */
	if (!chan->HasUser(user))
	{
		Rep::E442(NR_IN, chan->GetName());
		return ;
	}
	if (!chan->IsOpped(user))
	{
		Rep::E442(NR_IN, chan->GetName());
		return ;
	}

	/* invalid input */
	if (!Utils::ValidModeParam(input[2], false))
	{
		Rep::E472(NR_IN, input[2]);
		return ;
	}

	std::string msg = ":" + user->GetNick() + " MODE " + chan->GetName() + " " + input[2];

	switch (input[2][1])
	{
		/* switch statement magic, all of these only flip a flag, so we just fall through any */
		case 'p':
		case 's':
		case 'i':
		case 't':
		case 'n':
		case 'm':
			chan->SetMode(Utils::ChanModeParamToFlag(input[2][1]), (input[2][0] == '+'));
		break;

		/* this one requires an arg no matter what */
		case 'o': 
			if (input.size() < 4) { Rep::E461(NR_IN, input[0]); return ;} /* missing input */
			if (!chan->HasUser(input[3])) { Rep::E401(NR_IN, input[3]); return ;} /* user not on channel */

			/* if the user is on the channel then we know they exist */
			chan->ChangeUserOp(_nickToUserLookup[input[3]], (input[2][0] == '+')); /* Set|Unset them as OP */

			msg += " " + input[3];
		break;

		case 'v':
			if (input.size() < 4) { Rep::E461(NR_IN, input[0]); return ;} /* missing input */
			if (_nickToUserLookup.find(input[3]) == _nickToUserLookup.end()) { Rep::E401(NR_IN, input[3]); return;} /* no such user */
			if (!chan->HasUser(input[3])) { Rep::E442(NR_IN, chan->GetName()); return;} /* user not on channel */

			chan->ChangeUserVoice(_nickToUserLookup.find(input[3])->second, (input[2][0] == '+'));
		break;

		/* these require an arg only in + mode */
		case 'l':
			if (input[2][0] == '+')
			{
				if (input.size() < 4) { Rep::E461(NR_IN, input[0]); return ;} /* missing input */
				if (!Utils::IsValidNumber(input[3])) { /* Rep::E6666666();*/ return ;} /* limit is not a number, silent fail? */
				chan->SetLimit(std::atoi(input[3].c_str()));
			}
			chan->SetMode(Utils::ChanModeParamToFlag(input[2][1]), (input[2][0] == '+'));
		break;

		case 'b':
			if (input.size() < 4)
			{
				const std::vector<std::string> banList = chan->GetUserBanList();
				for (std::vector<std::string>::const_iterator bit = banList.begin(); bit != banList.end(); ++bit)
				{
					if (!(*bit).empty()) /* The channel bans query code may return empty strings, so skip them */
						Rep::R367(NR_IN, chan->GetName(), *bit);
				}
				Rep::R368(NR_IN, chan->GetName());
				return ;
			}
			if (input[2][0] == '+')
				chan->Ban(input[3]);
			else
				chan->Unban(input[3]);
			chan->SetMode(Utils::ChanModeParamToFlag(input[2][1]), chan->HasBans());
			return ;
		break;

		case 'k':
			if (input[2][0] == '+')
			{
				if (input.size() < 4) { Rep::E461(NR_IN, input[0]); return ;} /* missing input */
				chan->SetKey(input[3]);
				//msg += " " + input[3]; /* Don't display the key in the MODE message */
			}
			chan->SetMode(Utils::ChanModeParamToFlag(input[2][1]), (input[2][0] == '+'));
		break;
	}
	/* Notify everyone in the channel */
	SendToAllInChannel(chan, msg);
}