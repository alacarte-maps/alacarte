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
 *       Filename:  node_renderer.cpp
 *
 *    Description:  Wrapper for rendering operations of a node.
 *
 * =====================================================================================
 */

#include <boost/unordered_map.hpp>
#include <boost/math/constants/constants.hpp>

#include "general/geodata.hpp"
#include "general/node.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "node_renderer.hpp"

NodeRenderer::NodeRenderer(const shared_ptr<Geodata>& data,
						  NodeId nid,
						  const Style* s,
						  const cairo_matrix_t* transform)
	: ObjectRenderer(data, s, transform)
	, node(data->getNode(nid))
{
	const FixedPoint& coord = node->getLocation();
	location.x = coord.x;
	location.y = coord.y;
	cairo_matrix_transform_point(transform, &location.x, &location.y);
	bounds = FloatRect(location.x, location.y, location.x, location.y);
}

void NodeRenderer::casing(cairo_t* cr)
{
	// nothing to render
	if (s->casing_width <= 0.0)
		return;

	cairo_save(cr);

	cairo_arc(cr, location.x, location.y, s->width/2.0 + s->casing_width, 0, 2*boost::math::constants::pi<double>());

	cairo_set_source_rgba(cr, COLOR2RGBA(s->casing_color));

	cairo_fill(cr);
	cairo_restore(cr);
}

void NodeRenderer::stroke(cairo_t* cr)
{
	// nothing to stroke
	if (s->width <= 0.0)
		return;

	cairo_save(cr);

	cairo_arc(cr, location.x, location.y, s->width/2.0, 0, 2*boost::math::constants::pi<double>());

	cairo_set_source_rgba(cr, COLOR2RGBA(s->color));

	cairo_fill(cr);
	cairo_restore(cr);
}

void NodeRenderer::label(cairo_t* cr,
		std::list<shared_ptr<Label> >& labels,
		AssetCache& cache)
{
	// nothing to print
	if (s->text.str().size() == 0 || s->font_size <= 0)
		return;

	cairo_save(cr);

	cairo_set_font_size(cr, s->font_size);

	cairo_select_font_face(cr,
			s->font_family.c_str(),
			s->font_style == Style::STYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
			s->font_weight == Style::WEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
		);

	cairo_text_extents_t textSize;
	cairo_text_extents(cr, s->text.c_str(), &textSize);

	addLabel(labels, location + FloatPoint(0.0, s->text_offset), &textSize);

	cairo_restore(cr);
}

void NodeRenderer::shield(cairo_t* cr,
		std::list<shared_ptr<Shield> >& shields,
		AssetCache& cache)
{
	// nothing to print
	if (s->shield_text.str().size() == 0 || s->font_size <= 0)
		return;

	cairo_save(cr);

	cairo_set_font_size(cr, s->font_size);

	cairo_select_font_face(cr,
			s->font_family.c_str(),
			s->font_style == Style::STYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
			s->font_weight == Style::WEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
		);

	cairo_text_extents_t textSize;
	cairo_text_extents(cr, s->shield_text.c_str(), &textSize);

	addShield(shields, location, &textSize);

	cairo_restore(cr);
}

void NodeRenderer::icon(cairo_t* cr, AssetCache& cache)
{
	// path to icon not set
	if (s->icon_image.str().size() == 0 || s->icon_width == 0.0 || s->icon_height == 0.0)
		return;

	cairo_save(cr);

	cairo_surface_t* image = cache.getImage(s->icon_image.str());
	double surface_width = cairo_image_surface_get_width(image);
	double surface_height = cairo_image_surface_get_height(image);
	double width = s->icon_width < 0 ?  surface_width : s->icon_width;
	double height = s->icon_height < 0 ? surface_height : s->icon_height;
	double x0 = floor(location.x - width/2.0);
	double y0 = floor(location.y - height/2.0);
	cairo_translate(cr, x0, y0);
	cairo_scale(cr, width / surface_width,
			  height / surface_height);
	cairo_set_source_surface(cr, image, 0, 0);

	if (s->icon_opacity < 1.0)
		cairo_paint_with_alpha(cr, s->icon_opacity);
	else
		cairo_paint(cr);

	cairo_restore(cr);
}

