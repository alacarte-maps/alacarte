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



#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>


#include "server/eval/binary_operation_node.hpp"
#include "server/eval/function_operation_node.hpp"
#include "server/parser/eval_grammar.hpp"
#include "server/eval/eval.hpp"
#include "server/eval/stleaf.hpp"


namespace phx = boost::phoenix;


namespace eval {


//! Symbol parser for level 1 operations
struct Lvl1Operator : public qi::symbols<char, op::BinaryOperationEnum>
{
	Lvl1Operator()
	{
		this->add
			("==", op::Equal)
			("!=", op::Unequal)
			("<", op::Less)
			("<=", op::LessEqual)
			(">", op::Greater)
			(">=", op::GreaterEqual)
			("eq", op::StringEqual)
			("ne", op::StringUnequal);
	}
} Lvl1OpSymbols;

//! Symbol parser for level 2 operations
struct Lvl2Operator : public qi::symbols<char, op::BinaryOperationEnum>
{
	Lvl2Operator()
	{
		this->add
			("+", op::Add)
			("-", op::Minus);
	}
} Lvl2OpSymbols;

//! Symbol parser for level 3 operations
struct Lvl3Operator : public qi::symbols<char, op::BinaryOperationEnum>
{
	Lvl3Operator()
	{
		this->add
			("*", op::Mul);
	}
} Lvl3OpSymbols;

//! Symbol parser for level 4 operations
struct Lvl4Operator : public qi::symbols<char, op::BinaryOperationEnum>
{
	Lvl4Operator()
	{
		this->add
			("/", op::Div)
			(".", op::Concatination);
	}
} Lvl4OpSymbols;

//! Symbol parser for the function operations
struct FunctionOperators : public qi::symbols<char, op::FunctionEnum>
{
	FunctionOperators()
	{
		this->add
			// Canonical
			("boolean", op::Boolean)
			("str",		op::Str)
			("num",		op::Num)
			// Mathematics
			("sqrt",	op::Sqrt)
			("int",		op::Int)
			("not",		op::Not)
			//Functions
			("tag",		op::Tag)
			("cond",	op::Cond)
			("colgen",	op::Colgen);
	}
} FuncOpSymbols;





/**
 * @brief Creates an eval grammar.
 *
 **/
EvalGrammer::EvalGrammer()
	: EvalGrammer::base_type(rule_lvl1_expr, "eval-MainGrammar")
	//, start("test-MainRule")
{
	using namespace qi::labels;
	using boost::phoenix::new_;
	using boost::phoenix::construct;
	using boost::phoenix::val;
	auto& _1 = qi::_1;
	auto& _2 = qi::_2;
	auto& _3 = qi::_3;

	rule_string = (('\'' > *(qi::char_ - '\'') > '\'') | +qi::char_("a-zA-Z_0-9#"));
	
	rule_func_expr =	( FuncOpSymbols >> '(' >> (rule_lvl1_expr % ',') >> ')' )
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<FunctionOperationNode>(_1, _2))
							]
						| ( '(' >> rule_lvl1_expr >> ')')
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<FunctionOperationNode>(_1))
							];

	rule_lvl4_expr =	(rule_func_expr >> Lvl4OpSymbols >> rule_lvl4_expr)
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<BinaryOperationNode>(_1, _2, _3))
							]
						| rule_func_expr[_val = _1]
						| (rule_string >> Lvl4OpSymbols >> rule_lvl4_expr)
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<BinaryOperationNode>(phx::construct<STNode::node_ptr>(phx::new_<STLeaf>(_1)), _2, _3))
							]
						| rule_string
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<STLeaf>(_1))
							];

	rule_lvl3_expr =	(rule_lvl4_expr >> Lvl3OpSymbols >> rule_lvl3_expr)
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<BinaryOperationNode>(_1, _2, _3))
							]
						| rule_lvl4_expr[_val = _1];

	rule_lvl2_expr =	(rule_lvl3_expr >> Lvl2OpSymbols >> rule_lvl2_expr)
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<BinaryOperationNode>(_1, _2, _3))
							]
						| rule_lvl3_expr[_val = _1];


	rule_lvl1_expr =	(rule_lvl2_expr >> Lvl1OpSymbols >> rule_lvl1_expr)
							[
								_val = phx::construct<STNode::node_ptr>(phx::new_<BinaryOperationNode>(_1, _2, _3))
							]
						| rule_lvl2_expr[_val = _1];


	rule_lvl3_expr.name("lvl 3");
	rule_lvl2_expr.name("lvl 2");
	rule_lvl1_expr.name("lvl 1");
	rule_string.name("string");

	/*debug(rule_lvl3_expr);
	debug(rule_lvl2_expr);
	debug(rule_lvl1_expr);
	debug(rule_string);*/
}



}
