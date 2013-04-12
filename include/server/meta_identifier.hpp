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

#pragma once
#ifndef META_IDENTIFIER_HPP
#define META_IDENTIFIER_HPP

#include "settings.hpp"
#include "server/tile_identifier.hpp"

/**
 * @brief A MetaIdentifier identifies a set of Tiles that a rendered together
 **/
class MetaIdentifier : public TileIdentifier
{
public:
	static shared_ptr<MetaIdentifier> Create(const shared_ptr<TileIdentifier>& origin);
	MetaIdentifier(const TileIdentifier& origin);

	TESTABLE int getWidth() const;
	TESTABLE int getHeight() const;
	bool contains(const shared_ptr<TileIdentifier> tid) const;

	TESTABLE const std::vector<shared_ptr<TileIdentifier>>& getIdentifiers() const;
	TESTABLE void getSubIdentifiers(std::vector<shared_ptr<MetaIdentifier>>& tiles) const;

private:
	//! with of the meta tile in tiles
	int width;
	//! height of the meta tile in tiles
	int height;
	//! list of all contained tiles
	std::vector<shared_ptr<TileIdentifier>> tids;
};

#endif
