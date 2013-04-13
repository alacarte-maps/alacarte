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
 *  Copyright 2012-2013 Lisa Winter
 *  Copyright 2012-2013 Tobias Kahlert
**/

#pragma once
#ifndef GEODATAMOCK_HPP
#define GEODATAMOCK_HPP

#include "settings.hpp"
#include <fstream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/shared_ptr_132.hpp>
#include "general/geodata.hpp"

class Node;
class Way;
class Relation;
class Geodata;

class GeodataMock : public Geodata						//erbt von Geodata
{
public:
	GeodataMock();
	GeodataMock(const Geodata& data);
	~GeodataMock();

	TESTABLE void insertNodes(const shared_ptr<std::vector<Node> >& nodes);
	TESTABLE void insertWays(const shared_ptr<std::vector<Way> >& ways);
	TESTABLE void insertRelations(const shared_ptr<std::vector<Relation> >& relations);
	TESTABLE shared_ptr<std::vector<NodeId> > getNodeIDs(const FixedRect& rect) const;

	TESTABLE shared_ptr<std::vector<WayId> > getWayIDs(const FixedRect& rect) const;
	TESTABLE shared_ptr<std::vector<RelId> > getRelationIDs(const FixedRect& rect) const;

	TESTABLE Node* getNode(NodeId id) const;
	TESTABLE Way* getWay(WayId id) const ;
	
	TESTABLE FixedRect calculateBoundingBox(const std::vector<NodeId>& nodeIDs) const;
	TESTABLE FixedRect calculateBoundingBox(const Way* way) const;
	TESTABLE FixedRect calculateBoundingBox(const Relation* relation) const;

};

#endif
