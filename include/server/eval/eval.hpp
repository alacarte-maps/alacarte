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
#ifndef _EVAL_HPP
#define _EVAL_HPP



#include <boost/variant/apply_visitor.hpp>
#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>


#include "settings.hpp"
#include "eval_helpers.hpp"
#include "eval.hpp"
#include "stnode.hpp"
#include "../parser/parser_logger.hpp"
#include "../parser/parse_info.hpp"

class GeoObject;

namespace eval {


shared_ptr<STNode> parseEval(string::const_iterator begin, string::const_iterator end, const shared_ptr<ParserLogger>& logger);

/**
 * @brief Represents a value in a style template
 *
 * Can be any value or an evalueable expression
 *
 **/
template<typename TargetType>
class Eval
{
public:
	typedef shared_ptr<STNode> node_ptr;
	typedef boost::variant<TargetType, node_ptr> value_type;

	/**
	 * @brief Creates an Eval from the value type, the eval should evaluated to
	 *
	 * The direct value, which should be returned, when overwrite is called
	 *
	 * \param value used internaly by this eval
	 **/
	Eval(const TargetType& value)
		: value(value)
		, failed(false)
		, info("[hard coded]", 0, 0, ToString(value))
	{

	}

	/**
	 * @brief Creates an eval from a string
	 *
	 * First the constructor will try to parse the string ad an eval.
	 * If that does not work, the constructor will try to convert the string
	 * into the target type. If that does not work either, a ParseException is thrown.
	 *
	 * \param expr which should be parsed
	 * \param logger which should be used by this eval
	 * \param info about the eval
	 **/
	Eval(const string& expr, const shared_ptr<ParserLogger>& logger, const ParseInfo& info)
		: value(TargetType())
		, logger(logger)
		, failed(false)
		, info(info)
	{
		shared_ptr<STNode> result = parseEval(expr.cbegin(), expr.cend(), logger);

		// If this is now eval expression, try to parse the type directly
		if(result)
		{
			value = result;
		} else {

			// If we extract a string here than cache it
			if(!Conv(expr, &boost::get<TargetType>(value), true))
			{
				BOOST_THROW_EXCEPTION(excp::ParseException()
										<< excp::InfoFailureLineContent(expr)
										<< excp::InfoFailureLine(info.getLine())
										<< excp::InfoWhat("Failed to parse the expression into an eval or a '" + string(typeid(TargetType).name()) + "'"));
			}else{
				if(expr.find("eval") == 0)
				{
					logger->warnStream() << "In line " << info.getLine() << " could be an eval but failed to parse so.";
					logger->warnStream() << "Expression is used as " << string(typeid(TargetType).name()) << "!";
					logger->warnStream() << "'" << info.getLineContent() << "'";
				}
			}
		}
	}

	/**
	 * @brief Will overwrite the second parameter with an evaluated value.
	 *
	 * \param obj The geo object used for some eval expressions
	 * \param v the value, which should be overwritten
	 **/
	void overwrite(GeoObject* obj, TargetType* v) const
	{
		// which() == 0 -> TargetType
		// which() == 1 -> Eval
		if(!isEval())
		{
			*v = boost::get<TargetType>(value);
		}else{
			assert(value.which() == 1);

			string result = boost::get<node_ptr>(value)->eval(obj);
			if(!Conv(result, v) && !failed && logger)
			{
				failed = true;
				logger->warnStream() << "Evaluation evaluated into \"" << result << "\", then failed to convert into " << typeid(TargetType).name() << ".";
				logger->warnStream() << "[in line " << info.getLine() << " with content \"" << info.getLineContent() << "\"]";
			}
		}
	}

	bool isEval() const
	{
		// which() == 0 -> TargetType
		// which() == 1 -> Eval
		return value.which() == 1;
	}
private:
	// Internal target value or evaluateable expression
	value_type value;

	// Logger for this eval
	shared_ptr<ParserLogger> logger;
	
	// Flag if this eval failed before
	mutable bool failed;

	// Info about the source this eval is coming from.
	ParseInfo info;
};




/**
 * @brief Same as eval but with more than one value
 *
 **/
template<typename TargetType>
class Eval< std::vector<TargetType> >
{
public:
	typedef shared_ptr<STNode> node_ptr;
	typedef boost::variant<TargetType, node_ptr> value_type;

	Eval(const std::vector<TargetType>& values)
		: valuelist(values)
		, failed(false)
		, info("[hard coded]", 0, 0, ToString(values))
	{

	}

	Eval(const string& expr, const shared_ptr<ParserLogger>& logger, const ParseInfo& info)
		: logger(logger)
		, failed(false)
		, info(info)
	{
		// Use simple parser to separate commas
		bool quote = false;
		bool dquote = false;
		int parentheses = 0;

		string::const_iterator begin = expr.cbegin();
		string::const_iterator end = begin;

		while(end != expr.end())
		{
			switch(*end)
			{
			case ',':
				// Handle ,
				if(parentheses == 0 && !dquote && !quote)
				{
					addValue(string(begin, end));
					begin = ++end;
					continue;
				}
				break;

			case '(':
			case ')':
				// Handle parentheses
				if(!dquote && !quote)
				{
					if(*end == '(')
					{
						++parentheses;
					}
					else if(*end == ')' && parentheses > 0)
					{
						--parentheses;
					}
				}
				break;

			case '\"':
				// Handle "
				if(!quote)
				{
					dquote = !dquote;
				}
				break;

			case '\'':
				// Handle '
				if(!dquote)
				{
					quote = !quote;
				}
				break;

			default:
				break;
			}

			++end;
		}

		addValue(string(begin, end));

		
	}

	void overwrite(GeoObject* obj, std::vector<TargetType>* v) const
	{
		v->clear();
		for(auto& e : valuelist)
		{
			// which() == 0 -> TargetType
			// which() == 1 -> Eval
			if(e.which() == 0)
			{
				v->push_back(boost::get<TargetType>(e));
			}else{
				string result = boost::get<node_ptr>(e)->eval(obj);
				TargetType target;
				if(!Conv(result, &target) && !failed && logger)
				{
					failed = true;
					logger->warnStream() << "Evaluation evaluated into \"" << result << "\", then failed to convert into " << typeid(TargetType).name() << ".";
					logger->warnStream() << "[in line " << info.getLine() << " with content \"" << info.getLineContent() << "\"]";
				}else{
					v->push_back(target);
				}
			}
		}
	}

	int size() const
	{
		return valuelist.size();
	}

	bool isEval(int i) const
	{
		// which() == 0 -> TargetType
		// which() == 1 -> Eval
		return valuelist[i].which() == 1;
	}
private:
	void addValue(const string& part)
	{
		valuelist.push_back(TargetType());
		if(!Conv(part, &boost::get<TargetType>(valuelist.back()), true))
		{
			shared_ptr<STNode> result = parseEval(part.cbegin(), part.cend(), logger);

			if(!result)
				BOOST_THROW_EXCEPTION(excp::ParseException()
										<< excp::InfoFailureLineContent(part)
										<< excp::InfoFailureLine(info.getLine())
										<< excp::InfoWhat("Failed to parse the expression into an list of eval's or a " + string(typeid(TargetType).name()) + "'s"));

			valuelist.back() = result;
		}
	}


	// Internal target values or evaluateable expressions
	std::vector<value_type> valuelist;

	// Logger for this eval
	shared_ptr<ParserLogger> logger;

	// Flag if this eval failed before
	mutable bool failed;

	// Info about the source this eval is coming from.
	ParseInfo info;
};


}


#endif
