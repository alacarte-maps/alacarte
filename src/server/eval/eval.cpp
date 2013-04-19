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

#include "server/eval/eval.hpp"
#include "server/eval/stnode.hpp"
#include "server/eval/eval_helpers.hpp"

#include "server/eval/eval.hpp"
#include "server/parser/eval_grammar.hpp"

#include "utils/colorTable.hpp"



namespace eval {

/*
template<typename T>
static T EasyStringConv(const string& str, T* value)
{
	T extract;
	if(Conv(str, &extract))
	{
		*value = extract;
	}

	return T();
}

struct IntVisitor : public boost::static_visitor<int>
{
	GeoObject* obj;
	int* target;
	IntVisitor(GeoObject* obj, int* target)
		: obj(obj)
		, target(target)
	{}

	int operator()(int i) const { return *target = i; }
	int operator()(const Color& color) const { / * Do nothing! * / return 0;  }
	int operator()(double d) const { return *target = static_cast<int>(d); }
	int operator()(const string& s) const { return EasyStringConv(s, target); }
	int operator()(const eval::Eval::node_ptr& node) const { return EasyStringConv(node->eval(obj), target); }
};

struct DoubleVisitor : public boost::static_visitor<double>
{
	GeoObject* obj;
	double* target;
	DoubleVisitor(GeoObject* obj, double* target)
		: obj(obj)
		, target(target)
	{}

	double operator()(int i) const { return *target = static_cast<double>(i); }
	double operator()(const Color& color) const { / * Do nothing! * / return 0;  }
	double operator()(double d) const { return *target = d; }
	double operator()(const string& s) const { return EasyStringConv(s, target); }
	double operator()(const eval::Eval::node_ptr& node) const { return EasyStringConv(node->eval(obj), target); }
};

struct ColorVisitor : public boost::static_visitor<Color>
{
	GeoObject* obj;
	Color* target;
	ColorVisitor(GeoObject* obj, Color* target)
		: obj(obj)
		, target(target)
	{}

	Color operator()(int i) const { / * Do nothing! * / return Color(); }
	Color operator()(const Color& color) const { return *target = color;  }
	Color operator()(double d) const { / * Do nothing! * / return Color(); }
	Color operator()(const string& s) const { ColorTable::Inst().resolve(s, target); return Color(); }
	Color operator()(const eval::Eval::node_ptr& node) const { ColorTable::Inst().resolve(node->eval(obj), target); return Color(); }
};

struct StringVisitor : public boost::static_visitor<string>
{
	GeoObject* obj;
	string* target;
	StringVisitor(GeoObject* obj, string* target)
		: obj(obj)
		, target(target)
	{}

	string operator()(int i) const { return (*target = ToString(i)); }
	string operator()(const Color& color) const {return (*target = ToString(color));  }
	string operator()(double d) const { return (*target = ToString(d)); }
	string operator()(const string& s) const { return *target = s; }
	string operator()(const eval::Eval::node_ptr& node) const { return *target = node->eval(obj); }
};*/


/**
 * @brief Creates a new eval
 *
 *	\param value The internal value
 **/
/*
Eval::Eval(const Eval::value_type& value)
	: value(value)
{
}*/


/**
 * @brief Overwrites the double if the internal value can be resolved into a double
 *
 * \param obj which will be used to execute an eval expression
 * \param dv The value to overwrite
 **/
/*
void Eval::overwrite(GeoObject* obj, double* dv) const
{
	assert(dv);
	boost::apply_visitor(DoubleVisitor(obj, dv), value);
}*/

/**
 * @brief Overwrites the int if the internal value can be resolved into a int
 *
 * \param obj which will be used to execute an eval expression
 * \param iv The value to overwrite
 **/
/*
void Eval::overwrite(GeoObject* obj, int* iv) const
{
	assert(iv);
	boost::apply_visitor(IntVisitor(obj, iv), value);
}*/

/**
 * @brief Overwrites the Color if the internal value can be resolved into a Color
 *
 * \param obj which will be used to execute an eval expression
 * \param cv The value to overwrite
 **/
/*
void Eval::overwrite(GeoObject* obj, Color* cv) const
{
	assert(cv);
	boost::apply_visitor(ColorVisitor(obj, cv), value);
}
*/

/**
 * @brief Overwrites the string if the internal value can be resolved into a string
 *
 * \param obj which will be used to execute an eval expression
 * \param sv The value to overwrite
 **/
/*
void Eval::overwrite(GeoObject* obj, string* sv) const
{
	assert(sv);
	boost::apply_visitor(StringVisitor(obj, sv), value);
}
*/



/**
 * @brief parses an eval 
 *
 * \param begin of the parseable string
 * \param end of the parseable string
 * \param logger used for output 
 *
 * \return the root node for the expression or null, if something failed.
 **/
shared_ptr<STNode> parseEval(string::const_iterator begin, string::const_iterator end, const shared_ptr<ParserLogger>& logger)
{
	EvalGrammer eval_;
	shared_ptr<STNode> root;

	GrammarIterator iterBegin(begin, end);
	GrammarIterator iterEnd;

	bool result = false;
	try {
		result = qi::phrase_parse(iterBegin, iterEnd, (qi::lit("eval") >> '(' >> (('\"' > eval_ > '\"') | eval_) >> ')'), chs::space, root);
	} catch(qi::expectation_failure<GrammarIterator>&)
	{
		result = false;
	}

	if(!result || iterBegin != iterEnd)
		return shared_ptr<STNode>();

	return root;
}

}