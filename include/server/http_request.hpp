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
#ifndef HTTP_REQUEST_HPP
#define HTTP_REQUEST_HPP

#include "settings.hpp"
#include <server/http/request_parser.hpp>
#include <array>

class HttpServer;
class Tile;
class RequestManager;

class HttpRequest : public boost::enable_shared_from_this<HttpRequest>, private boost::noncopyable
{
	friend class HttpRequestParser;
public:
	
	struct Header {
		std::string name;
		std::string value;
	};
	
	struct RequestData {
		std::string method;
		std::string uri;
		int http_version_major;
		int http_version_minor;
		std::vector<Header> headers;
	};
	
	class Reply
	{
	public:
		enum StatusType {
			ok = 200,
			created = 201,
			accepted = 202,
			no_content = 204,
			multiple_choices = 300,
			moved_permanently = 301,
			moved_temporarily = 302,
			not_modified = 304,
			bad_request = 400,
			unauthorized = 401,
			forbidden = 403,
			not_found = 404,
			internal_server_error = 500,
			not_implemented = 501,
			bad_gateway = 502,
			service_unavailable = 503,
		};
		/// The Status of the Reply
		StatusType status;
		/// The headers to be included in the Reply.
		std::vector<Header> headers;
		/// The content to be sent in the Reply.
		std::string content;
		/// The Tile to be sent in the Reply.
		shared_ptr<Tile> tile;
		/// Convert the Reply into a vector of buffers. The buffers do not own the
		/// underlying memory blocks, therefore the reply object must remain valid and
		/// not be changed until the write operation has completed.
		std::vector<boost::asio::const_buffer> toBuffers();
	};
	
public:
	explicit HttpRequest ( boost::asio::io_service &ioService, const shared_ptr<HttpServer>& server, const shared_ptr<RequestManager> &manager );

	TESTABLE const string& getURL() const;

	TESTABLE void answer ( const  shared_ptr<Tile>& tile, Reply::StatusType status = Reply::ok );
	TESTABLE void answer ( Reply::StatusType status );
	/// Start the first asynchronous operation for the connection.
	void startCollectingData();

	/// Stop all asynchronous operations associated with the connection.
	void close();

	boost::asio::ip::tcp::socket &getSocket();
private:
	/// Handle completion of a read operation.
	void handleRead ( const boost::system::error_code &e, std::size_t bytes_transferred );

	/// Handle completion of a write operation.
	void handleWrite ( const boost::system::error_code &e );

	void answer();
	bool checkifAnswered();
	void readSome();
	
protected:
	bool answered;
	/// The reply to be sent back to the client.
	Reply reply;
	RequestData data;
private:
	
	/// Socket for the connection.
	boost::asio::ip::tcp::socket socket;
	/// Buffer for incoming data.
	std::array<char, 8192> buffer;
	/// The parser for the incoming request.
	HttpRequestParser parser;
	
	weak_ptr<RequestManager> manager;
	weak_ptr<HttpServer> server;
};


#endif
