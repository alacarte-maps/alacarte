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
 *       Filename: nodeKdTree.cpp
 *
 *    Description:  This is the implementation of a 2 demension kd-tree. It functions as
 *    datastructer behind geodata.
 *    As we do not want to delete any content we do not provide a delete function.
 *    Also a tree is only build once and it is not possible to add any new nodes after it has
 *    been constructed.
 *
 * =====================================================================================
 */
#include "general/nodeKdTree.hpp"
#include "general/node.hpp"
#include "utils/rect.hpp"
#include <stack>

#define DEBUG(...) (log4cpp::Category::getInstance("NodeTree").info(__VA_ARGS__));

void NodeKdTree::buildTree(const shared_ptr<std::vector<Node> >& ns)
{
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << "Nodes: " << ns->size();

	log.infoStream() << " - creating leaves";
	std::vector<shared_ptr<kdData> > nodes;
	nodes.reserve(ns->size());
	for (unsigned int i = 0; i < ns->size(); i++ ) {
		boost::shared_ptr<kdData> data = boost::make_shared<kdData>(i, ns->at(i).getLocation());
		nodes.push_back ( data );
	}

	log.infoStream() << " - building tree";
	TIMER_START(building);
	if (nodes.size() > 0)
		root = buildKDtree ( nodes );
	TIMER_STOP(building);
	log.info("Build in: %02i:%02i", (int) TIMER_MIN(building), (int) TIMER_SEC(building) % 60);
}

/**
 * This Methode builds the kd-tree
 * it has an vector which stores all kd Tree nodes
 *
 * left and right refers to the child nodes not sides
 */
shared_ptr<NodeKdTree::kdNode> NodeKdTree::buildKDtree ( std::vector<shared_ptr<kdData> >&  toInsert ) {

	std::stack<BuildStackEntry> buildStack;
	shared_ptr<kdNode> root = boost::make_shared<kdNode>();
	buildStack.push(BuildStackEntry(root, toInsert, 0));

	do {
		BuildStackEntry se = buildStack.top();
		buildStack.pop();

		shared_ptr<kdNode> newNode = se.node;
		std::vector<shared_ptr<kdData>>& nodes = se.toInsert;
		int depth = se.depth;

		if ( nodes.size() <= 1024 ) {
			newNode->ids.reserve(nodes.size());
			newNode->points.reserve(nodes.size());
			for (auto& n : nodes) {
				newNode->ids.push_back(n->id);
				newNode->points.push_back(n->pos);
			}
			continue;
		}

		coord_t median;
		std::vector<shared_ptr<kdData>> leftL;
		std::vector<shared_ptr<kdData>> rightL;
		// split along median
		if ( ( depth % 2 ) == 0 )
		{
			median =  getMedianX ( nodes );
			for (auto& n : nodes)
				if ( n->pos.x <= median )
					leftL.push_back ( n );
				else
					rightL.push_back ( n );
		}
		else
		{
			median =  getMedianY ( nodes );
			for (auto& n : nodes)
				if ( n->pos.y <= median )
					leftL.push_back ( n );
				else
					rightL.push_back ( n );
		}
		newNode->key = median;

		if (leftL.size() > 0) {
			newNode->left = boost::make_shared<kdNode>();
			buildStack.push(BuildStackEntry(newNode->left, leftL,depth+1));
		}

		if (rightL.size() > 0) {
			newNode->right = boost::make_shared<kdNode>();
			buildStack.push(BuildStackEntry(newNode->right, rightL, depth+1));
		}

	} while(!buildStack.empty());

	return root;
}

bool NodeKdTree::search ( boost::shared_ptr<std::vector<NodeId> >& result, const FixedRect& searchRect, bool returnOnFirst ) const
{
	if (!root) return false;

	FixedRect globalRect = FixedRect (
			std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::max(),
			std::numeric_limits<coord_t>::max()
			);

	std::stack< SearchStackEntry> stack;
	stack.push( SearchStackEntry(root, globalRect, 0));

	do {
		SearchStackEntry se = stack.top();
		stack.pop();

		FixedRect leftRect = se.rect;
		FixedRect rightRect = se.rect;
		shared_ptr<kdNode> node = se.node;
		int depth = se.depth;

		// no child nodes
		if (!node->left && !node->right) {
			for (int i = 0; i < node->points.size(); i++) {
				if (searchRect.contains( node->points[i] )) {
					if (returnOnFirst) return true;
					result->push_back( node->ids[i] );
				}
			}
			continue;
		}

		// at least one child node
		if ( ( depth % 2 ) == 0 ) {
			leftRect.maxX  = node->key;
			rightRect.minX = node->key;
		} else {
			leftRect.maxY  = node->key;
			rightRect.minY = node->key;
		}

		if (node->left)
		{
			if (searchRect.contains(leftRect))
			{
				if (returnOnFirst) return true;
				getSubTree(result, node->left);
			}
			else if (searchRect.intersects(leftRect)){
				stack.push( SearchStackEntry(node->left, leftRect, depth+1));
			}
		}
		if (node->right)
		{
			if (searchRect.contains(rightRect))
			{
				if (returnOnFirst) return true;
				getSubTree(result, node->right);
			}
			else if (searchRect.intersects(rightRect)) {
				stack.push( SearchStackEntry(node->right, rightRect, depth+1));
			}
		}
	} while (!stack.empty());

	return returnOnFirst ? false : result->size();
}


void NodeKdTree::getSubTree(shared_ptr<std::vector<NodeId> >& result, const shared_ptr<kdNode>& node) const
{
	std::stack<shared_ptr<kdNode>> nodeSubTreeStack;
	nodeSubTreeStack.push(node);

	do {
		shared_ptr<kdNode> subNode = nodeSubTreeStack.top();
		nodeSubTreeStack.pop();

		// leaf node
		if (!subNode->left && !subNode->right) {
			result->insert(result->end(), subNode->ids.begin(), subNode->ids.end());
			continue;
		}

		if(subNode->left)
			nodeSubTreeStack.push(subNode->left);
		if(subNode->right)
			nodeSubTreeStack.push(subNode->right);
	} while (!nodeSubTreeStack.empty());
}

bool NodeKdTree::contains(const FixedRect& rect) const
{
	shared_ptr<std::vector<NodeId> > nodeIDs = boost::make_shared< std::vector<NodeId> >();
	return search(nodeIDs, rect, true);
}

coord_t NodeKdTree::getMedianX ( std::vector<shared_ptr<kdData > > &  points )
{
	size_t n = points.size() / 2;
	std::nth_element(points.begin(), points.begin()+n, points.end(), &NodeKdTree::operatorSortX);
	return points[n]->pos.x;
}

coord_t NodeKdTree::getMedianY ( std::vector<shared_ptr<kdData > > &  points )
{
	size_t n = points.size() / 2;
	std::nth_element(points.begin(), points.begin()+n, points.end(), &NodeKdTree::operatorSortY);
	return points[n]->pos.y;
}

bool NodeKdTree::operatorSortY ( const shared_ptr<kdData>& a, const shared_ptr<kdData>& b ) {
	return (a->pos.y < b->pos.y);
}
bool NodeKdTree::operatorSortX ( const shared_ptr<kdData>& a, const shared_ptr<kdData>& b ) {
	return (a->pos.x < b->pos.x);
}
