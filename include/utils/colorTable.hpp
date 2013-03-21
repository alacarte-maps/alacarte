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
#ifndef COLORTABLE_HPP
#define COLORTABLE_HPP

#include <boost/unordered_map.hpp>

#include "settings.hpp"

/**
 * Simple table for name to color mappings
 */
class ColorTable
{
public:
	ColorTable();

	void resolve(const string& name, Color* target) const;

	const boost::unordered_map<string, Color>& getTable() const;


	static const ColorTable& Inst();
private:
	//! Value of the table
	boost::unordered_map<string, Color> table;
};


#endif