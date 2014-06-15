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

#include <chrono>
#define TIMER_START(_X) auto _X##_start = std::chrono::steady_clock::now(), _X##_stop = _X##_start
#define TIMER_STOP(_X) _X##_stop = std::chrono::steady_clock::now()
#define TIMER_MSEC(_X) std::chrono::duration_cast<std::chrono::milliseconds>(_X##_stop - _X##_start).count()
#define TIMER_SEC(_X) (0.001*std::chrono::duration_cast<std::chrono::milliseconds>(_X##_stop - _X##_start).count())
#define TIMER_MIN(_X) std::chrono::duration_cast<std::chrono::minutes>(_X##_stop - _X##_start).count()

typedef std::int32_t coord_t;

// include utils
#include "utils/point.hpp"
#include "utils/exceptions.hpp"
#include "utils/color.hpp"
#include "utils/rect.hpp"
#include "utils/typedId.hpp"
#include "utils/cached_string.hpp"

constexpr std::string DEFAULT_CONFIG_NAME = "alacarte.conf";

constexpr unsigned META_TILE_SIZE = 4;
constexpr std::string DEFAULT_FONT = "DejaVu Sans";
constexpr double TILE_OVERLAP = (1.0/META_TILE_SIZE * 0.25);
constexpr unsigned ALAC_ZOOM_BOTTOM = 0;
constexpr unsigned ALAC_ZOOM_TOP = 18;

typedef TypedId<0> NodeId;
typedef TypedId<1> WayId;
typedef TypedId<2> RelId;

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
