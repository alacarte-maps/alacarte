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

#include <cairomm/surface.h>
#include <cairomm/context.h>

#include "object_renderer.hpp"

class Node;
class Geodata;
class Style;
class IconCache;
class Label;

class NodeRenderer : public ObjectRenderer {
private:
	const FixedPoint& location;
	FloatPoint transformedLocation;

	void transformLocation(const Cairo::RefPtr<Cairo::Context>& cr);

public:
	NodeRenderer(const shared_ptr<Geodata>& data, NodeId nid, const Style* s);

	void casing(const Cairo::RefPtr<Cairo::Context>& cr);
	void stroke(const Cairo::RefPtr<Cairo::Context>& cr);
	void label(const Cairo::RefPtr<Cairo::Context>& cr,
			std::list<shared_ptr<Label> >& labels);
	void shield(const Cairo::RefPtr<Cairo::Context>& cr,
			std::list<shared_ptr<Shield> >& shields);
	void icon(const Cairo::RefPtr<Cairo::Context>& cr, IconCache& cache);
};

#endif
