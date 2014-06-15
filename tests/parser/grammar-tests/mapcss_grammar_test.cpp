
#include "../../tests.hpp"

#include <boost/filesystem.hpp>

#include "../../shared/compare.hpp"

#include "server/parser/mapcss_parser.hpp"


namespace fs = boost::filesystem;

struct MapCSSParserTest
{
	MapCSSParserTest()
	{
	}

	void testFile(const fs::path& path)
	{
		BOOST_TEST_CHECKPOINT("Checking " << path.std::string());

		bool shouldFail = false;
		{
			std::ifstream file(path.std::string());
			BOOST_REQUIRE(file.is_open());

			std::string firstLine;
			std::getline(file, firstLine);

			auto& npos = std::string::npos;

			bool isTrue = (firstLine.find("@success") != npos);
			bool isFalse = (firstLine.find("@fail") != npos);
			BOOST_REQUIRE_MESSAGE(isTrue != isFalse, "In the first line only @success xor @fail have to appear in " << path.filename() << "!");
			shouldFail = isFalse;
		}

		std::shared_ptr<Geodata> data;
		MapCssParser parser(data);
		parser.load(path.std::string());

		BOOST_CHECK_MESSAGE((!parser.parsedStylesheet) == shouldFail, "Expected another conclusion in " << path.filename() << "!");
		if(!parser.parsedStylesheet && !shouldFail)
		{
			BOOST_TEST_MESSAGE(boost::diagnostic_information(parser.catchedException));
		}
	}

	void testDirectory(const fs::path& dir)
	{
		int count = 0;
		for(auto it = fs::directory_iterator(dir); it != fs::directory_iterator(); ++it)
		{
			auto& p = *it;
			if(p.path().extension() != ".error")
			{
				testFile(p);
				++count;
			}
		}

		BOOST_REQUIRE_MESSAGE(count > 0, "No files have been tested!");
	}

};

ALAC_START_FIXTURE_TEST(MapCSSParserTest)

	ALAC_FIXTURE_TEST(testDirectory, getInputDirectory() / "parser" / "mapcss");

ALAC_END_FIXTURE_TEST();

