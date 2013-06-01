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
 *  Maintainer: Tobias Kahlert
 */

#pragma once
#ifndef EXCEPTIONS_HPP
#define EXCEPTIONS_HPP
/*
 * =====================================================================================
 *
 *       Filename:  exceptions.hpp
 *
 *    Description:  New exceptions are defined here.
 *
 * =====================================================================================
 */

#include "settings.hpp"

class ParserLogger;

namespace excp {

	//! Use this info to give an what msg to the exception
	typedef boost::error_info<struct TagWhatInfo, string> InfoWhat;
	//! Use this to inform about a file name
	typedef boost::error_info<struct TagFileName, string> InfoFileName;
	//! Use this this to specify the exception related xml entity
	typedef boost::error_info<struct TagXmlEntityName, string> InfoXmlEntityName;
	//! String representation of a bad source value
	typedef boost::error_info<struct TagBadSourceValue, string> InfoBadSourceValue;
	//! Specifies the id, which was not resolvable
	typedef boost::error_info<struct TagUnresolvableId, long> InfoUnresolvableId;
	//! Specifies the Line in the file where the failure appeared
	typedef boost::error_info<struct TagFailureLine, int> InfoFailureLine;
	//! Specifies the Column in the Line in the file where the failure appeared
	typedef boost::error_info<struct TagFailureColumn, int> InfoFailureColumn;
	//! Contains the content of the line where a failure appeared
	typedef boost::error_info<struct TagFailureLineContent, string> InfoFailureLineContent;
	//! Contains the logger used while parsing
	typedef boost::error_info<struct TagParserLogger, shared_ptr<ParserLogger> > InfoParserLogger;

	//! Base of most exceptions thrown by alacarte
	struct ExceptionBase : public boost::exception, std::exception
	{
		virtual const char* what() const throw()
		{
			if(string const* info = boost::get_error_info<InfoWhat>(*this))
				return info->c_str();
			return std::exception::what();
		}
	};

	//! Thrown on a point where implementation is missing
	struct NotImplementedException : public ExceptionBase {};
	//! Thrown if a file was not found
	struct FileNotFoundException : public ExceptionBase {};
	//! Thrown if a file was not writeable
	struct FileNotWritable : public ExceptionBase {};
	//! Thrown if input was not in the right format
	struct InputFormatException : public ExceptionBase {};
	//! Thrown if an osm id was not specified before resolving.
	struct BadOsmIdException : public ExceptionBase {};
    //! Thrown if URL is not in SlippyMap-URL-Format.
	struct MalformedURLException : public ExceptionBase {};
	//! Thrown if an unknown image format is requested.
	struct UnknownImageFormatException : public ExceptionBase {};
	//! Thrown if the parsing fails
	struct ParseException : public ExceptionBase {};
	//! Thrown if no more time is left
	struct TimeoutException : public ExceptionBase {};



	template<typename Info>
	struct ErrorOut
	{
		typedef typename boost::exception_detail::get_error_info_return_type<boost::exception,typename Info::value_type>::type value_type;


		ErrorOut(boost::exception& e, const string& fallbackInfo = "'unknown'")
			: fallbackInfo(fallbackInfo)
			, value(boost::get_error_info<Info>(e))
		{
		}

		string fallbackInfo;
		value_type value;
	};

	template<typename Stream, typename Info>
	std::basic_ostream<Stream>& operator <<(std::basic_ostream<Stream>& os, const ErrorOut<Info>& info)
	{
		if(info.value)
		{
			os << *info.value;
		} else {
			os << info.fallbackInfo;
		}

		return os;
	}
}




#endif
