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
#ifndef _FUNCTION_OPERATION_NODE_HPP
#define _FUNCTION_OPERATION_NODE_HPP


#include "settings.hpp"
#include "stnode.hpp"


class GeoObject;

namespace eval {

namespace op {

//! Enum with all functions
enum FunctionEnum
{
	// Simple (...)
	Nothing,

	// Canonical
	Boolean,
	Str,
	Num,

	// Mathematics
	Sqrt,
	Int,
	Not,
	//Metric,
	//ZMetric,

	// Functions
	Tag,
	Cond,
	Colgen
};

}




/**
 * @brief Represents a node which executes a certain operation on 1 to n subexpressions
 *
 **/
class FunctionOperationNode
	: public STNode
{
public:
	typedef STNode base_type;
	typedef base_type::node_ptr node_ptr;

	FunctionOperationNode(const node_ptr& enclosed);
	FunctionOperationNode(op::FunctionEnum operation, const std::vector<node_ptr>& enclosed);
	
	virtual string eval(GeoObject* obj) const;
private:
	//! The operation type
	op::FunctionEnum operation;
	//! The subexpressions enclosed by this operation
	std::vector<node_ptr> enclosed;
};




}


#endif