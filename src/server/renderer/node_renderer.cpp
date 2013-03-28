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

#include "includes.hpp"
/*
 * =====================================================================================
 *
 *       Filename:  node_renderer.cpp
 *
 *    Description:  Wrapper for rendering operations of a node.
 *
 * =====================================================================================
 */

#include <boost/unordered_map.hpp>

#include "general/geodata.hpp"
#include "general/node.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "node_renderer.hpp"

NodeRenderer::NodeRenderer(const shared_ptr<Geodata>& data,
						  NodeId nid,
						  const Style* s,
						  const Cairo::Matrix& transform)
	: ObjectRenderer(data, s, transform)
	, node(data->getNode(nid))
{
	const FixedPoint& coord = node->getLocation();
	location.x = coord.x;
	location.y = coord.y;
	transform.transform_point(location.x, location.y);
	bounds = FloatRect(location.x, location.y, location.x, location.y);
}

void NodeRenderer::casing(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// nothing to render
	if (s->casing_width <= 0.0)
		return;

	cr->save();

	cr->arc(location.x, location.y, s->width/2.0 + s->casing_width, 0, 2*M_PI);

	cr->set_source_color(s->casing_color);

	cr->fill();
	cr->restore();
}

void NodeRenderer::stroke(const Cairo::RefPtr<Cairo::Context>& cr)
{
	// nothing to stroke
	if (s->width <= 0.0)
		return;

	cr->save();

	cr->arc(location.x, location.y, s->width/2.0, 0, 2*M_PI);

	cr->set_source_color(s->color);

	cr->fill();
	cr->restore();
}

void NodeRenderer::label(const Cairo::RefPtr<Cairo::Context>& cr,
		std::list<shared_ptr<Label> >& labels)
{
	// nothing to print
	if (s->text.str().size() == 0 || s->font_size <= 0)
		return;

	cr->save();

	cr->set_font_size(s->font_size);

	Cairo::TextExtents textSize;
	cr->get_text_extents(s->text.str(), textSize);

	addLabel(labels, location, textSize);

	cr->restore();
}

void NodeRenderer::shield(const Cairo::RefPtr<Cairo::Context>& cr,
		std::list<shared_ptr<Shield> >& shields)
{
	// nothing to print
	if (s->shield_text.str().size() == 0 || s->font_size <= 0)
		return;

	cr->save();

	cr->set_font_size(s->font_size);

	Cairo::TextExtents textSize;
	cr->get_text_extents(s->shield_text.str(), textSize);

	addShield(shields, location, textSize);

	cr->restore();
}

void NodeRenderer::icon(const Cairo::RefPtr<Cairo::Context>& cr, IconCache& cache)
{
	// path to icon not set
	if (s->icon.str().size() == 0 || s->icon_width == 0.0 || s->icon_height == 0.0)
		return;

	cr->save();

	Cairo::RefPtr<Cairo::ImageSurface> image = cache.getIcon(s->icon.str());
	double width = s->icon_width < 0 ? image->get_width() : s->icon_width;
	double height = s->icon_height < 0 ? image->get_height() : s->icon_height;
	double x0 = floor(location.x - width/2.0);
	double y0 = floor(location.y - height/2.0);
	cr->translate(x0, y0);
	cr->scale(width / image->get_width(),
			  height / image->get_height());
	cr->set_source(image, 0, 0);
	cr->paint();

	cr->restore();
}

