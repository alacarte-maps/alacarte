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

#include "includes.hpp"
/*
 * =====================================================================================
 *
 *       Filename:  geodata.cpp
 *
 *    Description:  
 *
 * =====================================================================================
 */
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>

#include "general/geodata.hpp"

#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "general/nodeKdTree.hpp"
#include "general/rectKdTree.hpp"
#include "utils/rect.hpp"
#include <limits>


Geodata::Geodata()
{
}

Geodata::~Geodata()
{
}

void Geodata::insertNodes(const shared_ptr<std::vector<Node> >& nodes)
{
	this->nodesTree = boost::make_shared<NodeKdTree>();
	nodesTree->buildTree(nodes);
	this->nodes = nodes;
}

void Geodata::insertWays(const shared_ptr<std::vector<Way> >& ways)
{
	this->waysTree = boost::make_shared<RectKdTree<Way> >(ways, this);
	waysTree->buildTree(ways);
	this->ways = ways;
}

void Geodata::insertRelations(const shared_ptr<std::vector<Relation> >& relations)
{	
	this->relTree = boost::make_shared<RectKdTree<Relation> >(relations, this);
	relTree->buildTree(relations);
	this->relations = relations;
}

bool Geodata::containsData(const FixedRect &rect) const
{
	return nodesTree->contains(rect) || waysTree->contains(rect) || relTree->contains(rect);
}

shared_ptr<std::vector<NodeId> > Geodata::getNodeIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<NodeId> > nodeIDs = boost::make_shared< std::vector<NodeId> >();
	nodesTree->search(nodeIDs, rect);
	return nodeIDs;
}

shared_ptr<std::vector<WayId> > Geodata::getWayIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<WayId> > wayIDs = boost::make_shared< std::vector<WayId> >();
	waysTree->search(wayIDs, rect);
	return wayIDs;
}

shared_ptr<std::vector<RelId> > Geodata::getRelationIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<RelId> > relationIDs = boost::make_shared< std::vector<RelId> >();
	relTree->search(relationIDs, rect);
	return relationIDs;
}


Node* Geodata::getNode(NodeId id) const
{
	return &nodes->at(id.getRaw());
}

Way* Geodata::getWay(WayId id) const
{
	return &ways->at(id.getRaw());
}

Relation* Geodata::getRelation(RelId id) const
{
	return &relations->at(id.getRaw());
}

void Geodata::load(const string& path)
{
	log4cpp::Category& log = log4cpp::Category::getInstance("Geodata");
	log.infoStream() << "Load geodata from \"" << path << "\"";

	std::ifstream ifs(path, std::ios::binary | std::ios::in);
	boost::archive::binary_iarchive ia(ifs);

	ia >> *this;
}

void Geodata::save(const string& path) const
{
	log4cpp::Category& log = log4cpp::Category::getInstance("Geodata");
	log.infoStream() << "Save geodata to \"" << path << "\"";
	std::ofstream ofs(path, std::ios::binary | std::ios::out);
	boost::archive::binary_oarchive oa(ofs);
	oa << *this;
}

FixedRect Geodata::calculateBoundingBox(const Way* way) const {

	return calculateBoundingBox(way->getNodeIDs());
}

FixedRect Geodata::calculateBoundingBox(const Relation* relation) const {


	coord_t maxX = std::numeric_limits<coord_t>::min();
	coord_t maxY = std::numeric_limits<coord_t>::min();
	coord_t minX = std::numeric_limits<coord_t>::max();
	coord_t minY = std::numeric_limits<coord_t>::max();

	const std::vector<NodeId>& nodeIDs = relation->getNodeIDs();
	if (nodeIDs.size() > 0)
	{
		FixedRect result = calculateBoundingBox(nodeIDs);
		minX = result.minX;
		minY = result.minY;
		maxX = result.maxX;
		maxY = result.maxY;
	}

	const std::vector<WayId>& wayIDs = relation->getWayIDs();
	for (WayId i : wayIDs) {
		FixedRect bounds = calculateBoundingBox(getWay(i));
		minX = std::min(minX, bounds.minX);
		minY = std::min(minY, bounds.minY);
		maxX = std::max(maxX, bounds.maxX);
		maxY = std::max(maxY, bounds.maxY);
	}

	if (maxX < minX || maxY < minY)
		return FixedRect(0, 0, 0, 0);

	return FixedRect(minX, minY, maxX, maxY);
}

FixedRect Geodata::calculateBoundingBox(const std::vector<NodeId>& nodeIDs) const {

	FixedPoint help;
	coord_t maxX = std::numeric_limits<coord_t>::min();
	coord_t maxY = std::numeric_limits<coord_t>::min();
	coord_t minX = std::numeric_limits<coord_t>::max();
	coord_t minY = std::numeric_limits<coord_t>::max();

	for (NodeId i : nodeIDs) {
		help = getNode(i)->getLocation();

		maxX = std::max(maxX, help.x);
		maxY = std::max(maxY, help.y);
		minX = std::min(minX, help.x);
		minY = std::min(minY, help.y);
	}

	if (maxX < minX || maxY < minY)
		return FixedRect(0, 0, 0, 0.0);

	return FixedRect(minX, minY, maxX, maxY);
}
