#pragma once
#ifndef _RANDOM_HPP
#define _RANDOM_HPP

#include "settings.hpp"


class Random
{
public:
	Random();
	Random(unsigned long _init);
	~Random();

	void next();

	template<typename T>
	inline T rand();
	inline int rand();
	template<typename T>
	inline T rand(T _max);
	template<typename T>
	inline T rand(T _min, T _max);


	inline static void Next();

	template<typename T>
	inline static T Rand();
	inline static int Rand();
	template<typename T>
	inline static T Rand(T _max);
	template<typename T>
	inline static T Rand(T _min, T _max);

private:
	static Random	m_RandomGenerator;
	unsigned long	m_RandomSeed;
};


template<typename T>
inline T Random::rand()
{
	next();
	return static_cast<T>(m_RandomSeed);
}

inline int Random::rand()
{
	return rand<int>();
}

template<typename T>
inline T Random::rand(T _max)
{
	return rand(static_cast<T>(0), _max);
}

template<typename T>
inline T Random::rand(T _min, T _max)
{
	return static_cast<T>(_min + ((rand<int>() - 1) / 2147483646.0) * (_max - _min));
}



inline void Random::Next()
{
	m_RandomGenerator.next();
}

template<typename T>
inline T Random::Rand()
{
	return m_RandomGenerator.rand<T>();
}

inline int Random::Rand()
{
	return m_RandomGenerator.rand<int>();
}

template<typename T>
inline T Random::Rand(T _max)
{
	return m_RandomGenerator.rand(_max);
}

template<typename T>
inline T Random::Rand(T _min, T _max)
{
	return m_RandomGenerator.rand(_min, _max);
}


#endif
