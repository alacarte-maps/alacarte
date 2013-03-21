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

#include "includes.hpp"

#include "server/request_manager.hpp"
#include <server/http_request.hpp>
#include "server/jobs/user_request_job.hpp"
#include <server/jobs/pre_render_job.hpp>
#include <general/configuration.hpp>


/**
 * @brief Constructs a new RequestManager with the given parameters. And creates worker Threads.
 *
 * @param config The Configuration, e.g. for the amount of threads.
 * @param data The Geodata
 * @param renderer The Renderer
 * @param cache The Cache
 * @param ssm The StylesheetManager
 **/
RequestManager::RequestManager( const shared_ptr<Configuration>& config,
								const shared_ptr<Geodata>& data,
								const shared_ptr<Renderer>& renderer,
								const shared_ptr<Cache>& cache,
								const shared_ptr<StylesheetManager>& ssm )
	: config(config)
	, data(data)
	, renderer(renderer)
	, cache(cache)
	, ssm(ssm)
	, preventStop(jobPool)
	, log(log4cpp::Category::getInstance("RequestManager"))
{
	int threads = config->get<int>(opt::server::num_threads);
	currentPrerenderingThreads = 0;
	for (int i = 0; i < threads; ++i) {
		workers.push_back(
			boost::make_shared<boost::thread>(
				boost::bind(&boost::asio::io_service::run, &jobPool)
			)
		);
	}
}

/**
 * @brief Stops the io_service, joins all Threads and destructs the RequestManager.
 *
 **/
RequestManager::~RequestManager() {
	stop();
	log.debugStream() << "RequestManager destructed";
}

/**
 * @brief stops all jobs
 *
 **/
void RequestManager::stop()
{
	jobPool.stop();
	for (auto worker : workers) {
		worker->join();
	}
	workers.clear();
}


/**
 * @brief Creates a new UserRequestJob and enqueues it.
 *
 * @param r The HttpRequest which should be processed.
 **/
void RequestManager::enqueue(const shared_ptr<HttpRequest>& r)
{
	userRJMutex.lock();
	if (userRequestJobs.size() >= config->get<int>(opt::server::max_queue_size)) {
		r->answer(HttpRequest::Reply::service_unavailable);
		userRJMutex.unlock();
		return;
	}
	userRequestJobs.push(
		boost::make_shared<UserRequestJob>(config, r, shared_from_this())
	);
	userRJMutex.unlock();
	jobPool.post( boost::bind(&RequestManager::processNextJob, shared_from_this()) );
}


/**
 * @brief Creates a new PreRenderJob and enqueues it.
 *
 * @param ti The TileIdentifier which identifies the Tile which should be renderer.
 **/
void RequestManager::enqueue(const shared_ptr<TileIdentifier>& ti)
{
	preRJMutex.lock();
	preRenderJobs.push(
		boost::make_shared<PreRenderJob>(config, ti, shared_from_this())
	);
	preRJMutex.unlock();
	jobPool.post( boost::bind(&RequestManager::processNextJob, shared_from_this()) );
}

/**
 * @brief Selects the next Job and runs it process Method.
 **/
void RequestManager::processNextJob()
{
	shared_ptr<UserRequestJob> urj;	
	userRJMutex.lock();
	if (!userRequestJobs.empty()) {
		urj = userRequestJobs.front();
		userRequestJobs.pop();
	}
	userRJMutex.unlock();
	if (urj) {
		urj->process();
		return;
	}
	
	shared_ptr<PreRenderJob> prj;
	preRJMutex.lock();
	if (!preRenderJobs.empty()) {
		prj = preRenderJobs.front();
		preRenderJobs.pop();
		currentPrerenderingThreads++;
	}
	preRJMutex.unlock();
	if (prj){
		prj->process();
		preRJMutex.lock();
		currentPrerenderingThreads--;
		preRJMutex.unlock();
		if (prj->prerenderingFinished() && currentPrerenderingThreads == 0 && preRenderJobs.size() == 0)
		{
			log.infoStream() << "Prerendering finished.";
		}
		return;
	}
	//Error: Trying to run a job, but there isn’t one.
	log << log4cpp::Priority::ERROR << "Trying to run a job, but there isn’t one.";
}

/**
 * @brief Returns the Geodata
 *
 * @return The Geodata
 **/
shared_ptr<Geodata> RequestManager::getGeodata() const
{
	return data;
}


/**
 * @brief Returns the StylesheetManager
 *
 * @return The StylesheetManager
 **/
shared_ptr<StylesheetManager> RequestManager::getStylesheetManager() const
{
	return ssm;
}


/**
 * @brief Returns the Cache
 *
 * @return The Cache
 **/
shared_ptr<Cache> RequestManager::getCache() const
{
	return cache;
}


/**
 * @brief Returns the Renderer
 *
 * @return The Renderer
 **/
shared_ptr<Renderer> RequestManager::getRenderer() const
{
	return renderer;
}

