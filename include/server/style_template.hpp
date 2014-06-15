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
 *  Maintainer: Tobias Kahlert
 */

#pragma once
#ifndef STYLE_TEMPLATE_HPP
#define STYLE_TEMPLATE_HPP

#include "settings.hpp"

#include "style.hpp"

namespace eval {
	template<typename TargetType>
	class Eval;
}

class GeoObject;
class Stylesheet;

/**
 * A StyleTemplate is the "raw" / "unevaluated" form of a Style specified by a single MapCSS rule.
 * When the selector for the rule matches, style.overmerge(styletemplate) is called to merge the new
 * Style into the current interim result. 
 */
class StyleTemplate
{
public:
	StyleTemplate();


	std::shared_ptr< eval::Eval<Color> > color;
	std::shared_ptr< eval::Eval<Color> > fill_color;

	std::shared_ptr< eval::Eval<MaybeCachedString> > image;
	std::shared_ptr< eval::Eval<MaybeCachedString> > fill_image;

	std::shared_ptr< eval::Eval<float> > width;
	std::shared_ptr< eval::Eval<float> > casing_width;
	std::shared_ptr< eval::Eval<Color> > casing_color;

	std::shared_ptr< eval::Eval<MaybeCachedString> > text;
	std::shared_ptr< eval::Eval<Style::TextPosition> > text_position;
	std::shared_ptr< eval::Eval<Color> > text_color;
	std::shared_ptr< eval::Eval<float> > text_offset;
	std::shared_ptr< eval::Eval<float> > font_size;
	std::shared_ptr< eval::Eval<MaybeCachedString> > font_family;
	std::shared_ptr< eval::Eval<Style::FontWeight> > font_weight;
	std::shared_ptr< eval::Eval<Style::FontStyle> > font_style;

	std::shared_ptr< eval::Eval<Color> > text_halo_color;
	std::shared_ptr< eval::Eval<float> > text_halo_radius;

	std::shared_ptr< eval::Eval<MaybeCachedString> > icon_image;
	std::shared_ptr< eval::Eval<float> > icon_width;
	std::shared_ptr< eval::Eval<float> > icon_height;
	std::shared_ptr< eval::Eval<float> > icon_opacity;

	std::shared_ptr< eval::Eval<Color> > shield_color;
	std::shared_ptr< eval::Eval<float> > shield_opacity;
	std::shared_ptr< eval::Eval<Color> > shield_frame_color;
	std::shared_ptr< eval::Eval<float> > shield_frame_width;
	std::shared_ptr< eval::Eval<Color> > shield_casing_color;
	std::shared_ptr< eval::Eval<float> > shield_casing_width;

	std::shared_ptr< eval::Eval<MaybeCachedString> > shield_text;
	std::shared_ptr< eval::Eval<MaybeCachedString> > shield_image;
	std::shared_ptr< eval::Eval<Style::ShieldShape> > shield_shape;

	std::shared_ptr< eval::Eval<int> > z_index;

	std::shared_ptr< eval::Eval<Style::LineCap> > linecap;
	std::shared_ptr< eval::Eval<Style::LineJoin> > linejoin;
	std::shared_ptr< eval::Eval<Style::LineCap> > casing_linecap;
	std::shared_ptr< eval::Eval<Style::LineJoin> > casing_linejoin;

	std::shared_ptr< eval::Eval<std::vector<double> > > dashes;
	std::shared_ptr< eval::Eval<std::vector<double> > > casing_dashes;

};

#endif
