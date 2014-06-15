#include "../../tests.hpp"

#include "configmockup.hpp"
#include "general/configuration.hpp"

using boost::program_options::options_description;

shared_ptr<Configuration> ConfigMockup::Config(char* argv[], int argc)
{
	
	#define OPT(_name, _shortcut)	(std::string(_name).append(",").append(_shortcut)).c_str()
		using namespace boost::program_options;


		cmd_desc.add_options()
			(OPT(opt::help, "h"),																			"produce help message")
			(OPT(opt::config, "c"),	value<std::string>()->default_value("alacarte.ini")/*->value_name("path")*/,	"specifies a config file which will be loaded at program start")
			;

		// in cmd and config
		config_desc.add_options()
			(OPT(opt::logfile, "l"), value<std::string>()->default_value("log.txt")/*->value_name("path")*/,										"specifies the logfile")
			(OPT(opt::server::path_to_geodata, "g"),	value<std::string>()->required()/*->value_name("path")*/,								"path, where preprocessed data will be saved")
			(OPT(opt::server::style_source, "s"),	value<std::string>()->required()->default_value(".")/*->value_name("path")*/,									"path, to be observed for stylesheets")
			(OPT(opt::server::access_log, "a"),	value<std::string>()/*->value_name("path")*/,													"file where server access will be logged")
			(OPT(opt::server::path_to_default_style, "d"),	value<std::string>()/*->value_name("path")*/,										"default stylesheet")
			(OPT(opt::server::path_to_default_tile, "t"),	value<std::string>()/*->value_name("image")*/,										"default tile")
			(OPT(opt::server::num_threads, "n"),	value<int>()->default_value(8)/*->value_name("num")*/,									"number of threads used to process a request")
			(OPT(opt::server::parse_timeout, "o"),	value<int>()/*->value_name("ms")*/,														"maximal time in ms to parse a stylesheet")
			(OPT(opt::server::prerender_level, "z"),	value<int>()->default_value(8)/*->value_name("ms")*/,													"highest zoomlevel to enqueue for prerendering")
			(OPT(opt::server::server_port, "p"),	value<std::string>()->required()->default_value("8080")/*->value_name("port")*/,					"port to bind the server")
			(OPT(opt::server::max_queue_size, "q"),	value<int>()->default_value(1024)/*->value_name("size")*/,								"size for server queue")
			(opt::server::cache_size,	value<int>()->default_value(10)/*->value_name("size")*/,											"cache size (amount of tiles)")
			(opt::server::cache_keep_tile,				value<int>()->default_value(12)/*->value_name("size")*/,							"from 0 this zoomlevel, tiles are written to harddrive")
			(opt::server::cache_path,					value<std::string>()->default_value("cache")/*->value_name("path")*/,					"path to store evicted prerendered tiles")
			;


		cmd_desc.add(config_desc);
		pos_desc.add(opt::server::path_to_geodata, 1).add(opt::server::style_source, 1);
		
		
	auto ptr =  std::make_shared<Configuration>(cmd_desc, config_desc, pos_desc, argc, argv);
	return ptr;
}




