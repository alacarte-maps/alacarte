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

#pragma once
#ifndef GEODATA_HPP
#define GEODATA_HPP


#include "settings.hpp"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/shared_ptr_132.hpp>

class Node;
class Way;
class Relation;
class NodeKdTree;
template<class id_t, class data_t>
class RTree;

class Geodata
{
public:
	Geodata();
	~Geodata();

	TESTABLE void insertNodes(const shared_ptr<std::vector<Node> >& nodes);
	TESTABLE void insertWays(const shared_ptr<std::vector<Way> >& ways);
	TESTABLE void insertRelations(const shared_ptr<std::vector<Relation> >& relations);

	bool containsData(const FixedRect &rect) const;
	TESTABLE shared_ptr<std::vector<NodeId> > getNodeIDs(const FixedRect& rect) const;

	TESTABLE shared_ptr<std::vector<WayId> > getWayIDs(const FixedRect& rect) const;
	TESTABLE shared_ptr<std::vector<RelId> > getRelationIDs(const FixedRect& rect) const;

	TESTABLE Node* getNode(NodeId id) const;
	TESTABLE Way* getWay(WayId id) const ;
	TESTABLE Relation* getRelation(RelId id) const;

	TESTABLE void load(const string& path);
	TESTABLE void save(const string& path);

protected:
	shared_ptr<std::vector<Way> > ways;
	shared_ptr<std::vector<Node> > nodes;
	shared_ptr<std::vector<Relation> > relations;

	//! note the trees are initialized by buildTree on serialisation
	shared_ptr<RTree<NodeId, FixedPoint>> nodesTree;
	shared_ptr<RTree<WayId, FixedRect>> waysTree;
	shared_ptr<RTree<RelId, FixedRect>> relTree;

private:
	void buildTrees(const string& nodePath, const string& wayPath, const string& relationPath);
	void serialize(const string& serPath) const;
	TESTABLE FixedRect calculateBoundingBox(const Way& way) const;
	TESTABLE FixedRect calculateBoundingBox(const Relation& relation) const;
	FixedRect calculateBoundingBox(const std::vector<NodeId>& nodeIDs) const;
	FixedRect calculateBoundingBox(const std::vector<WayId>& nodeIDs) const;

	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & nodes;
		ar & ways;
		ar & relations;
		ar & nodesTree;
		ar & waysTree;
		ar & relTree;
	}

};

#endif
