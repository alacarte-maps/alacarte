
#include "../tests.hpp"
#include "../shared/compare.hpp"
#include "test_config.hpp"


TestConfig::TestConfig()
{

}


std::shared_ptr<TestConfig> TestConfig::clone() const
{
	std::shared_ptr<TestConfig> config = std::make_shared<TestConfig>();

	config->options = options;

	return config;
}

std::shared_ptr<TestConfig> TestConfig::Create()
{
	//Initialize which some default values
	return std::make_shared<TestConfig>()
	->add<std::string>(opt::server::access_log, 			"unitTest_access_log.log")
	->add<std::string>(opt::server::cache_path, 			"cache")
	->add<int>(opt::server::cache_size, 			1024)
	->add<int>(opt::server::cache_keep_tile, 		12)
	//->add<int>(opt::server::request_timeout, 		XXX)
	//->add<std::string>(opt::server::log_mute_component, 	"") //doesn’t work in unitTest
	//->add<std::string>(opt::server::performance_log, 	"")
	->add<int>(opt::server::max_queue_size, 		1024)
	->add<int>(opt::server::num_threads, 			1)
	->add<int>(opt::server::parse_timeout, 			750)
	->add<std::string>(opt::server::path_to_default_style, "default")
	->add<std::string>(opt::server::path_to_default_tile,(getTestDirectory() / "../data/default.png").string())
	//->add<std::string>(opt::server::path_to_geodata, 	(getTestDirectory() / "/input/karlsruhe_big.carte").string())
	->add<int>(opt::server::prerender_level, 		12)
	->add<std::string>(opt::server::server_address, 		"localhost")
	->add<std::string>(opt::server::server_port, 		"8080")
	->add<std::string>(opt::server::log_mute_component, 	"comp1, comp2")
	->add<std::string>(opt::server::style_source, 		(getTestDirectory() / "../data/mapcss/").string());
}


const boost::any& TestConfig::getValueByKey(const std::string& key) const
{
	return testOptions.at(key);
}
