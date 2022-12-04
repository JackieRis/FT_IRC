#ifndef NUMERICREPLIES_HPP
# define NUMERICREPLIES_HPP

# include "SocketIo.hpp"

class Rep
{
public:
	/* Replies */
	static void R001(SocketIo& io, const std::string& cNick);
	static void R002(SocketIo& io, const std::string& cNick); /* hardcoding name and version */
	static void R003(SocketIo& io, const std::string& cNick, const std::string& date);
	static void R004(SocketIo& io, const std::string& cNick); /* hardcoding every available modes */

	static void R332(SocketIo& io, const std::string& chanName, const std::string& cNick, const std::string& topic);

	static void R353(SocketIo& io, const std::string& chanName, const std::string& cNick, const std::string& nick);
	static void R366(SocketIo& io, const std::string& chanName, const std::string& cNick);


	/* Errors */
	static void E401(SocketIo& io, const std::string& cNick, const std::string& inputNick);
	static void E403(SocketIo& io, const std::string& cNick, const std::string& chanName);
	static void E404(SocketIo& io, const std::string& cNick, const std::string& chanName);

	static void E421(SocketIo& io, const std::string& cNick, const std::string& cmd);

	static void E431(SocketIo& io, const std::string& cNick);
	static void E432(SocketIo& io, const std::string& cNick, const std::string& badNick);
	static void E433(SocketIo& io, const std::string& cNick, const std::string& badNick);
	//442 is missing, need for PART
	static void E451(SocketIo& io, const std::string& cNick);
	static void E461(SocketIo& io, const std::string& cNick, const std::string& cmd);
	static void E462(SocketIo& io, const std::string& cNick);
	static void E464(SocketIo& io, const std::string& cNick);
	static void E465(SocketIo& io, const std::string& cNick);

	/* 497 - 478 channel access restrictions */
};

#endif