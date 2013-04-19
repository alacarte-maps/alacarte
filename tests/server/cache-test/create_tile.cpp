#include "../../tests.hpp"
#include <boost/test/unit_test.hpp>

#include "configmockup.hpp"

#include "server/tile_identifier.hpp"
#include "server/stylesheet_manager.hpp"

class MockStylesheetManager : public StylesheetManager
{
public:
	MockStylesheetManager(const shared_ptr<Configuration>& config)
		: StylesheetManager(config)
	{
	}

	virtual bool hasStylesheet(const string& path)
	{
		return true;
	}
};

BOOST_AUTO_TEST_SUITE(tile_identifier_test)
struct tile_identifier_test
{
	shared_ptr<Configuration> config;
	shared_ptr<StylesheetManager> ssm;
	tile_identifier_test() {
		ConfigMockup* mock = new ConfigMockup();
		const char* argv[] = {"alacarte-server", "-g", "ala.carte"};
		config = mock->Config((char**) argv, 3);
		ssm = boost::dynamic_pointer_cast<StylesheetManager>(boost::make_shared<MockStylesheetManager>(config));

	}
	
	void test_identifier() {
		// TODO: Check for correct message.
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/a/0/0.png", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/-2/0.png", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/a.png", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/apng", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.wrong", ssm, config), excp::UnknownImageFormatException);
	}
	
	void test_image_formats() {
		BOOST_CHECK_NO_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.png", ssm, config));
		BOOST_CHECK_NO_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.svg", ssm, config));
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.jpg", ssm, config), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.jpeg", ssm, config), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.gif", ssm, config), excp::UnknownImageFormatException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/0/0/0.svgz", ssm, config), excp::UnknownImageFormatException);
	}
	
	void test_plausibility_check() {
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/3/2345/0.png", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/3/0/2345.png", ssm, config), excp::MalformedURLException);
		BOOST_CHECK_THROW(TileIdentifier::Create((const string&)"/stylesheet/28/0/0.png", ssm, config), excp::MalformedURLException);
	}
	
	void test_correct() {
		shared_ptr<TileIdentifier> ti;
		BOOST_CHECK_NO_THROW(ti = TileIdentifier::Create((const string&)"/folder/stylesheet/2/1/0.svg", ssm, config));
		BOOST_CHECK_EQUAL(ti->getX(), 1);
		BOOST_CHECK_EQUAL(ti->getY(), 0);
		BOOST_CHECK_EQUAL(ti->getZoom(), 2);
		BOOST_CHECK_EQUAL(ti->getImageFormat(), TileIdentifier::Format::SVG);
		BOOST_CHECK_EQUAL(ti->getStylesheetPath(), "folder/stylesheet");
		BOOST_CHECK_NO_THROW(ti = TileIdentifier::CreateEmptyTID(ti->getStylesheetPath(), ti->getImageFormat()));
		BOOST_CHECK_EQUAL(ti->getX(), -2);
		BOOST_CHECK_EQUAL(ti->getY(), -2);
		BOOST_CHECK_EQUAL(ti->getZoom(), -2);
		BOOST_CHECK_EQUAL(ti->getImageFormat(), TileIdentifier::Format::SVG);
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
