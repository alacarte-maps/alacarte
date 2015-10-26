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
 *  Maintainer: Bernhard Scheirle
 */



#include <boost/thread.hpp>
#include "server/http_server.hpp"
#include "server/request_manager.hpp"
#include "server/http_request.hpp"
#include <general/configuration.hpp>

/**
 * @brief Creates a new HttpServer with the given Configuration and RequestManager
 *
 * @param config The Configuration which should be used
 * @param manager The RequestManager in which the HttpRequest should be enqueued.
 **/
HttpServer::HttpServer( const shared_ptr<Configuration>& config, const shared_ptr<RequestManager>& manager )
	: config(config)
	, manager(manager)
	, io_service()
	, signals(io_service)
	, acceptor(io_service)
{
	// Register to handle the signals that indicate when the server should exit.
	// It is safe to register for the same signal multiple times in a program,
	// provided all registration for the specified signal is made through Asio.
	signals.add(SIGINT);
	signals.add(SIGTERM);
	#if defined(SIGQUIT)
		signals.add(SIGQUIT);
	#endif // defined(SIGQUIT)
	signals.async_wait(boost::bind(&HttpServer::quit, this));
}


/**
 * @brief The HttpServer starts listening for new connections. This function will never return (Blocking call).
 **/
void HttpServer::listen()
{
	// Open the acceptor with the option to reuse the address (i.e. SO_REUSEADDR).
	boost::asio::ip::tcp::resolver resolver(io_service);
	boost::asio::ip::tcp::resolver::query query(
												config->get<string>(opt::server::server_address), 
												config->get<string>(opt::server::server_port)
											   );
	boost::asio::ip::tcp::endpoint endpoint = *resolver.resolve(query);
	acceptor.open(endpoint.protocol());
	acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
	try {
		acceptor.bind(endpoint);
	} catch (boost::system::system_error ex) {
		LOG_SEV(server_log, error) << "Invalid host name: " << ex.what()
							<< " (" << config->get<string>(opt::server::server_address) << ":"
							<< config->get<string>(opt::server::server_port) << ")";
		return;
	}
	acceptor.listen();
	start_accept();

	LOG_SEV(server_log, info) << "Server is now waiting for new connections ...";
	LOG_SEV(server_log, info) << "Send a \"SIGINT\", \"SIGTERM\" or \"SIGQUIT\" signal to shut down the server. (ctrl-c)";
	// The io_service::run() call will block until all asynchronous operations
	// have finished. While the server is running, there is always at least one
	// asynchronous operation outstanding: the asynchronous accept call waiting
	// for new incoming connections.
	io_service.run();
}

/**
 * @brief Stops a HttpRequest and deletes it from the request list.
 *
 * @param request The HttpRequest which should be stopped.
 **/
void HttpServer::stopRequest ( shared_ptr<HttpRequest> request ) {
	requests.erase(request);
	request->close();
}

/**
 * @brief Prepares the HttpServer for a new connection and waits asynchronous for it.
 **/
void HttpServer::start_accept()
{
	nextRequest.reset(new HttpRequest(io_service, this->shared_from_this(), manager));
	acceptor.async_accept(nextRequest->getSocket(), boost::bind(&HttpServer::handle_accept, this, boost::asio::placeholders::error));
}
/**
 * @brief Called when a new client connects to the server.
 **/
void HttpServer::handle_accept(const boost::system::error_code& e)
{
	// Check whether the server was stopped by a signal before this completion
	// handler had a chance to run.
	if (!acceptor.is_open())
	{
		return;
	}
	
	if (!e)
	{
		requests.insert(nextRequest);
		nextRequest->startCollectingData();
	}
	
	start_accept();
}

/**
 * @brief	Cleans up the HttpServer.
 * 			Called when the HttpServer should be terminated.
 **/
void HttpServer::quit()
{
	// The server is stopped by cancelling all outstanding asynchronous
	// operations. Once all operations have finished the io_service::run() call
	// will exit.
	acceptor.close();

	//Stop all open Requests
	std::for_each(requests.begin(), requests.end(), boost::bind(&HttpRequest::close, _1));
	requests.clear();

	Statistic::Get()->printStatistic();
}

