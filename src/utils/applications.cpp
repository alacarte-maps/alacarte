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


#include <boost/filesystem.hpp>

#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/formatter_parser.hpp>

#include "utils/application.hpp"
#include "general/configuration.hpp"

using boost::make_shared;

/**
 * @brief Inits the new Application                                                                     
 *
 **/
Application::Application()
{

}

/**
 * @brief Starts the aplication with or without Exceptionhandling                                                                     
 *
 * @param argc	number of arguments passed to this program
 * @param argv arguments passed to this program
 * @return Returns 0 if everything was fine and 1 if the program exited unexpected.
 **/
int Application::start(int argc, char** argv)
{
	// Use ALACARTE_NO_BACKBONE to prevent alacarte from catching every exception
#ifdef ALACARTE_NO_BACKBONE
	appRun(argc, argv);
#else
	try
	{
		appRun(argc, argv);
	}
	catch (...)
	{
		// We don't even have the slightest idea, what might have happened
		std::string info = boost::current_exception_diagnostic_information();

		BOOST_LOG_TRIVIAL(fatal) << "Boost diagnostic:\n" << info;
		return 1;
	}
#endif

	return 0;
}

/**
 * @brief Creates the Configuration and runs the application                                                            
 *
 * @param argc	number of arguments passed to this program
 * @param argv arguments passed to this program
 **/
void Application::appRun(int argc, char** argv)
{
	shared_ptr<Configuration> config;

	// create and parse config
	try{
		config = make_shared<Configuration>(cmd_desc, config_desc, pos_desc, argc, argv);
	} catch(boost::program_options::error& e)
	{
		std::cout << "Error: " << e.what() << std::endl;

		std::cout << cmd_desc << std::endl;

		return;
	}

	// handle help
	if(config->has(opt::help))
	{
		std::cout << cmd_desc << std::endl;
		return;
	}


	initLog(config);

	config->printConfigToLog();


	if (config->get<string>(opt::config) != DEFAULT_CONFIG_NAME && !config->usedConfigFile())
	{
		const std::vector<string> &dirs = config->getSeachDirectories();
		LOG_SEV(app_log, error) << "The given config file was not found. Searched for:";

		std::for_each(begin(dirs), end(dirs), [&](const string &dir)
		{
			LOG_SEV(app_log, error) << opt::config << " = \"" << dir << "/" << config->get<string>(opt::config) << "\"";
		});
		return;
	}

	if(!startupDiagnostic(config)) return;
	// now do what you have to do!
	onRun(config);
}

/**
 * @brief Inits the logfile                                                                     
 *
 * @param config Configuration containing more information about the logfile
 **/
void Application::initLog(const shared_ptr<Configuration>& config)
{
	fileLogger = logging::add_file_log(
			keywords::file_name = config->get<string>(opt::logfile),
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::format = "<%TimeStamp%>: [%Channel%] %Severity%: %Message%"
			);

	consoleLogger = logging::add_console_log(std::clog,
			keywords::format = "[%Channel%] %Severity%: %Message%"
			);

	logging::add_common_attributes();

	logging::core::get()->set_filter
	(
		logging::trivial::severity >= logging::trivial::info
	);

	customInitLog(config);
}

bool Application::diagnosticCheckFile(const shared_ptr<Configuration>& config, const string& key) 
{
	if (config->has(key)) {
		boost::filesystem::path file = config->get<string>(key);
		bool exists =  boost::filesystem::exists(file);
		if (!exists) {
			LOG_SEV(app_log, error) << key << " = \"" << file.string() << "\" does not exist.";
		}
		return exists;
	} else {
		LOG_SEV(app_log, error) << key << " is not set.";
		return false;
	}
}
