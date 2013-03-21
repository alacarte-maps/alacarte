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
#ifndef _EVAL_HELPERS_HPP
#define _EVAL_HELPERS_HPP


#include "settings.hpp"



namespace eval {

/**
 * @brief Converts a string into another type
 *
 * \param str string to be converted
 * \param out Target for the conversion
 * \return if the conversion was successful
 **/
template<typename T>
bool Conv(const string& str, T* out, bool tryToCache = false)
{
	assert(out);
	std::istringstream is(str);
	is >> *out;

	return !is.bad() && is.eof();
}

//! Use some extra function for bool conversion
template<>
bool Conv(const string& str, bool* out, bool tryToCache);

//! Use some extra function for color conversion
template<>
bool Conv(const string& str, Color* out, bool tryToCache);

//! Use some extra function for string conversion
template<>
bool Conv(const string& str, string* out, bool tryToCache );

//! Use some extra function for MaybeCachedString conversion
template<>
bool Conv(const string& str, MaybeCachedString* out, bool tryToCache);


/**
 * @brief Converts a value into a string
 *
 * \param v The value
 * \return The converted string
 **/
template<typename T>
string ToString(const T& v)
{
	std::ostringstream os;
	os << v;

	return os.str();
}

// Use extra Conversion if the type is a bool
template<>
string ToString(const bool& v);

/**
 * @brief Converts a value list into a string
 *
 * \param v The values
 * \return The converted string
 **/
template<typename Iter1, typename Iter2>
string ToString(Iter1 begin, Iter2 end)
{
	string result;

	while(begin != end)
	{
		result += ToString(*begin);
		++begin;
	}

	return result;
}



}



#endif