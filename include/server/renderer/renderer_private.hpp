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

#include <cairomm/surface.h>
#include <cairomm/context.h>

#define DEBUG(...) (log4cpp::Category::getInstance("Renderer").info(__VA_ARGS__));

#define RENDERER_SHIELD_DISTANCE 128.0
#define RENDERER_SHIELD_OVERLAP 0.1
#define RENDERER_LABEL_OVERLAP 0.1

#define set_source_color(_X) set_source_rgba(_X.r, _X.g, _X.b, _X.a)

class Style;

// there's out-of-box hash for this in boost 1.52 (but not 1.48), so for compatibility
// we use custom hashing function. Also a bit more effecient.
class FontTypeHash {
public:
	size_t operator()(const std::tuple<string, Cairo::FontSlant, Cairo::FontWeight>& v) const {
		return std::hash<string>()(std::get<0>(v)) ^ (int)std::get<1>(v) ^ ((int)std::get<2>(v) << 16);
	}
};

// TODO make thread safe
class AssetCache {
private:
	boost::unordered_map<string, Cairo::RefPtr<Cairo::ImageSurface> > images;
	boost::unordered_map<
				std::tuple<string, Cairo::FontSlant, Cairo::FontWeight>,
				Cairo::RefPtr<Cairo::ToyFontFace>,
				FontTypeHash
			> fonts;

public:
	Cairo::RefPtr<Cairo::ImageSurface> getImage(string path)
	{
		auto it = images.find(path);
		if (it != images.end())
			return (*it).second;

		Cairo::RefPtr<Cairo::ImageSurface> image = Cairo::ImageSurface::create_from_png(path);
		images.insert(std::make_pair(path, image));
		return image;
	}

	Cairo::RefPtr<Cairo::ToyFontFace> getFont(string family = DEFAULT_FONT,
			Cairo::FontSlant slant = Cairo::FONT_SLANT_NORMAL,
			Cairo::FontWeight weight = Cairo::FONT_WEIGHT_NORMAL)
	{
		auto it = fonts.find(std::make_tuple(family, slant, weight));
		if (it != fonts.end())
			return (*it).second;

		Cairo::RefPtr<Cairo::ToyFontFace> font = Cairo::ToyFontFace::create(family, slant, weight);
		fonts.insert(std::make_pair(std::make_tuple(family, slant, weight), font));
		return font;
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
