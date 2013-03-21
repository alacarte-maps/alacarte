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

#pragma once
#ifndef CONFIGURATION_HPP
#define CONFIGURATION_HPP


#include "settings.hpp"

#include <boost/program_options/variables_map.hpp>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>


namespace opt {
	// general
	//! Use Configuration::has to check weather the user wanted help
	static const char* help						= "help";

	//! Option to get the configuration filename (type: string)
	static const char* config					= "config";

	//! Option to get the filename of the log (type: string)
	static const char* logfile					= "logfile";

	// importer
	namespace importer {
		//! Option to get the path to osm xml file (type: string)
		static const char* path_to_osmdata			= "importer.osm-data";

		//! Option to get the filename to save geodata (type: string)
		static const char* path_to_geodata			= "importer.geo-data";

		//! Check all xml entities
		static const char* check_xml_entities = "importer.check-xml-entities";
	}

	namespace server {
		// server
		//! Filepath where the geodata is located (type: string)
		static const char* path_to_geodata			= "server.geo-data";

		//! Filepath to the access log (type: string)
		static const char* access_log				= "server.access-log";

		//! Option to get number of worker threads (type: int)
		static const char* num_threads				= "server.num-threads";

		//! Path to be observed for stylesheets (type: string)
		static const char* style_source				= "server.style-src";

		//! Address of the server (type: string)
		static const char* server_address			= "server.address";

		//! Port the server should listen at (type: int)
		static const char* server_port				= "server.port";

		//! Maximum size for the queue (type: int)
		static const char* max_queue_size			= "server.max-queue";

		//! Path to the default stylesheet (type: string)
		static const char* path_to_default_style	= "server.default-style";

		//! Path to the default tile (type: string)
		static const char* path_to_default_tile		= "server.default-tile";

		//! Option to get the cache size (type: int)
		static const char* cache_size				= "server.cache-size";

		//! Option to get the zoomlevel until tiles are kept on harddrive (type: int)
		static const char* cache_keep_tile			= "server.cache-keep-tile";

		//! Option to get the cache path (type: string)
		static const char* cache_path				= "server.cache-path";

		//! Option to get the timeout for stylesheet-parsing (type: int)
		static const char* parse_timeout			= "server.parse-timeout";

		//! Option to get the timeout for request-processing (type: int)
		//static const char* request_timeout			= "server.request-timeout";

		//! Option to get the highest zoomlevel that is enqueued for prerendering (type: int)
		static const char* prerender_level			= "server.prerender-level";

		//! Option to get the muted Components
		static const char* log_mute_component		= "server.log-mute-component";

		//! Filepath to the performance log (type: string)
		static const char* performance_log				= "server.performance-log";
	}
}

/**
 * @brief Represents a set of options accessible via strings.
 *
 **/
class Configuration : public boost::enable_shared_from_this<Configuration>
{
public:
	Configuration();

	Configuration(	boost::program_options::options_description& cmd_desc,
					boost::program_options::options_description& config_desc,
					boost::program_options::positional_options_description& pos_desc,
					int argc, char** argv);

	bool has(const string& key);

	/**
	 * @brief Returns a value identified by a key.
	 *
	 * @param key the key identifying the value
	 * @return the value in the identified by the key casted to the wanted type
	 **/
	template<typename T>
	T get(const string& key)
	{
		return boost::any_cast<const T&>(getValueByKey(key));
	}

	void printConfigToLog();

protected:
	TESTABLE const boost::any& getValueByKey(const string& key) const;

	//! Internal map holding the options.
	boost::program_options::variables_map options;
};




#endif
