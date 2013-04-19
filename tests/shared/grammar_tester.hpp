#pragma once
#ifndef _GRAMMAR_TESTER_HPP
#define _GRAMMAR_TESTER_HPP

#include <boost/lexical_cast.hpp>

#include "server/parser/mapcss_def.hpp"

#define TEMP_FILE "eval-test.tmp"

template<typename Grammar, typename Result, typename Skipper>
void testGrammar(Grammar& grammar, Result& result, const string& expr, const Skipper& skipper)
{
	GrammarIterator position_begin(expr.begin(), expr.end(), "line:" + expr);
	GrammarIterator position_end;

	try
	{
		try
		{
			bool r = qi::phrase_parse(position_begin, position_end, grammar, skipper, result);

			if(!r || position_begin != position_end)
			{
				BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Failed to parse eval"));
			}

		} catch(const qi::expectation_failure<GrammarIterator>& e)
		{
			const classic::file_position_base<std::string>& pos = e.first.get_position();

			BOOST_THROW_EXCEPTION(excp::ParseException() 
				<< excp::InfoFailureLine(pos.line)
				<< excp::InfoFailureColumn(pos.column)
				<< excp::InfoFailureLineContent(e.first.get_currentline())
				<< excp::InfoWhat("Illegal syntax! Expected valid " + boost::lexical_cast<string>(e.what_) + "!"));

		} catch(excp::ParseException& e)
		{
			const classic::file_position_base<std::string>& pos = position_begin.get_position();
			e	<< excp::InfoFailureLine(pos.line)
				<< excp::InfoFailureColumn(pos.column)
				<< excp::InfoFailureLineContent(position_begin.get_currentline());
			throw e;
		}

	} catch(excp::ParseException& e) 
	{
		std::cout	<< excp::ErrorOut<excp::InfoWhat>(e, "unknown reason!") << std::endl
			<< "In line " << excp::ErrorOut<excp::InfoFailureLine>(e)
			<< " column " << excp::ErrorOut<excp::InfoFailureColumn>(e) << ":\n";

		const string* errLine = boost::get_error_info<excp::InfoFailureLineContent>(e);
		const int* errColumn = boost::get_error_info<excp::InfoFailureColumn>(e);

		if(errLine && errColumn)
		{
			std::cout	<< "'" << *errLine << "'" << std::endl
				<< string(*errColumn, ' ') << "^-here\n\n";
		}
		throw e << excp::InfoFileName(TEMP_FILE);
	}
}


template<typename Grammar, typename Result, typename Skipper>
void testGrammar(Grammar& grammar, Result& result, string::const_iterator begin, string::const_iterator end, const Skipper& skipper)
{
	try
	{
		try
		{
			bool r = qi::phrase_parse(begin, end, grammar, skipper, result);

			if(!r || begin != end)
			{
				BOOST_THROW_EXCEPTION(excp::ParseException() << excp::InfoWhat("Failed to parse eval"));
			}

		} catch(const qi::expectation_failure<string::iterator>& e)
		{

			BOOST_THROW_EXCEPTION(excp::ParseException() 
				<< excp::InfoFailureLineContent(string(begin, end))
				<< excp::InfoWhat(string("Illegal syntax! Expected valid ") + boost::lexical_cast<string>(e.what_) + "!"));

		} 
	} catch(excp::ParseException& e) 
	{
		throw e << excp::InfoFileName(TEMP_FILE);
	}
}




#endif // !_GRAMMAR_TESTER_HPP
