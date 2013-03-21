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
 *  Maintainer: Florian Jacob
 */

#pragma once
#ifndef STYLE_HPP
#define STYLE_HPP

#include "settings.hpp"


class GeoObject;
class Stylesheet;
class StyleTemplate;

/**
 * A Style stores the MapCSS properties for a single Node or Way, or a Relation of type multipolygon.
 */
class Style
{
public:
	enum TextPosition {
		POSITION_LINE = 0,
		POSITION_CENTER,
		POSITION_NULL
	};
	enum LineCap {
		CAP_NONE = 0,
		CAP_ROUND,
		CAP_SQUARE
	};
	enum LineJoin {
		JOIN_MITER = 0,
		JOIN_BEVEL,
		JOIN_ROUND
	};
	enum ShieldShape {
		ROUNDED = 0,
		RECTANGULAR
	};

	/**
	 * @brief Takes all non-"null" (meaning only properties that were explicitely set) properties from the given StyleTemplate
	 * 			and uses them to overwrite the properties of this Style.
	 * @param style the StyleTemplate to take all non-"null" properties from
	 */
	TESTABLE void overmerge(GeoObject* obj, const shared_ptr<StyleTemplate>& style);

	/**
	 * @brief Performs finishing operations on the style, like resolve tags for texts or check for icon path existence.
	 */
	void finish(GeoObject* associatedObject, shared_ptr<const Stylesheet> stylesheet);

	Color color;
	Color fill_color;
	
	// only used for canvas
	MaybeCachedString fill_image;

	double width;
	double casing_width;
	Color casing_color;

	MaybeCachedString text;
	TextPosition text_position;
	Color text_color;
	double font_size;

	// The colour (hex or CSS) of the 'halo' or 'pull-out' used to make the text stand out from features underneath it
	Color text_halo_color;
	double text_halo_radius;

	LineCap linecap;
	LineJoin linejoin;
	LineCap casing_linecap;
	LineJoin casing_linejoin;

	MaybeCachedString icon;
	double icon_width;
	double icon_height;


	Color shield_color;
	// How transparent the shield is, from 0 (transparent) to 1 (opaque)
	double shield_opacity;
	Color shield_frame_color;
	// Width of the frame. If 0, no frame is rendered
	double shield_frame_width;
	Color shield_casing_color;
	double shield_casing_width;
	MaybeCachedString shield_text;
	MaybeCachedString shield_image;
	ShieldShape shield_shape;


	std::vector<double> dashes;
	std::vector<double> casing_dashes;

	int z_index;

	Style();
};

template<typename Stream>
Stream& operator >> (Stream& stream, Style::TextPosition& position)
{
	string expr;
	stream >> expr;

	if(expr == "line")
		position = Style::POSITION_LINE;
	else if(expr == "center")
		position = Style::POSITION_CENTER;

	return stream;
}

template<typename Stream>
Stream& operator >> (Stream& stream, Style::LineCap& cap)
{
	string expr;
	stream >> expr;

	if(expr == "round")
		cap = Style::CAP_ROUND;
	else if(expr == "square")
		cap = Style::CAP_SQUARE;

	return stream;
}

template<typename Stream>
Stream& operator >> (Stream& stream, Style::LineJoin& join)
{
	string expr;
	stream >> expr;

	if(expr == "bevel")
		join = Style::JOIN_BEVEL;
	else if(expr == "miter")
		join = Style::JOIN_MITER;
	else if(expr == "round")
		join = Style::JOIN_ROUND;

	return stream;
}

template<typename Stream>
Stream& operator >> (Stream& stream, Style::ShieldShape& shape)
{
	string expr;
	stream >> expr;

	if(expr == "rounded")
		shape = Style::ROUNDED;
	else if(expr == "rectangular")
		shape = Style::RECTANGULAR;

	return stream;
}

#endif
