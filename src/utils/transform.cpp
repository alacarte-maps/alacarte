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
 *  Maintainer: Patrick Niklaus
 */


/*
 * =====================================================================================
 *
 *       Filename:  tranform.cpp
 *
 *    Description: Utility functions to transfrom from Merkcator to lat/lon.
 *
 * =====================================================================================
 */

#include <cmath>
#include "pi.hpp"

#include "settings.hpp"

#define NORM (FACTOR * RADIUS/2.0)
// earth radius WGS84
#define RADIUS 6378137.0
// 2 decimal places
#define FACTOR 100.0

/**
 * @brief converts tile coordinates to north-west corner of the tile in the Mercator projection.
 *
 * @param tx x coordinate of the tile
 * @param ty y coordinate of the tile
 * @param zoom zoomlevel of the tile
 * @param x reference to store x coords.
 * @param y reference to store y coords.
 */
void tileToMercator(int tx, int ty, int zoom, coord_t& x, coord_t& y)
{
	int n = (1 << zoom);
	/*
	 * Calculate the inverse to:
	 * tx = floor(n * (1 + (x / (F * R/2))) / 2)
	 * ty = floor(n * (1 - (y / (F * R/2))) / 2)
	 */
	x =  (tx * 2.0 / n - 1) * NORM;
	y = -(ty * 2.0 / n - 1) * NORM;
}

/**
 * @brief converts mercator coordinates to the coords of the tile they are contained in.
 *
 * @param x coordinate in the Mercator projection.
 * @param y coordinate in the Mercator projection.
 * @param zoom zoomlevel of the tile
 * @param tx reference to store the x coordinate of the tile
 * @param ty reference to store the y coordinate of the tile
 */
void mercatorToTile(coord_t x, coord_t y, int zoom, int& tx, int& ty)
{
	int n = (1 << zoom);
	tx = (int) (n * (1 + (x / NORM)) / 2);
	ty = (int) (n * (1 - (y / NORM)) / 2);
}

/**
 * @brief converts cooridinates in lat/lon (WGS84) to spherical Mercator (EPSG:3857).
 *
 * @param p point in lat/lon coordinates.
 * @param x reference to store x coords.
 * @param y reference to store y coords.
 */
void projectMercator(const FloatPoint& p, coord_t& x, coord_t& y)
{
	double lon = p.lon / 180.0 * M_PI;
	double lat = p.lat / 180.0 * M_PI;
	x = lon / M_PI * NORM;
	y = log(tan(M_PI / 4.0 + lat / 2.0)) / M_PI * NORM;
}

/**
 * @brief converts cooridinates in lat/lon (WGS84) to spherical Mercator (EPSG:3857).
 *
 * @param p point in Mercator coordinates.
 * @param lat reference to store lat coords.
 * @param lon reference to store lon coords.
 */
void inverseMercator(const FixedPoint& p, double& lat, double& lon)
{
	lon = p.x / NORM * 180.0;
	lat = atan(sinh(p.y / NORM * M_PI)) / M_PI * 180.0;
}

