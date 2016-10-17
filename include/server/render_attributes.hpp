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
#ifndef RENDER_ATTRIBUTES_HPP
#define RENDER_ATTRIBUTES_HPP

#include "settings.hpp"
#include <boost/unordered_map.hpp>
#include <boost/pool/object_pool.hpp>
#include "server/style.hpp"

/**
 * A RenderAttributes object contains mappings from NodeIDs and WayIDs to Style objects which define how
 * the Node or Way has to be rendered.
 * The exception of the rule that only nodes and ways are rendered are relations of type multipolygon,
 * therefore we still need a relationStyles map.
 */
class RenderAttributes
{
public:
	static const unsigned int ChunkSize = 1024;

	TESTABLE inline const boost::unordered_map<WayId, Style*>& getWayMap() const
	{
		return wayStyles;
	}
	TESTABLE inline const boost::unordered_map<NodeId, Style*>& getNodeMap() const
	{
		return nodeStyles;
	}
	TESTABLE inline const boost::unordered_map<RelId, Style*>& getRelationMap() const
	{
		return relationStyles;
	}
	TESTABLE inline Style* getCanvasStyle()
	{
		return &canvasStyle;
	}

	TESTABLE Style* getNewStyle(NodeId id)	{ return newStyle(id, nodeStyles); }
	TESTABLE Style* getNewStyle(WayId id)	{ return newStyle(id, wayStyles); }
	TESTABLE Style* getNewStyle(RelId id)	{ return newStyle(id, relationStyles); }

private:
	template<typename IdType>
	Style* newStyle(IdType id, boost::unordered_map<IdType, Style*>& map)
	{
		Style* newStyle = stylePool.construct();
		map.insert(std::make_pair(id, newStyle));
		return newStyle;
	}

	boost::unordered_map<NodeId, Style*> nodeStyles;
	boost::unordered_map<WayId, Style*> wayStyles;
	boost::unordered_map<RelId, Style*> relationStyles;

	boost::object_pool<Style> stylePool;

	Style canvasStyle;
};


#endif
