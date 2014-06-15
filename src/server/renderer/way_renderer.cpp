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



#include <boost/unordered_map.hpp>
#include <boost/math/constants/constants.hpp>

#include "general/geodata.hpp"
#include "general/way.hpp"
#include "general/node.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "way_renderer.hpp"

/**
 * @brief Adds the path of the given way to current path, if path is not set it creates it form the data
 * @param cr the cairo contetx to add the path to
 *
 */
void WayRenderer::addWayPath(cairo_t* cr)
{
	cairo_new_path(cr);
	if (path != NULL) {
		cairo_append_path(cr, path);
		return;
	}

	const std::vector<NodeId>& children = way->getNodeIDs();
	paintLine(cr, children);

	path = cairo_copy_path(cr);

	double x0, y0, x1, y1;
	cairo_path_extents(cr, &x0, &y0, &x1, &y1);
	bounds = FloatRect(x0, y0, x1, y1);
}

//! Find the best fitting segment on a cairo path and return angle.
bool WayRenderer::getTextPosition(cairo_path_t* path,
								  int width, FloatPoint& best, double& angle)
{
	cairo_path_data_t *data, last_move_to, current;
	double diff;
	double bestDiff = -std::numeric_limits<double>::infinity();
	double dx, dy;
	double length;

	for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
		data = &path->data[i];
		switch (data->header.type) {
			case CAIRO_PATH_MOVE_TO:
				last_move_to = data[1];
				current      = data[1];
			break;

			case CAIRO_PATH_CLOSE_PATH:
				data = (&last_move_to) - 1;
			case CAIRO_PATH_LINE_TO:
				dx = current.point.x - data[1].point.x;
				dy = current.point.y - data[1].point.y;

				length = sqrt(dx*dx + dy*dy);
				diff = length - width;
				if (diff > 0 && diff > bestDiff) {
					if (abs(dx) > 0)
						angle = atan(dy/dx);
					else
						angle = -boost::math::constants::pi<double>()/2.0;
					best.x = current.point.x - dx * 0.5;
					best.y = current.point.y - dy * 0.5;
					bestDiff = diff;
				}

				current = data[1];
			break;

			case CAIRO_PATH_CURVE_TO:
				current = data[3];
			break;
		}
	}

	return (bestDiff > 0);
}

//! Find the best fitting segment on a cairo path for a shield
void WayRenderer::getShieldPosition(cairo_path_t* path, std::list<FloatPoint>& positions)
{
	cairo_path_data_t *data, last_move_to, current;
	double dis = RENDERER_SHIELD_DISTANCE;
	double dx, dy;
	for (int i = 0; i < path->num_data; i += path->data[i].header.length) {
		data = &path->data[i];
		switch (data->header.type) {
			case CAIRO_PATH_MOVE_TO:
				last_move_to = data[1];
				current      = data[1];
			break;

			case CAIRO_PATH_CLOSE_PATH:
				data = (&last_move_to) - 1;
			case CAIRO_PATH_LINE_TO:
				dx = current.point.x - data[1].point.x;
				dy = current.point.y - data[1].point.y;
				dis += sqrt(dx*dx + dy*dy);
				if (dis > RENDERER_SHIELD_DISTANCE)
				{
					positions.push_back(FloatPoint(current.point.x - 0.5*dx,
												   current.point.y - 0.5*dy));
					dis = 0;
				}

				current = data[1];
			break;

			case CAIRO_PATH_CURVE_TO:
				current = data[3];
			break;
		}
	}
}

WayRenderer::WayRenderer(const shared_ptr<Geodata>& data,
						 WayId wid,
						 const Style* s,
						 const cairo_matrix_t* transform)
	: ObjectRenderer(data, s, transform)
	, path(NULL)
	, way(data->getWay(wid))
{
}

WayRenderer::~WayRenderer()
{
	if (path != NULL)
		cairo_path_destroy(path);
}

void WayRenderer::fill(cairo_t* cr, AssetCache& cache)
{
	if (!way->isClosed())
		return;

	addWayPath(cr);

	cairo_save(cr);

	const string& bg = s->fill_image.str();
	if (!bg.empty()) {
		cairo_pattern_t* pattern = cairo_pattern_create_for_surface(cache.getImage(bg));
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source(cr, pattern);
		cairo_fill(cr);
		cairo_pattern_destroy(pattern);
	} else {
		cairo_set_source_rgba(cr, COLOR2RGBA(s->fill_color));
		cairo_fill(cr);
	}

	cairo_restore(cr);
}

void WayRenderer::casing(cairo_t* cr)
{
	if (s->casing_width <= 0.0)
		return;

	addWayPath(cr);

	cairo_save(cr);

	setLineCap(cr,  s->casing_linecap);
	setLineJoin(cr, s->casing_linejoin);
	cairo_set_source_rgba(cr, COLOR2RGBA(s->casing_color));

	if (s->casing_dashes.size() > 0)
		cairo_set_dash(cr, s->casing_dashes.data(), s->casing_dashes.size(), 0.0);
	cairo_set_line_width(cr, s->casing_width*2 + s->width);

	cairo_stroke(cr);

	cairo_restore(cr);
}

void WayRenderer::stroke(cairo_t* cr, AssetCache& cache)
{
	if (s->width <= 0.0)
		return;

	addWayPath(cr);

	cairo_save(cr);

	setLineCap(cr,  s->linecap);
	setLineJoin(cr, s->linejoin);
	cairo_set_source_rgba(cr, COLOR2RGBA(s->color));
	const string& image = s->image.str();
	cairo_pattern_t* pattern = NULL;
	if (!image.empty()) {
		pattern = cairo_pattern_create_for_surface(cache.getImage(image));
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source(cr, pattern);
	}

	if (s->dashes.size() > 0)
		cairo_set_dash(cr, s->dashes.data(), s->dashes.size(), 0.0);
	cairo_set_line_width(cr, s->width);

	cairo_stroke(cr);

	if (pattern != NULL)
		cairo_pattern_destroy(pattern);

	cairo_restore(cr);
}

void WayRenderer::label(cairo_t* cr,
		std::list<shared_ptr<Label> >& labels, AssetCache& cache)
{
	if (s->text.str().size() == 0 || s->font_size <= 0.0)
		return;

	// make sure path is initialized
	addWayPath(cr);
	cairo_new_path(cr);

	cairo_save(cr);

	cairo_set_font_size(cr, s->font_size);

	cairo_select_font_face(cr,
				s->font_family.c_str(),
				s->font_style == Style::STYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
				s->font_weight == Style::WEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
			);

	cairo_text_extents_t textSize;
	cairo_text_extents(cr, s->text.c_str(), &textSize);

	if (s->text_position == Style::POSITION_CENTER)
	{
		// request a centered label
		addLabel(labels, bounds.getCenter() + FloatPoint(0.0, s->text_offset), &textSize);
	}
	else if (s->text_position == Style::POSITION_LINE)
	{
		FloatPoint best;
		double angle = 0;
		bool placed = getTextPosition(path, textSize.width, best, angle);

		if (placed) {
			cairo_translate(cr, best.x, best.y);
			cairo_rotate(cr, angle);
			cairo_translate(cr, 0, s->text_offset);

			cairo_move_to(cr, -textSize.width/2.0 - textSize.x_bearing,
						-textSize.height/2.0 - textSize.y_bearing);

			cairo_text_path(cr, s->text.c_str());

			if (s->text_halo_radius > 0.0)
			{
				cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
				cairo_set_line_width(cr, s->text_halo_radius*2.0);
				cairo_set_source_rgba(cr, COLOR2RGBA(s->text_halo_color));
				cairo_stroke_preserve(cr);
			}
			cairo_set_source_rgba(cr, COLOR2RGBA(s->text_color));
			cairo_fill(cr);
		}
	}

	cairo_restore(cr);
}

void WayRenderer::shield(cairo_t* cr,
		std::list<shared_ptr<Shield> >& shields,
		AssetCache& cache)
{
	if (s->shield_text.str().size() == 0 || s->font_size <= 0.0)
		return;

	// make sure path is initialized
	addWayPath(cr);
	cairo_new_path(cr);

	cairo_save(cr);

	cairo_set_font_size(cr, s->font_size);

	cairo_select_font_face(cr,
				s->font_family.c_str(),
				s->font_style == Style::STYLE_ITALIC ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
				s->font_weight == Style::WEIGHT_BOLD ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
			);

	cairo_text_extents_t textSize;
	cairo_text_extents(cr, s->shield_text.c_str(), &textSize);

	std::list<FloatPoint> positions;
	getShieldPosition(path, positions);

	for (FloatPoint& p : positions)
	{
		addShield(shields, p, &textSize);
	}

	cairo_restore(cr);
}
