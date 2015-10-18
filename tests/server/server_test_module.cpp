// Simply define the module
#define BOOST_TEST_MODULE server test

#include <boost/test/unit_test.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

namespace logging = boost::log;

struct TestSetup
{
public:
	TestSetup() {
		logging::core::get()->set_filter(
			logging::trivial::severity >= logging::trivial::debug
			);

		BOOST_TEST_MESSAGE("Start logging.");
	}
};

BOOST_GLOBAL_FIXTURE( TestSetup );
