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
#ifndef META_TILE_HPP
#define META_TILE_HPP

#include "settings.hpp"

#include <cairomm/surface.h>

class MetaIdentifier;

class MetaTile
{
public:
	MetaTile(const shared_ptr<MetaIdentifier>& mid);

	TESTABLE const shared_ptr<MetaIdentifier>& getIdentifier() const;
	TESTABLE const Cairo::RefPtr<Cairo::Surface>& getData() const;
	TESTABLE void setData(const Cairo::RefPtr<Cairo::Surface>& surface);

private:
	shared_ptr<MetaIdentifier> mid;
	Cairo::RefPtr<Cairo::Surface> surface;
};

#endif
