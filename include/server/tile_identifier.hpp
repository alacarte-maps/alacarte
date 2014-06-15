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
#ifndef TILE_IDENTIFIER_HPP
#define TILE_IDENTIFIER_HPP

#include "settings.hpp"

class StylesheetManager;
class Configuration;

/**
 * @brief A TileIdentifier identifies a Tile
 **/
class TileIdentifier
{
public:
	/**
	 * @brief Enumeration which contains all supported image formats.
	 **/
	enum Format
	{
		PNG,
//		JPEG,
//		GIF,
		SVG,
//		SVGZ,
		enumSize
	};
private:
	static const std::string FormatString[enumSize];
public:
	static std::shared_ptr<TileIdentifier> Create(const std::string& url, shared_ptr<StylesheetManager> StylesheetManager, const shared_ptr<Configuration>& config);
	static std::shared_ptr<TileIdentifier> CreateEmptyTID(const std::string& stylesheetPath,
												 	 TileIdentifier::Format format);
	static int std::stringToInt(const char* c);

	TileIdentifier(int x, int y, int zoom, std::string styleSheetpath, Format imageFormat);


	TESTABLE int getX() const;
	TESTABLE int getY() const;
	TESTABLE int getZoom() const;
	TESTABLE Format getImageFormat() const;
	TESTABLE const std::string& getImageFormatString() const;
	TESTABLE const std::string& getStylesheetPath() const;
	TESTABLE bool isDefaultIdentifier() const;
	TESTABLE bool isNoneDataIdentifier() const;

protected:
	//! x coordinate of the Tile.
	int x;
	//! y coordinate of the Tile.
	int y;
	//! zoom level of the Tile.
	int zoom;
	//! path to the Stylesheet which should be used for rendering.
	std::string styleSheetpath;
	//! Format of the image.
	Format imageFormat;
};

std::ostream& operator<<(std::ostream& out, const TileIdentifier& ti);
std::size_t hash_value(const TileIdentifier &ti);
bool operator==(const TileIdentifier &a, const TileIdentifier &b);

#endif
