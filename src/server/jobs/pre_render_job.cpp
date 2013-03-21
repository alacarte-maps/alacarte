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

#include "server/jobs/pre_render_job.hpp"
#include "server/tile_identifier.hpp"
#include "server/request_manager.hpp"
#include "server/cache.hpp"
#include "server/tile.hpp"
#include "general/configuration.hpp"

/**
 * @brief Creates a new PreRenderJob.
 *
 * @param config The Configuration
 * @param tileIdentifier The TileIdentifier
 * @param manager The RequestManager
 **/
PreRenderJob::PreRenderJob(const shared_ptr<Configuration>& config, const shared_ptr<TileIdentifier>& tileIdentifier, const shared_ptr<RequestManager>& manager)
	: Job(config, manager)
	, tileIdentifier(tileIdentifier)
	, finished(true)
//	, log(log4cpp::Category::getInstance("PreRenderJob"))
{

}

PreRenderJob::~PreRenderJob()
{
}

bool PreRenderJob::prerenderingFinished()
{
	return finished;
}

/**
 * @brief Processes the PreRenderJob.
 **/
void PreRenderJob::process()
{
	if (!computeTile(tileIdentifier)->getIdentifier()->isNoneDataIdentifier() && tileIdentifier->getZoom() < config->get<int>(opt::server::prerender_level)) {
		int z = tileIdentifier->getZoom() + 1;
		int x = tileIdentifier->getX()*2;
		int y = tileIdentifier->getY()*2;
		
		manager->enqueue(boost::make_shared<TileIdentifier>(x,	y,	z,tileIdentifier->getStylesheetPath(),tileIdentifier->getImageFormat()));
		manager->enqueue(boost::make_shared<TileIdentifier>(x+1,y,	z,tileIdentifier->getStylesheetPath(),tileIdentifier->getImageFormat()));
		manager->enqueue(boost::make_shared<TileIdentifier>(x,	y+1,z,tileIdentifier->getStylesheetPath(),tileIdentifier->getImageFormat()));
		manager->enqueue(boost::make_shared<TileIdentifier>(x+1,y+1,z,tileIdentifier->getStylesheetPath(),tileIdentifier->getImageFormat()));
		finished = false;
	}

// 	log.infoStream() << "Prerendering for " << tileIdentifier->getStylesheetPath() << "/"
// 											<< tileIdentifier->getZoom() << "/"
// 											<< tileIdentifier->getX() << "/"
// 											<< tileIdentifier->getY() << " finished";
}

