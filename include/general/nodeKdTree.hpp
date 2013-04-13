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
#ifndef NODEKDTREE_HPP
#define NODEKDTREE_HPP


#include "settings.hpp"
#include <fstream>
#include <algorithm>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/shared_ptr.hpp>

class Node;
class Way;
class Relation;

class NodeKdTree {
public:
	NodeKdTree(const shared_ptr<std::vector<FixedPoint>>& points) : points(points) {};

	void buildTree ( );
	bool search ( boost::shared_ptr<std::vector<NodeId> >& result, const FixedRect& rect, bool returnOnFirst = false) const;
	bool contains(const FixedRect& rect) const;

protected:
	class kdNode {
		public:
			std::vector<NodeId> ids;
			coord_t key;
			shared_ptr<kdNode> left;
			shared_ptr<kdNode> right;
		private:
			friend class boost::serialization::access;
			template<typename Archive>
				void serialize(Archive &ar, const unsigned int version){
					ar & ids;
					ar & left;
					ar & right;
					ar & key;
				}
	};

	struct SearchStackEntry {
		SearchStackEntry(shared_ptr<kdNode> node, FixedRect rect, int depth)
			: node(node)
			, rect(rect)
			, depth(depth)
		{ };

		FixedRect rect;
		shared_ptr<kdNode> node;
		int depth;
	};

	struct BuildStackEntry {
		BuildStackEntry(shared_ptr<kdNode> node,std::vector<NodeId>&  toInsert, int depth)
			: node(node)
			, toInsert(toInsert)
			, depth(depth)
		{ };

		shared_ptr<kdNode> node;
		std::vector<NodeId>  toInsert;
		int depth;
	};

	shared_ptr<kdNode> root;
	shared_ptr<std::vector<FixedPoint>> points;

private:
	shared_ptr<kdNode> buildKDtree ( std::vector<NodeId>&  toInsert );
	void getSubTree(shared_ptr<std::vector<NodeId> >& result, const shared_ptr<kdNode>& node) const;
	coord_t getMedianX ( std::vector<NodeId> &  points );
	coord_t getMedianY ( std::vector<NodeId> &  points );

private:
	NodeKdTree(){};
	friend class boost::serialization::access;
	template<typename Archive>
		void serialize(Archive &ar, const unsigned int version)
		{
			ar & root;
			ar & points;
		}
};
#endif

