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
 *  Copyright 2012-2013 Tobias Kahlert
 */

#pragma once
#ifndef MAPCSS_PARSER_HPP
#define MAPCSS_PARSER_HPP


#include "mapcss_def.hpp"
#include "mapcss_grammar.hpp"
#include "parser_logger.hpp"

#include "server/rule.hpp"

#include <boost/filesystem/path.hpp>

class Geodata;
class Stylesheet;



struct MapCssParser
{
	void addAttributeToTemplate(StylePtr& style, const shared_ptr<AttributeCreator>& attrType, const string& specifier, const ParseInfo& info);
	void applyStyleToRules(std::vector<RulePtr>& rules, const StylePtr& style);
	SelectorPtr createSelectorFromObjectType(const SelectorPtr& next, const shared_ptr<Rule>& rule, obj::ObjectTypeEnum objType, Rule::AcceptableTypes& type);
	SelectorPtr createChildSelectorFromObjectType(const SelectorPtr& next, const shared_ptr<Rule>& rule, obj::ObjectTypeEnum objType);
	SelectorPtr createSelectorFromUnaryCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const UnaryCondition& condition);
	SelectorPtr createSelectorFromBinaryCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const BinaryCondition& condition);
	SelectorPtr createSelectorFromCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const ConditionType& condition);
	RulePtr createSelectorChain(const std::vector<SelectorItem>& items);
	void warnUnsupportedAttribute(const string& attribute) const;

	MapCssParser(const shared_ptr<Geodata>& geodata);
	void load(const string& path);

	//! The geodata used to inject them in the rules
	shared_ptr<Geodata> geodata;
	//! The parsed stylesheet
	shared_ptr<Stylesheet> parsedStylesheet;
	//! The exception that may be catched
	excp::ParseException catchedException;
	//! Output logger for errors and warnings
	shared_ptr<ParserLogger> logger;
};

#endif
