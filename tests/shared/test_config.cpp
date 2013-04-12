#include "includes.hpp"
#include "../tests.hpp"
#include "../shared/compare.hpp"
#include "test_config.hpp"


TestConfig::TestConfig()
{

}


shared_ptr<TestConfig> TestConfig::clone() const
{
	shared_ptr<TestConfig> config = boost::make_shared<TestConfig>();

	config->options = options;

	return config;
}

shared_ptr<TestConfig> TestConfig::Create()
{
	//Initialize which some default values
	return boost::make_shared<TestConfig>()
	->add<string>(opt::server::access_log, 			"unitTest_access_log.log")
	->add<string>(opt::server::cache_path, 			"cache")
	->add<int>(opt::server::cache_size, 			1024)
	->add<int>(opt::server::cache_keep_tile, 		12)
	//->add<int>(opt::server::request_timeout, 		XXX)
	//->add<string>(opt::server::log_mute_component, 	"") //doesn’t work in unitTest
	//->add<string>(opt::server::performance_log, 	"")
	->add<int>(opt::server::max_queue_size, 		1024)
	->add<int>(opt::server::num_threads, 			1)
	->add<int>(opt::server::parse_timeout, 			750)
	->add<string>(opt::server::path_to_default_style, "default")
	->add<string>(opt::server::path_to_default_tile,(getTestDirectory() / "../data/default.png").string())
	//->add<string>(opt::server::path_to_geodata, 	(getTestDirectory() / "/input/karlsruhe_big.carte").string())
	->add<int>(opt::server::prerender_level, 		12)
	->add<string>(opt::server::server_address, 		"localhost")
	->add<string>(opt::server::server_port, 		"8080")
	->add<string>(opt::server::log_mute_component, 	"comp1, comp2")
	->add<string>(opt::server::style_source, 		(getTestDirectory() / "../data/mapcss/").string());
}


const boost::any& TestConfig::getValueByKey(const string& key) const
{
	return testOptions.at(key);
}
