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
#ifndef _PARSE_INFO_PARSER_HPP
#define _PARSE_INFO_PARSER_HPP


#include "settings.hpp"
#include "parse_info.hpp"
#include "mapcss_def.hpp"

namespace pip {

	BOOST_SPIRIT_TERMINAL(pinfo);

}


BOOST_FUSION_ADAPT_STRUCT(ParseInfo,
						  (string, sourceline)
						  (unsigned int, column)
						  (unsigned int, line)
						  (string, linecontent)
						  );

namespace boost { namespace spirit
{
	// We want custom_parser::iter_pos to be usable as a terminal only,
	// and only for parser expressions (qi::domain).
	template <>
	struct use_terminal<qi::domain, pip::tag::pinfo>
		: mpl::true_
	{};
}}


namespace pip {
	struct pinfo_parser
		: boost::spirit::qi::primitive_parser<pinfo_parser>
	{
		// Define the attribute type exposed by this parser component
		template <typename Context, typename Iterator>
		struct attribute
		{
			typedef ParseInfo type;
		};

		// This function is called during the actual parsing process
		template <typename WrappedIterator, typename Context
			, typename Skipper, typename Attribute>
			bool parse(classic::position_iterator2<WrappedIterator>& first, classic::position_iterator2<WrappedIterator> const& last
			, Context&, Skipper const& skipper, Attribute& attr) const
		{
			boost::spirit::qi::skip_over(first, last, skipper);

			const classic::file_position_base<std::string>& pos = first.get_position();

			boost::spirit::traits::assign_to(ParseInfo(pos.file, pos.column, pos.line, first.get_currentline()), attr);
			return true;
		}

		// This function is called during error handling to create
		// a human readable string for the error context.
		template <typename Context>
		boost::spirit::info what(Context&) const
		{
			return boost::spirit::info("pinfo_parser");
		}
	};

}

namespace boost { namespace spirit { namespace qi
{
	// This is the factory function object invoked in order to create
	// an instance of our iter_pos_parser.
	template <typename Modifiers>
	struct make_primitive<pip::tag::pinfo, Modifiers>
	{
		typedef pip::pinfo_parser result_type;

		result_type operator()(unused_type, unused_type) const
		{
			return result_type();
		}
	};
}}}


#endif