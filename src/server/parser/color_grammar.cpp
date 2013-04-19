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


#include "server/parser/color_grammar.hpp"

#include "utils/colorTable.hpp"

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_object.hpp>
#include <boost/spirit/include/phoenix_stl.hpp>
#include <boost/spirit/include/phoenix_bind.hpp>
#include <boost/spirit/include/phoenix.hpp>
#include <boost/spirit/include/qi_symbols.hpp>


ColorSymbols::ColorSymbols()
{
	auto& table = ColorTable::Inst().getTable();

	for(auto& entry : table)
	{
		this->add(entry.first, entry.second);
	}
}


/**
 * Creates all rules for the color grammar
 */
ColorGrammar::ColorGrammar()
	: ColorGrammar::base_type(rule_color, "color-grammar")
{
	//using namespace boost::phoenix::local_names;
	using namespace qi::labels;
	using boost::phoenix::construct;
	auto& _1 = qi::_1;

	rule_hexvalue = (hexdigit[_a = _1] >> hexdigit[_val = _a * 16 + _1]);
	rule_hexcolor = qi::eps[_d = 255] >> (( rule_hexvalue[_a = _1] >> rule_hexvalue[_b = _1] >> rule_hexvalue[_c = _1] >> -(rule_hexvalue[_d = _1]))[_val = phx::construct<Color>(_a, _b, _c, _d)]
		| (hexdigit[_a = _1] >> hexdigit[_b = _1] >> hexdigit[_val = phx::construct<Color>(_a, _b, _1)]));

	rule_rgb = '(' >> qi::float_[_a = _1] >> ',' >> qi::float_[_b = _1] >> ',' >> qi::float_[_val = phx::construct<Color>(_a, _b, _1)] >> ')';
	rule_rgba = '(' >> qi::float_[_a = _1] >> ',' >> qi::float_[_b = _1] >> ',' >> qi::float_[_c = _1] >> ',' >> qi::float_[_val = phx::construct<Color>(_a, _b, _c, _1)] >> ')';
	rule_color = ('#' >> rule_hexcolor)
				| (qi::lit("rgb") >> rule_rgb)
				| (qi::lit("rgba") >> rule_rgba)
				| colorSymbol_;
		
			//qi::debug(rule_color);
			//qi::debug(rule_rgba);
			//qi::debug(rule_rgb);
	// 		qi::debug(rule_hexcolor);
	// 		qi::debug(rule_hexvalue);
}
