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



#include "server/selectors/apply_selector.hpp"
#include "server/render_attributes.hpp"
#include "server/style.hpp"
#include "server/rule.hpp"
#include "server/style_template.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "server/tile_identifier.hpp"

ApplySelector::ApplySelector(const shared_ptr<Rule>& rule)
	: Selector(rule, shared_ptr<Selector>())
{
}

void ApplySelector::matchNode(NodeId nodeID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	auto entry = attributes->getNodeMap().find(nodeID);
	Style* style;
	if (entry == attributes->getNodeMap().end()) {
		// add a new Style entry
		style = attributes->getNewStyle(nodeID);
	}else{
		style = entry->second;
	}

	// overmerge with the currently existing style
	style->overmerge(geodata->getNode(nodeID), rule.lock()->getStyleTemplate());
}

void ApplySelector::matchWay(WayId wayID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	auto entry = attributes->getWayMap().find(wayID);
	Style* style;
	if (entry == attributes->getWayMap().end()) {
		// add a new Style entry
		style = attributes->getNewStyle(wayID);
	}else{
		style = entry->second;
	}

	style->overmerge(geodata->getWay(wayID), rule.lock()->getStyleTemplate());
}

// this should only be reached for relations of type multipolygon
void ApplySelector::matchRelation(RelId relID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	auto entry = attributes->getRelationMap().find(relID);
	Style* style;
	if (entry == attributes->getRelationMap().end()) {
		// add a new Style entry
		style = attributes->getNewStyle(relID);
	}else{
		style = entry->second;
	}

	style->overmerge(geodata->getRelation(relID), rule.lock()->getStyleTemplate());
}