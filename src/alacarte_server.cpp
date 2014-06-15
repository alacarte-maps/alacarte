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



#include <iostream>
#include <thread>

#include "settings.hpp"

#include "utils/application.hpp"

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>

#include "general/configuration.hpp"
#include "general/geodata.hpp"

#include "server/cache.hpp"
#include "server/http_server.hpp"
#include "server/renderer/renderer.hpp"
#include "server/request_manager.hpp"
#include "server/stylesheet_manager.hpp"



using boost::program_options::options_description;


/**
 * @brief Represents the Server-Application                                                                     
 *
 **/
class AlacarteServerApp
	: public Application
{
public:
	/**
	 * @brief Inits the Configuration descriptions                                                                      
	 *
	 **/
	AlacarteServerApp()
	{
#define OPT(_name, _shortcut)	(std::string(_name).append(",").append(_shortcut)).c_str()
		using namespace boost::program_options;


		cmd_desc.add_options()
			(OPT(opt::help, "h"),																			"produce help message")
			(OPT(opt::config, "c"),	value<std::string>()->default_value(DEFAULT_CONFIG_NAME)/*->value_name("path")*/,
				"Specifies a config file which will be loaded at program start. Absolute and relative paths are possible. Additionally we search in /etc/.")
			;

		// in cmd and config
		config_desc.add_options()
			(OPT(opt::logfile, "l"), value<std::string>()->default_value("log.txt")/*->value_name("path")*/,										"specifies the logfile")
			(OPT(opt::server::path_to_geodata, "g"),	value<std::string>()->required()/*->value_name("path")*/,								"path, where preprocessed data will be saved")
			(OPT(opt::server::style_source, "s"),	value<std::string>()->required()->default_value(".")/*->value_name("path")*/,				"path, to be observed for stylesheets")
			(OPT(opt::server::access_log, "a"),	value<std::string>()->default_value("access_log.txt")/*->value_name("path")*/,					"file where server access will be logged")
			(OPT(opt::server::path_to_default_style, "d"),	value<std::string>()->default_value("default"),										"default stylesheet")
			(OPT(opt::server::path_to_default_tile, "t"),	value<std::string>()->default_value("default.png")/*->value_name("image")*/,			"default tile")
			(OPT(opt::server::num_threads, "n"),	value<int>()->default_value(std::thread::hardware_concurrency())/*->value_name("num")*/,"number of threads used to process a request")
			(OPT(opt::server::parse_timeout, "o"),	value<int>()->default_value(750)/*->value_name("ms")*/,									"maximal time in ms to parse a stylesheet")
			//(OPT(opt::server::request_timeout, "r"),	value<int>()/*->value_name("ms")*/,													"maximal time in ms to process a request")
			(OPT(opt::server::prerender_level, "z"),	value<int>()->default_value(12)/*->value_name("ms")*/,								"highest zoomlevel to enqueue for prerendering")
			(opt::server::server_address,				value<std::string>()->default_value("0.0.0.0")/*->value_name("addr")*/,				"Address of the server")
			(OPT(opt::server::server_port, "p"),		value<std::string>()->required()->default_value("8080")/*->value_name("port")*/,			"port to bind the server")
			(OPT(opt::server::max_queue_size, "q"),		value<int>()->default_value(1024)/*->value_name("size")*/,							"size for server queue")
			(opt::server::cache_size,					value<int>()->default_value(1024)/*->value_name("size")*/,							"maximal amount of tiles in cache")
			(opt::server::cache_keep_tile,				value<int>()->default_value(12)/*->value_name("size")*/,							"from 0 this zoomlevel, tiles are written to harddrive")
			(opt::server::cache_path,					value<std::string>()->default_value("cache")/*->value_name("path")*/,					"path to store evicted prerendered tiles")
			(opt::server::log_mute_component, 			value<std::vector<std::string> >()->multitoken(), 										"List of all components which should be muted.")
			(opt::server::performance_log, 				value<std::string>(), 																	"path, where the performance log will be saved. If not set the performance log will not be created.")
			;


		cmd_desc.add(config_desc);
		pos_desc.add(opt::server::path_to_geodata, 1).add(opt::server::style_source, 1);
	}
	
protected:
	virtual bool startupDiagnostic(const std::shared_ptr<Configuration>& config)
	{
		log4cpp::Category& log = log4cpp::Category::getInstance("StartupDiagnostic");

		if (!diagnosticCheckFile(config, opt::server::path_to_geodata, log) ) 	return false;

		int max_queue_size = config->get<int>(opt::server::max_queue_size);
		if (max_queue_size < 1) {
			log.errorStream() << "It's not possible to use a max_queue_size(" << opt::server::max_queue_size << ") less than 1";
			return false;
		}

		int hardware_concurrency = std::thread::hardware_concurrency();
		int threads = config->get<int>(opt::server::num_threads);
		if (hardware_concurrency  == 0)
		{
			log.infoStream() << "Couldn't detect amount of cores in this machine.";
		} 
		else if (threads > hardware_concurrency)
		{
			log.infoStream() << "It's not recommended to use more than " << hardware_concurrency << " (amount of cores) threads.";
		}

		if (threads < 1)
		{
			log.errorStream() << "It's not possible to use less then 1 thread for rendering. Please set " << opt::server::num_threads << " appropriate.";
			return false;
		}

		int parse_timeout = config->get<int>(opt::server::parse_timeout);
		if (parse_timeout < 50) {
			log.errorStream() << "It's not possible to use less than 50ms for " << opt::server::parse_timeout;
			return false;
		}
		/*
		if (config->has(opt::server::request_timeout)) {
			int request_timeout = config->get<int>(opt::server::request_timeout);
		}
		
		if (true) {
			log.infoStream() << opt::server::request_timeout << " not implemented yet";
		}
		*/
		if (config->has(opt::server::style_source)) {
			boost::filesystem::path folder = config->get<std::string>(opt::server::style_source);
			if (!boost::filesystem::is_directory(folder)) {
				log.errorStream() << opt::server::style_source << " = \"" << folder.string() << "\" is not a directory.";
				return false;
			}
			
		} else {
			log.errorStream() << opt::server::style_source << " is not set.";
			return false;
		}
		
		if (config->has(opt::server::cache_path)) {
			boost::filesystem::path folder = config->get<std::string>(opt::server::cache_path);
			if (!boost::filesystem::is_directory(folder)) {
				try {
					boost::filesystem::create_directory(folder);
				} catch (boost::filesystem::filesystem_error) {
					log.errorStream() << opt::server::cache_path << " = \"" << folder.string() << "\" could not be created.";
					return false;
				}
			}
		}
		
		if (!diagnosticCheckFile(config, opt::server::path_to_default_tile, log))	return false;

		return true;
	}
	
	virtual void customInitLog(const std::shared_ptr<Configuration>& config, log4cpp::Appender *logFile) 
	{
		log4cpp::PatternLayout *accessLogFileLayout = new log4cpp::PatternLayout();
		accessLogFileLayout->setConversionPattern("%m%n");
		
		log4cpp::Appender *accessLogFileAppender = new log4cpp::FileAppender("AccessLogFile", config->get<std::string>(opt::server::access_log), false);
		accessLogFileAppender->setLayout(accessLogFileLayout);
		
		log4cpp::Category::getInstance("AccessLog").addAppender(accessLogFileAppender);
		log4cpp::Category::getInstance("AccessLog").setAdditivity(false);
		if( config->has(opt::server::log_mute_component) ) {
			std::vector<std::string> list = config->get<std::vector<string>>(opt::server::log_mute_component);
			for(std::string component : list) {
				log4cpp::Category::getInstance(component).addAppender(logFile);
				log4cpp::Category::getInstance(component).setAdditivity(false);
			}
		}
	}
	
	
	
	/**
	 * @brief Contains the main functionality for the server                                                                  
	 *
	 * @param config the configuration for the application
	 **/
	virtual void onRun( const std::shared_ptr<Configuration>& config )
	{
		Statistic::Init(config);

		std::shared_ptr<Geodata> geodata = std::make_shared<Geodata>();
		try {
			geodata->load(config->get<std::string>(opt::server::path_to_geodata));
		} catch(...)
		{
			log4cpp::Category& log = log4cpp::Category::getRoot();
			log.errorStream() << "Failed to load geodata!";
			log.errorStream() << "Try to import your osm data again!";
			return;
		}
		std::shared_ptr<Cache> cache = std::make_shared<Cache>(config);

		std::shared_ptr<StylesheetManager> ssm = std::make_shared<StylesheetManager>(config);

		std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(geodata);

		std::shared_ptr<RequestManager> req_manager = std::make_shared<RequestManager>(config, geodata, renderer, cache, ssm);

		std::shared_ptr<HttpServer> server = std::make_shared<HttpServer>(config, req_manager);

		ssm->startStylesheetObserving(req_manager);

		server->listen();

		// Stop all threads so they loose all their handles!
		req_manager->stop();

		ssm->stopStylesheetObserving();
	}

};




//! \cond internal
int main(int argc, char** argv)
{
	AlacarteServerApp app;

	int result = app.start(argc, argv);

	CachedString::Shutdown();

	return result;
}
//! \endcond
