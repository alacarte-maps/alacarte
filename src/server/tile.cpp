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



#include "server/tile.hpp"

#include "server/tile_identifier.hpp"

/**
 * @brief Constructs a new Tile with the given TileIdentifier.
 *
 * @param id The TileIdentifier which identifies the Tile.
 **/
Tile::Tile(const shared_ptr<TileIdentifier>& id)
	: id(id)
{
}

Tile::~Tile() 
{
}

/**
 * @brief Returns if the Tile is already rendered.
 *
 * @return true if the Tile is already rendered. false if not.
 **/
bool Tile::isRendered() const
{
	return (image != nullptr);
}

/**
 * @brief Returns the rendered image.
 *
 * @return The rendered image or a nullptr if it is not rendered yet.
 **/
const Tile::ImageType& Tile::getImage() const
{
	return image;
}

/**
 * @brief Sets the image of this Tile.
 *
 * @param image A pointer to a memory block, which contains the rendered image.
 * @param length The length of the memory block.
 **/
void Tile::setImage(const Tile::ImageType&  image)
{
	this->image = image;
}

/**
 * @brief Returns the TileIdentifier of this Tile.
 *
 * @return The TileIdentifier of this Tile.
 **/
const shared_ptr<TileIdentifier>& Tile::getIdentifier() const
{
	return id;
}

