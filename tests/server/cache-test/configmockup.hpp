#ifndef CONFIGMOCKUP_H
#define CONFIGMOCKUP_H

#include "general/configuration.hpp"

class ConfigMockup
{
public:
	std::shared_ptr<Configuration> Config(char* argv[], int argc);
	
	template<typename T>
	T get(const std::string& key)
	{
		return CacheSize;
	}
	
	boost::program_options::options_description				cmd_desc;
	boost::program_options::options_description				config_desc;
	boost::program_options::positional_options_description	pos_desc;
	int argc;
	char** argv;
	
private:
	int CacheSize;
};

#endif // CONFIGMOCKUP_H
