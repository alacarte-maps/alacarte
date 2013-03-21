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
#ifndef _PARSE_INFO_HPP
#define _PARSE_INFO_HPP


#include "settings.hpp"



struct ParseInfo
{
	inline ParseInfo()
		: column(0)
		, line(0)
	{

	}

	inline ParseInfo(const string& sourcefile, unsigned int column, unsigned int line, const string& linecontent)
		: sourcefile(sourcefile)
		, column(column)
		, line(line)
		, linecontent(linecontent)
	{

	}

	inline const string& getSourceFile() const { return sourcefile; }
	inline const unsigned int& getColumn() const { return column; }
	inline const unsigned int& getLine() const { return line; }
	inline const string& getLineContent() const { return linecontent; }


private:
	string			sourcefile;
	unsigned int	column;
	unsigned int	line;
	string			linecontent;
};



#endif