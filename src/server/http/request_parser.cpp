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


//
// request_parser.cpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2012 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include "server/http/request_parser.hpp"
#include "server/http_request.hpp"

HttpRequestParser::HttpRequestParser()
	: state_ ( method_start )
{
}

void HttpRequestParser::reset()
{
	state_ = method_start;
}

boost::tribool HttpRequestParser::consume ( shared_ptr<HttpRequest> req, char input )
{
	switch ( state_ ) {
	case method_start:

		if ( !is_char ( input ) || is_ctl ( input ) || is_tspecial ( input ) ) {
			return false;
		} else {
			state_ = method;
			req->data.method.push_back ( input );
			return boost::indeterminate;
		}

	case method:

		if ( input == ' ' ) {
			state_ = uri;
			return boost::indeterminate;
		} else if ( !is_char ( input ) || is_ctl ( input ) || is_tspecial ( input ) ) {
			return false;
		} else {
			req->data.method.push_back ( input );
			return boost::indeterminate;
		}

	case uri:

		if ( input == ' ' ) {
			state_ = http_version_h;
			return boost::indeterminate;
		} else if ( is_ctl ( input ) ) {
			return false;
		} else {
			req->data.uri.push_back ( input );
			return boost::indeterminate;
		}

	case http_version_h:

		if ( input == 'H' ) {
			state_ = http_version_t_1;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_t_1:

		if ( input == 'T' ) {
			state_ = http_version_t_2;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_t_2:

		if ( input == 'T' ) {
			state_ = http_version_p;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_p:

		if ( input == 'P' ) {
			state_ = http_version_slash;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_slash:

		if ( input == '/' ) {
			req->data.http_version_major = 0;
			req->data.http_version_minor = 0;
			state_ = http_version_major_start;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_major_start:

		if ( is_digit ( input ) ) {
			req->data.http_version_major = req->data.http_version_major * 10 + input - '0';
			state_ = http_version_major;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_major:

		if ( input == '.' ) {
			state_ = http_version_minor_start;
			return boost::indeterminate;
		} else if ( is_digit ( input ) ) {
			req->data.http_version_major = req->data.http_version_major * 10 + input - '0';
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_minor_start:

		if ( is_digit ( input ) ) {
			req->data.http_version_minor = req->data.http_version_minor * 10 + input - '0';
			state_ = http_version_minor;
			return boost::indeterminate;
		} else {
			return false;
		}

	case http_version_minor:

		if ( input == '\r' ) {
			state_ = expecting_newline_1;
			return boost::indeterminate;
		} else if ( is_digit ( input ) ) {
			req->data.http_version_minor = req->data.http_version_minor * 10 + input - '0';
			return boost::indeterminate;
		} else {
			return false;
		}

	case expecting_newline_1:

		if ( input == '\n' ) {
			state_ = header_line_start;
			return boost::indeterminate;
		} else {
			return false;
		}

	case header_line_start:

		if ( input == '\r' ) {
			state_ = expecting_newline_3;
			return boost::indeterminate;
		} else if ( !req->data.headers.empty() && ( input == ' ' || input == '\t' ) ) {
			state_ = header_lws;
			return boost::indeterminate;
		} else if ( !is_char ( input ) || is_ctl ( input ) || is_tspecial ( input ) ) {
			return false;
		} else {
			req->data.headers.push_back ( HttpRequest::Header() );
			req->data.headers.back().name.push_back ( input );
			state_ = header_name;
			return boost::indeterminate;
		}

	case header_lws:

		if ( input == '\r' ) {
			state_ = expecting_newline_2;
			return boost::indeterminate;
		} else if ( input == ' ' || input == '\t' ) {
			return boost::indeterminate;
		} else if ( is_ctl ( input ) ) {
			return false;
		} else {
			state_ = header_value;
			req->data.headers.back().value.push_back ( input );
			return boost::indeterminate;
		}

	case header_name:

		if ( input == ':' ) {
			state_ = space_before_header_value;
			return boost::indeterminate;
		} else if ( !is_char ( input ) || is_ctl ( input ) || is_tspecial ( input ) ) {
			return false;
		} else {
			req->data.headers.back().name.push_back ( input );
			return boost::indeterminate;
		}

	case space_before_header_value:

		if ( input == ' ' ) {
			state_ = header_value;
			return boost::indeterminate;
		} else {
			return false;
		}

	case header_value:

		if ( input == '\r' ) {
			state_ = expecting_newline_2;
			return boost::indeterminate;
		} else if ( is_ctl ( input ) ) {
			return false;
		} else {
			req->data.headers.back().value.push_back ( input );
			return boost::indeterminate;
		}

	case expecting_newline_2:

		if ( input == '\n' ) {
			state_ = header_line_start;
			return boost::indeterminate;
		} else {
			return false;
		}

	case expecting_newline_3:
		return ( input == '\n' );
	default:
		return false;
	}
}

bool HttpRequestParser::is_char ( int c )
{
	return c >= 0 && c <= 127;
}

bool HttpRequestParser::is_ctl ( int c )
{
	return ( c >= 0 && c <= 31 ) || ( c == 127 );
}

bool HttpRequestParser::is_tspecial ( int c )
{
	switch ( c ) {
	case '(':
	case ')':
	case '<':
	case '>':
	case '@':
	case ',':
	case ';':
	case ':':
	case '\\':
	case '"':
	case '/':
	case '[':
	case ']':
	case '?':
	case '=':
	case '{':
	case '}':
	case ' ':
	case '\t':
		return true;
	default:
		return false;
	}
}

bool HttpRequestParser::is_digit ( int c )
{
	return c >= '0' && c <= '9';
}

