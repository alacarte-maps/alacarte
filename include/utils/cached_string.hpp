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
#ifndef _CACHED_STRING_HPP
#define _CACHED_STRING_HPP

#include <boost/noncopyable.hpp>
#include <sstream>

#include "settings.hpp"



class MaybeCachedString;

/**
 * @brief Represents a string which is cached into an internal cache
 *
 * \Note It is not safe to use an instance of this class in more than one thread!!!
 *
 **/
class CachedString
{
	friend class boost::serialization::access;
	friend class MaybeCachedString;
private:
	struct StringStorageElement
	{
		StringStorageElement(const string& str);
		StringStorageElement(const StringStorageElement& other);

		const string value;
		const std::size_t hash;

		bool operator == (const StringStorageElement& other) const;
		struct Hasher;
		struct StringComparator;
	};
	class StringStorage;

public:
	CachedString();
	explicit CachedString(const char* str);
	explicit CachedString(const string& str);
	CachedString(const CachedString& other);
	~CachedString();


	CachedString& operator =(const string& str);
	CachedString& operator =(const CachedString& other);

	bool equals(const string& other) const;
	bool equals(const CachedString& other) const;


	void assign(const string& str);
	void assign(const CachedString& other);

	void clear();

	const char* c_str() const;
	const string& str() const;

	std::size_t hash() const;

	inline bool operator <(const CachedString& other) const
	{
		return internalString < other.internalString;
	}

	static void Shutdown();

private:
	template<typename Archive>
	void save(Archive & ar, const unsigned int version) const
	{
		// Save the the hole string, sorry
		ar << str();
	}
	template<class Archive>
	void load(Archive & ar, const unsigned int version)
	{
		string str;
		ar >> str;
		*this = CachedString(str);
	}

	BOOST_SERIALIZATION_SPLIT_MEMBER()
private:
	const StringStorageElement* internalString;
};










/**
 * @brief Represents a string which could be cached into an internal cache
 *
 * \Note It is not safe to use an instance of this class in more than one thread!!! 
 *
 **/
class MaybeCachedString
{
	friend class CachedString;
private:
	typedef CachedString::StringStorageElement	StringStorageElement;
	typedef CachedString::StringStorage			StringStorage;
public:
	MaybeCachedString();
	explicit MaybeCachedString(const string& str);
	MaybeCachedString(const CachedString& other);
	MaybeCachedString(const MaybeCachedString& other);
	~MaybeCachedString();


	MaybeCachedString& operator =(const string& str);
	MaybeCachedString& operator =(const CachedString& other);
	MaybeCachedString& operator =(const MaybeCachedString& other);

	bool equals(const string& other) const;
	bool equals(const CachedString& other) const;
	bool equals(const MaybeCachedString& other) const;

	void assign(const string& str);
	void assign(const MaybeCachedString& other);
	void assign(const CachedString& other);

	const char* c_str() const;
	const string& str() const;

	void clear();
	std::size_t hash() const;
	bool isCached() const;

	inline bool operator <(const MaybeCachedString& other) const
	{
		if(cached && other.cached)
		{
			return internalString < other.internalString;
		} else {
			return str() < other.str();
		}
	}

private:
	void reset();

	bool cached;
	const StringStorageElement* internalString;
};



// ############################# equal operators #################################

inline bool operator ==(const CachedString& first, const CachedString& second)				{ return second.equals(first); }
inline bool operator ==(const CachedString& first, const string& second)					{ return first.equals(second); }
inline bool operator ==(const CachedString& first, const MaybeCachedString& second)			{ return second.equals(first); }

inline bool operator ==(const string& first, const CachedString& second)					{ return second.equals(first); }
inline bool operator ==(const string& first, const MaybeCachedString& second)				{ return second.equals(first); }

inline bool operator ==(const MaybeCachedString& first, const string& second)				{ return first.equals(second); }
inline bool operator ==(const MaybeCachedString& first, const CachedString& second)			{ return first.equals(second); }
inline bool operator ==(const MaybeCachedString& first, const MaybeCachedString& second)	{ return first.equals(second); }



// ############################# unequal operators #################################

inline bool operator !=(const CachedString& first, const CachedString& second)				{ return !second.equals(first); }
inline bool operator !=(const CachedString& first, const string& second)					{ return !first.equals(second); }
inline bool operator !=(const CachedString& first, const MaybeCachedString& second)			{ return !second.equals(first); }

inline bool operator !=(const string& first, const CachedString& second)					{ return !second.equals(first); }
inline bool operator !=(const string& first, const MaybeCachedString& second)				{ return !second.equals(first); }

inline bool operator !=(const MaybeCachedString& first, const string& second)				{ return !first.equals(second); }
inline bool operator !=(const MaybeCachedString& first, const CachedString& second)			{ return !first.equals(second); }
inline bool operator !=(const MaybeCachedString& first, const MaybeCachedString& second)	{ return !first.equals(second); }



// ############################# stream operators #################################

template<typename Stream>
Stream& operator >> (Stream& stream, CachedString& str)
{
	str = CachedString(static_cast<std::stringstream const&>(std::stringstream() << stream.rdbuf()).str());

	return stream;
}

template<typename Stream>
Stream& operator << (Stream& stream, const CachedString& str)
{
	stream << str.str();

	return stream;
}


// ############################# hash calculation operators #################################

inline std::size_t hash_value(const CachedString& cs)
{
	return cs.hash();
}

inline std::size_t hash_value(const MaybeCachedString& mcs)
{
	return mcs.hash();
}


// ############################# Comparator for CachedString's #################################

struct CachedComparator
	: public std::binary_function<MaybeCachedString, CachedString, bool>
{
	bool operator()(const MaybeCachedString& str, const CachedString& cs) const
	{
		return cs == str;
	}
};

#endif
