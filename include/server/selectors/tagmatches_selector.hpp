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
#ifndef TAGMATCHES_SELECTOR_HPP
#define TAGMATCHES_SELECTOR_HPP
/*
 * =====================================================================================
 *
 *       Filename:  tagmatches_selector.hpp
 *
 *    Description:  selectors of type [tag=~/value/] where value is a regular expression
 *
 * =====================================================================================
 */

#include "settings.hpp"

#include "server/selectors/selector.hpp"
#include <boost/regex.hpp>

class TagMatchesSelector : public Selector {
public:
	TagMatchesSelector(const shared_ptr<Rule>& rule, const shared_ptr<Selector>& next, const string& tag, const string& value);
	virtual void matchNode(NodeId nodeID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchWay(WayId wayID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;
	virtual void matchRelation(RelId relID, const shared_ptr<TileIdentifier>& ti, RenderAttributes* attributes) const;

private:
	const CachedString tag;
	const boost::regex expression;
};

#endif
