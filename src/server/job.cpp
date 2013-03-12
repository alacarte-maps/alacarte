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

#include "server/job.hpp"
#include "server/request_manager.hpp"
#include "server/cache.hpp"
#include "server/tile.hpp"
#include "server/tile_identifier.hpp"
#include "server/stylesheet_manager.hpp"
#include "server/stylesheet.hpp"
#include "server/renderer/renderer.hpp"
#include "server/meta_identifier.hpp"
#include "server/meta_tile.hpp"
#include "server/http_request.hpp"
#include "general/geodata.hpp"
#include "general/configuration.hpp"
#include "server/render_attributes.hpp"
#include "utils/rect.hpp"
#include "utils/transform.hpp"
#include "utils/statistic.hpp"

#define DEBUG(...) (log4cpp::Category::getInstance("Job").info(__VA_ARGS__));
#define STAT_START(_X) Statistic::instance().start(measurement, _X)
#define STAT_STOP(_X) Statistic::instance().stop(measurement, _X)
#define STAT_STATS(_X, _Y, _Z) Statistic::instance().setStats(measurement, _X, _Y, _Z)
#define STAT_WRITE() Statistic::instance().writeToFile(measurement, mid->getStylesheetPath(), config)

/**
 * @brief Creates a new Job and sets the RequestManager.
 *
 * @param config The Configuration, e.g. for the prerender_level.
 * @param manager The RequestManager which holds all important components.
 **/
Job::Job(const shared_ptr<MetaIdentifier>& mid,
		 const shared_ptr<Configuration>& config,
		 const shared_ptr<RequestManager>& manager)
	: manager(manager)
	, config(config)
	, mid(mid)
	, measurement(Statistic::instance().startNewMeasurement(mid->getZoom()))
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

/**
 * @brief Computes an rectangle for the given MetaIdentifier.
 *
 * @param ti The MetaIdentifier.
 **/
FixedRect Job::computeRect(const shared_ptr<MetaIdentifier>& ti)
{
	int zoom = ti->getIdentifiers()[0]->getZoom();
	coord_t x0, x1, y0, y1;
	tileToMercator(ti->getX(), ti->getY(), zoom, x0, y0);
	tileToMercator(ti->getX() + ti->getWidth(),
				   ti->getY() + ti->getHeight(),
				   zoom, x1, y1);

	coord_t minX = std::min(x0, x1);
	coord_t minY = std::min(y0, y1);
	coord_t maxX = std::max(x0, x1);
	coord_t maxY = std::max(y0, y1);

	FixedRect tile(minX, minY, maxX, maxY);
	return tile.grow(tile.getWidth()  * TILE_OVERLAP,
					 tile.getHeight() * TILE_OVERLAP);
}

/**
 * @brief Computes an empty Tile.
 *
 **/
void Job::computeEmpty()
{
	const string& path = mid->getStylesheetPath();
	const TileIdentifier::Format format = mid->getImageFormat();
	shared_ptr<Stylesheet> stylesheet = manager->getStylesheetManager()->getStylesheet(mid->getStylesheetPath());
	shared_ptr<TileIdentifier> emptyID = TileIdentifier::CreateEmptyTID(path, format);
	shared_ptr<Tile> tile = manager->getCache()->getTile(emptyID);

	if(!tile->isRendered()) {
		shared_ptr<std::vector<NodeId>> nodeIDs 	= boost::make_shared< std::vector<NodeId>>();
		shared_ptr<std::vector<WayId>> 	wayIDs 		= boost::make_shared< std::vector<WayId>>();
		shared_ptr<std::vector<RelId>> 	relationIDs = boost::make_shared< std::vector<RelId>>();

		RenderAttributes renderAttributes;

		stylesheet->match(nodeIDs, wayIDs, relationIDs, mid, &renderAttributes);
		manager->getRenderer()->renderTile(renderAttributes, tile);
	}

	tiles.push_back(tile);
}

/**
 * @brief Computes a all tiles contained in the MetaIdentifier.
 *
 **/
void Job::computeTiles(const FixedRect& rect)
{
	STAT_START(Statistic::Cache);
		bool complete = true;
		for (auto& id : mid->getIdentifiers()) {
			shared_ptr<Tile> tile = manager->getCache()->getTile(id);
			tiles.push_back(tile);
			complete = complete && tile->isRendered();
		}
		shared_ptr<MetaTile> meta = boost::make_shared<MetaTile>(tiles, mid);
	STAT_STOP(Statistic::Cache);

	if(!complete) {
		shared_ptr<Geodata> geodata = manager->getGeodata();

		STAT_START(Statistic::GeoNodes);
			auto nodeIDs = geodata->getNodeIDs(rect);
		STAT_STOP(Statistic::GeoNodes);

		STAT_START(Statistic::GeoWays);
			auto wayIDs = geodata->getWayIDs(rect);
		STAT_STOP(Statistic::GeoWays);

		STAT_START(Statistic::GeoRelation);
			auto relationIDs = geodata->getRelationIDs(rect);
		STAT_STOP(Statistic::GeoRelation);

		STAT_STATS(nodeIDs->size(), wayIDs->size(), relationIDs->size());

		shared_ptr<Stylesheet> stylesheet = manager->getStylesheetManager()->getStylesheet(mid->getStylesheetPath());
		RenderAttributes renderAttributes;
		STAT_START(Statistic::StylesheetMatch);
			stylesheet->match(nodeIDs, wayIDs, relationIDs, mid, &renderAttributes);
		STAT_STOP(Statistic::StylesheetMatch);

		STAT_START(Statistic::Renderer);
			manager->getRenderer()->renderMetaTile(renderAttributes, meta);
		STAT_STOP(Statistic::Renderer);
	}
	STAT_WRITE();
}

/**
 * @brief Processes the job.
 *
 **/
void Job::process()
{
	FixedRect rect = computeRect(mid);

	shared_ptr<Geodata> geodata = manager->getGeodata();
	STAT_START(Statistic::GeoContainsData);
		isEmpty = !geodata->containsData(rect);
	STAT_STOP(Statistic::GeoContainsData);

	if (isEmpty) {
		STAT_WRITE();
		computeEmpty();
	} else
		computeTiles(rect);
}

/*
 * @brief answers the requets for the computed tiles.
 */
void Job::deliver()
{
	// deliver the same tile to all if empty
	if (isEmpty)
	{
		for (auto& pair : requests)
			for (auto& req : pair.second)
				req->answer(tiles[0]);
	}
	else
	{
		for (auto& tile : tiles) {
			const shared_ptr<TileIdentifier> tid = tile->getIdentifier();
			for (auto& req : requests[*tid])
				req->answer(tile);
		}
	}
}

