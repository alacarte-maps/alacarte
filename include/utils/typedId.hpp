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
#ifndef TYPEDID_HPP
#define TYPEDID_HPP

/*
 * =====================================================================================
 *
 *       Filename:  typedId.hpp
 *
 *    Description: Utility class for rectangle operations.
 *
 * =====================================================================================
 */


template<unsigned short TypeNumber>
class TypedId
{
	friend class boost::serialization::access;
public:
	static const unsigned short type_number = TypeNumber;

	inline explicit TypedId()
		: id(-1)
	{
	}

	inline explicit TypedId(unsigned int id)
		: id(id)
	{
	}

	inline unsigned int getRaw() const
	{
		return id;
	}


	inline bool operator ==(const TypedId& other) const
	{
		return other.id == id;
	}

	inline bool operator !=(const TypedId& other) const
	{
		return other.id != id;
	}

	inline bool operator <(const TypedId& right) const
	{
		return id < right.id;
	}

	inline bool operator >(const TypedId& right) const
	{
		return id > right.id;
	}

	inline bool operator >=(const TypedId& right) const
	{
		return id >= right.id;
	}

	inline bool operator <=(const TypedId& right) const
	{
		return id <= right.id;
	}
private:
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & id;
	}

	unsigned int id;
};

template<unsigned short TypeNumber>
inline std::size_t hash_value(TypedId<TypeNumber> id)
{
	return static_cast<std::size_t>(id.getRaw());
}

template<unsigned short TypeNumber>
inline std::ostream& operator <<(std::ostream& o, const TypedId<TypeNumber>& id)
{
	o << id.getRaw();
	return o;
}


class AnyId
{
public:
	template<unsigned int TypeNumber>
	inline AnyId(const TypedId<TypeNumber>& id)
		: type(TypedId<TypeNumber>::type_number)
		, id(id.getRaw())
	{

	}

	inline bool operator ==(const AnyId& other) const
	{
		return id == other.id && type == other.type;
	}

	template<typename IdType>
	inline IdType is() const
	{
		return IdType::type_number == type;
	}


	template<typename IdType>
	inline IdType get() const
	{
		assert(type == IdType::type_number);

		return IdType(id);
	}

private:
	unsigned short type;
	unsigned int id;
};






#endif
