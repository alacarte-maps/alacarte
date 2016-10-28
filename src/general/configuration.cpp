/**
 *  This file is part of alaCarte.
 *
 *  alaCarte is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  alaCarte is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with alaCarte. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright alaCarte 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter
 *  Maintainer: Tobias Kahlert
 */


#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "config.hpp"
#include "general/configuration.hpp"


/**
 * @brief Creates a new config and parses the command line and config file.
 *
 * Parses the command line arguments and stores them internally.
 * If a config file is specified, the file will be loaded and parsed.
 * If options are specified in both ways, the command line argument will be taken.
 * This function will search for the config file in different locations:
 *		- current working directory
 *		- install location
 *
 * @param cmd_desc		Option descriptions for the command line parser
 * @param config_desc	Option descriptions for the config file parser
 * @param pos_desc		Description for relative arguments in the command line
 * @param argc			Number of arguments
 * @param argv			arguments
 **/
Configuration::Configuration(	boost::program_options::options_description& cmd_desc,
								boost::program_options::options_description& config_desc,
								boost::program_options::positional_options_description& pos_desc,
								int argc, char** argv)
	: ConfigFileUsed(false)
{
	using boost::filesystem::path;
	using boost::filesystem::is_regular_file;
	using boost::program_options::store;
	using boost::program_options::notify;
	using boost::program_options::command_line_parser;
	using boost::program_options::parse_config_file;
	using boost::program_options::positional_options_description;

	store(command_line_parser(argc, argv).options(cmd_desc).positional(pos_desc).run(), options);
	
	std::queue<path> directories;
	// Iterate over different directories ans search for config file
	// Adding a Directory? So don't forget to update the help of -c command of the server and importer
	directories.push(""); //root
	directories.push(path(argv[0]).parent_path()); // executable path
	directories.push(SYSCONFDIR);

	assert(options.count(opt::config));
	path configFile = options[opt::config].as<string>();

	while(directories.size())
	{
		searchDirectories.push_back(directories.front().string());
		path configPath = directories.front() / configFile;

		if(is_regular_file(configPath))
		{
			ConfigFileUsed = true;
			store(parse_config_file<char>(configPath.string().c_str(), config_desc, true), options);
			break;
		}

		directories.pop();
	}

	notify(options);
}

/**
 * @brief Tests if a key was specified in the command line or in the config file
 *
 * @param key the key to test
 * @return weather the key was present or not
 **/
bool Configuration::has(const string& key)
{
	return options.count(key) != 0;
}


bool Configuration::usedConfigFile() const
{
	return ConfigFileUsed;
}

const std::vector<string> & Configuration::getSeachDirectories() const
{
	return searchDirectories;
}
/**
 * @brief Prints the given configurations to the log.
 *
 **/
void Configuration::printConfigToLog()
{
	for(auto it = options.begin();
		it != options.end();
		++it)
	{
		boost::any& v = it->second.value();


		if(v.type() == typeid(int))
		{
			LOG_SEV(app_log, info) << it->first << ": " << it->second.as<int>();
		}else if (v.type() == typeid(std::string))
		{
			if (it->first == opt::config && ConfigFileUsed && !searchDirectories.back().empty()) {
				LOG_SEV(app_log, info) << it->first << ": " << searchDirectories.back() << "/" << it->second.as<std::string>();
			} else {
				LOG_SEV(app_log, info) << it->first << ": " << it->second.as<std::string>();
			}
		}else if (v.type() == typeid(std::vector<string>))
		{
			std::stringstream ss;
			for(string name : it->second.as<std::vector<string>>()) {
				ss << name << ", ";
			}
			LOG_SEV(app_log, info) << it->first << ": " << ss.str();
		}else{
			// We only take care of int and string, so if this message is printed on the log
			// feel free to add other types.
			// boost::any does not support implicit casting!!!
			LOG_SEV(app_log, info) << "Unknown type[" << v.type().name() << "]";
		}
	}
}



const boost::any& Configuration::getValueByKey(const string& key) const
{
	return options.at(key).value();
}
