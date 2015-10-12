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
#ifndef SETTINGS_HPP
#define SETTINGS_HPP

/*
 * =====================================================================================
 *
 *       Filename:  settings.hpp
 *
 *    Description:  Contains important includes and definitions for alacarte
 *
 * =====================================================================================
 */

// std includes
#include <cassert>
#include <string>
#include <map>
#include <vector>
#include <list>
#include <queue>
#include <stack>
#include <cstdint>
#include <iostream>
#include <stdexcept>

// boost includes
#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/basic_socket.hpp>
#include <boost/exception/all.hpp>

// log4cpp
#include <log4cpp/Priority.hh>
#include <log4cpp/Category.hh>

#ifdef ALACARTE_TEST
	#define TESTABLE virtual
#else
	#define TESTABLE
#endif

// Dont ask! this is c++ makro-blabla
//! Use ALAC_JOIN to join two identifiers in definitions
#define ALAC_JOIN(_a, _b)		ALAC_DO_JOIN1(_a, _b)	
#define ALAC_DO_JOIN1(_a, _b)	ALAC_DO_JOIN2(_a, _b)
#define ALAC_DO_JOIN2(_a, _b)	_a##_b


#define NOT_IMPLEMENTED() {BOOST_THROW_EXCEPTION(excp::NotImplementedException() << excp::InfoFileName(__FILE__));}
#define IMPLEMENTATION_TODO(_text)	{																\
										static bool ALAC_JOIN(_found_, __LINE__) = false;			\
										if(! ALAC_JOIN(_found_, __LINE__))							\
										{															\
											std::clog << "Not Implemented: "<< _text << std::endl;	\
											ALAC_JOIN(_found_, __LINE__) = true;					\
										}															\
									}

using boost::shared_ptr;
using boost::scoped_ptr;
using boost::weak_ptr;

// we want to use the std::string as stringtype
typedef std::string		string;
typedef std::int8_t		int8;
typedef std::uint8_t	uint8;
typedef std::int16_t	int16;
typedef std::uint16_t	uint16;
typedef std::int32_t	int32;
typedef std::uint32_t	uint32;
typedef std::int32_t	coord_t;

// include utils
#include "utils/point.hpp"
#include "utils/exceptions.hpp"
#include "utils/color.hpp"
#include "utils/rect.hpp"
#include "utils/typedId.hpp"
#include "utils/cached_string.hpp"

#define DEFAULT_CONFIG_NAME "alacarte.conf"

#define DEFAULT_FONT "DejaVu Sans"
#define TILE_OVERLAP (1.0/META_TILE_SIZE * 0.25)
#define META_TILE_SIZE 4
#define ALAC_ZOOM_BOTTOM 0
#define ALAC_ZOOM_TOP 18

typedef TypedId<0>	NodeId;
typedef TypedId<1>	WayId;
typedef TypedId<2>	RelId;

#include <boost/unordered_map.hpp>
template<typename Key, typename Value>
class DataMap
	: public boost::unordered_map<Key, Value>
{
	friend class boost::serialization::access;

//	typedef std::vector< std::pair<Key, Value> >	container;
//	typedef typename container::const_iterator				iterator;


	template<typename Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		int s = this->size();
		ar << s;
		for(auto& e : *this)
			ar << e;
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		typename boost::unordered_map<Key, Value>::value_type value;

		int s;
		ar >> s;

		while(s--)
		{
			ar >> value;
			this->insert(value);
		}
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()
public:
	/*iterator find(const Key& key) const
	{
		for(auto it = begin(); it != end(); ++it)
		{
			if(it->first == key)
			{
				return it;
			}
		}
		return end();
	}

	Value& operator[](const Key& key)
	{
		push_back(std::make_pair(key, Value()));
		return back().second;
	}

	const Value& operator[](const Key& key) const
	{
		push_back(std::make_pair(key, Value()));
		return back().second;
	}

	Value& at(const Key& key)
	{
		return find(key)->second;
	}

	const Value& at(const Key& key) const
	{
		return find(key)->second;
	}*/
};

#endif
