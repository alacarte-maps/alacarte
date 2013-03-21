#include "../../tests.hpp"
#include "utils/point.hpp"
#include "utils/rect.hpp"
#include "utils/transform.hpp"

BOOST_AUTO_TEST_SUITE(call_test)
#define MIN(a, b) (a < b ? a : b)
#define MAX(a, b) (a > b ? a : b)
void contains_test(int tx, int ty, int zoom, double py, double px)
{
	coord_t x0, y0, x1, y1;
	tileToMercator(tx, ty, zoom, x0, y0);
	tileToMercator(tx+1, ty+1, zoom, x1, y1);

	FixedRect r = FixedRect(FixedPoint(x0, y0), FixedPoint(x1, y1));	
	FloatPoint p = FloatPoint(px, py);
	FixedPoint tranformed;
	projectMercator(p, tranformed.x, tranformed.y);
	BOOST_TEST_MESSAGE("Rect (" << r.minX << ", " << r.minY << " | " << r.maxX << ", " << r.maxY << ")");
	BOOST_TEST_MESSAGE("Point (" << tranformed.x << ", " << tranformed.y << ")");
	BOOST_CHECK(r.contains(tranformed));
}

void tile_inv_test(int tx, int ty, int zoom)
{
	coord_t x, y;
	tileToMercator(tx, ty, zoom, x, y);
	int ntx, nty;
	mercatorToTile(x, y, zoom, ntx, nty);

	int dx = ntx - tx;
	int dy = nty - ty;
	/* account for rounding errors (we are at the very edge of 4 tiles)*/
	BOOST_CHECK(dx < 1 || dx > -1);
	BOOST_CHECK(dy < 1 || dy > -1);
}

void mercator_inv_test(double lat, double lon)
{
	coord_t x, y;
	projectMercator(FloatPoint(lat, lon), x, y);
	double nlat, nlon;
	inverseMercator(FixedPoint(x, y), nlat, nlon);

	double dlat = nlat - lat;
	double dlon = nlon - lon;

	BOOST_CHECK(dlat < 0.1e-14 || dlat > -0.1e-14);
	BOOST_CHECK(dlon < 0.1e-14 || dlon > -0.1e-14);
}

ALAC_PARAM_TEST(tile_inv_test, 0, 0, 18);
ALAC_PARAM_TEST(tile_inv_test, 120, 1337, 18);
ALAC_PARAM_TEST(tile_inv_test, (1<<18), (1<<18), 18);
ALAC_PARAM_TEST(tile_inv_test, 20, 44, 10);
ALAC_PARAM_TEST(tile_inv_test, 4, 10, 5);
ALAC_PARAM_TEST(tile_inv_test, 0, 0, 0);
ALAC_PARAM_TEST(tile_inv_test, 1, 1, 0);

ALAC_PARAM_TEST(mercator_inv_test, 45.0, 45.0);
ALAC_PARAM_TEST(mercator_inv_test, 33.0, 23.0);
ALAC_PARAM_TEST(mercator_inv_test, -45.0, 77.0);

ALAC_PARAM_TEST_NAMED(contains_test, contains_0, 0, 0, 0, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_1, 1, 0, 1, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_2, 2, 1, 2, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_3, 4, 2, 3, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_4, 8, 5, 4, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_5, 16, 10, 5, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_6, 33, 21, 6, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_7, 66, 43, 7, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_8, 132, 87, 8, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_9, 265, 174, 9, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_10, 531, 348, 10, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_11, 1062, 696, 11, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_12, 2124, 1392, 12, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_13, 4249, 2784, 13, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_14, 8498, 5568, 14, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_15, 16997, 11136, 15, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_16, 33994, 22273, 16, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_17, 67989, 44547, 17, 49.8309499, 6.7398346);
ALAC_PARAM_TEST_NAMED(contains_test, contains_18, 135979, 89095, 18, 49.8309499, 6.7398346);

BOOST_AUTO_TEST_SUITE_END(/*simple_point_test*/)
