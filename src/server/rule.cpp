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



#include "server/rule.hpp"
#include "server/style.hpp"
#include "server/selectors/selector.hpp"
#include "server/selectors/apply_selector.hpp"
#include "general/geodata.hpp"
#include "server/tile_identifier.hpp"
#include "server/style_template.hpp"

#include "config.hpp"

Rule::Rule(const shared_ptr<Geodata>& geodata)
	: geodata(geodata)
	, zoombottom(ALAC_ZOOM_BOTTOM)
	, zoomtop(ALAC_ZOOM_TOP)
	, accepableType(Accept_Any)
{
}

void Rule::match(const shared_ptr<std::vector<NodeId> >& nodeIDs,
				 const shared_ptr<std::vector<WayId> >& wayIDs,
				 const shared_ptr<std::vector<RelId> >& relIDs,
				 const shared_ptr<TileIdentifier>& ti,
				 RenderAttributes* renderAttributes) const
{
	if(zoombottom <= ti->getZoom() && ti->getZoom() <= zoomtop)
	{
		if( accepableType & Accept_Node) {
			for (NodeId nodeID : *nodeIDs) {
				first->matchNode(nodeID, ti, renderAttributes);
			}
		}

		if(accepableType & Accept_Way) {
			for (WayId wayID : *wayIDs) {
				first->matchWay(wayID, ti, renderAttributes);
			}
		}

		if(accepableType & Accept_Relation) {
			for (RelId relID : *relIDs) {
				first->matchRelation(relID, ti, renderAttributes);
			}
		}
	}
}

const shared_ptr<Geodata>& Rule::getGeodata() const {
	return geodata;
}

const shared_ptr<StyleTemplate>& Rule::getStyleTemplate() const {
	return styleTemplate;
}

void Rule::setStyleTemplate(const shared_ptr<StyleTemplate>& styleTemplate) {
	this->styleTemplate = styleTemplate;
}

void Rule::setFirstSelector(const shared_ptr<Selector>& first) {
	this->first = first;
}

void Rule::setZoomBounds(int bottom, int top) {
	zoombottom = bottom;
	zoomtop = top;
}

void Rule::setAcceptableType(AcceptableTypes type) {
	accepableType = type;
}