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



#include "server/request_manager.hpp"
#include "server/http_request.hpp"
#include "server/meta_identifier.hpp"
#include "server/tile_identifier.hpp"
#include "server/renderer/render_canvas.hpp"
#include "server/job.hpp"
#include "general/configuration.hpp"

#define DEBUG(...) (log4cpp::Category::getInstance("RequestManager").info(__VA_ARGS__));

class RequestManager::RunningQueue
{
private:
	boost::mutex runningMutex;
	std::list<Job*> jobs;

public:
	bool start(Job* job, const shared_ptr<MetaIdentifier>& mid)
	{
		boost::mutex::scoped_lock lock(runningMutex);
		for (auto j : jobs) {
			if (j->getIdentifier() == mid)
				return false;
		}

		jobs.push_back(job);
		return true;
	}

	//! @return true if the job needs to be executed
	bool start(Job* job, const shared_ptr<TileIdentifier>& ti, const shared_ptr<HttpRequest>& r)
	{
		boost::mutex::scoped_lock lock(runningMutex);
		for (auto j : jobs) {
			if (j->getIdentifier()->contains(ti)) {
				j->addRequest(r, ti);
				return false;
			}
		}

		jobs.push_back(job);
		job->addRequest(r, ti);
		return true;
	}

	void finished(Job* job)
	{
		boost::mutex::scoped_lock lock(runningMutex);
		jobs.remove(job);
	}
};

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
	, running(new RunningQueue())
	, log(log4cpp::Category::getInstance("RequestManager"))
{
	int threads = config->get<int>(opt::server::num_threads);
	currentPrerenderingThreads = 0;
	for (int i = 0; i < threads; ++i) {
		auto worker = boost::make_shared<boost::thread>(
			boost::bind(&boost::asio::io_service::run, &jobPool)
		);
		workers.push_back(worker);
		factories.push(boost::make_shared<RenderCanvasFactory>());
	}

	// start prerender timing
        prerender_start = std::chrono::system_clock::now();
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
 * @brief Enqueues the HttpRequest.
 *
 * @param r The HttpRequest which should be processed.
 **/
void RequestManager::enqueue(const shared_ptr<HttpRequest>& r)
{
	boost::mutex::scoped_lock userLock(userRJMutex);
	if (userRequests.size() >= config->get<int>(opt::server::max_queue_size)) {
		r->answer(HttpRequest::Reply::service_unavailable);
		return;
	}
	userRequests.push(r);
	userLock.unlock();

	jobPool.post( boost::bind(&RequestManager::processNextRequest, shared_from_this()) );
}


/**
 * @brief Enqueues the TileIdentifier.
 *
 * @param ti The MetaIdentifier which identifies the Tile which should be renderer.
 **/
void RequestManager::enqueue(const shared_ptr<MetaIdentifier>& ti)
{
	preRJMutex.lock();
	preRenderRequests.push(ti);
	preRJMutex.unlock();

	jobPool.post( boost::bind(&RequestManager::processNextRequest, shared_from_this()) );
}

/**
 * @brief Selects the next Job and runs it process Method.
 **/
void RequestManager::processNextRequest()
{
	if (!nextUserRequest())
		if (!nextPreRenderRequest())
			log << log4cpp::Priority::ERROR << "Trying to run a job, but there is none.";
}

/**
 * @brief Selects the next Job and runs it process Method.
 * @return true if a request was processed
 **/
bool RequestManager::nextUserRequest()
{
	boost::mutex::scoped_lock userLock(userRJMutex);
	if (userRequests.empty())
		return false;

	shared_ptr<HttpRequest> req = userRequests.front();
	userRequests.pop();
	userLock.unlock();

	shared_ptr<TileIdentifier> ti;
	try
	{
		ti = TileIdentifier::Create(req->getURL(), this->ssm, config);
	}
	catch (excp::MalformedURLException &e)
	{
		log.infoStream() << "MalformedURLException: "
			<< e.what()  << " Url: " << req->getURL();

		req->answer(HttpRequest::Reply::forbidden);
	}
	catch (excp::UnknownImageFormatException &e)
	{
		log.infoStream() << "UnknownImageFormatException: "
			<< e.what()  << " Url: " << req->getURL();

		req->answer(HttpRequest::Reply::not_implemented);
	}

	// identifier could not be parsed
	if (!ti)
		return true;

	shared_ptr<MetaIdentifier> mid = MetaIdentifier::Create(ti);

	factoriesMutex.lock();
	shared_ptr<RenderCanvasFactory> factory = factories.front();
	factories.pop();
	factoriesMutex.unlock();

	shared_ptr<RenderCanvas> canvas = factory->getCanvas(ti->getImageFormat());

	Job job(mid, config, shared_from_this(), canvas);

	// check if tiles are already in progress
	if (running->start(&job, ti, req))
	{
		job.process();

		running->finished(&job);

		job.deliver();
	}

	factoriesMutex.lock();
	factories.push(factory);
	factoriesMutex.unlock();

	return true;
}

bool RequestManager::nextPreRenderRequest()
{
	boost::mutex::scoped_lock preLock(preRJMutex);
	if (preRenderRequests.empty())
		return false;

	shared_ptr<MetaIdentifier> mid = preRenderRequests.front();
	preRenderRequests.pop();
	currentPrerenderingThreads++;
	preLock.unlock();

	factoriesMutex.lock();
	shared_ptr<RenderCanvasFactory> factory = factories.front();
	factories.pop();
	factoriesMutex.unlock();

	shared_ptr<RenderCanvas> canvas = factory->getCanvas(mid->getImageFormat());

	Job job(mid, config, shared_from_this(), canvas);

	// check if tiles are already in progress
	if (running->start(&job, mid))
	{
		job.process();

		running->finished(&job);

		job.deliver();
	}

	factoriesMutex.lock();
	factories.push(factory);
	factoriesMutex.unlock();

	if (!job.isEmpty() && mid->getZoom() < config->get<int>(opt::server::prerender_level)) {
		std::vector<shared_ptr<MetaIdentifier>> children;
		mid->getSubIdentifiers(children);
		for (auto& c : children)
			enqueue(c);
	}

	preLock.lock();
	currentPrerenderingThreads--;
	preLock.unlock();

	if (currentPrerenderingThreads == 0 && preRenderRequests.size() == 0) {
		prerender_stop = std::chrono::system_clock::now();
                int elapsed_seconds = std::chrono::duration_cast<std::chrono::seconds>
                             (prerender_stop-prerender_start).count();
		log.info("Prerendering finished in %02i:%02i", elapsed_seconds / 60, elapsed_seconds % 60);
	}

	return true;
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

