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
#ifndef _STNODE_HPP
#define _STNODE_HPP


#include "settings.hpp"

class GeoObject;

namespace eval {


/**
 * @brief The interface for nodes in the syntax tree.
 *
 **/
class STNode
{
public:
	//! Shared ptr to this node
	typedef shared_ptr<STNode> node_ptr;

	/**
	 * @brief Evaluates a subtree under this node
	 *
	 *	\param obj which is used to execute the eval tree
	 **/
	inline virtual string eval(GeoObject* obj) const
	{
		return "";
	}
};








}

#endif