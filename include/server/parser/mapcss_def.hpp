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
#ifndef MAPCSS_DEF_HPP
#define MAPCSS_DEF_HPP
#include "settings.hpp"

#include <boost/fusion/include/adapt_struct.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/variant/variant.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/qi_grammar.hpp>
#include <boost/spirit/include/qi_real.hpp>
#include <boost/spirit/include/qi_symbols.hpp>
#include <boost/spirit/include/qi_stream.hpp>
#include <boost/spirit/include/qi_uint.hpp>
#include <boost/spirit/include/classic_position_iterator.hpp>

namespace fsio = boost::fusion;
namespace spirit = boost::spirit;
namespace classic = boost::spirit::classic;
namespace qi = spirit::qi;
namespace chs = spirit::standard;


class StyleTemplate;

namespace obj
{
	//! Enum with object selectors
	enum ObjectTypeEnum
	{
		Node,		//!< Selects nodes
		Way,		//!< Selects ways
		Relation,	//!< Selects relations
		Area,		//!< Selects areas (closed ways)
		Line,		//!< Selects lines (unclosed ways)
		Any			//!< Selects everything
	};
};


namespace op
{
	//! Enum with binary operators used in selectors
	enum BinaryTypesEnum
	{
		Equal,			//!< Checks if two values are equal
		Unequal,		//!< Checks if two values are not equal
		SameAs,			//!< Checks if a value is recognised by a regex
		LessThen,		//!< Checks if a value is lesser than another
		GreaterThen,	//!< Checks if a value is greater then another
		LessEqual,		//!< Checks if a value is lesser or equal to another
		GreaterEqual	//!< Checks if a value is greater or equal to another
	};

	//! Enum with unary operators used in selectors
	enum UnaryTypesEnum
	{
		HasTag,			//!< Checks if a geoobject has a certain tag
		Not,			//!< Checks if a geoobject does not have a certain tag
		Minus
	};
};



namespace unit
{
	//! Contains all the units available for numeric values
	enum UnitTypesEnum
	{
		Pixel,
		Points,
		Percent
	};
};




namespace attr
{
	//! Contains all the attributes, which can be specified in a styleset
	enum AttributeTypesEnum
	{
		FillColor,		//!< Specifies a fillcolor
		ZIndex,			//!< Specifies the z index
		Width,			//!< Specifies the width
		Color,			//!< Specifies the color
		CasingWidth,	//!< Specifies the casing width
		CasingColor,	//!< Specifies the casing color
		FontSize,		//!< specifies the font size
		TextColor,		//!< Specifies the text color
		TextPosition,	//!< Specifies the text poisition
		Text,			//!< Specifies the text to be used
		IconPath,
		IconWidth,
		IconHeight
	};
};


class Stylesheet;
class Rule;
class Selector;
class ApplySelector;
class Style;

//! Shortcut for a shared pointer to stylesheet
typedef shared_ptr<Stylesheet> StylesheetPtr;

//! Shortcut for a shared pointer to rule
typedef shared_ptr<Rule> RulePtr;

//! Shortcut for a shared pointer to the base of all selectors
typedef shared_ptr<Selector> SelectorPtr;

//! Shortcut for a shared pointer to thr apply selector
typedef shared_ptr<ApplySelector> ApplySelectorPtr;

//! Shortcut for a shared pointer to style
typedef shared_ptr<StyleTemplate> StylePtr;



//! Iterator to iterate over a file
typedef std::string::const_iterator StringIterator;
//! Iterator wrapping  the file iterator and to be used in all grammars
typedef classic::position_iterator2<StringIterator> GrammarIterator;



#include "parse_info.hpp"

#endif
