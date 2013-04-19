
/*
 * =====================================================================================
 *
 *       Filename:  geodataMock.cpp
 *
 *    Description:  
 *
 * =====================================================================================
 */
#include <boost/archive/binary_iarchive.hpp>
#include <boost/archive/binary_oarchive.hpp>
#include <boost/serialization/base_object.hpp>
#include "geodataMock.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "utils/rect.hpp"
#include <limits>



GeodataMock::GeodataMock()
{
}
GeodataMock::GeodataMock(const Geodata& data) : Geodata(data) 
{}
GeodataMock::~GeodataMock()
{
}

void GeodataMock::insertNodes(const shared_ptr<std::vector<Node> >& nodes)
{
	this->nodes = nodes;
}

void GeodataMock::insertWays(const shared_ptr<std::vector<Way> >& ways)
{
	this->ways = ways;
}

void GeodataMock::insertRelations(const shared_ptr<std::vector<Relation> >& relations)
{	
		this->relations = relations;
}



shared_ptr<std::vector<NodeId> > GeodataMock::getNodeIDs(const FixedRect& rect) const
{
	shared_ptr<std::vector<NodeId> > nodeIDs = boost::make_shared< std::vector<NodeId> >();
 	for(int i = 0; i < nodes->size(); i++) {
 		if(rect.contains(nodes->at(i).getLocation())) {
 			nodeIDs->push_back(NodeId(i));
 		}
 	}
	return nodeIDs;
}

 shared_ptr<std::vector<WayId> > GeodataMock::getWayIDs(const FixedRect& rect) const
 {
 	shared_ptr<std::vector<WayId> > wayIDs = boost::make_shared< std::vector<WayId> >();
 	for(int i = 0; i < ways->size(); i++) {
 		if(rect.intersects(calculateBoundingBox(&ways->at(i)))) {
 				wayIDs->push_back(WayId(i));
 		}
 	}
 	return wayIDs;
 }


 shared_ptr<std::vector<RelId> > GeodataMock::getRelationIDs(const FixedRect& rect) const
 {
	shared_ptr<std::vector<RelId> > relationIDs = boost::make_shared< std::vector<RelId> >();

 	for(int i = 0; i < relations->size(); i++) {
 		if ( rect.intersects ( calculateBoundingBox(&relations->at(i)))) {
			relationIDs->push_back(RelId(i));
 		}
 	}
 	return relationIDs;
 }

Node* GeodataMock::getNode(NodeId id) const
{
	return &nodes->at(id.getRaw());
}

Way* GeodataMock::getWay(WayId id) const
{
	return &ways->at(id.getRaw());
}


FixedRect GeodataMock::calculateBoundingBox(const Way* way) const {

	return calculateBoundingBox(way->getNodeIDs());
}

FixedRect GeodataMock::calculateBoundingBox(const Relation* relation) const {


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

FixedRect GeodataMock::calculateBoundingBox(const std::vector<NodeId>& nodeIDs) const {

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


