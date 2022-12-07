#ifndef MODES_HPP
# define MODES_HPP

# define USERMODE_CHARLIST "ioOrw"
# define CHANNELMODE_CHARLIST "blikmstn"

enum UserModeE
{
	UM_INVISIBLE = 0x01,
	UM_OPER = 0x02,
	UM_LOPER = 0x04,
	//UM_REGISTERED = 0x08, // Don't need to support
	UM_WALLOPS = 0x10,
};

enum ChannelModeE
{
	CM_BAN = 0x0001,
	//CM_EXCEPT = 0x02, // don't support, not standard
	CM_LIMIT = 0x0004,
	CM_INVITEONLY = 0x0008,
	//CM_INVITEEXCEPT = 0x10 // don't either
	CM_KEY = 0x0020,
	CM_MODERATED = 0x0040,
	CM_SECRET = 0x0080,
	CM_PROTECTEDTOPIC = 0x0100,
	CM_NOEXTERNAL = 0x0200,
};

#endif