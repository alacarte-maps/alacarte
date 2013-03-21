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

#include "server/jobs/job.hpp"
#include "server/request_manager.hpp"
#include "server/cache.hpp"
#include "server/tile.hpp"
#include "server/tile_identifier.hpp"
#include "server/stylesheet_manager.hpp"
#include "server/stylesheet.hpp"
#include "server/renderer/renderer.hpp"
#include "general/geodata.hpp"
#include "general/configuration.hpp"
#include "server/render_attributes.hpp"
#include "utils/rect.hpp"
#include "utils/transform.hpp"
#include "utils/statistic.hpp"

/**
 * @brief Creates a new Job and sets the RequestManager.
 *
 * @param config The Configuration, e.g. for the prerender_level.
 * @param manager The RequestManager which holds all important components.
 **/
Job::Job(const shared_ptr<Configuration>& config, const shared_ptr<RequestManager>& manager)
	: manager(manager)
	, config(config)
{
}

Job::~Job()
{
}

/**
 * @brief Computes an rectangle for the given TileIdentifier.
 *
 * @param ti The TileIdentifier.
 **/
FixedRect Job::computeRect(const shared_ptr<TileIdentifier>& ti)
{
	int zoom = ti->getZoom();
	coord_t x0, x1, y0, y1;
	tileToMercator(ti->getX(), ti->getY(), zoom, x0, y0);
	tileToMercator(ti->getX() + 1, ti->getY() + 1, zoom, x1, y1);

	coord_t minX = std::min(x0, x1);
	coord_t minY = std::min(y0, y1);
	coord_t maxX = std::max(x0, x1);
	coord_t maxY = std::max(y0, y1);

	FixedRect tile(minX, minY, maxX, maxY);
	return tile.grow(tile.getWidth()  * TILE_OVERLAP,
					 tile.getHeight() * TILE_OVERLAP);
}

shared_ptr<Stylesheet> Job::getStylesheet(const shared_ptr<TileIdentifier>& orginalTI, shared_ptr<TileIdentifier>& newti)
{
	newti = orginalTI;
	shared_ptr<Stylesheet> stylesheet = manager->getStylesheetManager()->getStylesheet(newti);
	if(!stylesheet) {
		newti = boost::make_shared<TileIdentifier>(newti->getX(), newti->getY(), newti->getZoom(), config->get<string>(opt::server::path_to_default_style), newti->getImageFormat());
		stylesheet = manager->getStylesheetManager()->getStylesheet(newti);
		if(!stylesheet) {
			stylesheet = manager->getStylesheetManager()->getFallbackStylesheet();
			newti = boost::make_shared<TileIdentifier>(newti->getX(),newti->getY(), newti->getZoom(), ".fallback", newti->getImageFormat());
		}
	}
	return stylesheet;
}

/**
 * @brief Computes an empty Tile which is identified by the given TileIdentifier.
 *
 * @param ti The TileIdentifier
 * @return A rendered empty Tile
 **/
shared_ptr<Tile> Job::computeTileNoneData(const shared_ptr<TileIdentifier>& ti)
{
	shared_ptr<TileIdentifier> tid;
	shared_ptr<Stylesheet> stylesheet = getStylesheet(ti,tid);
	tid = TileIdentifier::CreateNoneDataTID(tid);
	shared_ptr<Tile> tile = manager->getCache()->getTile(tid);

	if(!tile->isRendered()) {
		shared_ptr<std::vector<NodeId>> nodeIDs 	= boost::make_shared< std::vector<NodeId>>();
		shared_ptr<std::vector<WayId>> 	wayIDs 		= boost::make_shared< std::vector<WayId>>();
		shared_ptr<std::vector<RelId>> 	relationIDs = boost::make_shared< std::vector<RelId>>();

		RenderAttributes renderAttributes;

		stylesheet->match(nodeIDs, wayIDs, relationIDs, tid, &renderAttributes);
		manager->getRenderer()->renderTile(renderAttributes, tile);
	}
	return tile;
}

/**
 * @brief Computes a Tile which is identified by the given TileIdentifier.
 *
 * @param ti The TileIdentifier
 * @param enqueueBelow if true: The 4 Tiles which are exactly below (one Zoomlevel higher) this Tile, will be enqueued.
 * @return A rendered Tile
 **/
shared_ptr<Tile> Job::computeTile(const shared_ptr<TileIdentifier>& ti)
{
	shared_ptr<Statistic::JobMeasurement> measurement = Statistic::instance().startNewMeasurement(ti->getZoom());
	Statistic::instance().start(measurement, Statistic::ComputeRect);
		FixedRect rect = computeRect(ti);
	Statistic::instance().stop(measurement, Statistic::ComputeRect);

	shared_ptr<Geodata> geodata = manager->getGeodata();
	Statistic::instance().start(measurement, Statistic::GeoContainsData);
		if ( !geodata->containsData(rect) ) {
			Statistic::instance().stop(measurement, Statistic::GeoContainsData);
			Statistic::instance().writeToFile(measurement, ti->getStylesheetPath(), config);
			return computeTileNoneData(ti);
		}
	Statistic::instance().stop(measurement, Statistic::GeoContainsData);
	
	shared_ptr<TileIdentifier> tid;
	shared_ptr<Stylesheet> stylesheet = getStylesheet(ti,tid);
		
	Statistic::instance().start(measurement, Statistic::Cache);
		shared_ptr<Tile> tile = manager->getCache()->getTile(tid);	
	Statistic::instance().stop(measurement, Statistic::Cache);
	
	if(!tile->isRendered()) {
		Statistic::instance().start(measurement, Statistic::GeoNodes);
			auto nodeIDs = geodata->getNodeIDs(rect);		
		Statistic::instance().stop(measurement, Statistic::GeoNodes);
		
		Statistic::instance().start(measurement, Statistic::GeoWays);
			auto wayIDs = geodata->getWayIDs(rect);
		Statistic::instance().stop(measurement, Statistic::GeoWays);
		
		Statistic::instance().start(measurement, Statistic::GeoRelation);
			auto relationIDs = geodata->getRelationIDs(rect);
		Statistic::instance().stop(measurement, Statistic::GeoRelation);
		
		Statistic::instance().setStats(measurement, nodeIDs->size(), wayIDs->size(), relationIDs->size());
		
		RenderAttributes renderAttributes;
		Statistic::instance().start(measurement, Statistic::StylesheetMatch);
			stylesheet->match(nodeIDs, wayIDs, relationIDs, tid, &renderAttributes);
		Statistic::instance().stop(measurement, Statistic::StylesheetMatch);
		
		Statistic::instance().start(measurement, Statistic::Renderer);
			manager->getRenderer()->renderTile(renderAttributes, tile);
		Statistic::instance().stop(measurement, Statistic::Renderer);
	}
	Statistic::instance().writeToFile(measurement, tid->getStylesheetPath(), config);
	return tile;
}
