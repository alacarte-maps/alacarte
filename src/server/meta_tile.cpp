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
 *  Maintainer: Tobias Kahlert
 */

#include "includes.hpp"

#include "server/tile.hpp"
#include "server/meta_tile.hpp"
#include "server/meta_identifier.hpp"

/**
 * @brief Constructs a new MetaTile that is several tiles big.
 *
 * @param mid The MetaIdentifier at the center of the meta-tile
 **/
MetaTile::MetaTile(const std::vector<shared_ptr<Tile> >& tiles,
				   const shared_ptr<MetaIdentifier>& mid)
	: tiles(tiles)
	, mid(mid)
{
}

/**
 * @return tiles in rows. tile at (x,y) is at index = y*width + x
 */
const std::vector<shared_ptr<Tile> >& MetaTile::getTiles() const
{
	return tiles;
}

/**
 * @return origin of the meta tile
 */
const shared_ptr<Tile>& MetaTile::getOrigin() const
{
	return tiles[0];
}

/**
 * @return the identifier that describes the meta tile
 */
const shared_ptr<MetaIdentifier>& MetaTile::getIdentifier() const
{
	return mid;
}
