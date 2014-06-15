
#include "../../tests.hpp"

#include <boost/spirit/include/qi.hpp>

#include "../../shared/grammar_tester.hpp"

#include "general/geo_object.hpp"
#include "server/eval/eval.hpp"
#include "server/parser/color_grammar.hpp"


Color makeColorFromUints(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
{
	return Color(r,g,b,a);
}


struct ColorGrammarTest
{
	ColorGrammarTest()
	{
	}




	void testString(const std::string& expr, const Color& expected)
	{
		Color result;
		ColorGrammar grammar;

		try {
			testGrammar(grammar, result, expr.begin(), expr.end(), chs::space);
		} catch(excp::ParseException&)
		{
			BOOST_FAIL("Failed to parse \"" + expr + "\"");
		}
		BOOST_CHECK_EQUAL(expected, result);
	}

};




ALAC_START_FIXTURE_TEST(ColorGrammarTest)
	ALAC_FIXTURE_TEST(testString, "#000000", Color(0xFF000000));
	ALAC_FIXTURE_TEST(testString, "#FF00FF", Color(0xFFFF00FF));
	ALAC_FIXTURE_TEST(testString, "#aa00bb", Color(0xFFAA00BB));

	ALAC_FIXTURE_TEST(testString, "rgb(1.0,1.0,1.0)", Color(1.0f, 1.0f, 1.0f));
	ALAC_FIXTURE_TEST(testString, "rgb(0.5,1.0,1.0)", Color(0.5f, 1.0f, 1.0f));
	ALAC_FIXTURE_TEST(testString, "rgb(1.0,0.5 , 1.0)", Color(1.0f, 0.5f, 1.0f));
	ALAC_FIXTURE_TEST(testString, "rgb (1.0,1.0,0.5)", Color(1.0f, 1.0f, 0.5f));
	ALAC_FIXTURE_TEST(testString, " rgb(0.2,0.2,0.2)", Color(0.2f, 0.2f, 0.2f));
	ALAC_FIXTURE_TEST(testString, "rgb(0.2, 0.2, 0.2)", Color(0.2f, 0.2f, 0.2f));
	ALAC_FIXTURE_TEST(testString, "rgb(0.2,0.2,0.2 )", Color(0.2f, 0.2f, 0.2f));
ALAC_END_FIXTURE_TEST();
