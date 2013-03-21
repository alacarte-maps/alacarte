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
 *  Maintainer: Bernhard Scheirle
 */

#ifndef REQUEST_MANAGER_HPP
#define REQUEST_MANAGER_HPP


#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>

#include "settings.hpp"

class UserRequestJob;
class PreRenderJob;

class Configuration;
class Cache;
class Renderer;
class Geodata;
class StylesheetManager;
class HttpRequest;
class TileIdentifier;

class RequestManager : public boost::enable_shared_from_this<RequestManager>
{
public:
	RequestManager( const shared_ptr<Configuration>& config, const shared_ptr<Geodata>& data, const shared_ptr<Renderer>& renderer, const shared_ptr<Cache>& cache, const shared_ptr<StylesheetManager>& ssm);
	~RequestManager();

	void stop();

	TESTABLE void enqueue(const shared_ptr<HttpRequest>& r);
	TESTABLE void enqueue(const shared_ptr<TileIdentifier>& ti);
	TESTABLE shared_ptr<Geodata> getGeodata() const;
	TESTABLE shared_ptr<StylesheetManager> getStylesheetManager() const;
	TESTABLE shared_ptr<Cache> getCache() const;
	TESTABLE shared_ptr<Renderer> getRenderer() const;

private:
	void processNextJob();
	
private:
	shared_ptr<Geodata> data;
	shared_ptr<Renderer> renderer;
	shared_ptr<Cache> cache;
	shared_ptr<StylesheetManager> ssm;
	shared_ptr<Configuration> config;

	boost::asio::io_service jobPool;
	boost::asio::io_service::work preventStop;
	std::vector< shared_ptr<boost::thread> > workers;
	
	boost::mutex userRJMutex;
	boost::mutex preRJMutex;
	std::queue< shared_ptr<UserRequestJob> > userRequestJobs;
	std::queue< shared_ptr<PreRenderJob> > preRenderJobs;
	
	unsigned int currentPrerenderingThreads;
	
	log4cpp::Category& log;
};


#endif
