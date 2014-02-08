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

#ifndef RELATION_RENDERER_HPP
#define RELATION_RENDERER_HPP
/*
 * =====================================================================================
 *
 *       Filename:  relation_renderer.hpp
 *
 *    Description:  Wrapper for way rendering operations.
 *
 * =====================================================================================
 */
#include "settings.hpp"

#include <cairo.h>

#include "object_renderer.hpp"

class Geodata;
class Node;
class Way;
class Style;
class Label;
class AssetCache;

class RelationRenderer : public ObjectRenderer
{
private:
	Relation* relation;
	void addRingPath(cairo_t* cr, const std::vector<WayId>& ids, int& idx, bool* used);

public:
	RelationRenderer(const shared_ptr<Geodata>& data,
					 RelId rid,
					 const Style* s,
					 const cairo_matrix_t* transform);

	void fill(cairo_t* cr, AssetCache& cache);
};

#endif
