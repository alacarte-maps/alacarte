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


#include "server/eval/eval_helpers.hpp"
#include "server/eval/binary_operation_node.hpp"


namespace eval {


template<typename FloatOperator, typename IntOperation>
string calculation(const string& left, const string& right, FloatOperator fop, IntOperation iop)
{
	float f1, f2;
	int i1, i2;
	if(Conv<int>(left, &i1) && Conv<int>(right, &i2))
	{
		int res = iop(i1, i2);

		return ToString(res);

	}else if(Conv<float>(left, &f1) && Conv<float>(right, &f2))
	{
		float res = fop(f1, f2);
		return ToString(res);
	}

	return "";
}

/**
 * @brief Creates a new binary operation
 *
 *	\param left the left child node (expression)
 *	\param operation The operation to execute
 *	\param right the right child node (expression)
 **/
BinaryOperationNode::BinaryOperationNode(const node_ptr& left, op::BinaryOperationEnum operation, const node_ptr& right)
	: left(left)
	, operation(operation)
	, right(right)
{
}



string BinaryOperationNode::eval(GeoObject* obj) const
{
	string first = left->eval(obj);
	string second = right->eval(obj);

	switch(operation)
	{
	/****************** Checks if both values are equal ******************/
	case op::Equal:
		{
			if(first == second)
			{
				return "true";
			}

			int i1, i2;
			if(		Conv<int>(first, &i1)
				&&	Conv<int>(second, &i2)
				&& i1 == i2)
			{
				return "true";
			}

			return "false";

		}break;

		/****************** Checks if both values are unequal ******************/
	case op::Unequal:
		{

			int i1, i2;
			if(		Conv<int>(first, &i1)
				&&	Conv<int>(second, &i2))
			{
				if(i1 != i2)
				{
					return "true";
				} 
			} else {
				if(first != second)
				{
					return "true";
				}
			}

			return "false";

		}break;

		/****************** Checks if first is less than second ******************/
	case op::Less:
		{
			float f1, f2;
			if(		Conv<float>(first, &f1)
				&&	Conv<float>(second, &f2))
			{
				if(f1 < f2)
				{
					return "true";
				} 
			}
			return "false";
		}

		/****************** Checks if first is lesser than second ******************/
	case op::LessEqual:
		{
			float f1, f2;
			if(		Conv<float>(first, &f1)
				&&	Conv<float>(second, &f2))
			{
				if(f1 <= f2)
				{
					return "true";
				} 
			}
			return "false";
		}
		/****************** Checks if first is greater than second ******************/
	case op::Greater:
		{
			float f1, f2;
			if(		Conv<float>(first, &f1)
				&&	Conv<float>(second, &f2))
			{
				if(f1 > f2)
				{
					return "true";
				} 
			}
			return "false";
		}
		/****************** Checks if first is greater or equal to second ******************/
	case op::GreaterEqual:
		{
			float f1, f2;
			if(		Conv<float>(first, &f1)
				&&	Conv<float>(second, &f2))
			{
				if(f1 >= f2)
				{
					return "true";
				} 
			}
			return "false";
		}

		/****************** Checks if both strings are equal ******************/
	case op::StringEqual:
		{
			if(first == second)
				return "true";
			return "false";
		}break;

		/****************** Checks if both strings are unequal ******************/
	case op::StringUnequal:
		{
			if(first != second)
				return "true";
			return "false";
		}break;

	// 2. lvl
	/****************** Add both values ******************/
	case op::Add:
		return calculation(	first,
							second,
							[](float f1, float f2)
							{
								return f1 + f2;
							},
							[](int i1, int i2)
							{
								return i1 + i2;
							});
	
							
							
	/****************** Substract both values ******************/
	case op::Minus:
		return calculation(	first,
							second,
							[](float f1, float f2)
							{
								return f1 - f2;
							},
								[](int i1, int i2)
							{
								return i1 - i2;
							});

	// 3. lvl
	/****************** Multipies both values ******************/
	case op::Mul:
		return calculation(	first,
							second,
							[](float f1, float f2)
							{
								return f1 * f2;
							},
								[](int i1, int i2)
							{
								return i1 * i2;
							});

	/****************** Devides both values ******************/
	case op::Div:
		{
			// Beware of divide by zero
			int i;
			if(Conv<int>(second, &i) && i == 0)
				return "";

			return calculation(	first,
								second,
								[](float f1, float f2)
								{
									return f1 / f2;
								},
									[](int i1, int i2)
								{
									return i1 / i2;
								});
		}
								

		/****************** Concatinates both strings ******************/
	case op::Concatination:
		{
			return first + second;
		}break;
	}

	assert(!"Invalid operation executed!");
	return "";
}



}
