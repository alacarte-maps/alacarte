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
#ifndef RECTKDTREE_HPP
#define RECTKDTREE_HPP


#include "settings.hpp"
#include "geodata.hpp"
#include <fstream>
#include <algorithm>
#include <stack>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>
#include <boost/serialization/shared_ptr_132.hpp>

#include "utils/auto_id_select.hpp"

class Node;
class Way;
class Relation;
class Geodata;

template <class geoO, class geoId >
class RectKdTree {
public:
	RectKdTree ( const shared_ptr<std::vector<geoO> >& ns, const Geodata* geo ) ;

private:
	class kdNode {                            // Defines a kd-tree node
		public:
			std::vector<geoId> ids;
			FixedPoint refPoint;
			shared_ptr<kdNode> left;                            //Defines the left child of a Node
			shared_ptr<kdNode> right;                        //Defines the right child of a Node
		private:
			friend class boost::serialization::access;
			template<typename Archive>
				void serialize(Archive &ar, const unsigned int version){
					ar & ids;
					ar & refPoint;
					ar & left;
					ar & right;
				}
	};

	struct SearchStackEntry {

		SearchStackEntry(shared_ptr<kdNode> node, FixedRect rect, int depth)
			: node(node)
			  , rect(rect)
			  , depth(depth)
		{
		};

		FixedRect rect;
		shared_ptr<kdNode> node;
		int depth;
	};

	struct BuildStackEntry {
		BuildStackEntry(shared_ptr<kdNode> node,std::vector<shared_ptr<kdNode> >&  toInsert, int depth)
			: node(node)
			  , toInsert(toInsert)
			  , depth(depth)
		{
		};

		shared_ptr<kdNode> node;
		std::vector<shared_ptr<kdNode> >  toInsert;
		int depth;
	};

public:
	bool search ( boost::shared_ptr<std::vector<geoId> >& result, const FixedRect& rect, bool returnOnFirst = false) const;
	void buildTree(const shared_ptr<std::vector<geoO> >& ns);
	bool contains(const FixedRect& rect) const
	{
		shared_ptr<std::vector<geoId> > geoIDs = boost::make_shared< std::vector<geoId> >();
		return search(geoIDs, rect, true);
	};

private:
	std::vector<shared_ptr<kdNode> > nodes;
	std::vector<FixedRect> rects;
	shared_ptr<kdNode> root;
	const Geodata* geo;

	shared_ptr<kdNode> buildKDtree ( std::vector<shared_ptr<kdNode> >&  toInsert, int depth );
	bool searchKdTree ( boost::shared_ptr<std::vector<geoId> >& result,  const FixedRect& rect, const FixedRect& current, int depth, bool returnOnFirst = false ) const;
	void getSubTree ( const shared_ptr<kdNode>& startN, boost::shared_ptr<std::vector<geoId> >& geoIds ) const;
	FixedPoint getMedian ( const std::vector<shared_ptr<kdNode > > &  points );
	static bool operatorSortY ( const shared_ptr<kdNode>& a, const shared_ptr<kdNode>& b );
	static bool operatorSortX ( const shared_ptr<kdNode>& a, const shared_ptr<kdNode>& b );
	static bool operatorSortGeoId ( const geoId& a, const geoId& b );
	void insertGeoIds (  const FixedRect& rect, const FixedRect& current, shared_ptr<kdNode>& startN, int depth, int id);
	void insertInSubTree ( shared_ptr<kdNode>& startN, int id, const FixedRect& rect);

	RectKdTree(){};
	friend class boost::serialization::access;
	template<typename Archive>
		void serialize(Archive &ar, const unsigned int version){
			ar & rects;
			ar & nodes;
			ar & root;
		}
};

template<class geoO, class geoId>
RectKdTree<geoO, geoId>::RectKdTree ( const shared_ptr<std::vector<geoO> >& ns, const Geodata* geo) : geo(geo) {
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << "Objects: " << ns->size();

	log.infoStream() << " - creating leaves";
	rects.reserve(ns->size());
	nodes.reserve(ns->size());
	for (unsigned int i = 0; i < ns->size(); i++ ) {
		FixedRect bBox = geo->calculateBoundingBox(&( *ns )[i]);
		rects.push_back(bBox);
		// empty bounding box don't insert
		if (bBox.minX == 0.0 && bBox.maxX == 0.0
				&& bBox.minY == 0.0 && bBox.maxY == 0.0)
		{
			log.warnStream() << "Empty bounding box!";
			continue;
		}
		shared_ptr<kdNode> nodeKDMdl = boost::make_shared<kdNode>();
		nodeKDMdl->refPoint = FixedPoint((bBox.minX+bBox.maxX)/2.0,
				(bBox.minY+bBox.maxY)/2.0);
		nodeKDMdl->left = shared_ptr<kdNode>();
		nodeKDMdl->right = shared_ptr<kdNode>();
		nodes.push_back ( nodeKDMdl );
	}
}

template<class geoO, class geoId>
void RectKdTree<geoO, geoId>::buildTree(const shared_ptr<std::vector<geoO> >& ns) {
	FixedRect current = FixedRect (std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::max(),
			std::numeric_limits<coord_t>::max());
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << " - building tree";
	if (nodes.size() > 0)
	{
		root = buildKDtree ( nodes, 0 );
		for (unsigned int i = 0; i < ns->size(); i++) {
			insertGeoIds(rects[i], current, root, 0, i);
		}
	}
}

/**
 * This Methode builds the kd-tree
 * it has an vector which stores all kd Tree nodes
 *
 * left and right refers to the child nodes not sides
 */
	template<class geoO, class geoId>
shared_ptr<typename RectKdTree<geoO, geoId>::kdNode> RectKdTree<geoO, geoId>::buildKDtree ( std::vector<shared_ptr<kdNode> >&  toInsert, int depth )
{
	shared_ptr<kdNode> root = boost::make_shared<kdNode>();

	std::stack<BuildStackEntry> buildStack;
	buildStack.push(BuildStackEntry(root, toInsert, 0));

	do {
		BuildStackEntry se = buildStack.top();
		buildStack.pop();

		shared_ptr<kdNode> newNode = se.node;
		std::vector<shared_ptr<kdNode>>& nodes = se.toInsert;
		int depth = se.depth;

		if ( nodes.size() <= 100 )
			continue;

		FixedPoint helpP;
		unsigned int count = 0;
		std::vector<shared_ptr<kdNode>> leftL;
		std::vector<shared_ptr<kdNode>> rightL;
		//To get a balaced tree I need to gain the median x-value or y value
		if ( ( depth % 2 ) == 0 ) {
			std::sort ( nodes.begin(), nodes.end(), &RectKdTree::operatorSortX ); // refer to x
			helpP =  getMedian ( nodes );
			while ( count < nodes.size() && nodes[count]->refPoint.x <= helpP.x) {
				leftL.push_back ( nodes[count] );
				count++;
			}
			while ( count < nodes.size() ) {
				rightL.push_back ( nodes[count] );
				count++;
			}
		} else if ( ( depth % 2 ) == 1 ) {				  //refer to y
			std::sort ( nodes.begin(), nodes.end(), &RectKdTree::operatorSortY );
			helpP =  getMedian ( nodes );
			while ( count < nodes.size() && nodes[count]->refPoint.y <= helpP.y ) {
				leftL.push_back ( nodes[count] );
				count++;
			}
			while ( count < nodes.size() ) {
				rightL.push_back ( nodes[count] );
				count++;
			}
		}

		newNode->refPoint = helpP;
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



/**
 *Is the method which actually inserts the Objects
 */
template<class geoO, class geoId>
void RectKdTree<geoO, geoId>::insertGeoIds ( const FixedRect& rect, const FixedRect& current, shared_ptr<kdNode>& startN, int depth, int id) {
	FixedRect leftRect = current;
	FixedRect rightRect = current;


	std::stack<SearchStackEntry> insertGstack;
	insertGstack.push(SearchStackEntry(startN, current,0));

	do {
		SearchStackEntry se = insertGstack.top();
		insertGstack.pop();

		FixedRect leftRect = se.rect;
		FixedRect rightRect = se.rect;
		startN = se.node;
		int depth = se.depth;

		// we reached a leaf
		if ( ! ( startN->left ) && ! ( startN->right ) ) {
			startN->ids.push_back(geoId(id));
			return;
		}

		if ( ( depth % 2 ) == 0 ) {
			leftRect.maxX = startN->refPoint.x;
			rightRect.minX = startN->refPoint.x;
		} else {
			leftRect.maxY = startN->refPoint.y;
			rightRect.minY = startN->refPoint.y;
		}

		if(startN->left) {
			if ( rect.contains ( leftRect ) ) {
				insertInSubTree ( startN, id, rect);
			} else if ( rect.intersects ( leftRect )) {
				insertGstack.push(SearchStackEntry(startN->left, leftRect,depth+1));
			}
		}

		if(startN->right) {
			if ( rect.contains ( rightRect ) ) {
				insertInSubTree ( startN, id, rect);
			} else if ( rect.intersects ( rightRect ) ) {
				insertGstack.push(SearchStackEntry(startN->right, rightRect, depth+1));
			}
		}
	}while(!insertGstack.empty());
}

template<class geoO, class geoId>
void RectKdTree<geoO, geoId>::insertInSubTree ( shared_ptr<kdNode>& startN, int id, const FixedRect& rect) {

	std::stack<SearchStackEntry> insertsubT;
	insertsubT.push(SearchStackEntry(startN, rect, 0));

	do {
		SearchStackEntry se = insertsubT.top();
		insertsubT.pop();

		FixedRect rect = se.rect;
		startN = se.node;

		if ( ! ( startN->left ) && ! ( startN->right ) ) {
			startN->ids.push_back(geoId(id));
			return;
		}
		if ( startN->left )
			insertsubT.push(SearchStackEntry(startN->left, rect, 0));

		if ( startN->right )
			insertsubT.push(SearchStackEntry(startN->right, rect, 0));

	} while(!insertsubT.empty());
}

//==================================search methods=========================================

/**
 * public method which starts search
 */
template<class geoO, class geoId>
bool RectKdTree<geoO, geoId>::search ( shared_ptr<std::vector<geoId> >& result, const FixedRect& rect, bool returnOnFirst) const
{
	if (!root)
		return false;
	FixedRect current = FixedRect (std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::min(),
			std::numeric_limits<coord_t>::max(),
			std::numeric_limits<coord_t>::max());
	bool containsData = searchKdTree ( result, rect, current, 0, returnOnFirst);
	if (returnOnFirst) {
		return containsData;
	}
	std::sort ( result->begin(), result->end(), &RectKdTree::operatorSortGeoId);
	result->erase( std::unique(result->begin(), result->end()), result->end());
	return result->size();
}

/**
 * Methode finds all gemetric point which collide with a search rectangle
 */
template<class geoO, class geoId>
bool RectKdTree<geoO, geoId>::searchKdTree (boost::shared_ptr<std::vector<geoId> >& result,  const FixedRect& rect, const FixedRect& current, int depth, bool returnOnFirst) const
{
	FixedRect leftRect = current;
	FixedRect rightRect = current;

	std::stack<SearchStackEntry> stack;
	stack.push(SearchStackEntry(root, current, 0));

	do {
		SearchStackEntry se = stack.top();
		stack.pop();

		FixedRect leftRect = se.rect;
		FixedRect rightRect = se.rect;
		shared_ptr<kdNode> startN = se.node;
		int depth = se.depth;

		if ( ! ( startN->left ) && ! ( startN->right ) ) {
			for (unsigned int i = 0; i < startN->ids.size(); i++)
				if (rects[startN->ids[i].getRaw()].intersects(rect)) {
					if (returnOnFirst) return true;
					result->push_back(startN->ids[i]);
				}
		} else {
			if ( ( depth % 2 ) == 0 ) {
				leftRect.maxX = startN->refPoint.x;
				rightRect.minX = startN->refPoint.x;
			} else {
				leftRect.maxY = startN->refPoint.y;
				rightRect.minY = startN->refPoint.y;
			}

			if(startN->left) {
				if ( rect.contains ( leftRect ) ) {
					if (returnOnFirst) return true;
					getSubTree ( startN->left, result);
				} else if ( rect.intersects ( leftRect )) {
					stack.push(SearchStackEntry(startN->left, leftRect, depth+1));
				}
			}

			if(startN->right) {
				if ( rect.contains ( rightRect ) ) {
					if (returnOnFirst) return true;
					getSubTree ( startN->right, result);
				} else if ( rect.intersects ( rightRect ) ) {
					stack.push(SearchStackEntry(startN->right, rightRect, depth+1));
				}
			}
		}
	} while (!stack.empty());

	return returnOnFirst ? false : result->size();
}



/**
 * Methode gets all nodes of a subtree
 */
template<class geoO, class geoId>
void RectKdTree<geoO, geoId>::getSubTree ( const shared_ptr<kdNode>& startN, boost::shared_ptr<std::vector<geoId> >& id ) const
{
	std::stack<shared_ptr<kdNode>> subTreeStack;
	subTreeStack.push(startN);

	do {
		shared_ptr<kdNode> subNode = subTreeStack.top();
		subTreeStack.pop();
		if ( ! ( subNode->left ) && ! ( subNode->right ) ) {
			id->insert(id->begin(), subNode->ids.begin(), subNode->ids.end());
		} else {
			if ( subNode->left != nullptr ) {
				subTreeStack.push(subNode->left);
			}
			if ( subNode->right != nullptr ) {
				subTreeStack.push(subNode->right);
			}
		}
	} while (!subTreeStack.empty());
}

//===========================================Help-Methodes====================================================================

/**
 * This Methode finds the Point of a vector of Points, which is best to split in Order to get a balaced tree
 */
template<class geoO, class geoId>
FixedPoint RectKdTree<geoO, geoId>::getMedian ( const std::vector<shared_ptr<kdNode > > &  points ) {
	FixedPoint helpP;
	if (points.size() < 2)
		return points[0]->refPoint;

	if ( points.size() %2 == 0 ) {   // even number of values
		helpP.x = ( points[points.size() / 2 - 1]->refPoint.x + points[ points.size() /2]->refPoint.x ) /2;			 // takes the Point which is in the Middle of the vector  zB: a,b,c -> b
		helpP.y = ( points[points.size() / 2 - 1]->refPoint.y + points[ points.size() /2]->refPoint.y ) /2;
	} else if ( points.size() %2 == 1 ) { // uneven number of values	// take the Point right of the middle zB: length: 4  a,b,c,d  4/2  -1 = 1  -> take second element of vector -> b
		helpP.x  = points[ ( points.size() - 1 ) /2 ]->refPoint.x;
		helpP.y  = points[ ( points.size() - 1 ) /2 ]->refPoint.y;
	}

	return helpP;

}

//=================================Sort Operateor methods ================================
template<class geoO, class geoId>
bool RectKdTree<geoO, geoId>::operatorSortY ( const shared_ptr<kdNode>& a, const shared_ptr<kdNode>& b ) {
	return (a->refPoint.y < b->refPoint.y);
}

template<class geoO, class geoId>
bool RectKdTree<geoO, geoId>::operatorSortX ( const shared_ptr<kdNode>& a, const shared_ptr<kdNode>& b ) {
	return (a->refPoint.x < b->refPoint.x);
}

template<class geoO, class geoId>
bool RectKdTree<geoO, geoId>::operatorSortGeoId ( const geoId& a, const geoId& b ) {
	return (a < b);
}



#endif

