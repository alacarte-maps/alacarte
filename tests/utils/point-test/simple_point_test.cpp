
#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_point_test)


void equal_test(const FloatPoint& p1, const FloatPoint& p2)
{
	BOOST_CHECK_EQUAL( p1, p2);
}

ALAC_PARAM_TEST(equal_test,FloatPoint(3, 4),FloatPoint(3, 4));
ALAC_PARAM_TEST(equal_test,FloatPoint(3, 8),FloatPoint(3, 8));
ALAC_PARAM_TEST(equal_test,FloatPoint(3, 4),FloatPoint(3, 4));



struct simple_check_test
{
	simple_check_test(const FloatPoint& p)
	{
		point = p;
	}

	void compare(const FloatPoint& p)
	{
		BOOST_CHECK_EQUAL(point, p);
	}

	FloatPoint point;
};

ALAC_START_FIXTURE_TEST(simple_check_test,FloatPoint(5, 5))
	ALAC_FIXTURE_TEST(compare,FloatPoint(5,5));
	ALAC_FIXTURE_TEST_NAMED(compare, compare_with_5_and_5,FloatPoint(5,5));
ALAC_END_FIXTURE_TEST()



BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
