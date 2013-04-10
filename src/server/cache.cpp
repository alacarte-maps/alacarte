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

#include "includes.hpp"

#include "server/cache.hpp"

#include "general/configuration.hpp"
#include "server/tile_identifier.hpp"
#include "server/tile.hpp"
#include "server/stylesheet.hpp"

#include "utils/exceptions.hpp"

#include <boost/unordered_map.hpp>


Cache::Cache(const shared_ptr<Configuration>& config)
	: Config(config)
	, AllCaches()
	, RecentlyUsedList()
	, DefaultTile()
	, log(log4cpp::Category::getInstance("Cache"))
{
}

Cache::~Cache()
{
	log.debugStream() << "Cache destructed";
}

void Cache::readFile(const Tile::ImageType& image, const string& filename) {
	std::ifstream file;
	file.open(filename, std::ios::in | std::ios::binary);
	file.seekg(0, std::ios::end);
	std::streampos length(file.tellg());
	if (length == std::streampos(-1) || !file.is_open()) {
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException() << excp::InfoFileName(filename));
	} else if (length) {
		file.seekg(0, std::ios::beg);

		image->resize(static_cast<std::size_t>(length));
		image->assign(std::istreambuf_iterator<char>(file),
                    std::istreambuf_iterator<char>());
	}
}

void Cache::writeFile(shared_ptr<Tile> tile, const string& path) {
	std::ofstream out(path, std::ios::out | std::ios::binary);
	if(out.is_open())
	{
		Tile::ImageType png = tile->getImage();
		if (png==0) {
			out.close();
			boost::filesystem::path file(path);
			boost::filesystem::remove(file);
			BOOST_THROW_EXCEPTION(excp::InputFormatException());
		} else {
			auto size = png->size();
			out.write((const char*) png->data(), size);
		}
	} else {
		// e.g. Disk full
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException() << excp::InfoFileName(path));
	}
}

string Cache::identifierToPath(const shared_ptr<TileIdentifier>& ti)
{
	std::stringstream path;
	path << Config->get<string>(opt::server::cache_path) << "/"
		 << ti->getStylesheetPath() << "/"
		 << ti->getZoom() << "-" << ti->getX() << "-" << ti->getY();
	switch (ti->getImageFormat())
	{
		case TileIdentifier::Format::PNG:
			path << ".png";
		break;
		case TileIdentifier::Format::SVG:
			path << ".svg";
		break;
		default:
			log << log4cpp::Priority::DEBUG << "Unknown image format.";
	}

	return path.str();
}

/**
 * @brief Gets a Tile where the image data can be stored. If the Tile isn't cached a new Tile is returned.
 * 
 * @param ti A shared pointer to the TileIdentifier of the Tile.
 **/
shared_ptr<Tile> Cache::getTile(const shared_ptr<TileIdentifier>& ti)
{
	// TODO: Finer Synchronization
	GlobalCacheLock.lock();
	shared_ptr<CacheOfOneStylesheet> cache;
	const string& stylesheet = ti->getStylesheetPath();
	auto cacheIt = AllCaches.find(stylesheet);
	if (cacheIt != AllCaches.end()) {
		// Found cache for Stylesheet.
		cache = cacheIt->second;
	} else {
		// Creating a new cache for stylesheet.
		cache = boost::make_shared<CacheOfOneStylesheet>();
		AllCaches[stylesheet] = cache;
		boost::filesystem::path dir(Config->get<string>(opt::server::cache_path) + "/" + ti->getStylesheetPath());
		boost::filesystem::create_directories(dir);
		log << log4cpp::Priority::DEBUG << "Stylesheetcache " << stylesheet << " created.";
	}
	// Get tile from map
	shared_ptr<Tile> tile;
	auto tileIt = cache->find(*ti);
	if (tileIt != cache->end()) {
		// Cache hit
		tile = tileIt->second.first;
		RecentlyUsedList.erase(tileIt->second.second);
		RecentlyUsedList.push_front(tileIt->second.first);
		tileIt->second.second = RecentlyUsedList.begin();
	}  else {
		// Cache miss
		tile = boost::make_shared<Tile>(ti);
		if (ti->getZoom() <= Config->get<int>(opt::server::cache_keep_tile)) {
			// Try to load prerendered image data from file.
			Tile::ImageType image = boost::make_shared<Tile::ImageType::element_type>();
			string path = identifierToPath(ti);
			try {
				readFile(image, path.c_str());
				tile->setImage(image);
			} catch (excp::FileNotFoundException) {
				log << log4cpp::Priority::DEBUG << "readFile: Not found: " << path.c_str();
			}
		}
		RecentlyUsedList.push_front(tile);
		cache->insert(std::make_pair(*ti, CacheElement(tile, RecentlyUsedList.begin())));
	}
	while (RecentlyUsedList.size() > Config->get<int>(opt::server::cache_size)) {
		shared_ptr<Tile> tileToDelete = RecentlyUsedList.back();
		// Evict a Tile when cache is full.
		if (tileToDelete->getIdentifier()->getZoom() <= Config->get<int>(opt::server::cache_keep_tile)) {
			// Evict to hard drive.
			shared_ptr<TileIdentifier> tiToDelete = tileToDelete->getIdentifier();
			string path = identifierToPath(tiToDelete);
			try {
				writeFile(tileToDelete, path.c_str());
			} catch (excp::FileNotFoundException) {
				log << log4cpp::Priority::DEBUG << "WriteFile: Could not open file " << path;
				// Disk is full
			} catch (excp::InputFormatException) {
				log << log4cpp::Priority::DEBUG << "WriteFile: PNG not yet rendered " << *tile->getIdentifier();
				RecentlyUsedList.push_front(tileToDelete);
				cacheIt = AllCaches.find(tileToDelete->getIdentifier()->getStylesheetPath());
				if (cacheIt != AllCaches.end()) {
					cacheIt->second->erase(*tileToDelete->getIdentifier());
					cacheIt->second->insert(std::make_pair(*tiToDelete, CacheElement(tileToDelete, RecentlyUsedList.begin())));
				}
				RecentlyUsedList.pop_back();
				break;
			}
		}
		// Delete tile
		log << log4cpp::Priority::DEBUG << "Deleting least recently used Tile." << *tileToDelete->getIdentifier();
		cacheIt = AllCaches.find(tileToDelete->getIdentifier()->getStylesheetPath());
		if (cacheIt != AllCaches.end()) {
			cacheIt->second->erase(*tileToDelete->getIdentifier());
		}
		RecentlyUsedList.pop_back();
	}
	GlobalCacheLock.unlock();
	return tile;
}

/**
 * @brief Get the default tile used for error and such.
 * 
 * @return shared_ptr to the default Tile with loaded png (image can be null if file not found).
 **/
shared_ptr<Tile> Cache::getDefaultTile() {
	if (!DefaultTile) {
		string path = Config->get<string>(opt::server::path_to_default_tile);
		shared_ptr<TileIdentifier> ti = boost::make_shared<TileIdentifier>(-1, -1, -1, "/", TileIdentifier::Format::PNG);
		DefaultTile = boost::make_shared<Tile>(ti);
		// Load default tile
		Tile::ImageType image = boost::make_shared<Tile::ImageType::element_type>();
		try {
			readFile(image, path);
			DefaultTile->setImage(image);
		} catch (excp::FileNotFoundException) {

		}
	}
	return DefaultTile;
}

/**
 * @brief Deletes all cached Tiles of the Stylesheet with the given path.
 * 
 * @param path The path to the stylesheet.
 **/
void Cache::deleteTiles(const string path)
{
	try {
		boost::filesystem::path dir(Config->get<string>(opt::server::cache_path) + path);
		boost::system::error_code ec;
		boost::filesystem::remove_all(dir, ec);
		if (ec.value() == 39) {
			log << log4cpp::Priority::WARN << "could not delete all tiles in folder.";
		}
		AllCaches.erase(path);
	} catch (std::out_of_range) {
		log << log4cpp::Priority::WARN << "trying to delete Tiles of non existant Stylesheet.";
	}
}

