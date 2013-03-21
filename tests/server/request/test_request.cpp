#include "includes.hpp"
#include "settings.hpp"
#include "../../tests.hpp"
#include "../../shared/compare.hpp"
#include "../../shared/test_config.hpp"

#include "general/geodata.hpp"
#include "server/http_server.hpp"
#include "server/http_request.hpp"
#include "server/cache.hpp"
#include "server/renderer/renderer.hpp"
#include "server/stylesheet_manager.hpp"
#include <server/request_manager.hpp>
#include "../non_prerendering_request_manager.hpp"

BOOST_AUTO_TEST_SUITE(test_request);

struct test_request
{
    boost::shared_ptr<NonPrerenderingRequestManager> req_manager;
	TestConfig::Ptr DefaultConfig;
    shared_ptr<boost::thread> thread;
	shared_ptr<HttpServer> server;
	shared_ptr<StylesheetManager> ssm;
	// setup
    test_request()
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
        server = boost::make_shared<HttpServer>(DefaultConfig, req_manager);
		ssm->startStylesheetObserving(req_manager);

		thread = boost::make_shared<boost::thread>(boost::bind(&HttpServer::listen, server));
		
		//wait for listen to listen
		boost::this_thread::sleep(boost::posix_time::milliseconds(100));
	}

	int sendRequest(std::string path, bool verbose = false)
	{
		boost::asio::io_service io_service;
		using boost::asio::ip::tcp;
		// Get a list of endpoints corresponding to the server name.
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(DefaultConfig->get<string>(opt::server::server_address), DefaultConfig->get<string>(opt::server::server_port));
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
		tcp::resolver::iterator end;
		
		// Try each endpoint until we successfully establish a connection.
		tcp::socket socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		while (error && endpoint_iterator != end)
		{
			socket.close();
			socket.connect(*endpoint_iterator++, error);
		}
		if (error){
			std::cout << "error: " << error.message() << "\n";
			throw boost::system::system_error(error);
		}
		
		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "GET " << path << " HTTP/1.0\r\n";
		request_stream << "Host: " << DefaultConfig->get<string>(opt::server::server_address) << "\r\n";
		request_stream << "Accept: */*\r\n";
		request_stream << "Connection: close\r\n\r\n";
		
		// Send the request.
		boost::asio::write(socket, request);
		
		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n");
		
		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			if(verbose)
				std::cout << "Invalid response\n";
			return -1;
		}
		if (status_code != 200)
		{
			if(verbose)
				std::cout << "Response returned with status code " << status_code << "\n";
			return status_code;
		}
		
		// Read the response headers, which are terminated by a blank line.
		boost::asio::read_until(socket, response, "\r\n\r\n");
		
		// Process the response headers.
		std::string header;
		while (std::getline(response_stream, header) && header != "\r") {
			if(verbose)
				std::cout << header << "\n";
		}
		if(verbose)
			std::cout << "\n";
		// Write whatever content we already have to output.
		if (verbose && response.size() > 0)
			std::cout << &response;
		
		// Read until EOF, writing data to output as we go.
		while (boost::asio::read(socket, response, boost::asio::transfer_at_least(1), error)) {
			if(verbose)
				std::cout << &response;
		}

		if (error != boost::asio::error::eof) {
			std::cout << "error != boost::asio::error::eof";
			throw boost::system::system_error(error);
		}
		
		return status_code;
	}
	
	void check_requestTile(std::string url, HttpRequest::Reply::StatusType responseStatus = HttpRequest::Reply::ok)
    {
		int result = -2;
		BOOST_CHECK_NO_THROW((result = sendRequest(url)));
		BOOST_CHECK_EQUAL(result, (int)responseStatus);
		
    }
	
	// teardown
    ~test_request()
	{
		server->quit();
		req_manager->stop();
		ssm->stopStylesheetObserving();
	}
};

ALAC_START_FIXTURE_TEST(test_request);
	// functionname, name of test, arguments of function...
	ALAC_FIXTURE_TEST(check_requestTile, "default/13/4289/2813.png");
	ALAC_FIXTURE_TEST(check_requestTile, "default/13/4286/2812.png");
	ALAC_FIXTURE_TEST(check_requestTile, "Invalid/Url", HttpRequest::Reply::forbidden);
	ALAC_FIXTURE_TEST(check_requestTile, "(Space)Invalid Request because SPACES", HttpRequest::Reply::bad_request);
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.png");
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.gif", HttpRequest::Reply::not_implemented);
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.jpg", HttpRequest::Reply::not_implemented);
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.jpeg", HttpRequest::Reply::not_implemented);
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.svg");
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.svgz", HttpRequest::Reply::not_implemented);
	ALAC_FIXTURE_TEST(check_requestTile, "default/15/17148/11253.somefile", HttpRequest::Reply::not_implemented);
ALAC_END_FIXTURE_TEST();

BOOST_AUTO_TEST_SUITE_END();
