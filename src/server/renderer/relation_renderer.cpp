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

#include "includes.hpp"

#include <boost/unordered_map.hpp>

#include "general/geodata.hpp"
#include "general/way.hpp"
#include "general/node.hpp"
#include "general/relation.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "relation_renderer.hpp"


RelationRenderer::RelationRenderer(const shared_ptr<Geodata>& data, RelId rid, const Style* s)
	: ObjectRenderer(data, s),
	  rid(rid)
{
}

/**
 * @brief tries to paint a ring starting at ids[idx]. All previous ways are already part of a ring.
 * @param cr the cairo context to add the path to
 * @param ids the ways to search for a ring
 * @param idx the start index in the way array, is increaed to last unused way
 * @param used array of bools to indicate which way is already used
 */
void RelationRenderer::addRingPath(const Cairo::RefPtr<Cairo::Context>& cr, const std::vector<WayId>& ids, int& idx, bool* used)
{
	NodeId startNode;
	NodeId lastNode;
	int i = idx;
	bool closed = false;
	do {
		if (used[i]) {
			// skip entrys the next time we search for unused ones
			if (i == idx)
				idx++;
			continue;
		}
		const std::vector<NodeId>& nodeIDs = data->getWay(ids[i])->getNodeIDs();

		bool reverse;
		if (i == idx || nodeIDs.front() == lastNode)
			reverse = false;
		else if (nodeIDs.back() == lastNode)
			reverse = true;
		else
			continue;

		paintLine(cr, nodeIDs, reverse, i != idx);
		used[i] = true;
		if (i == idx)
			startNode = nodeIDs.front();

		if (reverse)
			lastNode = nodeIDs.front();
		else
			lastNode = nodeIDs.back();

		closed = (startNode == lastNode);
		// begin search from start
		i = idx;
	} while (++i < ids.size() && !closed);

	cr->close_path();
}

void RelationRenderer::fill(const Cairo::RefPtr<Cairo::Context>& cr)
{
	Relation* relation = data->getRelation(rid);
	cr->begin_new_path();

	const std::vector<WayId>& ids = relation->getWayIDs();
	int n = ids.size();
	if (n == 0)
		return;

	// default false array
	bool* used = new bool[n]();
	// check if any way is part of a ring
	for (int i = 0; i < n; i++)
		if (!used[i]) {
			addRingPath(cr, ids, i, used);
			cr->begin_new_sub_path();
		}

	delete[] used;

	cr->push_group();
	cr->set_identity_matrix();

	cr->set_fill_rule(Cairo::FILL_RULE_EVEN_ODD);
	cr->set_source_rgba(s->fill_color.r,
						s->fill_color.g,
						s->fill_color.b,
						s->fill_color.a);
	cr->fill();
	cr->pop_group_to_source();
	cr->paint();
}

