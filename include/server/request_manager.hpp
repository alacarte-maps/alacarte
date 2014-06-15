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

#include "server/job.hpp"

#include "settings.hpp"

class Configuration;
class Cache;
class Renderer;
class Geodata;
class StylesheetManager;
class HttpRequest;
class TileIdentifier;
class RenderCanvasFactory;

class RequestManager : public std::enable_shared_from_this<RequestManager>
{
public:
	RequestManager( const std::shared_ptr<Configuration>& config, const std::shared_ptr<Geodata>& data, const std::shared_ptr<Renderer>& renderer, const shared_ptr<Cache>& cache, const shared_ptr<StylesheetManager>& ssm);
	~RequestManager();

	void stop();

	TESTABLE void enqueue(const std::shared_ptr<HttpRequest>& r);
	TESTABLE void enqueue(const std::shared_ptr<MetaIdentifier>& ti);
	TESTABLE std::shared_ptr<Geodata> getGeodata() const;
	TESTABLE std::shared_ptr<StylesheetManager> getStylesheetManager() const;
	TESTABLE std::shared_ptr<Cache> getCache() const;
	TESTABLE std::shared_ptr<Renderer> getRenderer() const;

private:
	void processNextRequest();
	bool nextUserRequest();
	bool nextPreRenderRequest();

private:
	std::shared_ptr<Geodata> data;
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Cache> cache;
	std::shared_ptr<StylesheetManager> ssm;
	std::shared_ptr<Configuration> config;

	boost::asio::io_service jobPool;
	boost::asio::io_service::work preventStop;
	std::vector< std::shared_ptr<boost::thread> > workers;

	std::queue<std::shared_ptr<RenderCanvasFactory>> factories;
	boost::mutex factoriesMutex;

	boost::mutex userRJMutex;
	boost::mutex preRJMutex;
	std::queue< std::shared_ptr<HttpRequest> > userRequests;
	std::queue< std::shared_ptr<MetaIdentifier> > preRenderRequests;

	//! thread-safe queue of running jobs
	class RunningQueue;
	std::unique_ptr<RunningQueue> running;

	unsigned int currentPrerenderingThreads;

	log4cpp::Category& log;

	timeval prerender_start, prerender_stop;
};


#endif
