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
#ifndef MAPCSS_GRAMMAR_HPP
#define MAPCSS_GRAMMAR_HPP




#include "mapcss_def.hpp"
#include "comment_skipper.hpp"
#include "server/eval/eval.hpp"
#include "eval_grammar.hpp"

#include "server/style_template.hpp"
#include "config.hpp"


namespace phx = boost::phoenix;


//! Simple structure to store a zoom range
struct Zoom
{
	//! Creates the standard zoom
	Zoom()
		: top(ALAC_ZOOM_TOP), bottom(ALAC_ZOOM_BOTTOM)
	{
	}

	//! Creates a zoom from given values
	Zoom(int bottom, int top)
		: top(top), bottom(bottom)
	{

	}

	void expand(const Zoom& other)
	{
		top = std::max(top, other.top);
		bottom = std::min(bottom, other.bottom);
	}

	void focus(const Zoom& other)
	{
		top = std::min(top, other.top);
		bottom = std::max(bottom, other.bottom);
	}


	//! Compares to zoom's
	bool operator == (const Zoom& z) const
	{
		return top == z.top && bottom == z.bottom;
	}

	//! The top of the zoom range
	int top;

	//! The bottom of the zoom range
	int bottom;
};


//! Stores informations about an unary condition
typedef fsio::vector<	op::UnaryTypesEnum,		/* operator */
						string					/* tag */
					> UnaryCondition;

//! Stores informations about an binary condition
typedef fsio::vector<	string,				/* tag */
						op::BinaryTypesEnum,	/* operator */
						string				/* tag */
					> BinaryCondition;

//! Possible Conditions
typedef boost::variant<UnaryCondition, BinaryCondition> ConditionType;

//! Represents a part of the style selector
struct SelectorItem
{
	//! Creates an empty selector item
	SelectorItem()
		: objectType(obj::Any)
	{}

	//! Creates an selector item from given values
	SelectorItem(obj::ObjectTypeEnum o, const Zoom& zoom, const std::vector<ConditionType>& conds)
		: objectType(o)
		, zoom(zoom)
		, conditions(conds)
	{}

	//! Compares to selector items
	bool operator == (const SelectorItem& i) const
	{
		return objectType == i.objectType && zoom == i.zoom && conditions == i.conditions;
	}

	//! The type of the object, which should be selected
	obj::ObjectTypeEnum objectType;
	//! The zoom which should be selected
	Zoom zoom;
	//! The condition of the item
	std::vector<ConditionType> conditions;
};



BOOST_FUSION_ADAPT_STRUCT(SelectorItem,
						  (obj::ObjectTypeEnum, objectType)
						  (Zoom, zoom)
						  (std::vector<ConditionType>, conditions)
						  )


//! symbols for the unit types
struct UnitTypes : qi::symbols<char, unit::UnitTypesEnum>
{

	UnitTypes()
	{
		this->add
			("px", unit::Pixel)
			("pt", unit::Points)
			("%", unit::Percent)
			;
	}

};

//! symbols for the objects
struct ObjectTypes : qi::symbols<char, obj::ObjectTypeEnum>
{

	ObjectTypes()
	{
		this->add
			("node", obj::Node)
			("way", obj::Way)
			("relation", obj::Relation)
			("area", obj::Area)
			("line", obj::Line)
			("*", obj::Any)
			;
	}

};

//! symbols for the binary operators
struct BinaryTypes : qi::symbols<char, op::BinaryTypesEnum>
{

	BinaryTypes()
	{
		this->add
			("=", op::Equal)
			("!=", op::Unequal)
			("=~", op::SameAs)
			("<", op::LessThen)
			(">", op::GreaterThen)
			("<=", op::LessEqual)
			(">=", op::GreaterEqual)
			;
	}

};

//! symbols for the unary operators
struct UnaryTypes : qi::symbols<char, op::UnaryTypesEnum>
{

	UnaryTypes()
	{
		this->add
			("!", op::Not)
			("-", op::Minus)
			;
	}

};

struct AttributeCreator
{
	virtual void addAttribute(const shared_ptr<StyleTemplate>& styleTemplate, const string& specifier, const shared_ptr<ParserLogger>& logger, const ParseInfo& info) = 0;
};

//! symbols for the attributes
struct AttributeTypes : qi::symbols<char, shared_ptr<AttributeCreator> >
{
	AttributeTypes();
};



struct MapCssParser;

/**
 *	\brief The grammar for the mapcss format
 *
 */
struct MapCSSGrammar : public qi::grammar<GrammarIterator, StylesheetPtr(), qi::locals<std::vector<RulePtr>, StylePtr>, CommentSkipper>
{
	//! The Iterator used in this grammar
	typedef GrammarIterator ItType;
	//! The skipper used in this grammar
	typedef CommentSkipper Skipper;

	//! The symbol parser for objects
	ObjectTypes selectorObject_;
	//! The symbol parser for binary operators
	BinaryTypes binaryOperator_;
	//! The symbol parser for unary operators
	UnaryTypes unaryOperator_;
	//! The symbol parser for style attributes
	AttributeTypes attributeType_;

	MapCSSGrammar(MapCssParser& parser);

	//! Rule to parse a styleset of a rule
	qi::rule<ItType, StylePtr(), qi::locals<shared_ptr<AttributeCreator>, ParseInfo>, Skipper> rule_styleset;
	//! Rule to parse generic (unknown) attribute name
	qi::rule<ItType, string(), Skipper> attribute_name;
	//! Rule to parse the specifier
	qi::rule<ItType, string(), Skipper> rule_specifier;
	//! Rule to parse a value
	qi::rule<ItType, string()> rule_value;
	//! Rule to parse a tag
	qi::rule<ItType, string()> rule_tag;
	//! Rule to parse a zoom range
	qi::rule<ItType, Zoom(), qi::locals<int, int>, Skipper> rule_zoom;
	//! Rule to parse a binary condition
	qi::rule<ItType, BinaryCondition()> rule_binary_condition;
	//! Rule to parse a unary condition
	qi::rule<ItType, UnaryCondition()> rule_unary_condition;
	//! Rule to parse mapcss classes
	qi::rule<ItType> rule_class;
	//! Rule to parse one part of the selector
	qi::rule<ItType, SelectorItem(), Skipper> rule_subselector;
	//! Rule to parse a selector
	qi::rule<ItType, RulePtr(), qi::locals<std::vector<SelectorItem> >, Skipper> rule_selector;

	//! Rule to parse one rule. Returns a list of rules, because selectors can be splited
	qi::rule<ItType, std::vector<RulePtr>(), Skipper> rule_cssrule;
	//! Main rule returning a stylesheet
	qi::rule<ItType, StylesheetPtr(), qi::locals<std::vector<RulePtr>, StylePtr>, Skipper> rule_stylesheet;
};



#endif
