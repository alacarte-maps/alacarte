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
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <vector>
#include <sstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

template<typename T>
class basic_vector2
{
private:
	friend class boost::serialization::access;
	template<typename Archive>
	void serialize(Archive &ar, const unsigned int version){
		ar & x;
		ar & y;
	}
public:
	basic_vector2()	: x(T(0)), y(T(0))									{}
	basic_vector2(const basic_vector2& _v)	: x(_v.x), y(_v.y)			{}
	basic_vector2(const T _v)				: x(_v), y(_v)				{}
	basic_vector2(const T _x, const T _y)	: x(_x), y(_y)				{}

	// Casting-Operatoren
	operator float* () {return (float*)(c);}

	// Zuweisungsoperatoren
	basic_vector2& operator = (const basic_vector2& v)	{x = v.x; y = v.y;		return (*this);}
	basic_vector2& operator += (const basic_vector2& v)	{x += v.x; y += v.y;	return (*this);}
	basic_vector2& operator -= (const basic_vector2& v)	{x -= v.x; y -= v.y;	return (*this);}
	basic_vector2& operator *= (const basic_vector2& v)	{x *= v.x; y *= v.y;	return (*this);}
	basic_vector2& operator *= (const T f)				{x *= f; y *= f;		return (*this);}
	basic_vector2& operator /= (const basic_vector2& v)	{x /= v.x; y /= v.y;	return (*this);}
	basic_vector2& operator /= (const T f)				{x /= f; y /= f;		return (*this);}

	// Arithmetische Operatoren
	inline basic_vector2 operator + (const basic_vector2& v)	const	{return basic_vector2(x + v.x, y + v.y);}
	inline basic_vector2 operator - (const basic_vector2& v)	const	{return basic_vector2(x - v.x, y - v.y);}
	inline basic_vector2 operator - ()							const	{return basic_vector2(-x, -y);}
	inline basic_vector2 operator * (const basic_vector2& v)	const	{return basic_vector2(x * v.x, y * v.y);}
	inline basic_vector2 operator * (const T f)					const	{return basic_vector2(x * f, y * f);}
	inline basic_vector2 operator / (const basic_vector2& v)	const	{return basic_vector2(x / v.x, y / v.y);}
	inline basic_vector2 operator / (const T f)					const	{return basic_vector2(x / f, y / f);}

	// Vergleichsoperatoren
	inline bool operator == (const basic_vector2& v) const {return (x == v.x && y == v.y);}
	inline bool operator != (const basic_vector2& v) const {return (x != v.x || y != v.y);}

	inline double distance(const basic_vector2& v) const { return std::sqrt(distanceQuad(v));}
	inline double distanceQuad(const basic_vector2& v) const { basic_vector2<double> delta(v - *this); delta*=delta; return delta.x + delta.y; }
	inline bool isInRange(const basic_vector2& v, double epsilon = 0.000000001) const { return distanceQuad(v) < epsilon; }

	operator basic_vector2<int>	 ()		const {return basic_vector2<int>(static_cast<int>(x), static_cast<int>(y));}
	operator basic_vector2<float> ()	const {return basic_vector2<float>(static_cast<float>(x), static_cast<float>(y));}
	operator basic_vector2<double>()	const {return basic_vector2<double>(static_cast<double>(x), static_cast<double>(y));}

	inline T area() const { return w * h; }

	union
	{
		struct
		{
			T x;
			T y;
		};

		struct
		{
			T lon;
			T lat;
		};

		struct
		{
			T w;
			T h;
		};
		
		T c[2];
	};
};


// Arithmetische Operatoren
template<typename T>
inline basic_vector2<T> operator * (const T f, const basic_vector2<T>& v)		{return basic_vector2<T>(v.x * f, v.y * f);}
template<typename T>
inline basic_vector2<T> operator / (const T f, const basic_vector2<T>& v)		{return basic_vector2<T>(v.x / f, v.y / f);}





template<typename T, typename C>
inline std::basic_istream<C>& operator >> (std::basic_istream<C>& is, basic_vector2<T>& v)
{
	C op;

	is >> v.x;
	is >> op;
	is >> v.y;

	if((op != C(',') && op != C('x') && op != C('|')) || is.bad())
	{
		throw std::runtime_error("Not able to convert from stream to vector!");
	}

	return is;
}

template<typename T, typename C>
std::basic_ostream<C>& operator << (std::basic_ostream<C>& os, const basic_vector2<T>& v)
{
	os << v.x << C(',') << C(' ') << v.y;
	return os;
}


typedef basic_vector2<coord_t>		FixedPoint;
typedef basic_vector2<double>		FloatPoint;
typedef basic_vector2<double>		Vector;

#endif
