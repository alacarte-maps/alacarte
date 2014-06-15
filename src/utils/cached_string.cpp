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




#include <boost/unordered_set.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/shared_mutex.hpp>
#include <boost/thread/locks.hpp>

#include "utils/cached_string.hpp"

/**
 * @brief Compares two StringStorageElements
 *
 * \return true if both elements are equal
 **/
bool CachedString::StringStorageElement::operator == (const StringStorageElement& other) const
{
	return (hash == other.hash) || (value == other.value);
}

/**
 * @brief Hasher for the StringStorageElement
 *
 **/
struct CachedString::StringStorageElement::Hasher
	: public std::unary_function<StringStorageElement, std::size_t>
{
	std::size_t operator()(const StringStorageElement& sse) const
	{
		return sse.hash;
	}
};

/**
 * @brief Comparator for comparing STringStorageELements with std::strings
 *
 **/
struct CachedString::StringStorageElement::StringComparator
	: public std::binary_function<string, StringStorageElement, bool>
{
	bool operator()(const std::string& str, const StringStorageElement& sse) const
	{
		return sse.value == str;
	}
};

/**
 * @brief Creates a new StringStorageELement
 *
 * \param str std::string for this element
 *
 **/
CachedString::StringStorageElement::StringStorageElement( const std::string& str )
	: value(str)
	, hash(boost::hash<string>()(str))
{
}

/**
 * @brief Creates a new StringStorageELement from another
 *
 * \param other element to be copied
 *
 **/
CachedString::StringStorageElement::StringStorageElement(const StringStorageElement& other)
	: value(other.value)
	, hash(other.hash)
{
}

/**
 * @brief Storage class for cached std::strings
 *
 * This is an singleton
 **/
class CachedString::StringStorage
{
private:
	StringStorage() { }
	StringStorage(const StringStorage& ss) = delete;
	StringStorage& operator=( const StringStorage& ) = delete;
public:
	/**
	 * @brief Returns the Storage
	 *
	 * If no storage exists a new one will be created once!
	 *
	 **/
	static StringStorage& Inst()
	{
		static StringStorage inst;
		return inst;
	}

	/**
	 * @brief Frees all memory allocated by this storage
	 *
	 **/
	void shutdown()
	{
		emptyString = nullptr;
		storage.clear();
	}

	/**
	 * @brief Returns the storage element for an empty std::string
	 *
	 **/
	const StringStorageElement* getEmptyString() const
	{
		return emptyString;
	}

	/**
	 * @brief Returns the storage element for a given std::string
	 *
	 * If no element exists for the std::string a new element is created.
	 *
	 **/
	const StringStorageElement* resolveString(const std::string& str)
	{
		boost::upgrade_lock<boost::shared_mutex> guard(accessMutex);
		// Search for the std::string using a direct string search
		auto it = storage.find(str, std::stringStorageElementHasher, stringComperator);

		// check if std::string was already in storage
		if(it == storage.end())
		{
			boost::upgrade_to_unique_lock<boost::shared_mutex> unique(guard);
			it = storage.insert(StringStorageElement(str)).first;
		}

		return &(*it);
	}
private:
	/**
	 * @brief Creates the StringStorage
	 *
	 **/
	StringStorage()
	{
		emptyString = resolveString("");
	}

private:
	//! The hasher used for StringStorageELements
	StringStorageElement::Hasher std::stringStorageElementHasher;
	//! The comparator for std::strings and storage elements
	StringStorageElement::StringComparator std::stringComperator;

	//! Pointer to the storage element holding the empty std::string.
	const StringStorageElement* emptyString;
	//! internal storage as an hash map
	boost::unordered_set<StringStorageElement, StringStorageElement::Hasher> storage;
	//! The mutex used to lock this class
	boost::shared_mutex accessMutex;
};



/**
 * @brief Cretaes a new Cached std::string
 *
 **/
CachedString::CachedString()
{
	clear();
}

/**
 * @brief Creates a new cached std::string from a char ptr
 *
 **/
CachedString::CachedString(const char* str)
{
	internalString = StringStorage::Inst().resolveString(str);
}

/**
 * @brief Creates a new cached std::string from a std string
 *
 **/
CachedString::CachedString( const std::string& str )
{
	assign(str);
}

/**
 * @brief Creates a new cached std::string from another cached string
 *
 **/
CachedString::CachedString( const CachedString& other )
{
	assign(other);
}


/**
 * @brief Tidies up the cached std::string
 *
 **/
CachedString::~CachedString()
{
}

/**
 * @brief Compares this cached std::string with an non cached string
 *
 **/
bool CachedString::equals( const std::string& other ) const
{
	return str() == other;
}

/**
 * @brief Creates a new cached std::string from another cached string
 *
 **/
bool CachedString::equals( const CachedString& other ) const
{
	return this->internalString == other.internalString;
}

/*
CachedString& CachedString::operator =(const char* str)
{
	return *this = CachedString(str);
}*/

/**
 * @brief Assigns a std std::string
 *
 **/
CachedString& CachedString::operator =(const std::string& str)
{
	assign(str);
	return *this;
}

/**
 * @brief Assigns a cached std::string
 *
 **/
CachedString& CachedString::operator=( const CachedString& other )
{
	assign(other);
	return *this;
}


/**
 * @brief Assigns a std std::string
 *
 **/
void CachedString::assign( const std::string& str )
{
	internalString = StringStorage::Inst().resolveString(str);
}

/**
 * @brief Assigns a cached std::string
 *
 **/
void CachedString::assign( const CachedString& other )
{
	this->internalString = other.internalString;
}


/**
 * @brief Sets this std::string to the empty string
 *
 **/
void CachedString::clear()
{
	internalString = StringStorage::Inst().getEmptyString();
}


/**
 * @brief Returns the internal c std::string
 *
 **/
const char* CachedString::c_str() const
{
	assert(internalString);
	return internalString->value.c_str();
}

/**
 * @brief Returns the internal std::string
 *
 **/
const std::string& CachedString::str() const
{
	assert(internalString);
	return internalString->value;
}

std::size_t CachedString::hash() const
{
	assert(internalString);
	return internalString->hash;
}

/**
 * @brief Frees all internal memory used by the cache mechanism
 *
 **/
void CachedString::Shutdown()
{
	CachedString::StringStorage::Inst().shutdown();
}


/**
 * @brief Returns the internal c std::string
 *
 **/
MaybeCachedString::MaybeCachedString()
	: cached(true)
{
	clear();
}

/*
MaybeCachedString::MaybeCachedString( const char* str )
{

}*/

/**
 * @brief Creates a non cached std::string from a string
 *
 **/
MaybeCachedString::MaybeCachedString( const std::string& str )
	: cached(true)
{
	assign(str);
}

/**
 * @brief Creates a cached std::string from a cached string
 *
 **/
MaybeCachedString::MaybeCachedString( const CachedString& other )
	: cached(true)
{
	assign(other);
}

/**
 * @brief Creates a maybe cached std::string from another maybe cached string
 **/
MaybeCachedString::MaybeCachedString( const MaybeCachedString& other )
	: cached(true)
{
	assign(other);
}

/**
 * @brief Destroys this object
 *
 **/
MaybeCachedString::~MaybeCachedString()
{
	reset();
}

/*
MaybeCachedString& MaybeCachedString::operator=( const char* str )
{

}*/

/**
 * @brief Assigns a non cached std std::string
 *
 **/
MaybeCachedString& MaybeCachedString::operator=( const std::string& str )
{
	assign(str);
	return *this;
}

/**
 * @brief Assigns a cached std::string
 *
 **/
MaybeCachedString& MaybeCachedString::operator =(const CachedString& other)
{
	assign(other);
	return *this;
}

/**
 * @brief Assigns a maybe cached std::string
 *
 **/
MaybeCachedString& MaybeCachedString::operator=( const MaybeCachedString& other )
{
	assign(other);
	return *this;
}


/**
 * @brief Assigns a non cached std std::string
 *
 **/
void MaybeCachedString::assign( const std::string& str )
{
	reset();
	cached = false;
	internalString = new StringStorageElement(str);
}

/**
 * @brief Assigns a maybe cached std::string
 *
 **/
void MaybeCachedString::assign( const MaybeCachedString& other )
{
	reset();
	cached = other.cached;
	if (cached) {
		internalString = other.internalString;
	} else {
		internalString = new StringStorageElement(*other.internalString);
	}
}

/**
 * @brief Assigns a cached std::string
 *
 **/
void MaybeCachedString::assign( const CachedString& other )
{
	cached = true;
	internalString = other.internalString;
}

/**
 * @brief Compares this std::string with a given std string
 *
 **/
bool MaybeCachedString::equals( const std::string& other ) const
{
	return str() == other;
}

/**
 * @brief Compares this std::string with a cached string
 *
 **/
bool MaybeCachedString::equals( const CachedString& other ) const
{
	if(cached)
	{
		return internalString == other.internalString;
	} else {
		return equals(other.str());
	}
}

/**
 * @brief Compares this std::string with another maybe cached string
 *
 **/
bool MaybeCachedString::equals( const MaybeCachedString& other ) const
{
	if(cached && other.cached)
	{
		return internalString == other.internalString;
	} else {
		return equals(other.str());
	}
}

/**
 * @brief Returns the internal c std::string
 *
 **/
const char* MaybeCachedString::c_str() const
{
	assert(internalString);
	return internalString->value.c_str();
}

/**
 * @brief Returns the internal std::string
 *
 **/
const std::string& MaybeCachedString::str() const
{
	assert(internalString);
	return internalString->value;
}

/**
 * @brief Returns the internal hash
 *
 **/
std::size_t MaybeCachedString::hash() const
{
	assert(internalString);
	return internalString->hash;
}

/**
 * @brief Sets this std::string to be the empty string
 *
 **/
void MaybeCachedString::clear()
{
	reset();
	cached = true;
	internalString = StringStorage::Inst().getEmptyString();
}

/**
 * @brief deletes rge internal std::string if it is not cached
 *
 **/
void MaybeCachedString::reset()
{
	if(!cached)
		delete internalString;
}

/**
 * @brief Returns weather this std::string is cached.
 *
 **/
bool MaybeCachedString::isCached() const
{
	return cached;
}
