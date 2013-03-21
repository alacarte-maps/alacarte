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
#ifndef RULE_HPP
#define RULE_HPP

#include "settings.hpp"
class Selector;
class Geodata;
class Style;
class RenderAttributes;
class TileIdentifier;
class ApplySelector;
class StyleTemplate;

class Rule {
	friend struct MapCssParser;

public:
	enum AcceptableTypes
	{
		Accept_Node		= 0x1,
		Accept_Way		= 0x2,
		Accept_Relation	= 0x4,
		Accept_Any		= Accept_Node | Accept_Way | Accept_Relation,
	};

private:
	const shared_ptr<Geodata> geodata;
	shared_ptr<Selector> first;
	shared_ptr<StyleTemplate> styleTemplate;
	int zoombottom, zoomtop;
	AcceptableTypes accepableType;

public:
	Rule(const shared_ptr<Geodata>& geodata);

	/**
	 * @brief
	 */
	TESTABLE void match(const shared_ptr<std::vector<NodeId> >& nodeIDs,
						const shared_ptr<std::vector<WayId> >& wayIDs,
						const shared_ptr<std::vector<RelId> >& relIDs,
						const shared_ptr<TileIdentifier>& ti,
						RenderAttributes* renderAttributes) const;

	TESTABLE const shared_ptr<Geodata>& getGeodata() const;

	/**
	 * @return the StyleTemplate representation of the attributes specified in the MapCSS file for this rule.
	 */
	TESTABLE const shared_ptr<StyleTemplate>& getStyleTemplate() const;

	/**
	 * @brief sets the StyleTemplate representation of the attributes specified in the MapCSS file for this rule.
	 */
	TESTABLE void setStyleTemplate(const shared_ptr<StyleTemplate>& styleTemplate);

	/**
	 * @brief what is the first selector in the selector chain for this rule?
	 */
	TESTABLE void setFirstSelector(const shared_ptr<Selector>& first);

	/**
	 * @brief from which to which zoom level does this rule apply?
	 */
	TESTABLE void setZoomBounds(int bottom, int top);

	/**
	 * @brief what types of GeoObjects does this rule accept in the first selector?
	 */
	TESTABLE void setAcceptableType(AcceptableTypes type);
};

#endif
