#include "../../tests.hpp"
#include <boost/test/unit_test.hpp>

#include "server/tile_identifier.hpp"

BOOST_AUTO_TEST_SUITE(tile_identifier_test)
struct tile_identifier_test
{
	tile_identifier_test() {
		
	}
	
	void test_identifier() {
		// TODO: Check for correct message.
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)""), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/a/0/0.png"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/-2/0.png"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/a.png"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/apng"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.wrong"), excp::UnknownImageFormatException);
	}
	
	void test_image_formats() {
		BOOST_CHECK_NO_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.png"));
		BOOST_CHECK_NO_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.svg"));
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.jpg"), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.jpeg"), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.gif"), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.svgz"), excp::UnknownImageFormatException);
	}
	
	void test_plausibility_check() {
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/3/2345/0.png"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/3/0/2345.png"), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/28/0/0.png"), excp::MalformedURLException);
	}
	
	void test_correct() {
		shared_ptr<TileIdentifier> ti;
		BOOST_CHECK_NO_THROW(ti = TileIdentifier::Create((const string&)"/folder/stylesheet/2/1/0.svg"));
		BOOST_CHECK_EQUAL(ti->getX(), 1);
		BOOST_CHECK_EQUAL(ti->getY(), 0);
		BOOST_CHECK_EQUAL(ti->getZoom(), 2);
		BOOST_CHECK_EQUAL(ti->getImageFormat(), TileIdentifier::Format::SVG);
		BOOST_CHECK_EQUAL(ti->getStylesheetPath(), "folder/stylesheet");
		BOOST_CHECK_NO_THROW(ti = TileIdentifier::CreateNoneDataTID(ti));
		BOOST_CHECK_EQUAL(ti->getX(), -2);
		BOOST_CHECK_EQUAL(ti->getY(), -2);
		BOOST_CHECK_EQUAL(ti->getZoom(), -2);
		BOOST_CHECK_EQUAL(ti->getImageFormat(), TileIdentifier::Format::PNG);
		BOOST_CHECK_EQUAL(ti->getStylesheetPath(), "folder/stylesheet");
		BOOST_CHECK(ti->isNoneDataIdentifier());
	}
};

ALAC_START_FIXTURE_TEST(tile_identifier_test)
	// functionname, name of test, arguments of function...
	ALAC_FIXTURE_TEST_NAMED(test_identifier, TileIdentifierWrongInput);
	ALAC_FIXTURE_TEST_NAMED(test_plausibility_check, TileIdentifierPlausibilityTest);
	ALAC_FIXTURE_TEST_NAMED(test_correct, TileIdentifierCorrectInput);
	ALAC_FIXTURE_TEST_NAMED(test_image_formats, TileIdentifierImageFormats);
ALAC_END_FIXTURE_TEST()

// Zum schluss noch suite schließen.
BOOST_AUTO_TEST_SUITE_END(/*tile_identifier_test*/)
