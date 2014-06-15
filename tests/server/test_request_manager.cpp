
#include "settings.hpp"
#include "../tests.hpp"
#include "../shared/compare.hpp"
#include "../shared/test_config.hpp"

#include "server/request_manager.hpp"
#include "server/meta_identifier.hpp"
#include "server/http_request.hpp"
#include "server/tile_identifier.hpp"
#include "server/cache.hpp"
#include "server/stylesheet_manager.hpp"
#include "general/geodata.hpp"
#include "server/renderer/renderer.hpp"
#include "server/http_server.hpp"

BOOST_AUTO_TEST_SUITE(test_requestManage)
class TestHttpRequest : public HttpRequest
{
public:
	explicit TestHttpRequest (std::string uri, boost::asio::io_service &ioService, const std::shared_ptr< HttpServer >& server, const shared_ptr< RequestManager >& manager )
	: HttpRequest(ioService, server, manager)
	{
		data.uri = uri;
	}
	void answer ( const  std::shared_ptr<Tile>& tile, Reply::StatusType status = Reply::ok )
	{
		reply.status = status;
		answered = true;
	};
	void answer ( Reply::StatusType status )
	{
		reply.status = status;
		answered = true;
	};
	bool isAnswered() {return answered;}
	HttpRequest::Reply& getReply()
	{
		return reply;
	}
};

struct test_requestManage
{
	std::shared_ptr<RequestManager> req_manager;
	TestConfig::Ptr DefaultConfig;
	std::shared_ptr<Cache> cache;
	std::shared_ptr<HttpServer> server;
	std::shared_ptr<StylesheetManager> ssm;
	// setup
	test_requestManage()
	{
		DefaultConfig = TestConfig::Create()
		->add<int>(opt::server::max_queue_size, 1)
		->add<int>(opt::server::prerender_level, 0)
		->add<string>(opt::server::path_to_geodata, 	(getInputDirectory() / "karlsruhe_big.carte").string());

		Statistic::Init(DefaultConfig);
		
		std::shared_ptr<Geodata> geodata = std::make_shared<Geodata>();
		BOOST_CHECK(boost::filesystem::exists(DefaultConfig->get<string>(opt::server::path_to_geodata)));
		geodata->load(DefaultConfig->get<string>(opt::server::path_to_geodata));
		cache = std::make_shared<Cache>(DefaultConfig);
		ssm = std::make_shared<StylesheetManager>(DefaultConfig);
		std::shared_ptr<Renderer> renderer = std::make_shared<Renderer>(geodata);
		req_manager = std::make_shared<RequestManager>(DefaultConfig, geodata, renderer, cache, ssm);
		server = std::make_shared<HttpServer>(DefaultConfig, req_manager);
		ssm->startStylesheetObserving(req_manager);
		boost::this_thread::sleep(boost::posix_time::milliseconds(2000));
	}
	
	void enqueueHttpRequest()
	{
		//enqueue more request than the server can handle, so it has to reply with service_unavailable
		boost::asio::io_service service;
		DefaultConfig->add<int>(opt::server::max_queue_size, 1);
		std::shared_ptr<TestHttpRequest> request = std::make_shared<TestHttpRequest>("default/15/17150/11253.png", service, server, req_manager);
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17143/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17144/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17145/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17146/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17147/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17148/11253.png", service, server, req_manager));
		req_manager->enqueue(std::make_shared<TestHttpRequest>("default/15/17149/11253.png", service, server, req_manager));
		req_manager->enqueue(request);
		BOOST_CHECK(request->isAnswered());
		BOOST_CHECK_EQUAL(request->getReply().status, HttpRequest::Reply::service_unavailable);
	}
	
	void isPrerendered()
	{
		//prerender a Tile and check if the (child?) Tiles are prerendered.
		DefaultConfig->add<int>(opt::server::prerender_level, 18);
		DefaultConfig->add<string>(opt::server::performance_log, 	"unitTest_Performance_log.delete_me");

		int x = 68595;
		int y = 45006;
		int z = 17;
		std::shared_ptr<TileIdentifier> ti = std::make_shared<TileIdentifier>(x,y,z,"default", 	TileIdentifier::PNG);
		std::shared_ptr<MetaIdentifier> mid = MetaIdentifier::Create(ti);

		req_manager->enqueue(mid);
		//wait for prerendering
		boost::this_thread::sleep(boost::posix_time::milliseconds(5000));

		//now ask for a prerenderd Tile
		std::vector<std::shared_ptr<MetaIdentifier>> children;
		mid->getSubIdentifiers(children);
		for (auto& c : children)
		{
			for (auto& id : c->getIdentifiers()) {
				auto tile = cache->getTile(id);
				BOOST_CHECK(tile->isRendered());
			}
		}
	}
	// teardown
	~test_requestManage()
	{
		server->quit();
		req_manager->stop();
		ssm->stopStylesheetObserving();
	}
};

ALAC_START_FIXTURE_TEST(test_requestManage)
	// functionname, name of test, arguments of function...
	ALAC_FIXTURE_TEST(isPrerendered);
	ALAC_FIXTURE_TEST(enqueueHttpRequest);
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END()
