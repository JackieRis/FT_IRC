#ifndef MODES_HPP
# define MODES_HPP

# define USERMODE_CHARLIST "iswo"
# define CHANNELMODE_CHARLIST "opsitnmlbvk"

enum UserModeE
{
	UM_NONE = 0x00,
	UM_INVISIBLE = 0x01,
	UM_NOTICE = 0x02,
	UM_WALLOPS = 0x04,
	UM_OPER = 0x08,
};

enum ChannelModeE
{
	CM_NONE = 0x0000,
	CM_CHANGEOPPER = 0x0001,
	CM_PRIVATE = 0x0002,
	CM_SECRET = 0x0004,
	CM_INVITEONLY = 0x0008,
	CM_PROTECTEDTOPIC = 0x0010,
	CM_NOEXTERNAL = 0x0020,
	CM_MODERATED = 0x0040,
	CM_LIMIT = 0x0080,
	CM_BAN = 0x0100,
	CM_VOICE = 0x0200,
	CM_KEY = 0x0400,
};



#endif