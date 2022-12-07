#include "NumericReplies.hpp"

/* Replies */

void Rep::R001(SocketIo& io, const std::string& cNick)
{
	io << "001 " << cNick << " :Welcome to the Internet Relay Network " << cNick;
	io.Send();
}

void Rep::R002(SocketIo& io, const std::string& cNick)
{
	io << "002 " << cNick << " :Your host is 42ircserv, running version 1.0 ";
	io.Send();
}

void Rep::R003(SocketIo& io, const std::string& cNick, const std::string& date)
{
	io << "003 " << cNick << " :This server was created " << date;
	io.Send();
}

void Rep::R004(SocketIo& io, const std::string& cNick)
{
	/* User modes https://www.rfc-editor.org/rfc/rfc2812#section-3.1.5 */
	/* Channel modes https://www.rfc-editor.org/rfc/rfc2811#section-4 */
	io << "004 " << cNick << " 42ircserv 1.0 " << CHANNELMODE_CHARLIST << " " << USERMODE_CHARLIST;
	io.Send();
}

void Rep::R211(SocketIo& , const std::string& , const std::string& )
{

}

void Rep::R212(SocketIo& , const std::string& , const std::string& )
{

}
void Rep::R219(SocketIo& , const std::string& , const std::string& )
{

}
void Rep::R242(SocketIo& , const std::string& , const std::string& )
{

}
void Rep::R243(SocketIo& , const std::string& , const std::string& )
{

}

void Rep::R221(SocketIo& io, const std::string& cNick, int modes)
{
	io << "221 " << cNick << " " << modes;
	io.Send();
}

void Rep::R331(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "331 " << cNick << " " << chanName << " :No topic set";
	io.Send();
}

void Rep::R332(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& topic)
{
	io << "332 " << cNick << " " << chanName << " :" << topic;
	io.Send();
}

void Rep::R333(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& setterNick, time_t timestamp)
{
	io << "333 " << cNick << " " << chanName << " " << setterNick << " " << timestamp;
	io.Send();
}

void Rep::R353(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& nick, char chanPrefix, char userPrefix)
{
	if (userPrefix == 'u')
		io << "353 " << cNick << " " << chanPrefix << " " << chanName << " :" << nick;
	else
		io << "353 " << cNick << " " << chanPrefix << " " << chanName << " :" << userPrefix << nick;
	io.Send();
}

void Rep::R366(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "366 " << cNick << " " << chanName << " :End of NAMES list";
	io.Send();
}

void Rep::R391(NR_ARG)
{
	io << "391 " << cNick << " 42ircserv " << time(0) << " :hello I'm not a date";
	io.Send();
}

/* Errors */
void Rep::E401(SocketIo& io, const std::string& cNick, const std::string& inputNick)
{
	io << "401 " << cNick << " " << inputNick << " :No such nick/channel";
	io.Send();
}

void Rep::E403(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "403 " << cNick << " " << chanName << " :No such channel";
	io.Send();
}

void Rep::E404(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "404 " << cNick << " " << chanName << " :Cannot send to channel";
	io.Send();
}

void Rep::E421(SocketIo& io, const std::string& cNick, const std::string& cmd)
{
	io << "421 " << cNick << " " << cmd << " :Unknown command";
	io.Send();
}

void Rep::E431(SocketIo& io, const std::string& cNick)
{
	io << "431 " << cNick << " :No nickname given";
	io.Send();
}

void Rep::E432(SocketIo& io, const std::string& cNick, const std::string& badNick)
{
	io << "432 " << cNick << " " << badNick << " :Erronous nickname";
	io.Send();
}

void Rep::E433(SocketIo& io, const std::string& cNick, const std::string& badNick)
{
	io << "433 " << cNick << " " << badNick << " :Nickname is already in use";
	io.Send();
}

void Rep::E441(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& inputNick)
{
	io << "441 " << cNick << " " << inputNick << " " << chanName << " :They aren't on that channel";
	io.Send();
}

void Rep::E442(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "442 " << cNick << " " << chanName << " :You're not on that channel";
	io.Send();
}

void Rep::E443(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& inputNick)
{
	io << "443 " << cNick << " " << inputNick << " " << chanName << " :Is already on channel";
	io.Send();
}

void Rep::E451(SocketIo& io, const std::string& cNick)
{
	io << "451 " << cNick << " :You have not registered";
	io.Send();
}

void Rep::E461(SocketIo& io, const std::string& cNick, const std::string& cmd)
{
	io << "461 " << cNick << " " << cmd << " :Not enough parameters";
	io.Send();
}

void Rep::E462(SocketIo& io, const std::string& cNick)
{
	io << "462 " << cNick << " :Unauthorized command (already registered)";
	io.Send();
}

void Rep::E464(SocketIo& io, const std::string& cNick)
{
	io << "464 " << cNick << " :Password incorrect";
	io.Send();
}

void Rep::E465(SocketIo& io, const std::string& cNick)
{
	io << "465 " << cNick << " :You are banned from this server";
	io.Send();
}

void Rep::E471(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "471 " << cNick << " " << chanName << ":Cannot join channel (+l)";
	io.Send();
}

void Rep::E472(SocketIo& io, const std::string& cNick, const std::string& modeChar)
{
	io << "472 " << cNick << " " << modeChar << " :is unknown mode char to me";
	io.Send();
}

void Rep::E473(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "473 " << cNick << " " << chanName << ":Cannot join channel (+i)";
	io.Send();
}

void Rep::E474(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "474 " << cNick << " " << chanName << ":Cannot join channel (+b)";
	io.Send();
}

void Rep::E475(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "475 " << cNick << " " << chanName << ":Cannot join channel (+k)";
	io.Send();
}

void Rep::E476(SocketIo& io, const std::string& cNick)
{
	io << "476 " << cNick << " :Bad Channel Mask";
	io.Send();
}

void Rep::E502(SocketIo& io, const std::string& cNick)
{
	io << "502 " << cNick << " :Can't change mode for other users";
	io.Send();
}