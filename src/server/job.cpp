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



#include "server/job.hpp"
#include "server/request_manager.hpp"
#include "server/cache.hpp"
#include "server/tile.hpp"
#include "server/tile_identifier.hpp"
#include "server/stylesheet_manager.hpp"
#include "server/stylesheet.hpp"
#include "server/renderer/renderer.hpp"
#include "server/meta_identifier.hpp"
#include "server/http_request.hpp"
#include "general/geodata.hpp"
#include "general/configuration.hpp"
#include "server/render_attributes.hpp"
#include "utils/rect.hpp"
#include "utils/transform.hpp"
#include "utils/statistic.hpp"

#define STAT_START(_X) 			Statistic::Get()->start(measurement, _X)
#define STAT_STOP(_X) 			Statistic::Get()->stop(measurement, _X)
#define STAT_STATS(_X, _Y, _Z) 	Statistic::Get()->setStats(measurement, _X, _Y, _Z)
#define STAT_WRITE() 			Statistic::Get()->finished(measurement)

/**
 * @brief Creates a new Job and sets the RequestManager.
 *
 * @param config The Configuration, e.g. for the prerender_level.
 * @param manager The RequestManager which holds all important components.
 **/
Job::Job(const shared_ptr<MetaIdentifier>& mid,
		 const shared_ptr<Configuration>& config,
		 const shared_ptr<RequestManager>& manager,
		 const shared_ptr<RenderCanvas>& canvas)
	: manager(manager)
	, config(config)
	, mid(mid)
	, canvas(canvas)
	, measurement(Statistic::Get()->startNewMeasurement(mid->getStylesheetPath(), mid->getZoom()))
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
shared_ptr<Tile> Job::computeEmpty()
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
		manager->getRenderer()->renderEmptyTile(renderAttributes, canvas, tile);
	}

	return tile;
}

/**
 * @brief Inits the internal list of tiles that are part of the MetaTile.
 * @return true if all contained tiles are in cache
 */
bool Job::initTiles()
{
	bool rendered = true;
	for (auto& id : mid->getIdentifiers())
	{
		shared_ptr<Tile> tile = manager->getCache()->getTile(id);
		rendered = rendered && tile->isRendered();
		tiles.push_back(tile);
	}

	return rendered;
}

/**
 * @brief Computes a all tiles contained in the MetaIdentifier.
 *
 **/
void Job::process()
{
	shared_ptr<Geodata> geodata = manager->getGeodata();

	FixedRect rect = computeRect(mid);
	STAT_START(Statistic::GeoContainsData);
		empty = !geodata->containsData(rect);
	STAT_STOP(Statistic::GeoContainsData);

	if(empty) {
		STAT_WRITE();
		return;
	}

	cached = initTiles();
	if (cached) {
		STAT_WRITE();
		return;
	}

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

	const shared_ptr<Renderer>& renderer = manager->getRenderer();
	STAT_START(Statistic::Renderer);
		renderer->renderMetaTile(renderAttributes, canvas, mid);
	STAT_STOP(Statistic::Renderer);
}

/*
 * @brief answers the requets for the computed tiles. Called by RequestManager
 */
void Job::deliver()
{
	if (empty)
	{
		shared_ptr<Tile> tile = computeEmpty();
		for (auto& id : mid->getIdentifiers())
		{
			for (auto& req : requests[*id])
				req->answer(tile);
		}
	} else {
		const shared_ptr<Renderer>& renderer = manager->getRenderer();
		STAT_START(Statistic::Slicing);
		for (auto& tile : tiles) {
			if (!tile->isRendered())
				renderer->sliceTile(canvas, mid, tile);

			for (auto& req : requests[*tile->getIdentifier()])
				req->answer(tile);
		}
		if (!cached)
			STAT_STOP(Statistic::Slicing);
	}

	STAT_WRITE();
}

