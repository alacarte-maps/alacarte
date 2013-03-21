#pragma once
#ifndef TESTS_HPP
#define TESTS_HPP
/*
 * =====================================================================================
 *
 *       Filename:  tests.hpp
 *
 *    Description:  Includes and stuff for testing
 *					See Program guide for more information about testing.
 *
 * =====================================================================================
 */

#include <boost/test/unit_test.hpp>

#include "settings.hpp"

#define ALAC_PARAM_TEST_CASE(_name, ...)	\
			void _name (__VA_ARGS__)



#define ALAC_PARAM_TEST(_func, ...)	\
			ALAC_PARAM_TEST_NAMED(_func, ALAC_JOIN(_func, ALAC_JOIN(_in_, __LINE__)), __VA_ARGS__)
#define ALAC_PARAM_TEST_NAMED(_func, _name, ...)	\
			BOOST_AUTO_TEST_CASE(_name)	{ _func(__VA_ARGS__); }

#define ALAC_START_FIXTURE_TEST(_test, ...)	\
			ALAC_START_FIXTURE_TEST_NAMED(ALAC_JOIN(_test, ALAC_JOIN(_in_, __LINE__)), _test, __VA_ARGS__)
#define ALAC_START_FIXTURE_TEST_NAMED(_name, _fixture, ...)								\
			struct ALAC_JOIN(_name, _help_fixture)										\
			{																			\
				typedef _fixture fixture;												\
				ALAC_JOIN(_name, _help_fixture)()										\
				{																		\
					m_fix = new fixture(__VA_ARGS__);									\
				}																		\
																						\
				~ALAC_JOIN(_name, _help_fixture)()										\
				{																		\
					delete m_fix;														\
				}																		\
																						\
				fixture* m_fix;															\
			};																			\
			BOOST_FIXTURE_TEST_SUITE(_name, ALAC_JOIN(_name, _help_fixture))			

#define ALAC_END_FIXTURE_TEST()		\
			BOOST_AUTO_TEST_SUITE_END()

#define ALAC_FIXTURE_TEST(_func, ...)	\
			ALAC_FIXTURE_TEST_NAMED(_func, ALAC_JOIN(_func, ALAC_JOIN(_in_, __LINE__)), __VA_ARGS__);
#define ALAC_FIXTURE_TEST_NAMED(_func, _name, ...)	\
			BOOST_AUTO_TEST_CASE(_name) { m_fix->_func(__VA_ARGS__); }





#endif
