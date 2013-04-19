
#include "../../tests.hpp"
#include "../../shared/test_config.hpp"





TestConfig::Ptr DefaultConfig = TestConfig::Create()
									->add<string>("name", "SirTobi")
									->add<int>("age", 20);



ALAC_PARAM_TEST_CASE(needTestConfig, TestConfig::Ptr config)
{
	// do nothing
	BOOST_CHECK_EQUAL(config->get<string>("additional"), "additional config");
}

ALAC_PARAM_TEST(needTestConfig,
				DefaultConfig->clone()
					->add<string>("additional", "additional config")
				)