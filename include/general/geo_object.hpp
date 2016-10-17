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
#ifndef GEO_OBJECT_HPP
#define GEO_OBJECT_HPP


#include "settings.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/map.hpp>

class GeoObject
{
private:
	friend class boost::serialization::access;
public:
	GeoObject() = default;
	GeoObject(const GeoObject& other) = default;
	GeoObject(GeoObject&& other) = default;
	GeoObject(const DataMap<CachedString, CachedString>& tags);
	virtual ~GeoObject() = default;

	//! Returns a map with key-to-tag-mapping for osm-tags. 
	TESTABLE const DataMap<CachedString, CachedString>& getTags() const;
protected:
	
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & tags;
	}
private:
	DataMap<CachedString, CachedString> tags;
};






#endif // GEO_OBJECT_HPP
