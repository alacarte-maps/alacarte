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
 *  Maintainer: Lisa Winter
 */



#include "general/relation.hpp"


Relation::Relation(	const std::vector<NodeId>& nodeIDs, 
					const DataMap<NodeId, CachedString>& nodeRoles,
					const std::vector<WayId>& wayIDs,
					const DataMap<WayId, CachedString>& wayRoles,
					const DataMap<CachedString, CachedString>& tags)
	: GeoObject(tags)
	, nodeIDs(nodeIDs)
	, nodeRoles(nodeRoles)
	, wayIDs(wayIDs)
	, wayRoles(wayRoles)
{
}

const std::vector<WayId>& Relation::getWayIDs() const
{
	return wayIDs;
}

const std::vector<NodeId>& Relation::getNodeIDs() const
{
	return nodeIDs;
}

const CachedString& Relation::getWayRole(WayId id) const
{
	assert(wayRoles.find(id) != wayRoles.end());
	return wayRoles.at(id);
}

const CachedString& Relation::getNodeRole(NodeId id) const
{
	assert(nodeRoles.find(id) != nodeRoles.end());
	return nodeRoles.at(id);

}
