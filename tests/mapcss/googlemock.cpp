#include "includes.hpp"
#include "../tests.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

struct InitGMock {
	InitGMock() {
		::testing::GTEST_FLAG(throw_on_failure) = true;
		//::testing::FLAGS_gmock_verbose = "info";
		::testing::InitGoogleMock(&boost::unit_test::framework::master_test_suite().argc,
		                          boost::unit_test::framework::master_test_suite().argv);
	}
	~InitGMock() { }
};
BOOST_GLOBAL_FIXTURE(InitGMock);
