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
#ifndef _BINARY_OPERATION_NODE_HPP
#define _BINARY_OPERATION_NODE_HPP



#include "settings.hpp"
#include "server/eval/stnode.hpp"



class GeoObject;

namespace eval {


namespace op {

/**
 * @brief Enumeration for all binary operations
 *
 **/
enum BinaryOperationEnum
{
	// 1. lvl
	Equal,
	Unequal,
	Less,
	LessEqual,
	Greater,
	GreaterEqual,
	StringEqual,
	StringUnequal,

	// 2. lvl
	Add,
	Minus,

	// 3. lvl
	Mul,
	Div,
	Concatination
};
}


/**
 * @brief represents and executes a binary operation in the eval syntax tree
 *
 **/
class BinaryOperationNode
	: public STNode
{
public:
	typedef STNode base_type;
	typedef base_type::node_ptr node_ptr;

	BinaryOperationNode(const node_ptr& left, op::BinaryOperationEnum operation, const node_ptr& right);

	virtual string eval(GeoObject* obj) const;
private:
	//! The left expression of the operator
	node_ptr left;
	//! The type of the operation
	op::BinaryOperationEnum operation;
	//! The right expression of the operator
	node_ptr right;
};


}



#endif
