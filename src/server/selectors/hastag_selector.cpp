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



#include "server/selectors/hastag_selector.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "server/rule.hpp"
#include "server/tile_identifier.hpp"


HasTagSelector::HasTagSelector(const shared_ptr<Rule>& rule, const shared_ptr<Selector>& next, const string& tag)
	: Selector(rule, next)
	, tag(tag)
{
}

void HasTagSelector::matchNode(NodeId nodeID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	Node* node = geodata->getNode(nodeID);
	auto& map = node->getTags();
	if (map.find(tag) != map.end()) {
		next->matchNode(nodeID, ti, attributes);
	}
}

void HasTagSelector::matchWay(WayId wayID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	Way* way = geodata->getWay(wayID);
	auto& map = way->getTags();
	if (map.find(tag) != map.end()) {
		next->matchWay(wayID, ti, attributes);
	}
}

void HasTagSelector::matchRelation(RelId relID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const {
	Relation* relation = geodata->getRelation(relID);

	auto& map = relation->getTags();
	if (map.find(tag) != map.end()) {
		next->matchRelation(relID, ti, attributes);
	}
}