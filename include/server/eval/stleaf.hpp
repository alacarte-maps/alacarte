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
 *  Maintainer: Tobias Kahlert
 */

#pragma once
#ifndef _STLEAF_HPP
#define _STLEAF_HPP




#include "settings.hpp"
#include "server/eval/stnode.hpp"


namespace eval {

/**
 * @brief Represents a leaf in the eval syntax tree
 *
 *	Every leaf contains a simple string, which will be used in other nodes.
 **/
class STLeaf
	: public STNode
{
public:
	STLeaf(const string& value);

	virtual string eval(GeoObject* obj) const;

private:
	//! The value given back
	const string value;
};





}



#endif