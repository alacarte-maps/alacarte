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
 *  Copyright alaCarte 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter0
 *  Maintainer: Patrick Niklaus
 */

#include "includes.hpp"

#include <boost/unordered_map.hpp>

#include "general/geodata.hpp"
#include "general/way.hpp"
#include "general/node.hpp"

#include "server/style.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "object_renderer.hpp"

/**
 * @brief Adds the path of the given way to current path, if path is not set it creates it form the data
 * @param cr the cairo contetx to add the path to
 *
 * @return true if the path is closed
 */
void ObjectRenderer::paintLine(const Cairo::RefPtr<Cairo::Context>& cr,  const std::vector<NodeId>& nodeIDs, bool reverse, bool connect) const
{
	Node* n;
	bool first = true;
	int size = nodeIDs.size();
	for (int i = 0; i < size; i++)
	{
		NodeId nid;
		if (reverse)
			nid = nodeIDs[size - 1 - i];
		else
			nid = nodeIDs[i];
		n = data->getNode(nid);
		const FixedPoint& p = n->getLocation();

		if (first && !connect)
			cr->move_to(p.x, p.y);
		else
			cr->line_to(p.x, p.y);

		first = false;
	}
}

ObjectRenderer::ObjectRenderer(const shared_ptr<Geodata>& data, const Style* s)
	: data(data), s(s)
{
}
