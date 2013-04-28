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


#include <limits>
#include <boost/algorithm/string/replace.hpp>
#include <boost/filesystem/operations.hpp>

#include "server/style.hpp"
#include "general/geo_object.hpp"
#include "server/stylesheet.hpp"
#include "server/style_template.hpp"
#include "server/eval/eval.hpp"

#include "utils/precached_strings.hpp"

#define OVERMERGE_IMPL(_attr)	if (templ-> _attr) {									\
									templ-> _attr ->overwrite(obj, &(this-> _attr ));	\
								}

void Style::overmerge(GeoObject* obj, const shared_ptr<StyleTemplate>& templ)
{
	OVERMERGE_IMPL(color);
	OVERMERGE_IMPL(fill_color);

	OVERMERGE_IMPL(image);
	OVERMERGE_IMPL(fill_image);

	OVERMERGE_IMPL(width);
	OVERMERGE_IMPL(casing_width);
	OVERMERGE_IMPL(casing_color);

	OVERMERGE_IMPL(text);
	OVERMERGE_IMPL(text_position);
	OVERMERGE_IMPL(text_color);
	OVERMERGE_IMPL(text_offset);
	OVERMERGE_IMPL(font_size);
	OVERMERGE_IMPL(font_family);
	OVERMERGE_IMPL(font_weight);
	OVERMERGE_IMPL(font_style);

	OVERMERGE_IMPL(text_halo_color);
	OVERMERGE_IMPL(text_halo_radius);

	OVERMERGE_IMPL(linecap);
	OVERMERGE_IMPL(linejoin);
	OVERMERGE_IMPL(casing_linecap);
	OVERMERGE_IMPL(casing_linejoin);

	OVERMERGE_IMPL(icon_image);
	OVERMERGE_IMPL(icon_width);
	OVERMERGE_IMPL(icon_height);
	OVERMERGE_IMPL(icon_opacity);

	OVERMERGE_IMPL(shield_color);
	OVERMERGE_IMPL(shield_opacity);
	OVERMERGE_IMPL(shield_frame_color);
	OVERMERGE_IMPL(shield_frame_width);
	OVERMERGE_IMPL(shield_casing_color);
	OVERMERGE_IMPL(shield_casing_width);
	OVERMERGE_IMPL(shield_text);
	OVERMERGE_IMPL(shield_image);
	OVERMERGE_IMPL(shield_shape);

	OVERMERGE_IMPL(dashes);
	OVERMERGE_IMPL(casing_dashes);

	OVERMERGE_IMPL(z_index);
}

void Style::finish(GeoObject* associatedObject, shared_ptr<const Stylesheet> stylesheet)
{

	if (this->icon_image.str().size()) {
		// if the icon path is set, prepend the path to the stylesheet directory and check for existence
		this->icon_image = (stylesheet->getPath().parent_path() / boost::filesystem::path(this->icon_image.str())).string();
		if (!boost::filesystem::exists(this->icon_image.str())) {
			// delete non existing icon paths so that every remaining icon path for the renderer is valid
			this->icon_image = "";
		}
	}

	if (this->shield_image.str().size()) {
		// if the shield image path is set, prepend the path to the stylesheet directory and check for existence
		this->shield_image = (stylesheet->getPath().parent_path() / boost::filesystem::path(this->shield_image.str())).string();
		if (!boost::filesystem::exists(this->shield_image.str())) {
			// delete non existing shield image paths so that every remaining path for the renderer is valid
			this->shield_image = "";
		}
	}

	if (this->image.str().size()) {
		// if the image is set, prepend the path to the stylesheet directory and check for existence
		this->image = (stylesheet->getPath().parent_path() / boost::filesystem::path(this->image.str())).string();
		if (!boost::filesystem::exists(this->image.str())) {
			// delete non existing image paths so that every remaining path for the renderer is valid
			this->image = "";
		}
	}

	if (this->fill_image.str().size()) {
		// if the fill image is set, prepend the path to the stylesheet directory and check for existence
		this->fill_image = (stylesheet->getPath().parent_path() / boost::filesystem::path(this->fill_image.str())).string();
		if (!boost::filesystem::exists(this->fill_image.str())) {
			// delete non existing fill image paths so that every remaining path for the renderer is valid
			this->fill_image = "";
		}
	}

	if (this->dashes.size()) {
		bool zero = true;
		bool negative = false;
		for (double d : this->dashes) {
			if (d > 0.0) {
				zero = false;
			} else if (d < 0.0) {
				negative = true;
				break;
			}
		}
		if (zero || negative)
			this->dashes.clear();
	}

	if (this->casing_dashes.size()) {
		bool zero = true;
		bool negative = false;
		for (double d : this->casing_dashes) {
			if (d > 0.0) {
				zero = false;
			} else if (d < 0.0) {
				negative = true;
				break;
			}
		}
		if (zero || negative)
			this->casing_dashes.clear();
	}

	if (!associatedObject)
		return;

	if (this->text.str().size())
	{
		// text is not null, so do a tag lookup and display the tag value
		auto entry = associatedObject->getTags().find(this->text, boost::hash<MaybeCachedString>(), CachedComparator());
		if (entry != associatedObject->getTags().end()) {
			this->text = entry->second.str();
		} else {
			// no tag of that name found. display empty text
			this->text = "";
		}
	}

	if (this->shield_text.str().size())
	{
		// shield_text is not null, so do a tag lookup and display the tag value
		auto entry = associatedObject->getTags().find(this->shield_text, boost::hash<MaybeCachedString>(), CachedComparator());
		if (entry != associatedObject->getTags().end()) {
			this->shield_text = entry->second.str();
		} else {
			// no tag of that name found. display empty text
			this->shield_text = "";
		}
	}

	auto entry = associatedObject->getTags().find(precached_layer);
	if (entry != associatedObject->getTags().end()) {
		std::stringstream strstream(entry->second.str());
		int layer;
		strstream >> layer;

		if (!strstream.bad()) {
			this->z_index += layer * 100;
		}
	}

}


Style::Style()
{
	color = Color(0.0f, 0.0f, 0.0f, 1.0f);

	fill_color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	width = 0.0;
	casing_width = 0.0;
	casing_color = Color(1.0f, 1.0f, 1.0f, 1.0f);

	text_position = Style::TextPosition::POSITION_CENTER;
	text_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
	text_offset = 0.0;
	font_size = 0.0;
	font_family = DEFAULT_FONT;
	font_weight = Style::FontWeight::WEIGHT_NORMAL;
	font_style = Style::FontStyle::STYLE_NORMAL;

	text_halo_color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	text_halo_radius = 0.0;

	linecap = Style::LineCap::CAP_NONE;
	linejoin = Style::LineJoin::JOIN_MITER;
	casing_linecap = Style::LineCap::CAP_NONE;
	casing_linejoin = Style::LineJoin::JOIN_MITER;

	icon_width = -1.0;
	icon_height = -1.0;
	icon_opacity = 1.0;


	shield_color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	float shield_opacity = 0.0f;
	Color shield_frame_color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	float shield_frame_width = 0.0f;
	Color shield_casing_color = Color(1.0f, 1.0f, 1.0f, 0.0f);
	float shield_casing_width = 0.0f;
	ShieldShape shield_shape = Style::ShieldShape::ROUNDED;


	z_index = 0;

}
