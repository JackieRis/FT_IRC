#include "Config.hpp"

static void AddToMap(std::map<std::string, std::string>* mapToEdit, const std::string& line)
{
	const char *ws = " \t\r\v\n\f";
	std::string key = line.substr(0, line.find("="));
	std::string val = line.substr(line.find("=") + 1);

	/* converts something like "  password  =     aled  " into the equivalent of "password=aled" */
	key.erase(key.find_last_not_of(ws) + 1);
	key.erase(0, key.find_first_not_of(ws));
	val.erase(val.find_last_not_of(ws) + 1);
	val.erase(0, val.find_first_not_of(ws));
	if (key.find_first_of(ws) != std::string::npos || (val.find_first_of(ws) != std::string::npos && key != "motd"))
		return ; /* input had whitespace, reject except for motd, yes this is quite hacky */
	if (mapToEdit->count(key) != 0)
		return ; /* first found in file takes priority */
	mapToEdit->insert(std::make_pair(key, val));
}

Config::Config()
{
	std::ifstream file;
	std::string line;
	std::map<std::string, std::string>* mapToEdit = NULL;

	file.open("Config.ini");
	if (!file.is_open())
		throw EMissingConfigFile();
	
	while (std::getline(file, line))
	{
		if (line.empty())
			continue ;
		if (line == "[servconfig]")
		{
			mapToEdit = &servConfig;
			continue ;
		}
		else if (line == "[opperaccounts]")
		{
			mapToEdit = &opperAccounts;
			continue ;
		}
		if (!mapToEdit)
			continue ;
		if (line.find("=") == std::string::npos)
			continue ;
		AddToMap(mapToEdit, line);
	}

	if (servConfig.count("name") == 0) /* Force a server name if missing in config */
		servConfig.insert(std::make_pair(std::string("name"), std::string("42ircserv")));

	file.close();
}

Config::~Config()
{

}

const char *Config::EMissingConfigFile::what() const throw()
{
	return ("\"Config.ini\" is missing or cannot be openned, server won't start");
}