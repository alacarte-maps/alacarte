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

#include "general/configuration.hpp"
#include "general/geodata.hpp"

#include "server/cache.hpp"
#include "server/http_server.hpp"
#include "server/renderer/renderer.hpp"
#include "server/request_manager.hpp"
#include "server/stylesheet_manager.hpp"

#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/file.hpp>


using boost::program_options::options_description;
using boost::make_shared;


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
#define OPT(_name, _shortcut)	(string(_name).append(",").append(_shortcut)).c_str()
		using namespace boost::program_options;


		cmd_desc.add_options()
			(OPT(opt::help, "h"),																			"produce help message")
			(OPT(opt::config, "c"),	value<string>()->default_value(DEFAULT_CONFIG_NAME)/*->value_name("path")*/,
				"Specifies a config file which will be loaded at program start. Absolute and relative paths are possible. Additionally we search in /etc/.")
			;

		// in cmd and config
		config_desc.add_options()
			(OPT(opt::logfile, "l"), value<string>()->default_value("log.txt")/*->value_name("path")*/,										"specifies the logfile")
			(OPT(opt::server::path_to_geodata, "g"),	value<string>()->required()/*->value_name("path")*/,								"path, where preprocessed data will be saved")
			(OPT(opt::server::style_source, "s"),	value<string>()->required()->default_value(".")/*->value_name("path")*/,				"path, to be observed for stylesheets")
			(OPT(opt::server::access_log, "a"),	value<string>()->default_value("access_log.txt")/*->value_name("path")*/,					"file where server access will be logged")
			(OPT(opt::server::path_to_default_style, "d"),	value<string>()->default_value("default"),										"default stylesheet")
			(OPT(opt::server::path_to_default_tile, "t"),	value<string>()->default_value("default.png")/*->value_name("image")*/,			"default tile")
			(OPT(opt::server::num_threads, "n"),	value<int>()->default_value(std::thread::hardware_concurrency())/*->value_name("num")*/,"number of threads used to process a request")
			(OPT(opt::server::parse_timeout, "o"),	value<int>()->default_value(750)/*->value_name("ms")*/,									"maximal time in ms to parse a stylesheet")
			//(OPT(opt::server::request_timeout, "r"),	value<int>()/*->value_name("ms")*/,													"maximal time in ms to process a request")
			(OPT(opt::server::prerender_level, "z"),	value<int>()->default_value(12)/*->value_name("ms")*/,								"highest zoomlevel to enqueue for prerendering")
			(opt::server::server_address,				value<string>()->default_value("0.0.0.0")/*->value_name("addr")*/,				"Address of the server")
			(OPT(opt::server::server_port, "p"),		value<string>()->required()->default_value("8080")/*->value_name("port")*/,			"port to bind the server")
			(OPT(opt::server::max_queue_size, "q"),		value<int>()->default_value(1024)/*->value_name("size")*/,							"size for server queue")
			(opt::server::cache_size,					value<int>()->default_value(1024)/*->value_name("size")*/,							"maximal amount of tiles in cache")
			(opt::server::cache_keep_tile,				value<int>()->default_value(12)/*->value_name("size")*/,							"from 0 this zoomlevel, tiles are written to harddrive")
			(opt::server::cache_path,					value<string>()->default_value("cache")/*->value_name("path")*/,					"path to store evicted prerendered tiles")
			(opt::server::log_mute_component, 			value<std::vector<string> >()->multitoken(), 										"List of all components which should be muted.")
			(opt::server::performance_log, 				value<string>(), 																	"path, where the performance log will be saved. If not set the performance log will not be created.")
			;


		cmd_desc.add(config_desc);
		pos_desc.add(opt::server::path_to_geodata, 1).add(opt::server::style_source, 1);
	}
	
protected:
	virtual bool startupDiagnostic(const shared_ptr<Configuration>& config)
	{
		if (!diagnosticCheckFile(config, opt::server::path_to_geodata)) 	return false;

		int max_queue_size = config->get<int>(opt::server::max_queue_size);
		if (max_queue_size < 1) {
			LOG_SEV(server_log, error) << "It's not possible to use a max_queue_size(" << opt::server::max_queue_size << ") less than 1";
			return false;
		}

		int hardware_concurrency = std::thread::hardware_concurrency();
		int threads = config->get<int>(opt::server::num_threads);
		if (hardware_concurrency  == 0)
		{
			LOG_SEV(server_log, info) << "Couldn't detect amount of cores in this machine.";
		} 
		else if (threads > hardware_concurrency)
		{
			LOG_SEV(server_log, info) << "It's not recommended to use more than " << hardware_concurrency << " (amount of cores) threads.";
		}

		if (threads < 1)
		{
			LOG_SEV(server_log, error) << "It's not possible to use less then 1 thread for rendering. Please set " << opt::server::num_threads << " appropriate.";
			return false;
		}

		int parse_timeout = config->get<int>(opt::server::parse_timeout);
		if (parse_timeout < 50) {
			LOG_SEV(server_log, error) << "It's not possible to use less than 50ms for " << opt::server::parse_timeout;
			return false;
		}
		/*
		if (config->has(opt::server::request_timeout)) {
			int request_timeout = config->get<int>(opt::server::request_timeout);
		}
		
		if (true) {
			LOG_SEV(server_log, info) << opt::server::request_timeout << " not implemented yet";
		}
		*/
		if (config->has(opt::server::style_source)) {
			boost::filesystem::path folder = config->get<string>(opt::server::style_source);
			if (!boost::filesystem::is_directory(folder)) {
				LOG_SEV(server_log, error) << opt::server::style_source << " = \"" << folder.string() << "\" is not a directory.";
				return false;
			}
			
		} else {
			LOG_SEV(server_log, error) << opt::server::style_source << " is not set.";
			return false;
		}
		
		if (config->has(opt::server::cache_path)) {
			boost::filesystem::path folder = config->get<string>(opt::server::cache_path);
			if (!boost::filesystem::is_directory(folder)) {
				try {
					boost::filesystem::create_directory(folder);
				} catch (boost::filesystem::filesystem_error) {
					LOG_SEV(server_log, error) << opt::server::cache_path << " = \"" << folder.string() << "\" could not be created.";
					return false;
				}
			}
		}
		
		return diagnosticCheckFile(config, opt::server::path_to_default_tile);
	}

	virtual void customInitLog(const shared_ptr<Configuration>& config)
	{
		fileLogger->set_filter(logging::expressions::attr<std::string>("Channel") != "Access");
		consoleLogger->set_filter(logging::expressions::attr<std::string>("Channel") != "Access");

		logging::add_file_log(
			keywords::file_name = config->get<string>(opt::server::access_log),
			keywords::rotation_size = 10 * 1024 * 1024,
			keywords::time_based_rotation = logging::sinks::file::rotation_at_time_point(0, 0, 0),
			keywords::format = "<%TimeStamp%>: %Message%",
			keywords::filter = (logging::expressions::attr<std::string>("Channel") == "Access")
			);
	}


	/**
	 * @brief Contains the main functionality for the server                                                                  
	 *
	 * @param config the configuration for the application
	 **/
	virtual void onRun( const shared_ptr<Configuration>& config )
	{
		Statistic::Init(config);

		shared_ptr<Geodata> geodata = make_shared<Geodata>();
		try {
			geodata->load(config->get<string>(opt::server::path_to_geodata));
		} catch(...)
		{
			BOOST_LOG_TRIVIAL(error) << "Failed to load geodata!";
			BOOST_LOG_TRIVIAL(error) << "Try to import your osm data again!";
			return;
		}
		shared_ptr<Cache> cache = make_shared<Cache>(config);

		shared_ptr<StylesheetManager> ssm = make_shared<StylesheetManager>(config);

		shared_ptr<Renderer> renderer = make_shared<Renderer>(geodata);

		shared_ptr<RequestManager> req_manager = make_shared<RequestManager>(config, geodata, renderer, cache, ssm);

		shared_ptr<HttpServer> server = make_shared<HttpServer>(config, req_manager);

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
