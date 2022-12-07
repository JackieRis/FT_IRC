#ifndef NUMERICREPLIES_HPP
# define NUMERICREPLIES_HPP

# include "SocketIo.hpp"

/*
	Numeric Replies always take the same two first argument
	To make our lives easier, both argument are defined as a single macro
	This wouldn't be generally recommended, but this is kept consitent throughout the whole program
	NR_ARG isn't used in NumericReplies.cpp to keep the argument names explicit where it is relevant
*/
# define NR_ARG SocketIo& io, const std::string& cNick
# define NR_IN *io, user->GetNick()

class Rep
{
public:
	/* Replies */
	static void R001(NR_ARG);
	static void R002(NR_ARG); /* hardcoding name and version */
	static void R003(NR_ARG, const std::string& date);
	static void R004(NR_ARG); /* hardcoding every available modes */

	static void R221(NR_ARG, int modes);

	static void R331(NR_ARG, const std::string& chanName);
	static void R332(NR_ARG, const std::string& chanName, const std::string& topic);
	static void R333(NR_ARG, const std::string& chanName, const std::string& setterNick, time_t timestamp);

	static void R353(NR_ARG, const std::string& chanName, const std::string& nick);
	static void R366(NR_ARG, const std::string& chanName);

	static void R391(NR_ARG);

	/* Errors */
	static void E401(NR_ARG, const std::string& inputNick);
	static void E403(NR_ARG, const std::string& chanName);
	static void E404(NR_ARG, const std::string& chanName);

	static void E421(NR_ARG, const std::string& cmd);

	static void E431(NR_ARG);
	static void E432(NR_ARG, const std::string& badNick);
	static void E433(NR_ARG, const std::string& badNick);

	static void E441(NR_ARG, const std::string& chanName, const std::string& inputNick);
	static void E442(NR_ARG, const std::string& chanName);
	static void E443(NR_ARG, const std::string& chanName, const std::string& inputNick);

	static void E451(NR_ARG);
	static void E461(NR_ARG, const std::string& cmd);
	static void E462(NR_ARG);
	static void E464(NR_ARG);
	static void E465(NR_ARG);

	/* 497 - 478 channel access restrictions */

	static void E502(NR_ARG);
};

#endif