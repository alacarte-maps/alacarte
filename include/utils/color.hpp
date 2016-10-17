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
#ifndef COLOR_HPP
#define COLOR_HPP

/*
 * =====================================================================================
 *
 *       Filename:  settings.h
 *
 *    Description:  
 *
 * =====================================================================================
 */

#include "settings.hpp"


#define cxCOLOR_CONV (0.003921568627450980392156862745098f)

class Color
{
public:
	Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
	Color(const Color& _c) : r(_c.r), g(_c.g), b(_c.b), a(_c.a) {}
	Color(const float _f) : r(_f), g(_f), b(_f), a(1.0f) {}
	explicit Color(const float _r, const float _g, const float _b)
		: r(_r), g(_g), b(_b), a(1.0f) {}
	explicit Color(const float _r, const float _g, const float _b, const float _a)
		: r(_r), g(_g), b(_b), a(_a) {}
	explicit Color(uint8 _r, uint8 _g, uint8 _b)
		: r((float)(_r) * cxCOLOR_CONV)
		, g((float)(_g) * cxCOLOR_CONV)
		, b((float)(_b) * cxCOLOR_CONV)
		, a(1.0f) {}
	explicit Color(uint8 _r, uint8 _g, uint8 _b, uint8 _a)
		: r((float)(_r) * cxCOLOR_CONV)
		, g((float)(_g) * cxCOLOR_CONV)
		, b((float)(_b) * cxCOLOR_CONV)
		, a((float)(_a) * cxCOLOR_CONV) {}
	Color(const float* _c)
		: r(_c[0]), g(_c[1]), b(_c[2]), a(_c[3]) {}
	Color(const uint8* pComponent)
		: r((float)(pComponent[0]) * cxCOLOR_CONV)
		, g((float)(pComponent[1]) * cxCOLOR_CONV)
		, b((float)(pComponent[2]) * cxCOLOR_CONV)
		, a((float)(pComponent[3]) * cxCOLOR_CONV) {}

	Color(uint32 _c)
		: r(cxCOLOR_CONV * (float)(uint8)(_c >> 16))
		, g(cxCOLOR_CONV * (float)(uint8)(_c >> 8))
		, b(cxCOLOR_CONV * (float)(uint8)(_c))
		, a(cxCOLOR_CONV * (float)(uint8)(_c >> 24))
	{}


	operator uint32 () const
	{
		return ((a >= 1.0f ? 255 : a <= 0.0f ? 0 : (uint32)(a * 255.0f)) << 24) |
			((r >= 1.0f ? 255 : r <= 0.0f ? 0 : (uint32)(r * 255.0f)) << 16) |
			((g >= 1.0f ? 255 : g <= 0.0f ? 0 : (uint32)(g * 255.0f)) << 8)  |
			(b >= 1.0f ? 255 : b <= 0.0f ? 0 : (uint32)(b * 255.0f));
	}

	operator float* ()			{return (float*)(c);}

	inline Color& operator +=	(const Color& c)	{r += c.r; g += c.g; b += c.b; a += c.a; return *this;}
	inline Color& operator -=	(const Color& c)	{r -= c.r; g -= c.g; b -= c.b; a -= c.a; return *this;}
	inline Color& operator *=	(const Color& c)	{r *= c.r; g *= c.g; b *= c.b; a *= c.a; return *this;}
	inline Color& operator *=	(const float f)		{r *= f; g *= f; b *= f; a *= f; return *this;}
	inline Color& operator /=	(const Color& c)	{r /= c.r; g /= c.g; b /= c.b; a /= c.a; return *this;}
	inline Color& operator /=	(const float f)		{r /= f; g /= f; b /= f; a /= f; return *this;}

	inline Color operator + (const Color& _c) const	{return Color(r + _c.r, g + _c.g, b + _c.b, a + _c.a);}
	inline Color operator - (const Color& _c) const	{return Color(r - _c.r, g - _c.g, b - _c.b, a - _c.a);}
	inline Color operator - () const 					{return Color(-r, -g, -b, a);}
	inline Color operator * (const Color& _c) const	{return Color(r * _c.r, g * _c.g, b * _c.b, a * _c.a);}
	inline Color operator * (const float f) const		{return Color(r * f, g * f, b * f, a * f);}
	inline Color operator / (const Color& _c) const	{return Color(r / _c.r, g / _c.g, b / _c.b, a / _c.a);}
	inline Color operator / (const float f) const		{return Color(r / f, g / f, b / f, a / f);}

	inline bool operator == (const Color& _c) const {return (uint32)*this == (uint32)_c;}
	inline bool operator != (const Color& _c) const {return (uint32)*this != (uint32)_c;}


	inline Color negate()             const { return Color(1.0f - r, 1.0f - g, 1.0f - b, a); }
	inline Color negateA()            const { return Color(1.0f - r, 1.0f - g, 1.0f - b, 1.0f - a); }
	inline Color min(const Color& _c) const { return Color(std::min(r, _c.r), std::min(g, _c.g), std::min(b, _c.b), std::min(a, _c.a)); }
	inline Color max(const Color& _c) const { return Color(std::max(r, _c.r), std::max(g, _c.g), std::max(b, _c.b), std::max(a, _c.a)); }

	friend std::ostream& operator<< (std::ostream& stream, const Color& color)
	{
		stream << "Color(" << color.r << "," << color.g << "," << color.b << "," << color.a << ")";
		return stream;
	}

	//inline Color&	rand()			{ r = Random::Rand<float>(1.0f); greturn (*this); }

	static const Color Null;
	static const Color Black;
	static const Color White;
	static const Color Red;
	static const Color Green;
	static const Color Blue;
	static const Color Yellow;
	static const Color Grey;
	static const Color LightGrey;

	union
	{
		struct
		{
			float r;
			float g;
			float b;
			float a;
		};
		struct
		{
			float red;
			float green;
			float blue;
			float alpha;
		};
		float			c[4];
	};
};

#endif
