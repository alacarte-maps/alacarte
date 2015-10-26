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
#ifndef APPLICATION_HPP
#define APPLICATION_HPP

#include <boost/program_options.hpp>
#include <boost/exception/enable_error_info.hpp>
#include <boost/exception/error_info.hpp>

#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>

#include "settings.hpp"


class Configuration;

class Application
{
public:
	Application();

	int start(int argc, char** argv);

protected:

	virtual void onRun(const shared_ptr<Configuration>& config) = 0;
	virtual void customInitLog(const shared_ptr<Configuration>& config) {};
	virtual bool startupDiagnostic(const shared_ptr<Configuration>& config) = 0;

	boost::program_options::options_description				cmd_desc;
	boost::program_options::options_description				config_desc;
	boost::program_options::positional_options_description	pos_desc;
	
	bool diagnosticCheckFile(const shared_ptr<Configuration>& config, const string& key);

	shared_ptr< logging::sinks::synchronous_sink< logging::sinks::text_file_backend > > fileLogger;
	shared_ptr< logging::sinks::synchronous_sink< logging::sinks::basic_text_ostream_backend< char > >> consoleLogger;

private:
	void appRun(int argc, char** argv);

	void initLog(const shared_ptr<Configuration>& config);
};


#endif
