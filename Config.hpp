#ifndef CONFIG_HPP
# define CONFIG_HPP

# include <string>
# include <fstream>
# include <iostream>
# include <map>
# include <stdexcept>

class Config
{
public:
	std::map<std::string, std::string>	servConfig;
	std::map<std::string, std::string>	opperAccounts;

	Config();
	~Config();

	class EMissingConfigFile : public std::exception
	{
		virtual const char *what() const throw();
	};

};

#endif