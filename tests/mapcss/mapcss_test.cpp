
#include "../tests.hpp"

#include "server/style.hpp"
#include "server/stylesheet.hpp"
#include "server/stylesheet_manager.hpp"
#include "server/tile_identifier.hpp"
#include "server/render_attributes.hpp"
#include "general/geodata.hpp"

#include <iostream>

struct MapCSSTest
{
	shared_ptr<Geodata> data;

	// setup
	MapCSSTest()
	{
		data = boost::make_shared<Geodata>();
	}

	// actual test
	void test() {
		// messages only appear with log levelg --log_level=message and below
		// http://www.boost.org/doc/libs/1_45_0/libs/test/doc/html/utf/user-guide/runtime-config/reference.html
		BOOST_TEST_MESSAGE("Alles ist gut.");
		BOOST_CHECK(true);
	}

	void testOvermerge() {
		Style style1;
		shared_ptr<Style> style2;
		//TODO: init style1 and style2 with default values, and then change some
		//style1.overmerge(style2);
		BOOST_CHECK(true);
	}

	void testLoadStylesheet() {
		shared_ptr<Stylesheet> stylesheet = Stylesheet::Load(string("test/stylesheet.mapcss"), data, 1000);
	}

	void testMatch() {
		shared_ptr<Stylesheet> stylesheet = Stylesheet::Load(string("test/stylesheet.mapcss"), data, 1000);

		shared_ptr<std::vector<NodeId> > nodeIDs(new std::vector<NodeId>());
		shared_ptr<std::vector<WayId> > wayIDs(new std::vector<WayId>());
		shared_ptr<std::vector<RelId> > relIDs(new std::vector<RelId>());
		shared_ptr<TileIdentifier> tileId(new TileIdentifier(0, 0, 2, string("path"), TileIdentifier::Format::PNG));
		RenderAttributes* attributes;

		stylesheet->match(nodeIDs, wayIDs, relIDs, tileId, attributes);
	}

	void testStartStylesheetObserving() {
		//TODO: pass zoomlevel and stylesheet directory here
		shared_ptr<Configuration> config;
		//TODO: create mock request manager
		shared_ptr<RequestManager> requestManager;
		StylesheetManager stylesheet_manager(config);
		stylesheet_manager.startStylesheetObserving(requestManager);
	}

	void testGetStylesheet() {
		// pass zoomlevel and stylesheet directory here
		shared_ptr<Configuration> config;
		shared_ptr<RequestManager> requestManager;
		StylesheetManager stylesheetManager(config);
		stylesheetManager.startStylesheetObserving(requestManager);
		shared_ptr<TileIdentifier> tileId(new TileIdentifier(0, 0, 2, string("path"), TileIdentifier::Format::PNG));
		shared_ptr<Stylesheet> stylesheet = stylesheetManager.getStylesheet(tileId);
	}

	// teardown
	~MapCSSTest()
	{
	}
};

ALAC_START_FIXTURE_TEST(MapCSSTest)
	// functionname, name of test, arguments of function...
	ALAC_FIXTURE_TEST_NAMED(test, MapCSS)
ALAC_END_FIXTURE_TEST()
