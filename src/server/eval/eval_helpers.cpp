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
#include <boost/algorithm/string.hpp>

#include "server/eval/eval_helpers.hpp"
#include "server/parser/color_grammar.hpp"

namespace eval {





template<>
bool Conv(const string& str, bool* out, bool tryToCache )
{
	assert(out);
	*out = true;

	if(str == "")
		*out = false;
	if(str == "false")
		*out = false;
	if(str == "no")
		*out = false;

	int i;
	if(Conv(str, &i) && i == 0)
	{
		*out = false;
	}

	return true;
}


ColorGrammar color_;

template<>
bool Conv(const string& str, Color* out, bool tryToCache )
{
	assert(out);
	StringIterator begin = str.cbegin();
	StringIterator end = str.cend();

	bool result = qi::phrase_parse(begin, end, color_, chs::space, *out);

	return begin == end && result;
}


template<>
bool Conv(const string& str, string* out, bool tryToCache )
{
	assert(out);
	out->clear();

	bool quote = false;
	for(char c : str)
	{
		if(c == '\"')
		{
			quote = !quote;
		}else{
			out->push_back(c);
		}
	}
	boost::algorithm::trim(*out);
	return !quote;
}


template<>
bool Conv(const string& str, MaybeCachedString* out, bool tryToCache )
{
	string extr;

	bool result = Conv<string>(str, &extr);

	if(result)
	{
		if(tryToCache)
		{
			*out = CachedString(extr);
		}else{
			*out = extr;
		}
	}

	return result;
}

	
template<>
string ToString(const bool& v)
{
	return v ? "true" : "false";
}



}
