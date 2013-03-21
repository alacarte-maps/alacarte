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


#include "server/jobs/user_request_job.hpp"
#include "server/tile_identifier.hpp"
#include "server/http_request.hpp"
#include "server/request_manager.hpp"
#include "server/cache.hpp"



/**
 * @brief Creates a new UserRequestJob
 *
 * @param config The Configuration.
 * @param request The HttpRequest
 * @param manager The RequestManager
 **/
UserRequestJob::UserRequestJob(const shared_ptr<Configuration>& config, const shared_ptr<HttpRequest>& request, const shared_ptr<RequestManager>& manager)
	: Job(config, manager)
	, req(request)
{
}

UserRequestJob::~UserRequestJob()
{

}

/**
 * @brief Processes the UserRequestJob.
 *
 **/
void UserRequestJob::process()
{
	shared_ptr<TileIdentifier> ti;
	try {
		ti = TileIdentifier::Create(req->getURL());
		req->answer(computeTile(ti));
	} catch (excp::MalformedURLException &e){
		log4cpp::Category& log = log4cpp::Category::getRoot();
		log.infoStream() << "MalformedURLException: " << e.what() << " Url: " << req->getURL();
		req->answer(HttpRequest::Reply::forbidden);
	} catch (excp::UnknownImageFormatException &e) {
		log4cpp::Category& log = log4cpp::Category::getRoot();
		log.infoStream() << "UnknownImageFormatException: " << e.what() << " Url: " << req->getURL();
		req->answer(HttpRequest::Reply::not_implemented);
	}
}


