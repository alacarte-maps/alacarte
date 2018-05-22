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
#ifndef COLOR_GRAMMAR_HPP
#define COLOR_GRAMMAR_HPP

#include <boost/spirit/home/qi/string/tst.hpp>
#include <boost/spirit/home/qi/string/tst_map.hpp>

#include "settings.hpp"
#include "comment_skipper.hpp"
#include "mapcss_def.hpp"

namespace spirit = boost::spirit;
namespace qi = spirit::qi;
namespace classic = boost::spirit::classic;
namespace phx = boost::phoenix;
namespace chs = spirit::standard;




//! symbols for the binary operators
struct ColorSymbols : qi::symbols<char, Color, qi::tst_map<char, Color> >
{
	ColorSymbols();
};


/**
 *	\brief A grammar to parse a mapcss color
 *
 * Supports:
 * + rgb(red, green, blue)
 * + rgba(red, green, blue, alpha)
 * + #COLORCODE
 *
 * \note Color names are parsed in another way!
 */
struct ColorGrammar : public qi::grammar<StringIterator, Color(), chs::space_type>
{
	//! Iterator type for this grammar
	typedef StringIterator ItType;
	//! Skipper type used in this grammar
	typedef chs::space_type Skipper;


	//! Rule to parse a number with up to 3 digits
	qi::uint_parser<uint8, 10, 1, 3> number;
	//! Rule to parse a hex digit
	qi::uint_parser<uint8, 16, 1, 1> hexdigit;
	//! Symbol parser for colors.
	ColorSymbols colorSymbol_;

	ColorGrammar();

	//! Rule to parse the hex color code
	qi::rule<ItType, uint8(), qi::locals<uint8, uint8> > rule_hexvalue;
	//! Rule for #COLORCODE
	qi::rule<ItType, Color(), qi::locals<uint8, uint8, uint8, uint8> > rule_hexcolor;
	//! Rule for rgb(...)
	qi::rule<ItType, Color(), qi::locals<float, float>, Skipper> rule_rgb;
	//! Rule for rgba(...)
	qi::rule<ItType, Color(), qi::locals<float, float, float>, Skipper> rule_rgba;
	//! Mainrule
	qi::rule<ItType, Color(), Skipper> rule_color;
};







#endif
