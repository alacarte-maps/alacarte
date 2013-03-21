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
#ifndef TAGLARGER_SELECTOR_HPP
#define TARLARGER_SELECTOR_HPP
/*
 * =====================================================================================
 *
 *       Filename:  taglarger_selector.hpp
 *
 *    Description:  selectors of type [tag>value]
 *
 * =====================================================================================
 */

#include "settings.hpp"

#include "server/selectors/selector.hpp"

class TagLargerSelector : public Selector {
public:
	TagLargerSelector(const shared_ptr<Rule>& rule, const shared_ptr<Selector>& next, const string& tag, const int& value);
	virtual void matchNode(NodeId nodeID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchWay(WayId wayID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchRelation(RelId relID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;

private:
	const CachedString tag;
	const int value;
};

#endif
