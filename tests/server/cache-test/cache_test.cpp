#include "includes.hpp"
#include "../../tests.hpp"
#include <boost/test/unit_test.hpp>
#include "configmockup.hpp"
#include <boost/filesystem.hpp>

#include "server/cache.hpp"

BOOST_AUTO_TEST_SUITE(cache_test)

struct cache_test
{
	cache_test() {
	}
	
	~cache_test() {
	}
	
	void test_default_tile() 
	{
		/*// Config with invalid path to default tile.
		char* argv[] = {(char*)"ala.carte", (char*)"ala.carte"};
		ConfigMockup* mock = new ConfigMockup();
		shared_ptr<Configuration> config = mock->Config(argv, 2);
		shared_ptr<Cache> cache = shared_ptr<Cache>(new Cache(config));
		BOOST_CHECK_THROW(shared_ptr<Tile> tile = cache->getDefaultTile(), excp);*/
		// Config with valid path
		char* argv[] = {(char*)"ala.carte", (char*)"ala.carte", (char*)"-t", (char*)"../tests/data/default.png"};
		ConfigMockup* mock = new ConfigMockup();
		shared_ptr<Configuration> config = mock->Config(argv, 4);
		shared_ptr<Cache> cache = shared_ptr<Cache>(new Cache(config));
		shared_ptr<Tile> tile;
		BOOST_CHECK_NO_THROW(tile = cache->getDefaultTile());
		BOOST_CHECK(tile->getIdentifier()->isDefaultIdentifier());
		BOOST_CHECK_EQUAL(tile->isRendered(), false);
	}
	
	void test_delete_tiles() {
		char* argv[] = {(char*)"ala.carte", (char*)"ala.carte"};
		ConfigMockup* mock = new ConfigMockup();
		shared_ptr<Configuration> config = mock->Config(argv, 2);
		shared_ptr<Cache> cache = shared_ptr<Cache>(new Cache(config));
		// Delete not existing cache.
		BOOST_CHECK_NO_THROW(cache->deleteTiles("nothing"));
		shared_ptr<TileIdentifier> ti = TileIdentifier::Create("/default/0/0/0.png");
		BOOST_CHECK_NO_THROW(cache->getTile(ti));
		// Delete existing cache.
		BOOST_CHECK_NO_THROW(cache->deleteTiles("/default"));
	}
	
	void test_get_tile() {
		char* argv[] = {(char*)"ala.carte", (char*)"ala.carte", (char*)"--server.cache-size", (char*)"10"};
		ConfigMockup* mock = new ConfigMockup();
		shared_ptr<Configuration> config = mock->Config(argv, 4);
		shared_ptr<Cache> cache = shared_ptr<Cache>(new Cache(config));
		// tileIdentifier is not in valid range, so it wont be prerendered.
		shared_ptr<TileIdentifier> ti1 = boost::make_shared<TileIdentifier>(200, 1, 1, "default", TileIdentifier::Format::PNG);
		Tile::ImageType image = boost::make_shared<Tile::ImageType::element_type>();
		// tries to read file that doesn't exist.
		if (boost::filesystem::exists(config->get<string>(opt::server::cache_path) + "/default/1-200-1.png")) {
			boost::filesystem::remove(config->get<string>(opt::server::cache_path) + "/default/1-200-1.png");
		}
		shared_ptr<Tile> tile;
		BOOST_CHECK_NO_THROW(tile = cache->getTile(ti1));
		// Check it has not loaded an image.
		BOOST_CHECK(tile->getImage() == 0);
		// Access cache already there and Tile already there.
		BOOST_CHECK_NO_THROW(cache->getTile(ti1));
		// Access a lot of tiles to bring tile ^ to evict.
		for(int i = 1; i < 20; i++) {
			std::stringstream path;
			path << "/default/15/1/" << i << ".png";
			shared_ptr<TileIdentifier> ti2 = TileIdentifier::Create(path.str().c_str());
			BOOST_CHECK_NO_THROW(cache->getTile(ti2));
		}
		// Main tile should not be evicted, because its not rendered yet.So check if theres no image on harddisk
		BOOST_CHECK(!boost::filesystem::exists(config->get<string>(opt::server::cache_path) + "/default/1-200-1.png"));
		image->push_back('a');
		tile->setImage(image);
		// One more time access many tile to evict main tile.
		for(int i = 1; i < 20; i++) {
			std::stringstream path;
			path << "/default/15/1/" << i << ".png";
			shared_ptr<TileIdentifier> ti2 = TileIdentifier::Create(path.str().c_str());
			BOOST_CHECK_NO_THROW(cache->getTile(ti2));
		}
		// Check if main tile has been written to harddrive.
		BOOST_CHECK(boost::filesystem::exists(config->get<string>(opt::server::cache_path) + "/default/1-200-1.png"));
		// Access a tile formerly evicted to hard drive. 
		BOOST_CHECK_NO_THROW(cache->getTile(ti1));
		// Change access rights on hdd
	}
};

ALAC_START_FIXTURE_TEST(cache_test)
	ALAC_FIXTURE_TEST_NAMED(test_default_tile, testOfTheDefaultTile);
	ALAC_FIXTURE_TEST_NAMED(test_delete_tiles, testToDeleteTilesOfAStylesheet);
	ALAC_FIXTURE_TEST_NAMED(test_get_tile, testToGetATile);
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END(/*cache_test*/)
