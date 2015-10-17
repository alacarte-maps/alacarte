/**
 *  This file is part of alaCarte.
 *
 *  alaCarte is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.

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


#include "server/eval/unary_operation_node.hpp"



namespace eval {


UnaryOperationNode::UnaryOperationNode(op::UnaryOperationEnum operation, const node_ptr& enclosed)
	: operation(operation)
	, enclosed(enclosed)
{

}

/**
 * This method does nothing for now as UnaryOperations are not used in parser
 */
string UnaryOperationNode::eval(GeoObject* obj) const
{
	assert(false);
	// This is needed because of MSVC
	return "";
}





}
