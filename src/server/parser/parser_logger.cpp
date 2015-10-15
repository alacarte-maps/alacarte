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



#include "server/parser/parser_logger.hpp"


/**
 * @brief Creates a log stream
 *
 * \param logger used for output
 * \param cat Category for this outputstream
 *
 **/
ParserLogger::LogStream::LogStream( const shared_ptr<ParserLogger>& logger, Category cat )
	: logger(logger)
	, category(cat)
{

}

/**
 * @brief Creates a log stream from another log stream
 *
 * \param other outputstream
 *
 **/
ParserLogger::LogStream::LogStream( const LogStream& other)
	: logger(other.logger)
	, category(other.category)

{
}

/**
 * @brief Destroyes the log stream
 *
 **/
ParserLogger::LogStream::~LogStream()
{
	if(!value.str().empty())
	{
		logger->logToStream(category, logvalue());
	}
}

/**
 * @brief Returns the log value
 *
 **/
string ParserLogger::LogStream::logvalue() const
{
	return value.str();
}

/**
 * @brief Creates a new parse logger
 *
 * \param parsed File the name of the filename associated with this stream
 *
 **/
ParserLogger::ParserLogger(const string& parsedFile)
	: filename(parsedFile + ".error")
{

}


/**
 * @brief Returns a warn stream
 *
 * \return the new warn stream
 **/
ParserLogger::LogStream ParserLogger::warnStream()
{
	return LogStream(this->shared_from_this(), Warning);
}

/**
 * @brief Returns a error stream
 *
 * \return the new error stream
 **/
ParserLogger::LogStream ParserLogger::errorStream()
{
	return LogStream(this->shared_from_this(), Error);
}

/**
 * @brief outputs an error message
 *
 **/
void ParserLogger::error(const string& str)
{
	logToStream(Error, str);
}

/**
 * @brief outputs an warn message
 *
 **/
void ParserLogger::warn(const string& str)
{
	logToStream(Warning, str);
}

/**
 * @brief prints a message to the log
 *
 * \param cat type of the message
 * \param str string to be outputed
 *
 **/
void ParserLogger::logToStream(Category cat, const string& str )
{
	if(str.empty())
		return;

	if(!outputStream.is_open() && filename.size())
	{
		outputStream.open(filename.c_str());

		if(!outputStream.is_open())
		{
			LOG_SEV(style_log, fatal) << "Failed to open \"" + filename + "\"";
			filename.clear();
			return;
		}
	}

	switch (cat)
	{
	case ParserLogger::Warning:
		outputStream << "Waring: " << str << "\n";
		LOG_SEV(style_log, warning) << str;
		break;
	default:
	case ParserLogger::Error:
		outputStream << "Error: " << str << "\n";
		LOG_SEV(style_log, error) << str;
		break;
	}

	outputStream.flush();
}

