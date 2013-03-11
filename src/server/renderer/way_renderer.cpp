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

#include <boost/unordered_map.hpp>

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
void WayRenderer::addWayPath(const Cairo::RefPtr<Cairo::Context>& cr)
{
	cr->begin_new_path();
	if (path != NULL)
		cr->append_path(*path);

	const std::vector<NodeId>& children = way->getNodeIDs();
	paintLine(cr, children);

	path = cr->copy_path();

	double x0, y0, x1, y1;
	cr->get_path_extents(x0, y0, x1, y1);
	bounds = FloatRect(x0, y0, x1, y1);
}

//! Find the best fitting segment on a cairo path and return angle.
bool WayRenderer::getTextPosition(Cairo::Path* transformedPath, int width, FloatPoint& best, double& angle)
{
	cairo_path_t* path = transformedPath->cobj();
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
						angle = -M_PI/2.0;
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
void WayRenderer::getShieldPosition(Cairo::Path* transformedPath, std::list<FloatPoint>& positions)
{
	cairo_path_t* path = transformedPath->cobj();
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
						 const Cairo::Matrix& transform)
	: ObjectRenderer(data, s, transform)
	, path(NULL)
	, way(data->getWay(wid))
{
}

WayRenderer::~WayRenderer()
{
	if (path != NULL)
		delete path;
}

void WayRenderer::fill(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (!way->isClosed())
		return;

	addWayPath(cr);

	cr->save();

	setColor(cr, s->fill_color);

	cr->fill();

	cr->restore();
}

void WayRenderer::casing(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (s->casing_width <= 0.0)
		return;

	addWayPath(cr);

	cr->save();

	setLineCap(cr,  s->casing_linecap);
	setLineJoin(cr, s->casing_linejoin);
	setColor(cr,    s->casing_color);

	if (s->casing_dashes.size() > 0)
		cr->set_dash(s->casing_dashes, 0.0);
	cr->set_line_width(s->casing_width*2 + s->width);

	// override cap (e.g. needed when for bridges on high osm layer)
	if (way->getType() == Way::WayType::CONNECTED_BOTH)
		cr->set_line_cap(Cairo::LINE_CAP_BUTT);

	cr->stroke();

	cr->restore();
}

void WayRenderer::stroke(const Cairo::RefPtr<Cairo::Context>& cr)
{
	if (s->width <= 0.0)
		return;

	addWayPath(cr);

	cr->save();

	setLineCap(cr,  s->linecap);
	setLineJoin(cr, s->linejoin);
	setColor(cr,    s->color);

	if (s->dashes.size() > 0)
		cr->set_dash(s->dashes, 0.0);
	cr->set_line_width(s->width);

	// override cap (e.g. needed when for bridges on higher osm layer)
	if (way->getType() == Way::WayType::CONNECTED_BOTH)
		cr->set_line_cap(Cairo::LINE_CAP_BUTT);

	cr->stroke();

	cr->restore();
}

void WayRenderer::label(const Cairo::RefPtr<Cairo::Context>& cr,
		std::list<shared_ptr<Label> >& labels)
{
	if (s->text.str().size() == 0 || s->font_size <= 0.0)
		return;

	// make sure path is initialized
	addWayPath(cr);
	cr->begin_new_path();

	cr->save();

	cr->set_font_size(s->font_size);

	Cairo::TextExtents textSize;
	cr->get_text_extents(s->text.str(), textSize);

	if (s->text_position == Style::POSITION_CENTER)
	{
		// request a centered label
		addLabel(labels, bounds.getCenter(), textSize);
	}
	else if (s->text_position == Style::POSITION_LINE)
	{
		FloatPoint best;
		double angle = 0;
		bool placed = getTextPosition(path, textSize.width, best, angle);

		if (placed) {
			cr->translate(best.x, best.y);
			cr->rotate(angle);

			cr->move_to(-textSize.width/2.0 - textSize.x_bearing,
						-textSize.height/2.0 - textSize.y_bearing);
			cr->text_path(s->text.str());

			if (s->text_halo_radius > 0.0)
			{
				cr->set_line_join(Cairo::LINE_JOIN_ROUND);
				cr->set_line_width(s->text_halo_radius*2.0);
				setColor(cr, s->text_halo_color);
				cr->stroke_preserve();
			}
			setColor(cr, s->text_color);
			cr->fill();
		}
	}

	cr->restore();
}

void WayRenderer::shield(const Cairo::RefPtr<Cairo::Context>& cr,
		std::list<shared_ptr<Shield> >& shields)
{
	if (s->shield_text.str().size() == 0 || s->font_size <= 0.0)
		return;

	// make sure path is initialized
	addWayPath(cr);
	cr->begin_new_path();

	cr->save();

	cr->set_font_size(s->font_size);

	Cairo::TextExtents textSize;
	cr->get_text_extents(s->shield_text.str(), textSize);

	std::list<FloatPoint> positions;
	getShieldPosition(path, positions);

	for (FloatPoint& p : positions)
	{
		addShield(shields, p, textSize);
	}

	cr->restore();
}
