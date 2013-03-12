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
 *  Maintainer: Simon Dreher
 */

#include "includes.hpp"

#include "server/tile_identifier.hpp"
#include "server/meta_identifier.hpp"

#define DEBUG(...) (log4cpp::Category::getInstance("MetaIdentifier").info(__VA_ARGS__));

/**
 * @brief Constructs a new TileIdentifier with the given parameters.
 *
 * @param topLeft corner of the meta-tile
 **/
shared_ptr<MetaIdentifier> MetaIdentifier::Create(const shared_ptr<TileIdentifier>& origin)
{
	return boost::make_shared<MetaIdentifier>(*origin);
}

MetaIdentifier::MetaIdentifier(const TileIdentifier& origin)
	: TileIdentifier(origin)
{
	// round to neared multiple of META_TILE_SIZE
	int x0 = origin.getX() / META_TILE_SIZE * META_TILE_SIZE;
	int y0 = origin.getY() / META_TILE_SIZE * META_TILE_SIZE;
	int x1 = x0 + META_TILE_SIZE;
	int y1 = y0 + META_TILE_SIZE;
	x1 = std::min(x1, (2 << origin.getZoom()) - 1);
	y1 = std::min(y1, (2 << origin.getZoom()) - 1);
	this->width  = x1 - x0;
	this->height = y1 - y0;
	this->x = x0;
	this->y = y0;

	for (int tx = x; tx < x1; tx++)
		for (int ty = y; ty < y1; ty++)
		{
			tids.push_back(
				boost::make_shared<TileIdentifier>(tx, ty, origin.getZoom(),
												   origin.getStylesheetPath(),
												   origin.getImageFormat())
			);
		}
}

const std::vector<shared_ptr<TileIdentifier>>& MetaIdentifier::getIdentifiers() const
{
	return tids;
}

int MetaIdentifier::getWidth() const
{
	return width;
}

int MetaIdentifier::getHeight() const
{
	return height;
}

bool MetaIdentifier::contains(const shared_ptr<TileIdentifier> tid) const
{
	int tx = tid->getX();
	int ty = tid->getY();
	return (x <= tx && tx < x+width
	     && y <= ty && ty < y+height
		 && tid->getZoom() == zoom
		 && tid->getStylesheetPath() == styleSheetpath);
}
