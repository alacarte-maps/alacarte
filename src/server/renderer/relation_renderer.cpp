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



#include <boost/unordered_map.hpp>

#include "general/geodata.hpp"
#include "general/way.hpp"
#include "general/node.hpp"
#include "general/relation.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "relation_renderer.hpp"


RelationRenderer::RelationRenderer(const shared_ptr<Geodata>& data,
								   RelId rid,
								   const Style* s,
								   const cairo_matrix_t* transform)
	: ObjectRenderer(data, s, transform)
	, relation(data->getRelation(rid))
{
}

/**
 * @brief tries to paint a ring starting at ids[idx]. All previous ways are already part of a ring.
 * @param cr the cairo context to add the path to
 * @param ids the ways to search for a ring
 * @param idx the start index in the way array, is increaed to last unused way
 * @param used array of bools to indicate which way is already used
 */
void RelationRenderer::addRingPath(cairo_t* cr, const std::vector<WayId>& ids, int& idx, bool* used)
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

	cairo_close_path(cr);
}

void RelationRenderer::fill(cairo_t* cr, AssetCache& cache)
{
	cairo_new_path(cr);

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
			cairo_new_sub_path(cr);
		}

	delete[] used;

	cairo_push_group(cr);

	cairo_set_fill_rule(cr, CAIRO_FILL_RULE_EVEN_ODD);
	const string& bg = s->fill_image.str();
	if (!bg.empty()) {
		cairo_pattern_t* pattern = cairo_pattern_create_for_surface(cache.getImage(bg));
		cairo_pattern_set_extend(pattern, CAIRO_EXTEND_REPEAT);
		cairo_set_source(cr, pattern);
		cairo_fill(cr);
		cairo_pattern_destroy(pattern);
	} else {
		cairo_set_source_rgba(cr, COLOR2RGBA(s->fill_color));
		cairo_fill(cr);
	}

	cairo_pop_group_to_source(cr);
	cairo_paint(cr);
}

