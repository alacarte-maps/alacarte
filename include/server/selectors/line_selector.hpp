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
 *  Maintainer: Florian Jacob
 */

#pragma once
#ifndef LINE_SELECTOR_HPP
#define LINE_SELECTOR_HPP
/*
 * =====================================================================================
 *
 *       Filename:  line_selector.hpp
 *
 *    Description:  "line" - starting selector selecting only ways with area=no
 *					or where the start node is not the end node
 *
 * =====================================================================================
 */

/*
Line
offen und area ungesetzt:        - Ja
geschlossen und area ungesetzt:  - Nein
offen und area = yes:            - Ja (Fallback wenn invalide Daten da sind)
geschlossen und area = yes:      - Nein
offen und area = no:             - Ja
geschlossen und area = no:       - Ja
*/

#include "settings.hpp"

#include "server/selectors/selector.hpp"

class LineSelector : public Selector {
public:
	LineSelector(const shared_ptr<Rule>& rule, const shared_ptr<Selector>& next);
	virtual void matchNode(NodeId nodeID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchWay(WayId wayID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchRelation(RelId relID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;

};

#endif
