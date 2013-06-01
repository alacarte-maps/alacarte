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
 *  Maintainer: Patrick Niklaus
 */

#pragma once
#ifndef RECT_HPP
#define RECT_HPP

/*
 * =====================================================================================
 *
 *       Filename:  rect.hpp
 *
 *    Description: Utility class for rectangle operations.
 *
 * =====================================================================================
 */
#include "point.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

template<typename T>
class basic_rect {
private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & minX;
		ar & minY;
		ar & maxX;
		ar & maxY;
	}
public:
	T minX;
	T minY;
	T maxX;
	T maxY;

	basic_rect() : minX(0), minY(0), maxX(0), maxY(0) {}

	basic_rect(const basic_vector2<T>& p1, const basic_vector2<T>& p2)
	{
		minX = std::min(p1.x, p2.x);
		minY = std::min(p1.y, p2.y);
		maxX = std::max(p1.x, p2.x);
		maxY = std::max(p1.y, p2.y);
		assert(maxY >= minY);
		assert(maxX >= minX);
	}

	basic_rect(T minX, T minY, T maxX, T maxY)
		: minY(minY)
		, maxY(maxY)
		, maxX(maxX)
		, minX(minX)
	{
		assert(maxY >= minY);
		assert(maxX >= minX);
	}

	basic_rect(const basic_vector2<T>& minCoord, T width, T height)
		: minX(minCoord.x)
		, minY(minCoord.y)
		, maxX(minCoord.x + width)
		, maxY(minCoord.y + height)
	{
	}

	inline basic_rect<T> translate(T dx, T dy) const
	{
		return basic_rect<T>(minX + dx, minY + dy, maxX + dx, maxY + dy);
	}

	inline basic_rect<T> grow(T dx, T dy) const
	{
		return basic_rect<T>(minX - dx, minY - dy, maxX + dx, maxY + dy);
	}

	inline void enclose(const basic_rect<T>& other)
	{
		minX = std::min(minX, other.minX);
		maxX = std::max(maxX, other.maxX);
		minY = std::min(minY, other.minY);
		maxY = std::max(maxY, other.maxY);
	}

	inline void enclose(const basic_vector2<T>& other)
	{
		minX = std::min(minX, other.x);
		maxX = std::max(maxX, other.x);
		minY = std::min(minY, other.y);
		maxY = std::max(maxY, other.y);
	}

	inline T getArea() const {
		return (maxY - minY) * (maxX - minX);
	}

	inline basic_vector2<T> getCenter() const {
		return basic_vector2<T>((minX + maxX) / 2.0, (minY + maxY) / 2.0);
	}

	inline basic_rect<T> getIntersection(const basic_rect<T>& other) const {
		T x0 = std::max(other.minX, minX);
		T y0 = std::max(other.minY, minY);
		T x1 = std::min(other.maxX, maxX);
		T y1 = std::min(other.maxY, maxY);
		if (x0 >= x1 || y0 >= y1)
			return basic_rect<T>();
		return basic_rect<T>(x0, y0, x1, y1);
	}

	inline bool contains(const basic_vector2<T>& p) const
	{
		return (between(minX, p.x, maxX) && between(minY, p.y, maxY));
	}

	inline bool contains(const basic_rect<T>& other) const
	{
		return (between(minX, other.minX, maxX)
			&&  between(minY, other.minY, maxY)
			&&  between(minX, other.maxX, maxX)
			&&  between(minY, other.maxY, maxY));
	}

	inline bool intersects(const basic_rect<T>& bounding) const {
		return ((between(minX, bounding.maxX, maxX)
			||  between(minX, bounding.minX, maxX)
			||  between(bounding.minX, maxX, bounding.maxX)
			||  between(bounding.minX, minX, bounding.maxX))
			&& (between(minY, bounding.maxY, maxY)
			||  between(minY, bounding.minY, maxY)
			||  between(bounding.minY, maxY, bounding.maxY)
			||  between(bounding.minY, minY, bounding.maxY)));
	}

	inline T getWidth() const {
		return (maxX - minX);
	}

	inline T getHeight() const {
		return (maxY - minY);
	}

private:
	inline bool between(const T& min, const T& value, const T& max) const
	{
		assert(min <= max);
		return (min <= value && value <= max);
	}
};

typedef basic_rect<double> FloatRect;
typedef basic_rect<coord_t> FixedRect;

template<class T>
bool operator==(const basic_rect<T>& a, const basic_rect<T>& b)
{
	return (a.minX == b.minX && a.minY == b.minY && a.maxX == b.maxX && a.maxY == b.maxY);
}

template<class T>
bool operator!=(const basic_rect<T>& a, const basic_rect<T>& b)
{
	return (a.minX != b.minX || a.minY != b.minY || a.maxX != b.maxX || a.maxY != b.maxY);
}

#endif
