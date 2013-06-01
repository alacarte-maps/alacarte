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
 *  Maintainer: Patrick Niklaus
 */

#pragma once
#ifndef RTREE_HPP
#define RTREE_HPP


#include "settings.hpp"

#include <boost/archive/basic_archive.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/iostreams/device/mapped_file.hpp>

#include <fstream>
#include <algorithm>
#include <stack>

#include "utils/transform.hpp"

#define NUM_CHILDREN 50
#define LEAF_SIZE (16 * 1024)

template <class id_t, class data_t>
class RTree {

private:
	// explicit typing to use function overloading
	template <class _id_t, class _data_t,
		size_t leaf_elements = ((LEAF_SIZE - sizeof(uint16_t)) / (sizeof(uint32_t) + sizeof(data_t)))>
	class RLeaf
	{
	public:
		static size_t num_elements() { return leaf_elements; }

		_id_t ids[leaf_elements];
		_data_t data[leaf_elements];
		//! number of contained elements
		uint16_t size;

		RLeaf(): size(0) {}

		void addData(const _data_t& d, _id_t id)
		{
			data[size] = d;
			ids[size] = id;
			size++;
		}

		bool isFull() { return (size == leaf_elements); }
	};

	class RNode
	{
	public:
		FixedRect bounds[NUM_CHILDREN];
		uint32_t children[NUM_CHILDREN];
		//! number of contained elements
		uint8_t size;

		RNode(): size(0) {}

		// for leaf nodes the index is -1 (0xFFFFFFFF)
		void addChild(const FixedRect& bound, uint32_t index = (uint32_t) -1)
		{
			bounds[size] = bound;
			children[size] = index;
			size++;
		}

		bool isFull() const { return (size == NUM_CHILDREN); }
		bool isLeaf() const { return (size > 0 && children[0] == (uint32_t) -1); }

	private:
		friend class boost::serialization::access;
		template<typename Archive>
		void serialize(Archive &ar, const unsigned int version){
			ar & bounds;
			ar & children;
			ar & size;
		}
	};

public:
	RTree () {}
	void build(std::vector<data_t>& data, const string& leafPath);
	bool search ( boost::shared_ptr<std::vector<id_t> >& result,
				  const FixedRect& rect, bool returnOnFirst = false ) const;
	bool contains(const FixedRect& rect) const
	{
		shared_ptr<std::vector<id_t> > geoIDs = boost::make_shared< std::vector<id_t> >();
		return search(geoIDs, rect, true);
	};

	//! is called after deserialisation to set offets in the archive
	void setLeafFile(const string& path, uint64_t offset, uint64_t length)
	{
		input.open(path, length, offset);
		if (!input.is_open())
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(path));
		this->offset = offset;
		this->length = length;
	}

private:
	std::vector<RNode> tree;
	boost::iostreams::mapped_file_source input;
	uint64_t offset;
	uint64_t length;

	void buildLeaves (const std::vector<data_t>& rects, const string& leafPath);
	void writeLeaves (const std::vector<id_t>& ids, const std::vector<data_t>& rects, const string& leafPath);
	void buildLevels ();
	RLeaf<id_t, data_t>* readLeaf (uint32_t nodeIdx, uint32_t childIdx) const;
	void getSubTree ( uint32_t rootIdx, boost::shared_ptr<std::vector<id_t> >& ids ) const;
	FixedRect getBoundingBox ( const FixedRect keys[], size_t size) const;
	FixedRect getBoundingBox ( const FixedPoint keys[], size_t size) const;
	coord_t getX (const FixedPoint& p);
	coord_t getX (const FixedRect& r);
	coord_t getY (const FixedPoint& p);
	coord_t getY (const FixedRect& r);
	bool searchLeaf (const RLeaf<id_t, FixedPoint>* leaf,
					 boost::shared_ptr<std::vector<id_t> >& result,
					 const FixedRect& rect,
					 bool returnOnFirst) const;
	bool searchLeaf (const RLeaf<id_t, FixedRect>* leaf,
					 boost::shared_ptr<std::vector<id_t> >& result,
					 const FixedRect& rect,
					 bool returnOnFirst) const;

	/* debug functions */
	bool validate () const;
	void printTree () const;
	void printLeaves (const string& leafPath) const;

	friend class boost::serialization::access;
	template<typename Archive>
	void load(Archive &ar, const unsigned int version){
		ar >> tree;
	}
	template<typename Archive>
	void save(Archive &ar, const unsigned int version) const
	{
		ar << tree;
	}
	BOOST_SERIALIZATION_SPLIT_MEMBER()
};

//! starts to build the tree for the given data
template<class id_t, class data_t>
void RTree<id_t, data_t>::build(std::vector<data_t>& data, const string& leafPath)
{
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << "Objects: " << data.size();

	// create leaf file and first tree level
	buildLeaves(data, leafPath);

	// build inner tree nodes
	log.infoStream() << " - build levels";
	buildLevels();

	log.infoStream() << " - reverse";
	std::reverse(tree.begin(), tree.end());
	for (RNode& node : tree)
	{
		for (int i = 0; i < NUM_CHILDREN; i++)
		{
			if (node.isLeaf())
				continue;
			node.children[i] = tree.size() - 1 - node.children[i];
		}
	}
}

//! for bounding box based types use the center
template<class id_t, class data_t>
coord_t RTree<id_t, data_t>::getX (const FixedRect& r)
{
	FixedPoint p = r.getCenter();
	return p.x;
}

//! get x coordinate of point based types
template<class id_t, class data_t>
coord_t RTree<id_t, data_t>::getX (const FixedPoint& p)
{
	return p.x;
}

//! for bounding box based types use the center
template<class id_t, class data_t>
coord_t RTree<id_t, data_t>::getY (const FixedRect& r)
{
	FixedPoint p = r.getCenter();
	return p.y;
}

//! get x coordinate of point based types
template<class id_t, class data_t>
coord_t RTree<id_t, data_t>::getY (const FixedPoint& p)
{
	return p.y;
}

template<class id_t, class data_t>
void RTree<id_t, data_t>::buildLeaves (const std::vector<data_t>& data, const string& leafPath)
{
	log4cpp::Category& log = log4cpp::Category::getRoot();

	log.infoStream() << " - computing ids";
	std::vector<id_t> ids;
	for (int i = 0; i < data.size(); i++)
		ids.push_back(id_t(i));

	log.infoStream() << " - sorting leaves";
	std::sort(ids.begin(), ids.end(),
		[&](id_t a, id_t b)
		{
			return (getX(data[a.getRaw()]) < getX(data[b.getRaw()]));
		}
	);

	// number of elements per leaf
	size_t n = RLeaf<id_t, data_t>::num_elements();
	// number of leaves
	size_t p = ceil(data.size() / (double) n);
	// number of slices
	size_t s = ceil(sqrt(p));
	// size of each slice
	size_t t = s * n;

	for (size_t start = 0; start < data.size(); start += t)
	{
		size_t end = start + t;
		if ( end > data.size() )
			end = data.size();

		// sort content of leaf by  y coordinate
		std::sort(ids.begin()+start, ids.begin()+end,
			[&](id_t a, id_t b)
			{
				return (getY(data[a.getRaw()]) < getY(data[b.getRaw()]));
			}
		);
	}

	writeLeaves(ids, data, leafPath);
}

//! Fills first tree layer for leaf RNode objects an saves RLeaf objects to file
template<class id_t, class data_t>
void RTree<id_t, data_t>::writeLeaves (const std::vector<id_t>& ids,
									   const std::vector<data_t>& data,
									   const string& leafPath)
{
	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << " - writing leaves";

	std::ofstream output(leafPath, std::ios::out | std::ios::binary);

	// fill data in leaves and write to disk
	RNode node;
	RLeaf<id_t, data_t> leaf;
	for (id_t id : ids)
	{
		leaf.addData(data[id.getRaw()], id);
		if (leaf.isFull())
		{
			FixedRect bound = getBoundingBox(leaf.data, leaf.size);
			node.addChild(bound);
			if (node.isFull())
			{
				tree.push_back(node);
				node.size = 0;
			}

			output.write((char*) &leaf, sizeof(leaf));
			leaf.size = 0;
		}
	}

	// last leaf was not full
	if (leaf.size > 0)
	{
		FixedRect bound = getBoundingBox(leaf.data, leaf.size);
		node.addChild(bound);
		if (node.isFull())
		{
			tree.push_back(node);
			node.size = 0;
		}

		output.write((char*) &leaf, sizeof(leaf));
	}

	// inner node was not completed
	if (node.size > 0)
		tree.push_back(node);
}

//! builds all levels above leafs
template<class id_t, class data_t>
void RTree<id_t, data_t>::buildLevels ()
{
	size_t start = 0;
	size_t end = tree.size() - 1;
	RNode node;

	uint32_t levels = 1;

	// add nodes until the root is reached
	while (start < end)
	{
		levels++;
		// add bounding boxes of NUM_CHILDREN child nodes to new node
		for (size_t i = start; i <= end; i++)
		{
			FixedRect bound = getBoundingBox(tree[i].bounds, tree[i].size);
			node.addChild(bound, i);
			if (node.isFull())
			{
				tree.push_back(node);
				node.size = 0;
			}
		}

		// node was not completly filled
		if (node.size > 0) {
			tree.push_back(node);
			node.size = 0;
		}

		start = end + 1;
		end = tree.size() - 1;
	}

	log4cpp::Category& log = log4cpp::Category::getRoot();
	log.infoStream() << "  -> " << levels << " levels.";
}

//! reads a leaf from file
template<class id_t, class data_t>
RTree<id_t, data_t>::RLeaf<id_t, data_t>*
RTree<id_t, data_t>::readLeaf (uint32_t nodeIdx, uint32_t childIdx) const
{
	// reverse index, so that it correspons with the id of the saved leaves
	uint32_t leafID = (tree.size() - 1 - nodeIdx) * NUM_CHILDREN + childIdx;
	return (((RLeaf<id_t, data_t>*) input.data()) + leafID);
}

//! debugging function to check validity of bounding boxes
template<class id_t, class data_t>
bool RTree<id_t, data_t>::validate () const
{
	for (int i = 0; i < tree.size(); i++)
	{
		const RNode& node = tree[i];

		if (node.isLeaf())
		{
			RLeaf<id_t, data_t>* leaf;
			for (int c = 0; c < node.size; c++)
			{
				leaf = readLeaf(i, c);
				FixedRect bounds = getBoundingBox(leaf->data, leaf->size);
				if (bounds != node.bounds[c])
					return false;
			}
			continue;
		}

		for (int c = 0; c < node.size; c++)
		{
			uint32_t childIdx = node.children[c];
			FixedRect bounds = getBoundingBox(tree[childIdx].bounds, tree[childIdx].size);
			if (bounds != node.bounds[c])
				return false;
		}
	}

	return true;
}

//! Specialisation for bounding box based data
template<class id_t, class data_t>
bool RTree<id_t, data_t>::searchLeaf (const RLeaf<id_t, FixedRect>* leaf,
									  boost::shared_ptr<std::vector<id_t> >& result,
									  const FixedRect& rect,
									  bool returnOnFirst) const
{
	for (int j = 0; j < leaf->size; j++)
	{
		if (rect.intersects(leaf->data[j]))
		{
			if (returnOnFirst)
				return true;
			result->push_back(leaf->ids[j]);
		}
	}

	return false;
}

//! Specialisation for point based data
template<class id_t, class data_t>
bool RTree<id_t, data_t>::searchLeaf (const RLeaf<id_t, FixedPoint>* leaf,
									  boost::shared_ptr<std::vector<id_t> >& result,
									  const FixedRect& rect,
									  bool returnOnFirst) const
{
	for (int j = 0; j < leaf->size; j++)
	{
		if (rect.contains(leaf->data[j]))
		{
			if (returnOnFirst)
				return true;
			result->push_back(leaf->ids[j]);
		}
	}

	return false;
}

//! if returnOnFirst is set it return true if it found data in the querry rectangle
template<class id_t, class data_t>
bool RTree<id_t, data_t>::search (boost::shared_ptr<std::vector<id_t> >& result,
								  const FixedRect& rect,
								  bool returnOnFirst) const
{
	if (tree.size() == 0)
		return false;

	// start at root
	std::stack<uint32_t> stack;
	stack.push(0);

	RLeaf<id_t, data_t>* leaf;
	do {
		uint32_t idx = stack.top();
		const RNode& node = tree[idx];
		stack.pop();

		// reached leaf check contained rects
		if ( node.isLeaf() )
		{
			// for every child node check bounds
			for (int i = 0; i < node.size; i++)
			{
				// if bound is contained add all ids
				if (rect.contains(node.bounds[i]))
				{
					if (returnOnFirst)
						return true;

					leaf = readLeaf(idx, i);
					result->insert(result->end(), &leaf->ids[0], &leaf->ids[leaf->size]);
				}
				// if bound intersects search in leaf
				else if (rect.intersects(node.bounds[i]))
				{
					leaf = readLeaf(idx, i);
					bool containedData = searchLeaf(leaf, result, rect, returnOnFirst);

					if (returnOnFirst && containedData)
						return true;
				}
			}

			continue;
		}

		// no leaf, check child nodes
		for (int i = 0; i < node.size; i++)
		{
			const FixedRect& bound = node.bounds[i];
			// if bound is contained add sub tree
			if ( rect.contains ( bound ) )
			{
				if (returnOnFirst)
					return true;

				getSubTree ( node.children[i], result);
			}
			// add intersecting nodes to stack to search next
			else if ( rect.intersects( bound ) )
				stack.push( node.children[i] );
		}

	} while (!stack.empty());

	return (result->size() > 0);
}

//! used when a sub-tree is fully contained in the search rectangle
template<class id_t, class data_t>
void RTree<id_t, data_t>::getSubTree ( uint32_t rootIdx, boost::shared_ptr<std::vector<id_t> >& ids ) const
{
	std::stack<uint32_t> subTreeStack;
	subTreeStack.push(rootIdx);

	RLeaf<id_t, data_t>* leaf;
	do {
		uint32_t idx = subTreeStack.top();
		const RNode& node = tree[idx];
		subTreeStack.pop();

		if ( node.isLeaf() )
		{
			for (int i = 0; i < node.size; i++)
			{
				leaf = readLeaf(idx, i);
				ids->insert(ids->end(), &leaf->ids[0], &leaf->ids[leaf->size]);
			}
		}
		else
		{
			for (int i = 0; i < node.size; i++)
				subTreeStack.push(node.children[i]);
		}
	} while (!subTreeStack.empty());
}

//! size muste be greater than 0
template<class id_t, class data_t>
FixedRect RTree<id_t, data_t>::getBoundingBox ( const FixedRect keys[], size_t size) const
{
	FixedRect bound = keys[0];

	for (int i = 1; i < size; i++)
		bound.enclose(keys[i]);

	return bound;
}

//! size muste be greater than 0
template<class id_t, class data_t>
FixedRect RTree<id_t, data_t>::getBoundingBox ( const FixedPoint keys[], size_t size) const
{
	FixedRect bound(keys[0].x, keys[0].y, keys[0].x, keys[0].y);

	for (int i = 1; i < size; i++)
		bound.enclose(keys[i]);

	return bound;
}

//! For debugging
template<class id_t, class data_t>
void RTree<id_t, data_t>::printLeaves (const string& leafPath) const
{
	std::ofstream log(leafPath + ".log", std::ios::out);

	for (int i = tree.size() - 1; i >= 0 && tree[i].isLeaf(); i--)
	{
			log << "L(";
			for (int c = 0; c < tree[i].size; c++)
			{
				FixedRect& r = tree[i].bounds[c];
				log << "R(" << r.minX << ", " << r.minY << ", " << r.maxX << ", " << r.maxY << "), ";
			}
			log << ") ";
	}
}

//! For debugging
template<class id_t, class data_t>
void RTree<id_t, data_t>::printTree ( ) const
{
	size_t start = 0;
	// walk levels
	while (start < tree.size())
	{
		if (tree[start].isLeaf()) {
			RLeaf<id_t, data_t> leaf;
			printf("L(");
			for (int c = 0; c < tree[start].size; c++)
			{
				uint32_t leafID = (tree.size() - 1 - start) * NUM_CHILDREN + c;
				printf(" %i", leafID);
			}
			printf(" ) ");
			start++;
		} else {
			// index of last child is first of next level
			size_t end = tree[start].children[tree[start].size - 1];
			for (int i = start; i < end; i++)
			{
				RNode& node = tree[i];
				printf("(");
				for (int c = 0; c < node.size; c++)
					printf(" %i", node.children[c]);
				printf(" ) ");
			}
			printf("\n");
			start = end;
		}
	}
	printf("\n");
}

#endif

