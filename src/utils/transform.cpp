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

#include <boost/math/constants/constants.hpp>
#include <cmath>

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
	double lon = p.lon / 180.0 * boost::math::constants::pi<double>();
	double lat = p.lat / 180.0 * boost::math::constants::pi<double>();
	x = lon / boost::math::constants::pi<double>() * NORM;
	y = log(tan(boost::math::constants::pi<double>() / 4.0 + lat / 2.0)) / boost::math::constants::pi<double>() * NORM;
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
	lat = atan(sinh(p.y / NORM * boost::math::constants::pi<double>())) / boost::math::constants::pi<double>() * 180.0;
}

uint64_t spreadBits32(uint32_t y)
{
	uint64_t B[] = {
		0x5555555555555555,
		0x3333333333333333,
		0x0f0f0f0f0f0f0f0f,
		0x00ff00ff00ff00ff,
		0x0000ffff0000ffff,
		0x00000000ffffffff
	};

	int S[] = { 1, 2, 4, 8, 16, 32 };

	uint64_t x = y;
	x = (x | (x << S[5])) & B[5];
	x = (x | (x << S[4])) & B[4];
	x = (x | (x << S[3])) & B[3];
	x = (x | (x << S[2])) & B[2];
	x = (x | (x << S[1])) & B[1];
	x = (x | (x << S[0])) & B[0];

	return x;
}

uint64_t interleave64(uint32_t x, uint32_t y)
{
	return spreadBits32(x) | (spreadBits32(y) << 1);
}

uint64_t xy2hilbert(FixedPoint p)
{
	// convert signed to unsigned with offset
	uint32_t x = p.x - std::numeric_limits<coord_t>::min();
	uint32_t y = p.y - std::numeric_limits<coord_t>::min();

	int r = 32;
	uint64_t mask = (1 << r) - 1;
	uint32_t hodd = 0;
	uint32_t heven = x ^ y;
	uint64_t notx = ~x & mask;
	uint64_t noty = ~y & mask;
	uint64_t temp = notx ^ y;

	uint64_t v0 = 0, v1 = 0;
	for (int k = 1; k < r; k++) {
		v1 = ((v1 & heven) | ((v0 ^ noty) & temp)) >> 1;
		v0 = ((v0 & (v1 ^ notx)) | (~v0 & (v1 ^ noty))) >> 1;
	}
	hodd = (~v0 & (v1 ^ x)) | (v0 & (v1 ^ noty));

	return interleave64(heven, hodd);
}

