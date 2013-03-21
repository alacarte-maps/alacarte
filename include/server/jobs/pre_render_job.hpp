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
 **/

#pragma once
#ifndef PRE_RENDER_JOB_HPP
#define PRE_RENDER_JOB_HPP

#include "settings.hpp"
#include "job.hpp"


class RequestManager;
class TileIdentifier;

/**
 * @brief Handles computation of Tiles which should be pre rendered.
 **/
class PreRenderJob : public Job
{
public:
	PreRenderJob(const shared_ptr<Configuration>& config, const shared_ptr<TileIdentifier>& tileIdentifier, const shared_ptr<RequestManager>& manager);
	~PreRenderJob();

	virtual void process();
	bool prerenderingFinished();

protected:
	//! TileIdentifier which identifies the Tile which should be computed.
	shared_ptr<TileIdentifier> tileIdentifier;
	bool finished;
	//log4cpp::Category& log;
};




#endif
