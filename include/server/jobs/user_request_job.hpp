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

#pragma once
#ifndef USER_REQUEST_JOB_HPP
#define USER_REQUEST_JOB_HPP

#include "settings.hpp"
#include "job.hpp"

class HttpRequest;
class RequestManager;

/**
 * @brief Handles computation of Tiles which are requested by a user.
 **/
class UserRequestJob : public Job
{
public:
	UserRequestJob(const shared_ptr<Configuration>& config, const shared_ptr<HttpRequest>& request, const shared_ptr<RequestManager>& manager);
	~UserRequestJob();

	virtual void process();

private:
	//! The HttpRequest which should be processed.
	shared_ptr<HttpRequest> req;
};








#endif
