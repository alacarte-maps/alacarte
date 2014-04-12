/**
 *  This file is part of alaCarte.
 *
 *  alaCarte is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

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
#ifndef TILE_HPP
#define TILE_HPP

#include "settings.hpp"

class TileIdentifier;
class Cache;

class Tile
{
public:
	typedef shared_ptr< std::vector<uint8_t> > ImageType;

	Tile(const shared_ptr<TileIdentifier>& id);
	~Tile();
	TESTABLE bool isRendered() const;
	TESTABLE const ImageType& getImage() const;
	TESTABLE void setImage(const ImageType& image);
	TESTABLE const shared_ptr<TileIdentifier>& getIdentifier() const;

private:
	//! Pointer to a memory block, which contains the rendered Image.
	ImageType image;
	//! TileIdentifier which identifies this Tile.
	const shared_ptr<TileIdentifier> id;
};



#endif
