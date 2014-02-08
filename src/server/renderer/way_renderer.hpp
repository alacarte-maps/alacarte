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

#ifndef WAY_RENDERER_HPP
#define WAY_RENDERER_HPP
/*
 * =====================================================================================
 *
 *       Filename:  way_renderer.hpp
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

class WayRenderer : public ObjectRenderer
{
private:
	cairo_path_t* path;
	Way* way;

	void addWayPath(cairo_t* cr);
	inline void setLineCap(cairo_t* cr, Style::LineCap cap)
	{
		switch(cap) {
			case Style::CAP_NONE:
				cairo_set_line_cap(cr, CAIRO_LINE_CAP_BUTT);
				break;
			case Style::CAP_ROUND:
				cairo_set_line_cap(cr, CAIRO_LINE_CAP_ROUND);
				break;
			case Style::CAP_SQUARE:
				cairo_set_line_cap(cr, CAIRO_LINE_CAP_SQUARE);
				break;
		}
	}
	inline void setLineJoin(cairo_t* cr, Style::LineJoin join)
	{
		switch(join) {
			case Style::JOIN_MITER:
				cairo_set_line_join(cr, CAIRO_LINE_JOIN_MITER);
				break;
			case Style::JOIN_BEVEL:
				cairo_set_line_join(cr, CAIRO_LINE_JOIN_BEVEL);
				break;
			case Style::JOIN_ROUND:
				cairo_set_line_join(cr, CAIRO_LINE_JOIN_ROUND);
				break;
		}
	}
	bool getTextPosition(cairo_path_t* transformedPath, int width, FloatPoint& best, double& angle);
	void getShieldPosition(cairo_path_t* transformedPath, std::list<FloatPoint>& positions);

public:
	WayRenderer(const shared_ptr<Geodata>& data,
				WayId wid,
				const Style* s,
				const cairo_matrix_t* transform);
	virtual ~WayRenderer();

	void fill(cairo_t* cr, AssetCache& cache);
	void casing(cairo_t* cr);
	void stroke(cairo_t* cr, AssetCache& cache);
	void label(cairo_t* cr,
			std::list<shared_ptr<Label> >& labels, AssetCache& cache);
	void shield(cairo_t* cr,
			std::list<shared_ptr<Shield> >& shields,
			AssetCache& cache);
};

#endif
