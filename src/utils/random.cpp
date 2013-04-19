
#include "utils/random.hpp"

#include <ctime>



Random Random::m_RandomGenerator;


Random::Random()
	: m_RandomSeed(0)
{
	m_RandomSeed = (unsigned int)clock();

	if(m_RandomSeed == 0)
		m_RandomSeed = 1;

	next();
}

Random::Random(unsigned long _init)
	: m_RandomSeed(_init)
{
}

Random::~Random()
{
}


void Random::next()
{
	// optimized standard minimal
	long tmp0 = 16807L * (m_RandomSeed & 0xFFFFL);
	long tmp1 = 16807L * (m_RandomSeed >> 16);
	long tmp2 = (tmp0 >> 16) + tmp1;
		 tmp0 = ((tmp0 & 0xFFFF)|((tmp2 & 0x7FFF) << 16)) + (tmp2 >> 15);

	// correction of the error
	if ((tmp0 & 0x80000000L) != 0)
		tmp0 = (tmp0 + 1) & 0x7FFFFFFFL;

	m_RandomSeed = tmp0;
}