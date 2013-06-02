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
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>

#include <limits>

#include "general/geodata.hpp"

#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "general/rtree.hpp"
#include "utils/rect.hpp"
#include "utils/archive.hpp"

Geodata::Geodata()
{
}

Geodata::~Geodata()
{
}

//! called when data is serialized to file
void Geodata::buildTrees(const string& nodePath, const string& wayPath, const string& relationPath)
{
	if (nodes->size() > 0)
	{
		std::vector<FixedPoint> points;;
		for (auto& n : *nodes)
			points.push_back(n.getLocation());

		nodesTree->build(points, nodePath);
	}

	if (ways->size() > 0)
	{
		std::vector<FixedRect> rects;
		for (auto& w : *ways)
			rects.push_back(calculateBoundingBox(w));

		waysTree->build(rects, wayPath);
	}

	if (relations->size() > 0)
	{
		std::vector<FixedRect> rects;
		for (auto& r : *relations)
			rects.push_back(calculateBoundingBox(r));

		relTree->build(rects, relationPath);
	}
}

void Geodata::insertNodes(const shared_ptr<std::vector<Node> >& nodes)
{
	this->nodes = nodes;
	if (nodes->size() > 0)
		this->nodesTree = boost::make_shared<RTree<NodeId, FixedPoint> >();
}

void Geodata::insertWays(const shared_ptr<std::vector<Way> >& ways)
{
	this->ways = ways;
	if (ways->size() > 0)
		this->waysTree = boost::make_shared<RTree<WayId, FixedRect> >();
}

void Geodata::insertRelations(const shared_ptr<std::vector<Relation> >& relations)
{
	this->relations = relations;
	if (relations->size() > 0)
		this->relTree = boost::make_shared<RTree<RelId, FixedRect>>();
}

bool Geodata::containsData(const FixedRect &rect) const
{
	return nodesTree->contains(rect) || waysTree->contains(rect) || relTree->contains(rect);
}

shared_ptr<std::vector<NodeId> > Geodata::getNodeIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<NodeId> > nodeIDs = boost::make_shared< std::vector<NodeId> >();
	if (nodesTree)
		nodesTree->search(nodeIDs, rect);
	return nodeIDs;
}

shared_ptr<std::vector<WayId> > Geodata::getWayIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<WayId> > wayIDs = boost::make_shared< std::vector<WayId> >();
	if (waysTree)
		waysTree->search(wayIDs, rect);
	return wayIDs;
}

shared_ptr<std::vector<RelId> > Geodata::getRelationIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<RelId> > relationIDs = boost::make_shared< std::vector<RelId> >();
	if (relTree)
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

	Archive a(path);
	std::vector<Archive::entry_t> entries;
	a.getEntries(entries);

	int i =  0;
	std::ifstream ifs(path, std::ios::binary | std::ios::in);
	ifs.seekg(entries[i++].offset);
	boost::archive::binary_iarchive ia(ifs);
	ia >> *this;

	// set offsets of leaf inside archive file
	if (nodesTree) {
		nodesTree->setLeafFile(path, entries[i].offset, entries[i].length);
		i++;
	}
	if (waysTree) {
		waysTree->setLeafFile(path, entries[i].offset, entries[i].length);
		i++;
	}
	if (relTree) {
		relTree->setLeafFile(path, entries[i].offset, entries[i].length);
		i++;
	}
}

void Geodata::serialize(const string& serPath) const
{
	log4cpp::Category& log = log4cpp::Category::getInstance("Geodata");
	log.infoStream() << "Serialize to \"" << serPath << "\"";

	std::ofstream ofs(serPath, std::ios::binary | std::ios::out);
	boost::archive::binary_oarchive oa(ofs);
	oa << *this;
}

void Geodata::save(const string& outPath)
{
	boost::filesystem::path out = boost::filesystem::absolute(boost::filesystem::path(outPath));
	boost::filesystem::path base = out.parent_path();
	string serPath = (base / "data.ser").string();
	string nodesPath = (base / "nodes.bin").string();
	string waysPath = (base / "ways.bin").string();
	string relationsPath = (base / "relations.bin").string();

	buildTrees(nodesPath, waysPath, relationsPath);

	serialize(serPath);

	log4cpp::Category& log = log4cpp::Category::getInstance("Geodata");
	log.infoStream() << "Save geodata to \"" << outPath << "\"";
	Archive a(outPath);
	a.addFile(serPath);
	if (nodesTree)
		a.addFile(nodesPath);
	if (waysTree)
		a.addFile(waysPath);
	if (relTree)
		a.addFile(relationsPath);
	a.write();

	// remove temp files
	remove(serPath.c_str());
	if (nodesTree)
		remove(nodesPath.c_str());
	if (waysTree)
		remove(waysPath.c_str());
	if (relTree)
		remove(relationsPath.c_str());
}

FixedRect Geodata::calculateBoundingBox(const Way& way) const
{
	return calculateBoundingBox(way.getNodeIDs());
}

FixedRect Geodata::calculateBoundingBox(const Relation& relation) const
{
	coord_t maxX = std::numeric_limits<coord_t>::min();
	coord_t maxY = std::numeric_limits<coord_t>::min();
	coord_t minX = std::numeric_limits<coord_t>::max();
	coord_t minY = std::numeric_limits<coord_t>::max();

	const std::vector<NodeId>& nodeIDs = relation.getNodeIDs();
	if (nodeIDs.size() > 0)
	{
		FixedRect result = calculateBoundingBox(nodeIDs);
		minX = result.minX;
		minY = result.minY;
		maxX = result.maxX;
		maxY = result.maxY;
	}

	const std::vector<WayId>& wayIDs = relation.getWayIDs();
	if (wayIDs.size() > 0)
	{
		FixedRect result = calculateBoundingBox(wayIDs);
		minX = std::min(minX, result.minX);
		minY = std::min(minY, result.minY);
		maxX = std::max(maxX, result.maxX);
		maxY = std::max(maxY, result.maxY);
	}

	if (maxX < minX || maxY < minY)
		return FixedRect(0, 0, 0, 0);

	return FixedRect(minX, minY, maxX, maxY);
}

FixedRect Geodata::calculateBoundingBox(const std::vector<NodeId>& nodeIDs) const
{
	FixedPoint help;
	coord_t maxX = std::numeric_limits<coord_t>::min();
	coord_t maxY = std::numeric_limits<coord_t>::min();
	coord_t minX = std::numeric_limits<coord_t>::max();
	coord_t minY = std::numeric_limits<coord_t>::max();

	for (NodeId i : nodeIDs) {
		help = nodes->at(i.getRaw()).getLocation();

		maxX = std::max(maxX, help.x);
		maxY = std::max(maxY, help.y);
		minX = std::min(minX, help.x);
		minY = std::min(minY, help.y);
	}

	if (maxX < minX || maxY < minY)
		return FixedRect(0, 0, 0, 0.0);

	return FixedRect(minX, minY, maxX, maxY);
}

FixedRect Geodata::calculateBoundingBox(const std::vector<WayId>& wayIDs) const
{
	coord_t maxX = std::numeric_limits<coord_t>::min();
	coord_t maxY = std::numeric_limits<coord_t>::min();
	coord_t minX = std::numeric_limits<coord_t>::max();
	coord_t minY = std::numeric_limits<coord_t>::max();

	for (WayId i : wayIDs) {
		FixedRect bounds = calculateBoundingBox(ways->at(i.getRaw()));
		minX = std::min(minX, bounds.minX);
		minY = std::min(minY, bounds.minY);
		maxX = std::max(maxX, bounds.maxX);
		maxY = std::max(maxY, bounds.maxY);
	}

	if (maxX < minX || maxY < minY)
		return FixedRect(0, 0, 0, 0.0);

	return FixedRect(minX, minY, maxX, maxY);
}
