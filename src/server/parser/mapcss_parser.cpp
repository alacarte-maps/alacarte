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



#include <boost/fusion/include/at_c.hpp>
#include <boost/spirit/include/qi.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/thread.hpp>

#include "server/style.hpp"
#include "server/stylesheet.hpp"

#include "server/parser/comment_skipper.hpp"
#include "server/parser/mapcss_parser.hpp"
#include "server/parser/mapcss_grammar.hpp"
#include "server/selectors/selectors.hpp"


/**
 * Replaces all tabs with spaces in a given string
 *
 * \param str String where tabs should be replaced by spaces
 * \return New String with spaces instead of tabs
 */
string TabToSpace(const string& str)
{
	string ret = str;

	for(char& c : ret)
	{
		if(c == '\t')
			c = ' ';
	}

	return ret;
}

//! \cond
// Visitors to access boost::variant
/*struct IntVisitor : boost::static_visitor<int>
{
	int operator()(int i) const { return i; }
	int operator()(const Color& color) const { BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Expected an integer!")); }
	int operator()(double d) const { return static_cast<int>(d); }
	int operator()(const string& s) const { return boost::lexical_cast<int>(s); }
};

struct DoubleVisitor : boost::static_visitor<double>
{
	double operator()(int i) const { return static_cast<double>(i); }
	double operator()(const Color& color) const { BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Expected an integer!")); }
	double operator()(double d) const { return d; }
	double operator()(const string& s) const { return boost::lexical_cast<double>(s); }
};

struct ColorVisitor : boost::static_visitor<Color>
{
	Color operator()(int i) const { BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Expected a color!")); }
	Color operator()(const Color& color) const { return color; }
	Color operator()(double d) const { BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Expected a color!")); }
	Color operator()(const string& s) const { return ResolveColorName(s); }
};

struct StringVisitor : boost::static_visitor<string>
{
	string operator()(int i) const { return boost::lexical_cast<string>(i); }
	string operator()(const Color& color) const { return boost::lexical_cast<string>(color); }
	string operator()(double d) const { return boost::lexical_cast<string>(d); }
	string operator()(const string& s) const { return s; }
};*/
//! \endcond

/**
 *	Converts a given string into a text position
 *	\param specifier selecting text position (values are "line" and "center")
 *	\return The specified text position
 */
/*Style::TextPosition SelectTextPosition(const SpecifierType& specifier)
{
	string spec = boost::apply_visitor(StringVisitor(), specifier);
	if(spec == "line")
	{
		return Style::POSITION_LINE;
	}else if (spec == "center")
	{
		return Style::POSITION_CENTER;
	}

	BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Expected a 'line' or 'center' !"));
}*/

/**
 * Adds an attribute to an existing style
 * \param style to add the attribute
 * \param attrType type of the attribute
 * \param specifier for the attribute
 */
void MapCssParser::addAttributeToTemplate(StylePtr& style, const shared_ptr<AttributeCreator>& attrType, const string& specifier, const ParseInfo& info)
{
	assert(style);
	assert(attrType);

	try {
		attrType->addAttribute(style, specifier, logger, info);
	} catch(excp::ParseException& e) {

		// Something went wrong!
		logger->warnStream() << "Failed to parse attribute in \"" + info.getSourceFile() + "\":";
		logger->warnStream() << excp::ErrorOut<excp::InfoWhat>(e, "unknown reason!");
		logger->warnStream() << "In line " << info.getLine() << " column " << info.getColumn() << ":";

		logger->warnStream() << "'" << TabToSpace(info.getLineContent()) << "'";
		logger->warnStream() << string(info.getColumn(), ' ') << "^-here";
		logger->warnStream() << "Attribute will be ignored!";
	}
}

/**
 *	Sets a given style as the style of given rules
 *
 *	\param rules rules, where the style should applied to
 *	\param style which should be applied to the rules
 */
void MapCssParser::applyStyleToRules(std::vector<RulePtr>& rules, const StylePtr& style)
{
	for(auto& rule : rules)
	{
		rule->setStyleTemplate(style);
	}
}

/**
 * Creates an selector used to match geoobject types
 *
 * \param next selector which is called, if this selector matches
 * \param rule containing the selector
 * \param objType which should be selected by the selector
 * \return The created selector
 */
SelectorPtr MapCssParser::createSelectorFromObjectType(const SelectorPtr& next, const shared_ptr<Rule>& rule, obj::ObjectTypeEnum objType, Rule::AcceptableTypes& type)
{
	switch(objType)
	{
	case obj::Any:		type = Rule::Accept_Any;	return next;
	case obj::Node:		type = Rule::Accept_Node;	return next;
	case obj::Way:		type = Rule::Accept_Way;	return next;
	case obj::Relation:	type = Rule::Accept_Relation; return next;
	case obj::Line:		type = Rule::Accept_Way;	return boost::make_shared<LineSelector>(rule, next);
	case obj::Area:		type = Rule::Accept_Way;	return boost::make_shared<AreaSelector>(rule, next);
	default:
		assert(false);
		return next;
	}
}

/**
 * Creates a selector, selecting children of the specified type
 *
 * \param next selector which is called, if this selector matches
 * \param rule containing the selector
 * \param objType which should be selected by this selector
 * \return The created selector
 */
SelectorPtr MapCssParser::createChildSelectorFromObjectType(const SelectorPtr& next, const shared_ptr<Rule>& rule, obj::ObjectTypeEnum objType)
{
	switch(objType)
	{
	case obj::Any:		return next;
	case obj::Node:		return boost::make_shared<ChildNodesSelector>(rule, next);
	case obj::Way:		return boost::make_shared<ChildWaysSelector>(rule, next);
	case obj::Line:		return boost::make_shared<ChildWaysSelector>(rule, boost::make_shared<LineSelector>(rule, next));
	case obj::Area:		return boost::make_shared<ChildWaysSelector>(rule, boost::make_shared<AreaSelector>(rule, next));
	default:
		assert(false);
		return next;
	}
}


/**
 * Creates a selector using an unary operator
 * \param next selector which is called, if this selector matches
 * \param rule containing the selector
 * \param condition used to select objects
 * \return The created selector
 */
SelectorPtr MapCssParser::createSelectorFromUnaryCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const UnaryCondition& condition)
{
	const string& tag = fsio::at_c<1>(condition);

	switch(fsio::at_c<0>(condition))
	{
	case op::HasTag:	return boost::make_shared<HasTagSelector>(rule, next, tag);
	case op::Not:		return boost::make_shared<HasNotTagSelector>(rule, next, tag);
	case op::Minus:
	default:
		assert(!"This unary condition is not implemented!");
		return next;
	}
}


/**
 * Creates a selector using an binary operator
 * \param next selector which is called, if this selector matches
 * \param rule containing the selector
 * \param condition used to select objects
 * \return The created selector
 */
SelectorPtr MapCssParser::createSelectorFromBinaryCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const BinaryCondition& condition)
{
	const string& tag = fsio::at_c<0>(condition);
	const string& value = fsio::at_c<2>(condition);

	try {
		switch(fsio::at_c<1>(condition))
		{
		case op::Equal:			return boost::make_shared<TagEqualsSelector>(rule, next, tag, value);
		case op::Unequal:		return boost::make_shared<TagUnequalsSelector>(rule, next, tag, value);
		case op::SameAs:		return boost::make_shared<TagMatchesSelector>(rule, next, tag, value);
		case op::LessThen:		return boost::make_shared<TagSmallerSelector>(rule, next, tag, boost::lexical_cast<int>(value));
		case op::GreaterThen:	return boost::make_shared<TagLargerSelector>(rule, next, tag, boost::lexical_cast<int>(value));
		case op::LessEqual:		return boost::make_shared<TagSmallerEqualsSelector>(rule, next, tag, boost::lexical_cast<int>(value));
		case op::GreaterEqual:	return boost::make_shared<TagLargerEqualsSelector>(rule, next, tag, boost::lexical_cast<int>(value));
		default:
			assert(!"This binary condition is not implemented!");
			return next;
		}
	}catch (boost::bad_lexical_cast&)
	{
		LOG_SEV(style_log, warning) << "Can not compare non numeric values";
		return next;
	}
}

/**
 * Creates a selector for a given condition
 * \param next selector which is called, if this selector matches
 * \param rule containing the selector
 * \param condition used to select objects
 * \return The created selector
 */
SelectorPtr MapCssParser::createSelectorFromCondition(const SelectorPtr& next, const shared_ptr<Rule>& rule, const ConditionType& condition)
{
	switch(condition.which())
	{
	case 0:
		// Unary condition
		return createSelectorFromUnaryCondition(next, rule, boost::get<UnaryCondition>(condition));
	case 1:
		// Binary condition
		return createSelectorFromBinaryCondition(next, rule, boost::get<BinaryCondition>(condition));
	default:
		assert(!"This is not a condition!");
		return next;
	}
}

/**
 *	Creates a chain of selectors from given subselectors.
 *	The last selector will always be an applyselector, which applies the style of the rule
 *
 *	\param items to be converted into a selector chain
 *	\return A new rule containing the selector chain
 */
RulePtr MapCssParser::createSelectorChain(const std::vector<SelectorItem>& items)
{
	RulePtr rule = boost::make_shared<Rule>(geodata);
	ApplySelectorPtr applier = boost::make_shared<ApplySelector>(rule);
	SelectorPtr topmost = boost::static_pointer_cast<Selector>(applier);

	Zoom zoom;
	Rule::AcceptableTypes acceptableType = Rule::Accept_Any;

	for(auto it = items.crbegin();
		it != items.crend();
		++it)
	{
		const SelectorItem& item = *it;

		for(auto& cond : item.conditions)
		{
			topmost = createSelectorFromCondition(topmost, rule, cond);
		}

		zoom.focus(item.zoom);

		if(it == items.crend()-1) {
			topmost = createSelectorFromObjectType(topmost, rule, item.objectType, acceptableType);
		} else {
			topmost = createChildSelectorFromObjectType(topmost, rule, item.objectType);
		}
	}

	//topmost = createSelectorFromZoom(topmost, rule, zoom);

	rule->first = topmost;
	rule->setZoomBounds(zoom.bottom, zoom.top);
	rule->setAcceptableType(acceptableType);

	return rule;
}

/**
 * Emits a warning for unknown mapcss attribute
 *
 * \param attribute name
 */
void MapCssParser::warnUnsupportedAttribute(const string& attribute) const {
	LOG_SEV(style_log, warning) << "Unsupported attribute '" << attribute << "' was ignored!";
}

/**
 *	Initializes the parser
 *
 *	\param geodata used in created rules
 */
MapCssParser::MapCssParser(const shared_ptr<Geodata>& geodata)
	: geodata(geodata)
{
}


/**
 *	Loads a file from the given path and tries to parse the content into a stylesheet
 *
 *	\param path to a file containing a stylesheet in mapcss form
 *	\return The created stylesheet
 */
void MapCssParser::load(const string& path)
{
	logger = boost::make_shared<ParserLogger>(path);
	LOG_SEV(style_log, info) <<  "Load stylesheet[" << path << "]";
	MapCSSGrammar mapscc_grammar(*this);


	std::ifstream cssStream(path.c_str());


	if(!cssStream)
	{
		LOG_SEV(style_log, warning) << "Failed to load stylesheet '" << path << "'!";
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException() << excp::InfoFileName(path));
	}
	cssStream >> std::noskipws;

	std::string buffer;
	buffer.reserve(1024 * 8);

	std::copy(std::istream_iterator<char>(cssStream), std::istream_iterator<char>(), std::back_inserter(buffer));


	GrammarIterator position_begin(buffer.begin(), buffer.end(), path);
	GrammarIterator position_end;

	StylesheetPtr stylesheet;
	try
	{
		try
		{
			bool r = qi::phrase_parse(position_begin, position_end, mapscc_grammar, CommentSkipper(), stylesheet);

			if(!r || position_begin != position_end || !stylesheet)
			{
				BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Unexpected error while parsing!"));
			}

		} catch(const qi::expectation_failure<GrammarIterator>& e)
		{
			const classic::file_position_base<std::string>& pos = e.first.get_position();

			BOOST_THROW_EXCEPTION(excp::ParseException()
									<< excp::InfoFailureLine(pos.line)
									<< excp::InfoFailureColumn(pos.column)
									<< excp::InfoFailureLineContent(TabToSpace(e.first.get_currentline()))
									<< excp::InfoWhat("Illegal syntax! Expected valid " + boost::lexical_cast<string>(e.what_) + "!"));
		}

	} catch(excp::ParseException& e)
	{
		catchedException = e << excp::InfoFileName(path) << excp::InfoParserLogger(logger);
		return;
	}

	parsedStylesheet =  stylesheet;
}

/**
 *	Creates a stylesheet using a specified file.
 *
 *	\param path to the mapcss file
 *	\param geodata used by in this stylesheet
 *	\return The created stylesheet
 */
shared_ptr<Stylesheet> Stylesheet::Load(const boost::filesystem::path& path, const shared_ptr<Geodata>& geodata, int timeout)
{

	shared_ptr<MapCssParser> parser = boost::make_shared<MapCssParser>(geodata);

	boost::thread timeoutThread(boost::bind(&MapCssParser::load, parser, path.string()));

	if(!timeoutThread.timed_join(boost::posix_time::millisec(timeout)))
	{
		BOOST_THROW_EXCEPTION(excp::TimeoutException() << excp::InfoWhat("Parse timeout!"));
	}


	shared_ptr<Stylesheet> stylesheet = parser->parsedStylesheet;

	if(!stylesheet)
	{
		throw parser->catchedException;
	}

	stylesheet->path = path;
	return stylesheet;
}
