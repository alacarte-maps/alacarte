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
#ifndef PARSER_LOGGER_HPP
#define PARSER_LOGGER_HPP


#include <boost/enable_shared_from_this.hpp>
#include <fstream>

#include "settings.hpp"

class ParserLogger
	: public boost::enable_shared_from_this<ParserLogger>
{
public:
	enum Category
	{
		Error,
		Warning
	};

	class LogStream
	{
	public:
		LogStream(const shared_ptr<ParserLogger>& logger, Category cat);
		LogStream(const LogStream& other);
		~LogStream();

		template<typename T>
		LogStream& operator <<(const T& v)
		{
			value << v;
			return *this;
		}

		string logvalue() const;
	private:
		std::ostringstream		value;
		shared_ptr<ParserLogger> logger;
		Category				category;
	};

	ParserLogger(const string& parsedFile);

	LogStream warnStream();
	LogStream errorStream();

	void warn(const string& str);
	void error(const string& str);
private:
	void logToStream(Category cat, const string& str);

	std::ofstream		outputStream;
	string				filename;
};



#endif
