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
 *  Copyright 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter
 *  Maintainer: Florian Jacob
 */

#pragma once
#ifndef STYLESHEET_HPP
#define STYLESHEET_HPP

#include "settings.hpp"
#include <boost/filesystem/path.hpp>

class Geodata;
class RenderAttributes;
class TileIdentifier;
class Rule;
class StyleTemplate;

/**
 * A Stylesheet parses a given MapCSS Stylesheet file and stores the defined rules.
 * Additionally, for a given set of GeoObjects, the Stylesheet generates a RenderAttributes
 * object containing all MapCSS attributes needed to draw the GeoObjects.
 */
class Stylesheet : public boost::enable_shared_from_this<Stylesheet>
{
	friend struct MapCssParser;
public:
	Stylesheet(const shared_ptr<Geodata>& geodata, const std::vector<shared_ptr<Rule> >& rules, const shared_ptr<StyleTemplate>& canvasStyle);


	/**
	 * @brief Parses the MapCSS Stylesheet at the given path and returns a new Stylesheet containing the defined rules.
	 *
	 * @param path the absolute or relative path to the stylesheet
	 * @param data a reference to the geodata object to use to resolve GeoObject IDs to the actual Geoobject
	 */
	static shared_ptr<Stylesheet> Load(const boost::filesystem::path& path, const shared_ptr<Geodata>& geodata, int timeout);

	/**
	 * @brief Returns a new RenderAttributes with a Style object for each given Node and Way.
	 * The Style object contains all MapCSS Attributes from this Stylesheet that apply to it.
	 *
	 * @param styleMap the output parameter to return a new RenderAttributes. Has to be deleted manually.
	 */
	TESTABLE void match(const shared_ptr<std::vector<NodeId> >& nodeIDs,
						const shared_ptr<std::vector<WayId> >& wayIDs,
						const shared_ptr<std::vector<RelId> >& relIDs,
						const shared_ptr<TileIdentifier>& ti,
						RenderAttributes* styleMap) const;

	/**
	 *  @return the path to this stylesheet, including the file name
	 */
	TESTABLE const boost::filesystem::path getPath() const;

private:
	shared_ptr<Geodata> geodata;
	const std::vector<shared_ptr<Rule> > rules;
	shared_ptr<StyleTemplate> canvasStyle;
	boost::filesystem::path path;
};

#endif
