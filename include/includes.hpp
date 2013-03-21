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

#ifndef INCLUDES_HPP
#define INCLUDES_HPP
/*
 * =====================================================================================
 *
 *       Filename:  includes.hpp
 *
 *    Description:  Contains all includes; define ALACARTE_USE_PRECOMPILED_HEADER to
 *					include all of them and use precompiled headers.
 *
 * =====================================================================================
 */

#ifdef ALACARTE_USE_PRECOMPILED_HEADER

// std includes
#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <cstdint>
#include <fstream>

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio.hpp>
#include <boost/unordered_map.hpp>
#include <boost/program_options.hpp>
#include <boost/thread/thread.hpp>
#include <boost/filesystem.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

// log4cpp
#include <log4cpp/Category.hh>
#include <log4cpp/Priority.hh>

#endif



#endif
