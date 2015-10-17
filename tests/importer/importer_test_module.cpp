
// Simply define the module
#define BOOST_TEST_MODULE importer test
#include <boost/test/unit_test.hpp>

#include <log4cpp/Appender.hh>
#include <log4cpp/Category.hh>
#include <log4cpp/OstreamAppender.hh>

struct TestSetup
{
public:
	TestSetup() {
		log4cpp::Appender *appender1 = new log4cpp::OstreamAppender("console", &std::cout);
		appender1->setLayout(new log4cpp::BasicLayout());

		BOOST_TEST_MESSAGE("Start logging.");
		log4cpp::Category& root = log4cpp::Category::getRoot();
		root.setPriority(log4cpp::Priority::DEBUG);
		root.addAppender(appender1);
	}
};

BOOST_GLOBAL_FIXTURE( TestSetup );
