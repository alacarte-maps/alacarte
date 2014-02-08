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
 *  Copyright alaCarte 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter0
 *  Maintainer: Patrick Niklaus
 */



#include <boost/unordered_map.hpp>

#include "general/geodata.hpp"
#include "general/way.hpp"
#include "general/node.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "object_renderer.hpp"

/**
 * @brief Adds the path of the given way to current path, if path is not set it creates it form the data
 * @param reverse paint the path in reverse order (used by relations)
 * @param connect to the last current point on the context (set by relations)
  */
void ObjectRenderer::paintLine(cairo_t* cr,
							   const std::vector<NodeId>& nodeIDs,
							   bool reverse, bool connect) const
{
	cairo_save(cr);
	cairo_set_matrix(cr, transform);

	Node* n;
	bool first = true;
	int size = nodeIDs.size();
	for (int i = 0; i < size; i++)
	{
		NodeId nid;
		if (reverse)
			nid = nodeIDs[size - 1 - i];
		else
			nid = nodeIDs[i];
		n = data->getNode(nid);
		const FixedPoint& p = n->getLocation();

		if (first && !connect)
			cairo_move_to(cr, p.x, p.y);
		else
			cairo_line_to(cr, p.x, p.y);

		first = false;
	}

	cairo_restore(cr);
}

/**
 * @brief used by node and way renderer to place a shield.
 * @param p point at the center of the shield
 */
void ObjectRenderer::addShield(std::list<shared_ptr<Shield> >& shields,
							   const FloatPoint& p,
							   const cairo_text_extents_t* textSize) const
{
	double border = ceil(s->shield_frame_width + s->shield_casing_width + 3.0);
	double x = floor(p.x - textSize->width / 2.0);
	double y = floor(p.y - textSize->height / 2.0);
	FloatPoint origin  = FloatPoint(x - textSize->x_bearing, y - textSize->y_bearing);
	FloatRect shield   = FloatRect(FloatPoint(x - border, y - border),
								   ceil(textSize->width + 2*border),
								   ceil(textSize->height + 2*border));
	FloatRect box = FloatRect(FloatPoint(p.x - RENDERER_SHIELD_DISTANCE / 2.0,
										 p.y - RENDERER_SHIELD_DISTANCE / 2.0),
							  RENDERER_SHIELD_DISTANCE, RENDERER_SHIELD_DISTANCE);
	shields.push_back(boost::make_shared<Shield>(box, bounds, s->shield_text, s, origin, shield));
}


/**
 * @brief used by node and way renderer to place a shield.
 * @param p point at the center of the shield
 */
void ObjectRenderer::addLabel(std::list<shared_ptr<Label> >& labels,
							   const FloatPoint& p,
							   const cairo_text_extents_t* textSize) const
{
	double x = p.x - textSize->width/2.0;
	double y = p.y - textSize->height/2.0;
	double border = s->text_halo_radius;
	FloatPoint origin = FloatPoint(x - textSize->x_bearing, y - textSize->y_bearing);
	FloatRect box     = FloatRect(FloatPoint(x, y), textSize->width, textSize->height).grow(border, border);
	labels.push_back(boost::make_shared<Label>(box, bounds, s->text, s, origin));
}

ObjectRenderer::ObjectRenderer(const shared_ptr<Geodata>& data, const Style* s, const cairo_matrix_t* transform)
	: data(data)
	, s(s)
	, transform(transform)
{
}
