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



#include "utils/random.hpp"

#include "general/geo_object.hpp"

#include "server/parser/color_grammar.hpp"

#include "server/eval/function_operation_node.hpp"
#include "server/eval/eval_helpers.hpp"



namespace eval {

/**
 * @brief Creates a new Function operation which simply does nothing.
 *
 *	Can only have one subexpression
 **/
FunctionOperationNode::FunctionOperationNode(const node_ptr& enclosed)
	: operation(op::Nothing)
{
	this->enclosed.push_back(enclosed);
}

/**
 * @brief Creates a new function operation with a certain operation and any number of subexpressions
 *
 *	\note Even you can pass any number of subexpressions, some operations need a certain number ob expressions!
 *
 *	\param operation executed by this node
 *	\param enclosed expressions evaluated by this node to execute the operation
 *
 **/
FunctionOperationNode::FunctionOperationNode(op::FunctionEnum operation, const std::vector<node_ptr>& enclosed)
	: operation(operation)
	, enclosed(enclosed)
{
	assert(enclosed.size());
}


string FunctionOperationNode::eval(GeoObject* obj) const
{
	string first = enclosed.front()->eval(obj);

	switch(operation)
	{
	case op::Str:
	case op::Nothing:
		return first;

	// Canonical
	case op::Boolean:
		{
			bool b;
			Conv<bool>(first, &b);
			return ToString(b);
		} 
		return first;
	case op::Num:
		{
			float f;
			if(Conv(first, &f))
			{
				return first;
			}
			return "";
		}

	// Mathematics
	case op::Sqrt:
		{
			float f;
			if(Conv(first, &f))
			{
				return ToString(sqrt(f));
			}
			return "";
		}
	case op::Int:
		{
			float f;
			if(Conv(first, &f))
			{
				return ToString((int)floor(f));
			}
			return "";
		}
	case op::Not:
		{
			bool b;
			Conv(first, &b);
			return ToString(!b);
		}
	//case op::Metric:
	//case op::ZMetric:


	case op::Tag:
		{
			MaybeCachedString first_uncached(first);
			auto it = obj->getTags().find(first_uncached, boost::hash<MaybeCachedString>(), CachedComparator());
			if(it != obj->getTags().cend())
			{
				return it->second.str();
			}else{
				return "";
			}
		}
	case op::Cond:
		{
			if(enclosed.size() != 3)
				return "";

			bool condition;
			Conv<bool>(first, &condition);

			if(condition)
			{
				return enclosed[1]->eval(obj);
			}else{
				return enclosed[2]->eval(obj);
			}
		}

	case op::Colgen:
		{
			Color base(0xFFFFFFFF);
			Color top(0xFFFFFFFF);
			if(enclosed.size() > 3)
				return "";

			// 0 = direct set
			// 1 = base multiplication
			// 2 = between set
			int operation = 0;

			if(enclosed.size() >= 2)
			{
				if(Conv(enclosed[1]->eval(obj), &base))
				{
					++operation;

					if(enclosed.size() >= 3)
					{
						if(Conv(enclosed[2]->eval(obj), &top))
							++operation;
					}
				}
			}

			Color result;
			Random rand(boost::hash_value(first));

			Color randColor((uint8)rand.rand(255), (uint8)rand.rand(255), (uint8)rand.rand(255), (uint8)rand.rand(255));



			switch (operation)
			{
			case 0:
				result = randColor;
				result.a =  0.3f;
				break;
			case 1:
				result = base * randColor;
				result.a = base.a;
				break;
			case 2:
				result = base + randColor * (top - base).max(Color(uint32(0x00000000)));
				break;
			default:
				return "";
			}

			std::ostringstream os;
			uint32 col = result;
			os << '#' << std::hex << std::setw(6) << std::setfill('0') << (col & 0xFFFFFF);
			os << std::hex << std::setw(2) << std::setfill('0') << (col >> 24);

			return os.str();
		}
	default:
		assert(false);
		break;
	}

	assert(!"Unknown operation executed");
	return "";
}





}