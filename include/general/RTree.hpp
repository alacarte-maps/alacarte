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

#define DEBUG(...) (log4cpp::Category::getInstance("RTree").info(__VA_ARGS__));

class Node;
class Way;
class Relation;
class Geodata;

template <class id_t >
class RTree {
public:
	RTree ( const shared_ptr<std::vector<FixedRect> >& rects ) : rects(rects) {}

private:
	class RNode {
		public:
			std::vector<id_t> ids;
			FixedRect key;
			shared_ptr<RNode> left;
			shared_ptr<RNode> right;
		private:
			friend class boost::serialization::access;
			template<typename Archive>
				void serialize(Archive &ar, const unsigned int version){
					ar & ids;
					ar & key;
					ar & left;
					ar & right;
				}
	};

	struct SearchStackEntry {
		SearchStackEntry(shared_ptr<RNode> node, FixedRect rect, int depth)
			: node(node)
			  , rect(rect)
			  , depth(depth)
		{ };

		FixedRect rect;
		shared_ptr<RNode> node;
		int depth;
	};

	struct BuildStackEntry {
		BuildStackEntry(shared_ptr<RNode> node, std::vector<id_t>&  toInsert, int depth)
			: node(node)
			  , toInsert(toInsert)
			  , depth(depth)
		{
		};

		shared_ptr<RNode> node;
		std::vector<id_t> toInsert;
		int depth;
	};

public:
	void buildTree();
	bool search ( boost::shared_ptr<std::vector<id_t> >& result,  const FixedRect& rect, bool returnOnFirst = false ) const;
	bool contains(const FixedRect& rect) const
	{
		shared_ptr<std::vector<id_t> > geoIDs = boost::make_shared< std::vector<id_t> >();
		return search(geoIDs, rect, true);
	};

private:
	shared_ptr<std::vector<FixedRect>> rects;
	shared_ptr<RNode> root;

	shared_ptr<RNode> buildRTree ( std::vector<id_t>&  toInsert );
	void getSubTree ( const shared_ptr<RNode>& startN, boost::shared_ptr<std::vector<id_t> >& ids ) const;
	coord_t getMedianX ( std::vector<id_t> & ids, std::vector<FixedPoint> & points);
	coord_t getMedianY ( std::vector<id_t> & ids, std::vector<FixedPoint> & points);
	FixedRect getBoundingBox ( const std::vector<id_t>& ids ) const;

	RTree(){};
	friend class boost::serialization::access;
	template<typename Archive>
		void serialize(Archive &ar, const unsigned int version){
			ar & rects;
			ar & root;
		}
};

template<class id_t>
void RTree<id_t>::buildTree() {
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << "Objects: " << rects->size();

	log.infoStream() << " - creating leaves";
	std::vector<id_t> nodes;
	nodes.reserve(rects->size());
	for (int i = 0; i < rects->size(); i++) {
		nodes.push_back(id_t(i));
	}

	log.infoStream() << " - building tree";
	root = buildRTree ( nodes );
}

template<class id_t>
shared_ptr<typename RTree<id_t>::RNode> RTree<id_t>::buildRTree ( std::vector<id_t>&  toInsert)
{
	shared_ptr<RNode> root = boost::make_shared<RNode>();

	std::stack<BuildStackEntry> buildStack;
	buildStack.push(BuildStackEntry(root, toInsert, 0));

	// precompute center of rectangles for median search
	std::vector<FixedPoint> points;
	points.reserve(rects->size());
	for (auto& r : *rects)
		points.push_back(r.getCenter());

	do {
		BuildStackEntry se = buildStack.top();
		buildStack.pop();

		shared_ptr<RNode> node = se.node;
		std::vector<id_t>& ids = se.toInsert;
		int depth = se.depth;

		node->key = getBoundingBox(ids);

		if ( ids.size() <= 100 )
		{
			node->ids.insert(node->ids.end(), ids.begin(), ids.end());
			continue;
		}

		coord_t median;
		std::vector<id_t> leftL;
		std::vector<id_t> rightL;

		if ( ( depth % 2 ) == 0 )
		{
			median = getMedianX ( ids, points );
			for (auto id : ids)
				if (points[id.getRaw()].x <= median)
					leftL.push_back(id);
				else
					rightL.push_back(id);
		}
		else
		{
			median = getMedianY ( ids, points );
			for (auto id : ids)
				if (points[id.getRaw()].y <= median)
					leftL.push_back(id);
				else
					rightL.push_back(id);
		}

		if (leftL.size() > 0) {
			node->left = boost::make_shared<RNode>();
			buildStack.push(BuildStackEntry(node->left, leftL, depth+1));
		}

		if (rightL.size() > 0) {
			node->right = boost::make_shared<RNode>();
			buildStack.push(BuildStackEntry(node->right, rightL, depth+1));
		}

	} while(!buildStack.empty());

	return root;
}

template<class id_t>
bool RTree<id_t>::search (boost::shared_ptr<std::vector<id_t> >& result,  const FixedRect& rect, bool returnOnFirst) const
{
	if (!root)
		return false;

	std::stack<shared_ptr<RNode>> stack;
	stack.push(root);

	do {
		shared_ptr<RNode> node = stack.top();
		stack.pop();

		// reached leaf check contained rects
		if ( ! ( node->left ) && ! ( node->right ) )
		{
			for (auto id : node->ids)
			{
				if (rects->at(id.getRaw()).intersects(rect))
				{
					if (returnOnFirst) return true;
					result->push_back(id);
				}
			}
			continue;
		}

		if(node->left)
		{
			const FixedRect& leftRect = node->left->key;
			if ( rect.contains ( leftRect ) ) {
				if (returnOnFirst) return true;
				getSubTree ( node->left, result);
			} else if ( rect.intersects ( leftRect )) {
				stack.push(node->left);
			}
		}

		if(node->right)
		{
			const FixedRect& rightRect = node->right->key;
			if ( rect.contains ( rightRect ) ) {
				if (returnOnFirst) return true;
				getSubTree ( node->right, result);
			} else if ( rect.intersects ( rightRect ) ) {
				stack.push(node->right);
			}
		}
	} while (!stack.empty());

	// skip sorting
	if (returnOnFirst)
		return (result->size() > 0);

	return (result->size() > 0);
}

template<class id_t>
void RTree<id_t>::getSubTree ( const shared_ptr<RNode>& startN, boost::shared_ptr<std::vector<id_t> >& ids ) const
{
	std::stack<shared_ptr<RNode>> subTreeStack;
	subTreeStack.push(startN);

	do {
		shared_ptr<RNode> subNode = subTreeStack.top();
		subTreeStack.pop();
		if ( ! ( subNode->left ) && ! ( subNode->right ) ) {
			ids->insert(ids->end(), subNode->ids.begin(), subNode->ids.end());
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

template<class id_t>
FixedRect RTree<id_t>::getBoundingBox ( const std::vector<id_t>& ids ) const
{
	FixedRect bound(
	std::numeric_limits<coord_t>::max(),
	std::numeric_limits<coord_t>::max(),
	std::numeric_limits<coord_t>::min(),
	std::numeric_limits<coord_t>::min());

	for (auto i : ids)
	{
		const FixedRect& r = rects->at(i.getRaw());
		bound.enclose(r);
	}

	return bound;
}

//===========================================Help-Methodes====================================================================


template<class id_t>
coord_t RTree<id_t>::getMedianX ( std::vector<id_t> & ids, std::vector<FixedPoint> & points )
{
	size_t n = ids.size() / 2;
	std::nth_element(ids.begin(), ids.begin()+n, ids.end(),
		[&points](id_t a, id_t b)
		{
			return (points[a.getRaw()].x < points[b.getRaw()].x);
		}
	);
	return points[ids[n].getRaw()].x;
}

template<class id_t>
coord_t RTree<id_t>::getMedianY ( std::vector<id_t> & ids, std::vector<FixedPoint> & points)
{
	size_t n = ids.size() / 2;
	std::nth_element(ids.begin(), ids.begin()+n, ids.end(),
		[&points](id_t a, id_t b)
		{
			return (points[a.getRaw()].y < points[b.getRaw()].y);
		}
	);
	return points[ids[n].getRaw()].y;
}


#endif

