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

#pragma once
#ifndef HTTP_SERVER_HPP
#define HTTP_SERVER_HPP

#include "settings.hpp"

#include <set>
#include <boost/asio.hpp>

class Configuration;
class RequestManager;
class HttpRequest;

/**
 * @brief A HttpServer waits for connections and creates HttpRequests.
 **/
class HttpServer : public boost::enable_shared_from_this<HttpServer>
{
public:
	HttpServer ( const shared_ptr<Configuration>& config,  const shared_ptr<RequestManager>& manager );

	TESTABLE void listen();
	TESTABLE void quit();
	void stopRequest ( shared_ptr<HttpRequest> request );

private:
	void start_accept();
	void handle_accept ( const boost::system::error_code &e );

private:
	/// The io_service used to perform asynchronous operations.
	boost::asio::io_service io_service;

	/// The signal_set is used to register process termination notifications.
	boost::asio::signal_set signals;

	/// Acceptor used to listen for incoming requests.
	boost::asio::ip::tcp::acceptor acceptor;

	/// The next connection to be accepted.
	shared_ptr<HttpRequest> nextRequest;

	/// The managed requests.
	std::set<shared_ptr<HttpRequest>> requests;

	shared_ptr<Configuration> config;
	shared_ptr<RequestManager> manager;
};

#endif
