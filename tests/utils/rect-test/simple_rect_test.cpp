#include "../../tests.hpp"
#include "utils/point.hpp"

BOOST_AUTO_TEST_SUITE(simple_rect_test)

void outside_test(const FloatRect& r, const FloatPoint& p)
{
	BOOST_CHECK(!r.contains(p));
}

void contains_test(const FloatRect& r, const FloatPoint& p)
{
	BOOST_CHECK(r.contains(p));
}

void intersects_test(const FloatRect& r1, const FloatRect& r2)
{
	BOOST_CHECK(r1.intersects(r2));
}

void intersecting_test(const FloatRect& r1, const FloatRect& r2, double area)
{
	double test = r1.getIntersection(r2).getArea();
	double diff = test - area;
	BOOST_TEST_MESSAGE("Area: " << test);
	BOOST_CHECK(diff > -0.0001 && diff < 0.0001);
}

/*
 * Quad I
 */
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(0.5, 0.5));
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, 1.0)), FloatPoint(0.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(2.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(0.5, 2.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, 1.0)), FloatPoint(2.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, 1.0)), FloatPoint(0.5, 2.5));
ALAC_PARAM_TEST(intersecting_test, FloatRect(FloatPoint(1.0, 1.0), FloatPoint(3.0, 4.0)), FloatRect(FloatPoint(2.0, 2.0), FloatPoint(3.5, 4.0)), 2.0);

/*
 * Quad II
 */
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(-1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(-0.5, 0.5));
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, 1.0)), FloatPoint(-0.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(-1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(-2.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(-1.0, 1.0), FloatPoint(0.1, 0.1)), FloatPoint(-0.5, 2.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, 1.0)), FloatPoint(-2.5, 0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, 1.0)), FloatPoint(-0.5, 2.5));
ALAC_PARAM_TEST(intersecting_test, FloatRect(FloatPoint(-1.0, 1.0), FloatPoint(-3.0, 4.0)), FloatRect(FloatPoint(-2.0, 2.0), FloatPoint(-3.5, 4.0)), 2.0);

/*
 * Quad III
 */
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(-1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(-0.5, -0.5));
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, -1.0)), FloatPoint(-0.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(-1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(-2.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(-1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(-0.5, -2.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, -1.0)), FloatPoint(-2.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(-1.0, -1.0)), FloatPoint(-0.5, -2.5));
ALAC_PARAM_TEST(intersecting_test, FloatRect(FloatPoint(-1.0, -1.0), FloatPoint(-3.0, -4.0)), FloatRect(FloatPoint(-2.0, -2.0), FloatPoint(-3.5, -4.0)), 2.0);

/*
 * Quad IV
 */
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(0.5, -0.5));
ALAC_PARAM_TEST(contains_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, -1.0)), FloatPoint(0.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(2.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(1.0, -1.0), FloatPoint(0.1, 0.1)), FloatPoint(0.5, -2.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, -1.0)), FloatPoint(2.5, -0.5));
ALAC_PARAM_TEST(outside_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, -1.0)), FloatPoint(0.5, -2.5));
ALAC_PARAM_TEST(intersecting_test, FloatRect(FloatPoint(1.0, -1.0), FloatPoint(3.0, -4.0)), FloatRect(FloatPoint(2.0, -2.0), FloatPoint(3.5, -4.0)), 2.0);
/*
 * Quad I
 */
// r1 intersects r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, 1.0)), FloatRect(FloatPoint(0.5, 0.5), FloatPoint(2.0, 2.0)));
// r2 intersects r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.5, 0.5), FloatPoint(2.0, 2.0)), FloatRect(FloatPoint(0.1, 0.1), FloatPoint(1.0, 1.0)));
// r1 contains r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.1, 0.1), FloatPoint(0.5, 0.5)), FloatRect(FloatPoint(0.0, 0.0), FloatPoint(1.0, 1.0)));
// r2 contains r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.0, 0.0), FloatPoint(1.0, 1.0)), FloatRect(FloatPoint(0.1, 0.1), FloatPoint(0.5, 0.5)));

/*
 * Quad II
 */
// r1 intersects r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.1, 0.1), FloatPoint(-1.0, 1.0)), FloatRect(FloatPoint(-0.5, 0.5), FloatPoint(-2.0, 2.0)));
// r2 intersects r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.5, 0.5), FloatPoint(-2.0, 2.0)), FloatRect(FloatPoint(-0.1, 0.1), FloatPoint(-1.0, 1.0)));
// r1 contains r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.1, 0.1), FloatPoint(-0.5, 0.5)), FloatRect(FloatPoint(0.0, 0.0), FloatPoint(-1.0, 1.0)));
// r2 contains r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.0, 0.0), FloatPoint(-1.0, 1.0)), FloatRect(FloatPoint(-0.1, 0.1), FloatPoint(-0.5, 0.5)));


/*
 * Quad III
 */
// r1 intersects r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.1, -0.1), FloatPoint(-1.0, -1.0)), FloatRect(FloatPoint(-0.5, -0.5), FloatPoint(-2.0, -2.0)));
// r2 intersects r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.5, -0.5), FloatPoint(-2.0, -2.0)), FloatRect(FloatPoint(-0.1, -0.1), FloatPoint(-1.0, -1.0)));
// r1 contains r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(-0.1, -0.1), FloatPoint(-0.5, -0.5)), FloatRect(FloatPoint(0.0, 0.0), FloatPoint(-1.0, -1.0)));
// r2 contains r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.0, 0.0), FloatPoint(-1.0, -1.0)), FloatRect(FloatPoint(-0.1, -0.1), FloatPoint(-0.5, -0.5)));

/*
 * Quad IV
 */
// r1 intersects r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.1, -0.1), FloatPoint(1.0, -1.0)), FloatRect(FloatPoint(0.5, -0.5), FloatPoint(2.0, -2.0)));
// r2 intersects r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.5, -0.5), FloatPoint(2.0, -2.0)), FloatRect(FloatPoint(0.1, -0.1), FloatPoint(1.0, -1.0)));
// r1 contains r2
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.1, -0.1), FloatPoint(0.5, -0.5)), FloatRect(FloatPoint(0.0, 0.0), FloatPoint(1.0, -1.0)));
// r2 contains r1
ALAC_PARAM_TEST(intersects_test, FloatRect(FloatPoint(0.0, 0.0), FloatPoint(1.0, -1.0)), FloatRect(FloatPoint(0.1, -0.1), FloatPoint(0.5, -0.5)));

BOOST_AUTO_TEST_SUITE_END()
