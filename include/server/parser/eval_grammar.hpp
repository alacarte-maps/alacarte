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
#ifndef _EVAL_GRAMMAR_HPP
#define _EVAL_GRAMMAR_HPP


#include "mapcss_def.hpp"
#include "server/eval/eval.hpp"
#include "server/eval/stnode.hpp"



namespace eval {

/**
 * @brief Grammar used to parse eval expressions
 *
 **/
struct EvalGrammer : public qi::grammar<GrammarIterator,  STNode::node_ptr(),  chs::space_type>
{
	//! Type of the iterator used to parse the grammar
	typedef GrammarIterator ItType;
	//! Type of the skipper used by this grammar
	typedef chs::space_type Skipper;


	EvalGrammer();


	//! rule for a simple string
	qi::rule<ItType, string()> rule_string;

	//! rule for function expressions
	qi::rule<ItType, STNode::node_ptr(), Skipper> rule_func_expr;
	//! rule for lvl 4 operations
	qi::rule<ItType, STNode::node_ptr(), Skipper> rule_lvl4_expr;
	//! rule for lvl 3 operations
	qi::rule<ItType, STNode::node_ptr(), Skipper> rule_lvl3_expr;
	//! rule for lvl 2 operations
	qi::rule<ItType, STNode::node_ptr(), Skipper> rule_lvl2_expr;
	//! rule for lvl 1 operations
	qi::rule<ItType, STNode::node_ptr(), Skipper> rule_lvl1_expr;
};









}




#endif
