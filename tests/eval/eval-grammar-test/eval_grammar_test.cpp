#include "includes.hpp"
#include "../../tests.hpp"

#include <boost/spirit/include/qi.hpp>

#include "../../shared/grammar_tester.hpp"

#include "server/eval/eval.hpp"
#include "server/parser/eval_grammar.hpp"
#include "general/geo_object.hpp"



struct EvalGrammarTest
{
	EvalGrammarTest()
	{
		DataMap<CachedString, CachedString> tags;
		tags[CachedString("street")] = CachedString("goldstreet");
		tags[CachedString("population")] = CachedString("5000");
		tags[CachedString("value")] = CachedString("123456");

		object = boost::make_shared<GeoObject>(tags);
	}


	

	void testString(const string& expr, const string& expected)
	{
		eval::Eval<string>::node_ptr result;
		eval::EvalGrammer grammar;

		testGrammar(grammar, result, expr, chs::space);

		string evaluated = result->eval(object.get());
		BOOST_CHECK_EQUAL(expected, evaluated);
	}

	shared_ptr<GeoObject> object;
};






ALAC_START_FIXTURE_TEST(EvalGrammarTest)
	ALAC_FIXTURE_TEST(testString, "2==2", "true");
	ALAC_FIXTURE_TEST(testString, "2!=2", "false");
	ALAC_FIXTURE_TEST(testString, "2==02", "true");
	ALAC_FIXTURE_TEST(testString, "2!=02", "false");
	ALAC_FIXTURE_TEST(testString, "02=='002'", "true");
	ALAC_FIXTURE_TEST(testString, "02!='002'", "false");
	ALAC_FIXTURE_TEST(testString, "'hallo' ne xxx", "true");
	ALAC_FIXTURE_TEST(testString, "'hallo' eq xxx", "false");
	ALAC_FIXTURE_TEST(testString, "'hallo' ne hallo", "false");
	ALAC_FIXTURE_TEST(testString, "'hallo' eq hallo", "true");

	ALAC_FIXTURE_TEST(testString, "'hallo' . xxx", "halloxxx");
	ALAC_FIXTURE_TEST(testString, "'5' + 4", "9");
	ALAC_FIXTURE_TEST(testString, "7 . 8 - 8", "70");
	ALAC_FIXTURE_TEST(testString, "7 * 2", "14");
	ALAC_FIXTURE_TEST(testString, "8 / 2", "4");
	ALAC_FIXTURE_TEST(testString, "8 / 0", "");

	ALAC_FIXTURE_TEST(testString, "'9.5' < 10", "true");
	ALAC_FIXTURE_TEST(testString, "'9.5' < '9.25'", "false");
	ALAC_FIXTURE_TEST(testString, "'9.5' < '9.55'", "true");

	ALAC_FIXTURE_TEST(testString, "4 <= 4", "true");
	ALAC_FIXTURE_TEST(testString, "'4.5' <= '4.5'", "true");
	ALAC_FIXTURE_TEST(testString, "4 <= '4.5'", "true");
	ALAC_FIXTURE_TEST(testString, "5 <= '4.5'", "false");
	ALAC_FIXTURE_TEST(testString, "'6.1' <= '5.5'", "false");


	ALAC_FIXTURE_TEST(testString, "'9.5' > 10", "false");
	ALAC_FIXTURE_TEST(testString, "'9.5' > '9.25'", "true");
	ALAC_FIXTURE_TEST(testString, "'9.5' > '9.55'", "false");

	ALAC_FIXTURE_TEST(testString, "4 >= 4", "true");
	ALAC_FIXTURE_TEST(testString, "'4.5' >= '4.5'", "true");
	ALAC_FIXTURE_TEST(testString, "4 >= '4.5'", "false");
	ALAC_FIXTURE_TEST(testString, "5 >= '4.5'", "true");
	ALAC_FIXTURE_TEST(testString, "'6.1' >= '5.5'", "true");

	ALAC_FIXTURE_TEST(testString, "(2+2)*3", "12");
	ALAC_FIXTURE_TEST(testString, "sqrt(16) >= 4", "true");
	ALAC_FIXTURE_TEST(testString, "sqrt(16) <= 4", "true");
	ALAC_FIXTURE_TEST(testString, "not(4-4)", "true");
	ALAC_FIXTURE_TEST(testString, "str('xxx bla')", "xxx bla");
	ALAC_FIXTURE_TEST(testString, "int('8.4')", "8");
	ALAC_FIXTURE_TEST(testString, "num(test)", "");
	ALAC_FIXTURE_TEST(testString, "num('-33.2')", "-33.2");

	ALAC_FIXTURE_TEST(testString, "cond(hallo, xxx, 123)", "xxx");
	ALAC_FIXTURE_TEST(testString, "cond(d.d eq hallo, 3*3, sqrt(8+8))", "4");
	ALAC_FIXTURE_TEST(testString, "cond(boolean(x.x), 2, 3)", "2");

	ALAC_FIXTURE_TEST(testString, "tag(street)", "goldstreet");
	ALAC_FIXTURE_TEST(testString, "tag(population) == 05000", "true");
	ALAC_FIXTURE_TEST(testString, "tag(value) - 23456", "100000");

	ALAC_FIXTURE_TEST(testString, "tag(population) / 1000 * 5", "25");
ALAC_END_FIXTURE_TEST();
