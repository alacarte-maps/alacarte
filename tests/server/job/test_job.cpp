#include "includes.hpp"
#include "settings.hpp"
#include "../../tests.hpp"
#include "../../shared/compare.hpp"
#include "../../shared/test_config.hpp"

#include <server/jobs/pre_render_job.hpp>
#include <server/stylesheet.hpp>
#include <server/tile_identifier.hpp>
#include <server/tile.hpp>
#include <general/geodata.hpp>
#include <server/cache.hpp>
#include <server/stylesheet_manager.hpp>
#include <server/renderer/renderer.hpp>
#include <server/request_manager.hpp>
#include <utils/rect.hpp>
#include "../non_prerendering_request_manager.hpp"

class TestJob : public PreRenderJob {
public:
	TestJob(const shared_ptr<Configuration>& config, const shared_ptr<TileIdentifier>& tileIdentifier, const shared_ptr<RequestManager>& manager)
	: PreRenderJob(config, tileIdentifier, manager)
	{}
	virtual ~TestJob() {};
	
	shared_ptr<Stylesheet> check_getStylesheet(const shared_ptr<TileIdentifier>& orginalTI, shared_ptr<TileIdentifier>& newti)
	{
		return getStylesheet(orginalTI, newti);
	}
	
	FixedRect check_computeRect(const shared_ptr<TileIdentifier>& ti, FixedRect expected)
	{
		return computeRect(ti);
	}
	
	shared_ptr<Tile> check_computeTile(const shared_ptr<TileIdentifier>& ti)
	{
		return computeTile(ti);
	}
	shared_ptr<Tile> check_computeTileNoneData(const shared_ptr<TileIdentifier>& ti)
	{
		return computeTileNoneData(ti);
	}
	void setTileIdentifier(const shared_ptr<TileIdentifier>& ti)
	{
		tileIdentifier = ti;
	}
};

BOOST_AUTO_TEST_SUITE(test_job)

struct test_job
{
	shared_ptr<NonPrerenderingRequestManager> req_manager;
	shared_ptr<StylesheetManager> ssm;
	TestConfig::Ptr DefaultConfig;
	// setup
	test_job()
	{
		DefaultConfig = TestConfig::Create()
		->add<string>(opt::server::path_to_geodata, 	(getTestDirectory() / "/input/karlsruhe_big.carte").string());
		
		shared_ptr<Geodata> geodata = boost::make_shared<Geodata>();
		BOOST_CHECK(boost::filesystem::exists(DefaultConfig->get<string>(opt::server::path_to_geodata)));
		geodata->load(DefaultConfig->get<string>(opt::server::path_to_geodata));
		shared_ptr<Cache> cache = boost::make_shared<Cache>(DefaultConfig);
		ssm = boost::make_shared<StylesheetManager>(DefaultConfig);
		shared_ptr<Renderer> renderer = boost::make_shared<Renderer>(geodata);
		req_manager = boost::make_shared<NonPrerenderingRequestManager>(DefaultConfig, geodata, renderer, cache, ssm);
		ssm->startStylesheetObserving(req_manager);
	}

	void check_getStylesheet(string expected, shared_ptr<TileIdentifier> orginalTI, string setDefaultStylesheet = "")
	{
		if (setDefaultStylesheet != "") {
			DefaultConfig->add<string>(opt::server::path_to_default_style, setDefaultStylesheet);
		}
		TestJob job(DefaultConfig, orginalTI, req_manager);
		shared_ptr<TileIdentifier> newti;
		job.check_getStylesheet(orginalTI, newti);
		BOOST_CHECK_EQUAL(expected, newti->getStylesheetPath());
	}
	
	void check_computeRect(FixedRect expected, const shared_ptr<TileIdentifier>& ti)
	{
		TestJob job(DefaultConfig, ti, req_manager);
		FixedRect computed = job.check_computeRect(ti, expected);
		BOOST_CHECK_EQUAL(expected.maxX, computed.maxX);
		BOOST_CHECK_EQUAL(expected.minX, computed.minX);
		BOOST_CHECK_EQUAL(expected.maxY, computed.maxY);
		BOOST_CHECK_EQUAL(expected.minY, computed.minY);
	}
	
	void check_computeTileIsRendered(const shared_ptr<TileIdentifier>& ti)
	{
		TestJob job(DefaultConfig, ti, req_manager);
		BOOST_CHECK(job.check_computeTile(ti)->isRendered());
	}

	void check_computeTileNoneData(const shared_ptr<TileIdentifier>& ti)
	{
		TestJob job(DefaultConfig, ti, req_manager);
		BOOST_CHECK(job.check_computeTile(ti)->isRendered());
	}
	
	void check_preRendering(bool expectPrerendering, int preRenderLevel, const shared_ptr<TileIdentifier>& ti)
	{
		req_manager->pseudoQueueSize = 0;
		DefaultConfig->add<int>(opt::server::prerender_level, preRenderLevel);
		TestJob job(DefaultConfig, ti, req_manager);
		job.setTileIdentifier(ti);
		job.process();
		BOOST_CHECK_EQUAL(expectPrerendering, req_manager->pseudoQueueSize > 0);
		BOOST_CHECK_EQUAL(expectPrerendering, !job.prerenderingFinished());
	}
	
	// teardown
	~test_job()
	{
		req_manager->stop();
		ssm->stopStylesheetObserving();
	}
};

ALAC_START_FIXTURE_TEST(test_job)
	// functionname, name of test, arguments of function...
	ALAC_FIXTURE_TEST(check_getStylesheet, "default", 	boost::make_shared<TileIdentifier>(34297,22506,16,"invalidStyle", 	TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_getStylesheet, "default",	boost::make_shared<TileIdentifier>(34297,22506,16,"default", 		TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_getStylesheet, "wire",		boost::make_shared<TileIdentifier>(34297,22506,16,"wire", 			TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_getStylesheet, ".fallback", boost::make_shared<TileIdentifier>(34297,22506,16,"invalidStyle", 	TileIdentifier::PNG), "InvalidDefault");
	
	ALAC_FIXTURE_TEST(check_computeTileIsRendered, boost::make_shared<TileIdentifier>(34297,22506,16,"invalidStyle", 	TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_computeTileIsRendered, boost::make_shared<TileIdentifier>(34297,22506,16,"default", 		TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_computeTileIsRendered, boost::make_shared<TileIdentifier>(34297,22506,16,"wire", 			TileIdentifier::PNG));
	
	ALAC_FIXTURE_TEST(check_computeTileNoneData, boost::make_shared<TileIdentifier>(34297,22506,16,"invalidStyle", 	TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_computeTileNoneData, boost::make_shared<TileIdentifier>(34297,22506,16,"default", 		TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_computeTileNoneData, boost::make_shared<TileIdentifier>(34297,22506,16,"wire", 			TileIdentifier::PNG));

	ALAC_FIXTURE_TEST(check_computeRect, FixedRect(14878199, 99860335, 14892798, 99874933), boost::make_shared<TileIdentifier>(34297,22506,16, "-", TileIdentifier::PNG));
	
	ALAC_FIXTURE_TEST(check_preRendering, true,  12, boost::make_shared<TileIdentifier>(1071,703,11, "-", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_preRendering, false, 12, boost::make_shared<TileIdentifier>(2143,1406,12, "-", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST(check_preRendering, false, 12, boost::make_shared<TileIdentifier>(1073,703,11, "-", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END()
