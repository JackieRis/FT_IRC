#include "NumericReplies.hpp"

# define MAXCLIENTS 1000

/* Replies */

void Rep::R001(SocketIo& io, const std::string& cNick)
{
	io << "001 " << cNick << " :Welcome to the Internet Relay Network " << cNick;
	io.Send();
}

void Rep::R002(SocketIo& io, const std::string& cNick, const std::string& servName, const std::string &servVersion)
{
	io << "002 " << cNick << " :Your host is " << servName << ", running version " << servVersion;
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

void Rep::R211(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "211 " << cNick << " " << infostr;
	io.Send();
}

void Rep::R212(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "212 " << cNick << " " << infostr << " 0 0";
	io.Send();
}
void Rep::R219(SocketIo& io, const std::string& cNick, const std::string& letters)
{
	io << "219 " << cNick << " " << letters << " :End of STATS report";
	io.Send();
}
void Rep::R242(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "242 " << cNick << " :" << infostr;
	io.Send();
}
void Rep::R243(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "243 " << cNick << " " << infostr;
	io.Send();
}

void Rep::R221(SocketIo& io, const std::string& cNick, int modes)
{
	io << "221 " << cNick << " " << modes;
	io.Send();
}

void Rep::R251(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "251 " << cNick << " " << infostr;
	io.Send();
}

void Rep::R252(SocketIo& io, const std::string& cNick, int ops)
{
	io << "252 " << cNick << " " << ops << " :operator(s) online";
	io.Send();
}

void Rep::R253(SocketIo& io, const std::string& cNick, int unknownConnnections)
{
	io << "253 " << cNick << " " << unknownConnnections << " :unknown connection(s)";
	io.Send();
}

void Rep::R254(SocketIo& io, const std::string& cNick, int channels)
{
	io << "254 " << cNick << " " << channels << " :channel(s) currently open";
	io.Send();
}

void Rep::R255(SocketIo& io, const std::string& cNick, const std::string& infostr)
{
	io << "255 " << cNick << " " << infostr;
	io.Send();
}

void Rep::R265(SocketIo& io, const std::string& cNick, int users)
{
	io << "265 " << cNick << " " << users << " " << MAXCLIENTS <<" :Current local users " << users << ", max " << MAXCLIENTS;
	io.Send();
}

void Rep::R266(SocketIo& io, const std::string& cNick, int users)
{
	io << "266 " << cNick << " " << users << " " << MAXCLIENTS <<" :Current global users " << users << ", max " << MAXCLIENTS;
	io.Send();
}

void Rep::R324(SocketIo& io, const std::string& cNick, const std::string& chanName, const std::string& chanModeStr, const std::string& chanModeArgs)
{
	io << "324 " << cNick << " " << chanName << " " << chanModeStr << " " << chanModeArgs;
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

void Rep::R341(SocketIo& io, const std::string& cNick, const std::string& otherNick, const std::string& chanName)
{
	io << "341 " << cNick << " " << otherNick << " " << chanName;
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

void	Rep::R381(SocketIo& io, const std::string& cNick)
{
	io << "381 " << cNick << " " << " :You are now an IRC operator";
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

void Rep::E411(SocketIo& io, const std::string& cNick, const std::string& cmd)
{
	io << "411 " << cNick << " :No recipient given (" << cmd << ")";
	io.Send();
}

void Rep::E412(SocketIo& io, const std::string& cNick)
{
	io << "412 " << cNick << " :No text to send";
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
	io << "471 " << cNick << " " << chanName << " :Cannot join channel (+l)";
	io.Send();
}

void Rep::E472(SocketIo& io, const std::string& cNick, const std::string& modeChar)
{
	io << "472 " << cNick << " " << modeChar << " :is unknown mode char to me";
	io.Send();
}

void Rep::E473(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "473 " << cNick << " " << chanName << " :Cannot join channel (+i)";
	io.Send();
}

void Rep::E474(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "474 " << cNick << " " << chanName << " :Cannot join channel (+b)";
	io.Send();
}

void Rep::E475(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "475 " << cNick << " " << chanName << " :Cannot join channel (+k)";
	io.Send();
}

void Rep::E476(SocketIo& io, const std::string& cNick)
{
	io << "476 " << cNick << " :Bad Channel Mask";
	io.Send();
}

void Rep::E482(SocketIo& io, const std::string& cNick, const std::string& chanName)
{
	io << "482 " << cNick << " " << chanName << " :You're not channel operator";
	io.Send();
}

void Rep::E502(SocketIo& io, const std::string& cNick)
{
	io << "502 " << cNick << " :Can't change mode for other users";
	io.Send();
}