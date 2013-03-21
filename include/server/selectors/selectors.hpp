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
 *  Maintainer: Florian Jacob
 */

#pragma once
#ifndef SELECTORS_HPP
#define SELECTORS_HPP
/*
 * =====================================================================================
 *
 *       Filename:  selectors.hpp
 *
 *    Description:  This header file servers as a shortcut to include all existing selectors
 *
 * =====================================================================================
 */

#include "settings.hpp"

#include "selector.hpp"


#include "line_selector.hpp"
#include "area_selector.hpp"

#include "childnodes_selector.hpp"
#include "childways_selector.hpp"

#include "hastag_selector.hpp"
#include "hasnottag_selector.hpp"

#include "tagequals_selector.hpp"
#include "tagunequals_selector.hpp"
#include "tagmatches_selector.hpp"

#include "taglarger_selector.hpp"
#include "taglargerequals_selector.hpp"
#include "tagsmaller_selector.hpp"
#include "tagsmallerequals_selector.hpp"

#include "apply_selector.hpp"

#endif
