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
 *  Maintainer: Florian Jacob
 */

#pragma once
#ifndef STYLESHEET_MANAGER_HPP
#define STYLESHEET_MANAGER_HPP

#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#include <boost/unordered_map.hpp>
#include "../extras/dirwatch/dir_monitor.hpp"
#include <boost/filesystem/path.hpp>

#include "settings.hpp"

class RequestManager;
class Stylesheet;
class Configuration;
class Geodata;

namespace fs = boost::filesystem;

/**
 * The StylesheetManager provides an interface to get arbitrary Stylesheets from the stylesheet directory.
 * Additionally, it observes the stylesheet directory for changes to prerender new stylesheets, and
 * drop deleted ones off the Cache.
 */
class StylesheetManager
	: public boost::enable_shared_from_this<StylesheetManager>
	, public boost::noncopyable
{
public:
	/**
	 * @brief Creates a new StylesheetManager with the given Configuration.
	 * 
	 * @param config the Configuration object to read the stylesheet directory path and prerender level from
	 */
	StylesheetManager(const shared_ptr<Configuration>& config);

	/**
	 * @brief starts observing the Stylesheet directory.
	 * On new or changed stylesheet files, the Tiles to prerender get enqueued in the RequestManager. 
	 * On changed or deleted stylesheet files, the Cache gets notified to drop the cached Tiles.
	 * 
	 * @param manager the RequestManager to use to enqueue new Tiles to prerender
	 */
	TESTABLE void startStylesheetObserving(const shared_ptr<RequestManager>& manager);

	/**
	 * @brief Stops the folder observation
	 *
	 * Stops the internal mechanism, which is used to watch the stylesheet folder.
	 **/
	TESTABLE void stopStylesheetObserving();

	/**
	 * @brief Returns the Stylesheet Object for the Stylesheet specified.
	 * 
	 * @param path path of the stylesheet
	 * @return the Stylesheet specified or the default Stylesheet
	 */
	TESTABLE shared_ptr<Stylesheet> getStylesheet(const string& path);
	TESTABLE bool hasStylesheet(const string& path);

private:
	/**
	 * @brief tries to read and parse the given file.
	 * 			If that succeeds, saves it in the Stylesheet Cache and enqueues it for prerendering.
	 */
	TESTABLE void onNewStylesheet(const fs::path& stylesheet_path);

	/**
	 * @brief removes the Stylesheet from the Stylesheet Cache and the prerendered tiles from the Cache.
	 */
	TESTABLE void onRemovedStylesheet(const fs::path& stylesheet_path);

	/**
	 * @brief handler method for dirwatch, forwards only .mapcss files to onNewStylesheet and onRemovedStylesheet
	 */
	TESTABLE void onFileSystemEvent(const boost::system::error_code &ec, const boost::asio::dir_monitor_event &ev);

private:
	shared_ptr<Configuration> config;
	weak_ptr<RequestManager> manager;

	boost::shared_mutex stylesheetsLock;
	boost::unordered_map<fs::path, shared_ptr<Stylesheet> > parsedStylesheets;

	fs::path stylesheetFolder;

	static shared_ptr<Stylesheet> makeFallbackStylesheet(const shared_ptr<Geodata>& geodata);

	boost::asio::io_service						ioService;
	boost::thread								monitorThread;
	boost::scoped_ptr<boost::asio::dir_monitor>	monitorService;
};


#endif
