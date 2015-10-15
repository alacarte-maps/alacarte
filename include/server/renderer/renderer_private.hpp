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

#ifndef RENDERER_PRIVATE_HPP
#define RENDERER_PRIVATE_HPP

#include <boost/unordered_map.hpp>

#include <cairo.h>

#define RENDERER_SHIELD_DISTANCE 128.0
#define RENDERER_SHIELD_OVERLAP 0.1
#define RENDERER_LABEL_OVERLAP 0.1

#define COLOR2RGBA(_X) _X.r, _X.g, _X.b, _X.a

class Style;

// TODO make thread safe
class AssetCache {
private:
	boost::unordered_map<string, cairo_surface_t*> images;

public:
	~AssetCache()
	{
		for (auto& pair : images)
			cairo_surface_destroy(pair.second);
	}

	cairo_surface_t* getImage(string path)
	{
		auto it = images.find(path);
		if (it != images.end())
			return (*it).second;

		cairo_surface_t* image = cairo_image_surface_create_from_png(path.c_str());
		images.insert(std::make_pair(path, image));
		return image;
	}
};

//! Stores bounding box and style of a label that needs to be placed
struct Label {
	Label(const FloatRect& box, const FloatRect& owner, const MaybeCachedString& text,
		  const Style* s, const FloatPoint& origin)
		: box(box),
		  owner(owner),
		  text(text),
		  style(s),
		  origin(origin) {}

	//! bounding box in device-space coordinates
	FloatRect box;
	//! bounding box of the object owing the label in device-space coordinates
	FloatRect owner;
	//! Origin of the label
	FloatPoint origin;
	//! Style that is used to paint the label
	const Style* style;
	//! The text that should be used (don't use the style text to make it generic)
	const MaybeCachedString& text;

	//! used by the placement algorith to move the label
	void translate(double dx, double dy)
	{
		box = box.translate(dx, dy);
		origin.x += dx;
		origin.y += dy;
	}
};

struct Shield : public Label {
	Shield(const FloatRect& box, const FloatRect& owner, const MaybeCachedString& text,
		   const Style* s, const FloatPoint& origin, const FloatRect& shield)
		: Label(box, owner, text, s, origin),
		  shield(shield) {}

	//! Dimensions of the shield that should be painted
	FloatRect shield;

	//! used by the placement algorith to move the shield
	void translate(double dx, double dy)
	{
		Label::translate(dx, dy);
		shield = shield.translate(dx, dy);
	}
};

#endif
