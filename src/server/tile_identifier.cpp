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



#include "general/configuration.hpp"
#include "server/tile_identifier.hpp"
#include "server/stylesheet_manager.hpp"
#include "utils/exceptions.hpp"
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>

const string TileIdentifier::FormatString[TileIdentifier::enumSize] = {
	"png",
//	"jpg",
//	"gif",
	"svg",
//	"svgz"
};
/**
 * @brief Converts the given char* to integer
 * 
 * @param p Char pointer
 * @return Int read from the char pointer
 * @throws MalformedURLException if no convertable string given.
 **/
int TileIdentifier::stringToInt(const char *p) {
    int x = 0;
    bool neg = false;
    if (*p == '-') {
        neg = true;
        ++p;
    }
    while (*p >= '0' && *p <= '9') {
        x = (x*10) + (*p - '0');
        ++p;
    }
    if (*p != 0) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Incorrect argument (No integer)."));
	}
    if (neg) {
        x = -x;
    }
    return x;
}

/**
 * @brief Constructs a new TileIdentifier with the given url
 *
 * @param url The url which should be parsed.
 * @param StylesheetManager used to validate the stylsheet parameter
 * @return A new TileIdentifier
 * @throws MalformedURLException if some part of the url isn't parseable.
 **/
shared_ptr<TileIdentifier> TileIdentifier::Create(const string& url, std::shared_ptr<StylesheetManager> stylesheetManager, const shared_ptr<Configuration>& config)
{
	int x, y, zoom;
	string styleSheetpath;
	Format imageFormat;
	// Boundaries for each Zoomlevel
	static int boundaries[] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536, 131072, 262144};

	// Parsing the URL
	std::vector<string> parts;
	boost::split(parts, url, boost::is_any_of("/"));
	int length = parts.size();
	// Check if there are enough parts for a valid url.
	// /stylesheet/zoom/x/y.format -> Minimum 4 parts. (stylesheet can be omitted)
	if (length < 4) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Not enough arguments."));
	}
	std::vector<string> subparts;
	boost::split(subparts, parts.at(length-1), boost::is_any_of("."));
	if (subparts.size() < 2) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Not enough arguments (No y.format)."));
	}
	// Convert format to enum
	string& format = subparts.at(1);
	std::transform(format.begin(), format.end(), format.begin(), ::tolower);
	if (format == "png") {
		imageFormat = PNG;
//	} else if (format == "jpg" | format == "jpeg") {
//		imageFormat = JPEG;
//		BOOST_THROW_EXCEPTION(excp::UnknownImageFormatException() << excp::InfoWhat("jpeg/jpg is not supported"));
//	} else if (format == "gif") {
//		imageFormat = GIF;
//		BOOST_THROW_EXCEPTION(excp::UnknownImageFormatException() << excp::InfoWhat("gif is not supported"));
	} else if (format == "svg") {
		imageFormat = SVG;
//	} else if (format == "svgz") {
//		imageFormat = SVGZ;
//		BOOST_THROW_EXCEPTION(excp::UnknownImageFormatException() << excp::InfoWhat("svgz is not supported"));
	} else {
		BOOST_THROW_EXCEPTION(excp::UnknownImageFormatException() << excp::InfoWhat("Unknown image format."));
	}
	// Parse zoom
	zoom = stringToInt(parts[length-3].c_str());
	if (zoom < 0 || zoom > 18) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Incorrect zoom argument (Out of range 0 - 18)."));
	}
	// Parse y coordinate
	y = stringToInt(subparts[0].c_str());
	if (y < 0 || y >= boundaries[zoom]) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Incorrect y argument (Out of range 0 - 2^zoom)."));
	}
	// Parse x coordinate
	x = stringToInt(parts[length-2].c_str());
	if (x < 0 || x >= boundaries[zoom]) {
		BOOST_THROW_EXCEPTION(excp::MalformedURLException() << excp::InfoWhat("Incorrect x argument (Out of range 0 - 2^zoom)."));
	}
	// Build stylesheetPath.
	styleSheetpath = "";
	for (int i = 1; i < length - 3; i++) {
		if (i != 1) {
			styleSheetpath += "/";
		}
		styleSheetpath += parts.at(i);
	}
	if (styleSheetpath == "" || !stylesheetManager->hasStylesheet(styleSheetpath))
	{
		styleSheetpath = config->get<string>(opt::server::path_to_default_style);

		if (!stylesheetManager->hasStylesheet(styleSheetpath))
			styleSheetpath = ".fallback";
	}

	return std::make_shared<TileIdentifier>(x, y, zoom, styleSheetpath, imageFormat);
}

/**
 * @brief Return the TileIdentifier used for Tiles without data.
 * 
 * @return std::shared_ptr to TileIdentifier
 **/
shared_ptr<TileIdentifier> TileIdentifier::CreateEmptyTID(const string& stylesheetPath, TileIdentifier::Format format)
{
	return std::make_shared<TileIdentifier>(-2, -2, -2, stylesheetPath, format);
}

/**
 * @brief Constructs a new TileIdentifier with the given parameters.
 *
 * @param x The x coordinate of the Tile.
 * @param y The y coordinate of the Tile.
 * @param zoom The zoom level of the Tile.
 * @param styleSheetpath The path to the Stylesheet, which should be used for rendering this Tile. (With preceeding slash).
 * @param imageFormat The image Format of the rendered Tile.
 **/
TileIdentifier::TileIdentifier(int x, int y, int zoom, string styleSheetpath, Format imageFormat) :
	x(x),
	y(y),
	zoom(zoom),
	styleSheetpath(styleSheetpath),
	imageFormat(imageFormat)
{
	assert(imageFormat != enumSize);
}

/**
 * @brief Returns the x coordinate of the Tile.
 *
 * @return The x coordinate of the Tile.
 **/
int TileIdentifier::getX() const
{
	return x;
}

/**
 * @brief Returns the y coordinate of the Tile.
 *
 * @return The y coordinate of the Tile.
 **/
int TileIdentifier::getY() const
{
	return y;
}

/**
 * @brief Returns the zoom level of the Tile.
 *
 * @return The zoom level of the Tile.
 **/
int TileIdentifier::getZoom() const
{
	return zoom;
}

/**
 * @brief Returns the image Format of the Tile.
 *
 * @return The image Format of the Tile.
 **/
TileIdentifier::Format TileIdentifier::getImageFormat() const
{
	return imageFormat;
}

/**
 * @brief Returns the image Format of the Tile.
 *
 * @return The image Format of the Tile.
 **/
const string& TileIdentifier::getImageFormatString() const
{
	assert(getImageFormat() != enumSize);
	return FormatString[getImageFormat()];
}

/**
 * @brief Returns the path to the Stylesheet which should be used for rendering.
 *
 * @return The path to the Stylesheet.
 **/
const string& TileIdentifier::getStylesheetPath() const
{
	return styleSheetpath;
}

/**
 * @brief Determine whether this is the Identifier for the default tile.
 * 
 * @return True if this is the identifier for the default tile.
 **/
bool TileIdentifier::isDefaultIdentifier() const
{
	return x==-1 && y==-1 && zoom==-1 && styleSheetpath=="/";
}

bool TileIdentifier::isNoneDataIdentifier() const
{
	return x==-2 && y==-2 && zoom==-2;
}


/**
 * @brief toString method for TileIdentifier (overloading << operator).
 * 
 * @param out ostream after that the representation is appended.
 * @param ti TileIdentifier whose representation shall be appended.
 * @return Returns an ostream with content of out and all attributes of the TileIdentifier.
 **/
std::ostream& operator<<(std::ostream& out, const TileIdentifier& ti)
{
	return out << "TileIdentifier(x=" << ti.getX()
							<< ", y=" << ti.getY()
							<< ", z=" << ti.getZoom()
							<< ", css=" << ti.getStylesheetPath()
							<< ", format=" << ti.getImageFormatString() << ")";
}

/**
 * @brief Returns a hash for the TileIdentifier.
 * 
 * @return Hash usable for boost::unordered_map.
 **/
std::size_t hash_value(const TileIdentifier &ti)
{
	std::size_t seed = 0;
	boost::hash_combine(seed, ti.getX());
	boost::hash_combine(seed, ti.getY());
	boost::hash_combine(seed, ti.getZoom());
	boost::hash_combine(seed, ti.getImageFormat());
	boost::hash_combine(seed, ti.getStylesheetPath());
	return seed;
}

/**
 * @brief Equals operator for two TileIdentifiers.
 * 
 * @return True if TileIdentifiers have equal values.
 **/
bool operator==(const TileIdentifier &a, const TileIdentifier &b)
{
	return a.getX() == b.getX() &&
		a.getY() == b.getY() &&
		a.getZoom() == b.getZoom() &&
		a.getImageFormat() == b.getImageFormat() &&
		a.getStylesheetPath() == b.getStylesheetPath();
}
