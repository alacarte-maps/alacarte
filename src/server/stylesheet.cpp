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



#include "server/stylesheet.hpp"
#include "server/style.hpp"
#include "server/rule.hpp"
#include "server/render_attributes.hpp"
#include "server/tile_identifier.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"

Stylesheet::Stylesheet(const shared_ptr<Geodata>& geodata, const std::vector<shared_ptr<Rule> >& rules, const shared_ptr<StyleTemplate>& canvasStyle)
	: geodata(geodata)
	, rules(rules)
	, canvasStyle(canvasStyle)
{
}


void Stylesheet::match( const shared_ptr<std::vector<NodeId> >& nodeIDs,
						const shared_ptr<std::vector<WayId> >& wayIDs,
						const shared_ptr<std::vector<RelId> >& relIDs,
						const shared_ptr<TileIdentifier>& ti,
						RenderAttributes* styleMap) const
{
	for(auto& rule : rules)
	{
		rule->match(nodeIDs, wayIDs, relIDs, ti, styleMap);
	}

	for(auto& pair : styleMap->getNodeMap())
	{
		pair.second->finish(geodata->getNode(pair.first), shared_from_this());
	}

	for(auto& pair : styleMap->getWayMap())
	{
		pair.second->finish(geodata->getWay(pair.first), shared_from_this());
	}

	for(auto& pair : styleMap->getRelationMap())
	{
		pair.second->finish(geodata->getRelation(pair.first), shared_from_this());
	}

	if(canvasStyle) {
		Style* s = styleMap->getCanvasStyle();
		s->overmerge(nullptr, canvasStyle);
		s->finish(nullptr, shared_from_this());
	}
}

const boost::filesystem::path Stylesheet::getPath() const
{
	return path;
}
