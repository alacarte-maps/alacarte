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

#pragma once
#ifndef TRANSFORM_HPP
#define TRANSFORM_HPP

#include "settings.hpp"

#include <utils/point.hpp>

void tileToMercator(int tx, int ty, int zoom, coord_t& x, coord_t& y);
void mercatorToTile(coord_t x, coord_t y, int zoom, int& tx, int& ty);

void projectMercator(const FloatPoint& p, coord_t& x, coord_t& y);
void inverseMercator(const FixedPoint& p, double& lat, double& lon);

void rot(uint32_t n, FixedPoint& p, bool rx, bool ry);
uint64_t xy2hilbert (FixedPoint p);

#endif
