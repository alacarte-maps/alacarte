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
**/

#pragma once
#ifndef RELATION_HPP
#define RELATION_HPP


#include "settings.hpp"
#include "general/geo_object.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>
class Relation : public GeoObject
{
private:
	friend class boost::serialization::access;
public:
	Relation() = default;
	Relation(const Relation& other) = default;
	Relation(Relation&& other) = default;
	Relation(	const std::vector<NodeId>& nodeIDs, 
				const DataMap<NodeId, CachedString>& nodeRoles,
				const std::vector<WayId>& wayIDs,
				const DataMap<WayId, CachedString>& wayRoles,
				const DataMap<CachedString, CachedString>& tags);
	virtual ~Relation() = default;
	
	TESTABLE const std::vector<WayId>& getWayIDs() const;
	TESTABLE const std::vector<NodeId>& getNodeIDs() const;    
	TESTABLE const CachedString& getWayRole(WayId id) const;
	TESTABLE const CachedString& getNodeRole(NodeId id) const;
private:
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		GeoObject::serialize(ar, version);
		ar & nodeIDs;
		ar & wayIDs;
		ar & nodeRoles;
		ar & wayRoles;
	}

private:
	std::vector<NodeId> nodeIDs;
	std::vector<WayId> wayIDs;
	DataMap<NodeId, CachedString> nodeRoles;
	DataMap<WayId, CachedString> wayRoles;
};




#endif
