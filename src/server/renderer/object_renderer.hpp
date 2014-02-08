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

#ifndef OBJECT_RENDERER_HPP
#define OBJECT_RENDERER_HPP
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

class Geodata;
class Label;
class Shield;

class ObjectRenderer
{
protected:
	const shared_ptr<Geodata>& data;
	const cairo_matrix_t* transform;
	const Style* s;
	//! is set by addWayPath for ways or in transformLocation for nodes
	FloatRect bounds;

	void paintLine(cairo_t* cr,  const std::vector<NodeId>& nodeIDs, bool reverse=false, bool connect=false) const;
	void addShield(std::list<shared_ptr<Shield> >& shields,
				   const FloatPoint& p,
				   const cairo_text_extents_t* textSize) const;
	void addLabel(std::list<shared_ptr<Label> >& labels,
				   const FloatPoint& p,
				   const cairo_text_extents_t* textSize) const;

public:
	ObjectRenderer(const shared_ptr<Geodata>& data,
				   const Style* s,
				   const cairo_matrix_t* transform);

};

#endif
