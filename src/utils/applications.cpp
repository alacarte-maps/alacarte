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

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>

#include "utils/application.hpp"
#include "general/configuration.hpp"

using boost::make_shared;

/**
 * @brief Inits the new Application                                                                     
 *
 **/
Application::Application()
	: mLogInitialized(false)
{

}

/**
 * @brief Tidy up hole Application                                                                     
 *
 **/
Application::~Application()
{
	log4cpp::Category::shutdown();
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

		if(mLogInitialized)
		{
			log4cpp::Category& log = log4cpp::Category::getRoot();
			log.critStream() << "Boost diagnostic:\n" << info;
		}else{
			std::cout << info;
		}
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
	
	
	log4cpp::Category& log = log4cpp::Category::getInstance("StartupDiagnostic");
	
	if (config->get<string>(opt::config) != DEFAULT_CONFIG_NAME && !config->usedConfigFile())
	{
		const std::vector<string> &dirs = config->getSeachDirectories();
		log.errorStream() << "The given config file was not found. Searched for:";
		std::for_each(begin(dirs), end(dirs), [&](const string &dir)
		{
			log.errorStream() << opt::config << " = \"" << dir << "/" << config->get<string>(opt::config) << "\"";
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
	assert(!mLogInitialized);
	
	log4cpp::PatternLayout *rootLayout = new log4cpp::PatternLayout();
	rootLayout->setConversionPattern("[%p] [%c] %m%n");
	
	log4cpp::PatternLayout *logFileLayout = new log4cpp::PatternLayout();
	logFileLayout->setConversionPattern("%d [%p] [%c] %m%n");
	
	log4cpp::Appender *rootAappender = new log4cpp::OstreamAppender("Console", &std::cout);
	rootAappender->setLayout(rootLayout);

	log4cpp::Appender *logFileAppender = new log4cpp::FileAppender("LogFile", config->get<string>(opt::logfile), false);
	logFileAppender->setLayout(logFileLayout);

	log4cpp::Category& root = log4cpp::Category::getRoot();
	root.setPriority(log4cpp::Priority::INFO);
	root.addAppender(rootAappender);
	root.addAppender(logFileAppender);
	
	customInitLog(config, logFileAppender);
	
	mLogInitialized = true;

	root.info("Logfile opened...");
}

bool Application::diagnosticCheckFile(const shared_ptr<Configuration>& config, const string& key, log4cpp::Category& log) 
{
	if (config->has(key)) {
		boost::filesystem::path file = config->get<string>(key);
		bool exists =  boost::filesystem::exists(file);
		if (!exists) {
			log.errorStream() << key << " = \"" << file.string() << "\" does not exists.";
		}
		return exists;
	} else {
		log.errorStream() << key << " is not set.";
		return false;
	}
}
