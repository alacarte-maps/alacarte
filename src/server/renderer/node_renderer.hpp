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

#ifndef NODE_RENDERER_HPP
#define NODE_RENDERER_HPP

#include "settings.hpp"

#include <cairo.h>

#include "object_renderer.hpp"

class Node;
class Geodata;
class Style;
class AssetCache;
class Label;

class NodeRenderer : public ObjectRenderer {
private:
	FloatPoint location;
	Node* node;

public:
	NodeRenderer(const shared_ptr<Geodata>& data,
				 NodeId nid,
				 const Style* s,
				 const cairo_matrix_t* transform);

	void casing(cairo_t* cr);
	void stroke(cairo_t* cr);
	void label(cairo_t* cr,
			std::list<shared_ptr<Label> >& labels, AssetCache& cache);
	void shield(cairo_t* cr,
			std::list<shared_ptr<Shield> >& shields, AssetCache& cache);
	void icon(cairo_t* cr, AssetCache& cache);
};

#endif
