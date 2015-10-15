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


#include <boost/lexical_cast.hpp>

#include "server/http_request.hpp"
#include "server/tile.hpp"
#include "server/tile_identifier.hpp"
#include "server/request_manager.hpp"
#include "server/http_server.hpp"
#include "server/cache.hpp"

HttpRequest::HttpRequest ( boost::asio::io_service &ioService, const shared_ptr<HttpServer>& server, const shared_ptr<RequestManager> &manager )
	: socket ( ioService )
	, server ( server )
	, manager ( manager )
	, answered (false)
{

};

void HttpRequest::startCollectingData()
{
	readSome();
}

void HttpRequest::close()
{
	socket.close();
}
void HttpRequest::readSome()
{
	socket.async_read_some ( boost::asio::buffer ( buffer ),
							 boost::bind(&HttpRequest::handleRead, shared_from_this(),
										 boost::asio::placeholders::error,
										 boost::asio::placeholders::bytes_transferred 
										) 
						   );
}
void HttpRequest::handleRead ( const boost::system::error_code &e, std::size_t bytes_transferred )
{
	shared_ptr<RequestManager> manager = this->manager.lock();
	shared_ptr<HttpServer> server = this->server.lock();
	assert(manager && server);

	if ( !e ) {
		boost::tribool result;
		boost::tie ( result, boost::tuples::ignore ) = parser.parse ( shared_from_this(), buffer.data(), buffer.data() + bytes_transferred );

		if ( result ) {
			manager->enqueue ( shared_from_this() );
		} else if ( !result ) {
			if (checkifAnswered()) return;
			answer ( HttpRequest::Reply::bad_request );
		} else {
			readSome();
		}
	} else if ( e != boost::asio::error::operation_aborted ) {
		server->stopRequest ( shared_from_this() );
	}

}

void HttpRequest::handleWrite ( const boost::system::error_code &e )
{
	shared_ptr<HttpServer> server = this->server.lock();
	assert(server);

	if ( !e ) {
		// Initiate graceful connection closure.
		boost::system::error_code ignored_ec;
		socket.shutdown ( boost::asio::ip::tcp::socket::shutdown_both, ignored_ec );
	}

	if ( e != boost::asio::error::operation_aborted ) {
		server->stopRequest ( shared_from_this() );
	}
}

const string &HttpRequest::getURL() const
{
	return data.uri;
}

boost::asio::ip::tcp::socket &HttpRequest::getSocket()
{
	return socket;
}

bool HttpRequest::checkifAnswered()
{
	if (answered) {
		LOG_SEV(server_log, error) << "Tried to answer an already answered HttpRequest: " << getURL();
	}
	return answered;
}

void HttpRequest::answer()
{
	answered = true;
	boost::asio::async_write ( socket, reply.toBuffers(),
							   boost::bind ( &HttpRequest::handleWrite, shared_from_this(),
									   boost::asio::placeholders::error ) );
}

void HttpRequest::answer ( Reply::StatusType status )
{
	shared_ptr<RequestManager> manager = this->manager.lock();
	assert(manager);

	answer(manager->getCache()->getDefaultTile(), status);
}

void HttpRequest::answer ( const shared_ptr<Tile>& tile, Reply::StatusType status )
{
	if (checkifAnswered()) return;
	reply.status = status;
	reply.content = "";
	reply.tile = tile;
	reply.headers.resize ( 2 );
	reply.headers[0].name = "Content-Length";
	reply.headers[0].value = boost::lexical_cast<string> ( tile->getImage()->size() );
	reply.headers[1].name = "Content-Type";
	reply.headers[1].value = "image/";
	reply.headers[1].value.append ( tile->getIdentifier()->getImageFormatString() );

	//	IP					Date				Method		url			Version  Reply Size duration
	//80.101.90.180 - [02/Jun/2009:15:11:52 -0400] "GET /css/style.css HTTP/1.1" 200 2816 12
	auto now = boost::posix_time::second_clock::local_time();
	LOG_SEV(access_log, info) << socket.remote_endpoint().address().to_string()
						<< " - ["
						<< now.date().day() << "/" << now.date().month() << "/" << now.date().year()
						<< ":" << now.time_of_day().hours() << ":" << now.time_of_day().minutes() << ":" << now.time_of_day().seconds()
						<< "] \""
						<< data.method << " " << data.uri << " HTTP/" << data.http_version_major << "." << data.http_version_minor << "\" "
						<< reply.status << " " << reply.headers[0].value;

	LOG_SEV(server_log, info) << "Answered \"" << data.uri << "\"";
	answer();
}

namespace status_strings
{

const std::string ok =
	"HTTP/1.0 200 OK\r\n";
const std::string created =
	"HTTP/1.0 201 Created\r\n";
const std::string accepted =
	"HTTP/1.0 202 Accepted\r\n";
const std::string no_content =
	"HTTP/1.0 204 No Content\r\n";
const std::string multiple_choices =
	"HTTP/1.0 300 Multiple Choices\r\n";
const std::string moved_permanently =
	"HTTP/1.0 301 Moved Permanently\r\n";
const std::string moved_temporarily =
	"HTTP/1.0 302 Moved Temporarily\r\n";
const std::string not_modified =
	"HTTP/1.0 304 Not Modified\r\n";
const std::string bad_request =
	"HTTP/1.0 400 Bad Request\r\n";
const std::string unauthorized =
	"HTTP/1.0 401 Unauthorized\r\n";
const std::string forbidden =
	"HTTP/1.0 403 Forbidden\r\n";
const std::string not_found =
	"HTTP/1.0 404 Not Found\r\n";
const std::string internal_server_error =
	"HTTP/1.0 500 Internal Server Error\r\n";
const std::string not_implemented =
	"HTTP/1.0 501 Not Implemented\r\n";
const std::string bad_gateway =
	"HTTP/1.0 502 Bad Gateway\r\n";
const std::string service_unavailable =
	"HTTP/1.0 503 Service Unavailable\r\n";

boost::asio::const_buffer to_buffer ( HttpRequest::Reply::StatusType status )
{
	switch ( status ) {
	case HttpRequest::Reply::ok:
		return boost::asio::buffer ( ok );
	case HttpRequest::Reply::created:
		return boost::asio::buffer ( created );
	case HttpRequest::Reply::accepted:
		return boost::asio::buffer ( accepted );
	case HttpRequest::Reply::no_content:
		return boost::asio::buffer ( no_content );
	case HttpRequest::Reply::multiple_choices:
		return boost::asio::buffer ( multiple_choices );
	case HttpRequest::Reply::moved_permanently:
		return boost::asio::buffer ( moved_permanently );
	case HttpRequest::Reply::moved_temporarily:
		return boost::asio::buffer ( moved_temporarily );
	case HttpRequest::Reply::not_modified:
		return boost::asio::buffer ( not_modified );
	case HttpRequest::Reply::bad_request:
		return boost::asio::buffer ( bad_request );
	case HttpRequest::Reply::unauthorized:
		return boost::asio::buffer ( unauthorized );
	case HttpRequest::Reply::forbidden:
		return boost::asio::buffer ( forbidden );
	case HttpRequest::Reply::not_found:
		return boost::asio::buffer ( not_found );
	case HttpRequest::Reply::internal_server_error:
		return boost::asio::buffer ( internal_server_error );
	case HttpRequest::Reply::not_implemented:
		return boost::asio::buffer ( not_implemented );
	case HttpRequest::Reply::bad_gateway:
		return boost::asio::buffer ( bad_gateway );
	case HttpRequest::Reply::service_unavailable:
		return boost::asio::buffer ( service_unavailable );
	default:
		return boost::asio::buffer ( internal_server_error );
	}
}

} // namespace status_strings

std::vector<boost::asio::const_buffer> HttpRequest::Reply::toBuffers ()
{
	static const char name_value_separator[] = { ':', ' ' };
	static const char crlf[] = { '\r', '\n' };
	std::vector<boost::asio::const_buffer> buffers;
	buffers.push_back ( status_strings::to_buffer ( status ) );

	for ( std::size_t i = 0; i < headers.size(); ++i ) {
		Header &h = headers[i];
		buffers.push_back ( boost::asio::buffer ( h.name ) );
		buffers.push_back ( boost::asio::buffer ( name_value_separator ) );
		buffers.push_back ( boost::asio::buffer ( h.value ) );
		buffers.push_back ( boost::asio::buffer ( crlf ) );
	}

	buffers.push_back ( boost::asio::buffer ( crlf ) );
	buffers.push_back ( boost::asio::buffer ( content ) );
	if (tile) {
		buffers.push_back ( boost::asio::const_buffer ( ( void * ) tile->getImage()->data(), tile->getImage()->size() ) );
	}
	return buffers;
}
