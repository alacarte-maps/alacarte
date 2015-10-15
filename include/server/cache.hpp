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
 *  Maintainer: Simon Dreher
 */

#pragma once
#ifndef CACHE_HPP
#define CACHE_HPP

#include "settings.hpp"

#include <boost/thread/mutex.hpp>
#include <boost/unordered_map.hpp>
#include <boost/filesystem.hpp>
#include <list>

#include "server/tile_identifier.hpp"
#include "server/tile.hpp"

class Configuration;
class Stylesheet;

class Cache
{
public:
  
	/**
	 * @brief A list that will store the last recently used tiles.
	 **/
	typedef std::list<shared_ptr<Tile>> TileList;
	
	/**
	 * @brief An element stored in the cache. Consists of shared_ptr to the Tile and an iterator pointing to the element in the least recently used list.
	 **/
	typedef std::pair<shared_ptr<Tile>, TileList::iterator> CacheElement;
	
	/**
	 * @brief HashMap with TileIdentifier as key and shared_ptr to Tiles as value.
	 **/
	typedef boost::unordered_map<TileIdentifier, CacheElement> CacheOfOneStylesheet;
	
	/**
	 * @brief HashMap with path to MapCSS as key and shared_ptr to CacheOfOneStylesheet as value.
	 **/
	typedef boost::unordered_map<string, shared_ptr<CacheOfOneStylesheet> > CacheContainer;
	
	
	Cache(const shared_ptr<Configuration>& config);
	
	TESTABLE shared_ptr<Tile> getTile(const shared_ptr<TileIdentifier>& tl);
	TESTABLE shared_ptr<Tile> getDefaultTile();
	TESTABLE void deleteTiles(const string path);

private:
	shared_ptr<Configuration> Config;
	boost::mutex GlobalCacheLock;
	CacheContainer AllCaches;
	TileList RecentlyUsedList;
	shared_ptr<Tile> DefaultTile;
	void readFile(const Tile::ImageType& image, const boost::filesystem::path& filename);
	void writeFile(shared_ptr<Tile> tile, const boost::filesystem::path& filename);
	const boost::filesystem::path getTilePath(const shared_ptr<TileIdentifier>& ti);
};

#endif
