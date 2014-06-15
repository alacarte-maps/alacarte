
#include "../../tests.hpp"

#include <boost/assign/list_of.hpp>

#include "../../shared/grammar_tester.hpp"

#include "general/geo_object.hpp"
#include "server/eval/eval.hpp"
#include "server/parser/color_grammar.hpp"
#include "server/parser/parser_logger.hpp"

enum ResultType
{
	Failed,
	IsEval,
	IsTargetType
};

template<typename EvalType>
struct EvalConstructionTest
{
	EvalConstructionTest()
	{
	}

	void testString(const std::string& expr, ResultType res)
	{
		try {
			eval::Eval<EvalType> eval(expr, std::make_shared<ParserLogger>(""), ParseInfo());

			if(eval.isEval())
				BOOST_CHECK_EQUAL(res, IsEval);
			else
				BOOST_CHECK_EQUAL(res, IsTargetType);
		} catch(excp::ParseException& e)
		{
			if(res != Failed)
			{
				BOOST_MESSAGE(boost::diagnostic_information(e));
			}
			BOOST_CHECK_EQUAL(res, Failed);
		}
	}

};


typedef EvalConstructionTest<Color> ColorEvalConstructionTest;
typedef EvalConstructionTest<string> StringEvalConstructionTest;


ALAC_START_FIXTURE_TEST(ColorEvalConstructionTest)

ALAC_FIXTURE_TEST(testString, "#000000", IsTargetType);
ALAC_FIXTURE_TEST(testString, "#00000", Failed);
ALAC_FIXTURE_TEST(testString, "eval(#00000)", IsEval);
ALAC_FIXTURE_TEST(testString, "eval(\"lh saldh#sd ##dd#s 0=12\")", Failed);

ALAC_END_FIXTURE_TEST();


ALAC_START_FIXTURE_TEST(StringEvalConstructionTest)

	ALAC_FIXTURE_TEST(testString, "\"--- hm ---\"", IsTargetType);
	ALAC_FIXTURE_TEST(testString, "#00000", IsTargetType);
	ALAC_FIXTURE_TEST(testString, "eval(#00000)", IsEval);
	ALAC_FIXTURE_TEST(testString, "eval(\"lh saldh#sd ##dd#s 0=12\")", IsTargetType);

ALAC_END_FIXTURE_TEST();




using boost::assign::list_of;

template<typename EvalType>
struct EvalListConstructionTest
{
	EvalListConstructionTest()
	{
	}

	void testString(const std::string& expr, const std::vector<ResultType>& resList)
	{
		try {
			eval::Eval<std::vector<EvalType>> eval(expr, std::make_shared<ParserLogger>(""), ParseInfo());

			BOOST_REQUIRE_EQUAL(resList.size(), eval.size());

			for(unsigned int i = 0; i < resList.size(); ++i)
			{
				if(eval.isEval(i))
					BOOST_CHECK_EQUAL(resList[i], IsEval);
				else
					BOOST_CHECK_EQUAL(resList[i], IsTargetType);
			}
		} catch(excp::ParseException& e)
		{
			BOOST_MESSAGE(boost::diagnostic_information(e));
		}
	}

	void testString(const std::string& expr)
	{
		try {
			eval::Eval<std::vector<EvalType>> eval(expr, std::make_shared<ParserLogger>(""), ParseInfo());

			BOOST_REQUIRE(false && "Parsing should fail!");
		} catch(excp::ParseException&)
		{
		}
	}
};


typedef EvalListConstructionTest<double> DoubleListEvalConstructionTest;

ALAC_START_FIXTURE_TEST(DoubleListEvalConstructionTest)

ALAC_FIXTURE_TEST(testString, "2.4, 2.5", list_of<ResultType>(IsTargetType)(IsTargetType));
ALAC_FIXTURE_TEST(testString, "2.7", list_of<ResultType>(IsTargetType));
ALAC_FIXTURE_TEST(testString, "eval(#00000)", list_of<ResultType>(IsEval));
ALAC_FIXTURE_TEST(testString, "eval(#00000) , 3", list_of<ResultType>(IsEval)(IsTargetType));
ALAC_FIXTURE_TEST(testString, "eval(\"lh saldh#sd ##dd#s 0=12\")");
ALAC_FIXTURE_TEST(testString, "eval(2) , eval(3),eval(0)", list_of<ResultType>(IsEval)(IsEval)(IsEval));
ALAC_FIXTURE_TEST(testString, "eval(2) eval(0)");

ALAC_END_FIXTURE_TEST();


typedef EvalListConstructionTest<string> StringListEvalConstructionTest;

ALAC_START_FIXTURE_TEST(DoubleListEvalConstructionTest)

ALAC_FIXTURE_TEST(testString, "2.4, 2.5", list_of<ResultType>(IsTargetType)(IsTargetType));
ALAC_FIXTURE_TEST(testString, "2.7", list_of<ResultType>(IsTargetType));
ALAC_FIXTURE_TEST(testString, "eval(#00000), hallo", list_of<ResultType>(IsEval)(IsTargetType));
ALAC_FIXTURE_TEST(testString, "\"hallo\" , \"jjj\"", list_of<ResultType>(IsTargetType)(IsTargetType));
ALAC_FIXTURE_TEST(testString, "eval(\"lh saldh#sd ##dd#s 0=12\")", list_of<ResultType>(IsTargetType));
ALAC_FIXTURE_TEST(testString, "eval(hallo) , eval(xxx),eval(yyy)", list_of<ResultType>(IsEval)(IsEval)(IsEval));
ALAC_FIXTURE_TEST(testString, "eval(2) eval(0)", list_of<ResultType>(IsTargetType));

ALAC_END_FIXTURE_TEST();
