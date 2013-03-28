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
#ifndef WAY_HPP
#define WAY_HPP

#include "settings.hpp"

#include "general/geo_object.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
class Way : public GeoObject
{
private:
    friend class boost::serialization::access;
public:
	enum WayType {
		UNCONNECTED     = 0,
		CONNECTED_START = 1,
		CONNECTED_END   = 2,
		CONNECTED_BOTH  = 3,
		CLOSED          = 4
	};

	Way(const std::vector<NodeId>& nodeIDs, const DataMap<CachedString, CachedString>& tags)
	: GeoObject(tags)
	, nodeIDs(nodeIDs)
	, type(WayType::UNCONNECTED)
	{ }

	inline WayType getType() const { return type; }
	inline void    setType(WayType type) { this->type = type; }
	inline const std::vector<NodeId>& getNodeIDs() const { return nodeIDs; }
	inline bool isClosed() const { return type & WayType::CLOSED; }

private:
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		GeoObject::serialize(ar, version);
		ar & nodeIDs;
		ar & type;
	}
	Way(){}

private:
	std::vector<NodeId> nodeIDs;
	WayType type;
};

#endif
