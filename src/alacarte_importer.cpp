/**
 *  This file is part of alaCarte.
 *
 *  alaCarte is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Affero General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  alaCarte is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU Affero General Public License for more details.
 *
 *  You should have received a copy of the GNU Affero General Public License
 *  along with alaCarte. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright alaCarte 2012-2013 Simon Dreher, Florian Jacob, Tobias Kahlert, Patrick Niklaus, Bernhard Scheirle, Lisa Winter
 *  Maintainer: Tobias Kahlert
 */



#include "settings.hpp"

#include "utils/application.hpp"

#include "general/configuration.hpp"
#include "general/geodata.hpp"

#include "importer/importer.hpp"




/**
 * @brief Represents the Importer-Application                                                                     
 *
 **/
class AlacarteImporterApp
	: public Application
{
public:
	/**
	 * @brief Inits the Configuration descriptions                                                                      
	 *
	 **/
	AlacarteImporterApp()
	{
#define OPT(_name, _shortcut)	(string(_name).append(",").append(_shortcut)).c_str()
		using namespace boost::program_options;


		// only in cmd
		cmd_desc.add_options()
			(OPT(opt::help, "h"),																			"produce help message")
			(OPT(opt::config, "c"),	value<string>()->default_value(DEFAULT_CONFIG_NAME),
				"specifies a config file which will be loaded at program start.  Absolute and relative paths are possible. Additionally we search in /etc/.")
			;

		// in cmd and config
		config_desc.add_options()
			(OPT(opt::logfile, "l"), value<string>()->default_value("alacarte_%N.log")/*->value_name("path")*/,										"specifies the logfile")
			(OPT(opt::importer::path_to_osmdata, "i"),	value<string>()->required()/*->value_name("path")*/,								"path to a xml file containing osm data")
			(OPT(opt::importer::path_to_geodata, "g"),	value<string>()->required()->default_value("ala.carte")/*->value_name("path")*/,	"path, where preprocessed data will be saved")
			(OPT(opt::importer::check_xml_entities, "x"),	value<bool>()->required()->default_value(true)/*->value_name("path")*/,			
				"Specifies weather the parser should not ignore unknown entities.")
			;


		cmd_desc.add(config_desc);

		pos_desc.add(opt::importer::path_to_osmdata, 1).add(opt::importer::path_to_geodata, 1);
	}

protected:
	virtual bool startupDiagnostic(const shared_ptr<Configuration>& config)
	{
		return diagnosticCheckFile(config, opt::importer::path_to_osmdata);
	}
	
	/**
	 * @brief Contains the main functionality for the importer                                                           
	 *
	 * @param config the configuration for the application
	 **/
	virtual void onRun( const shared_ptr<Configuration>& config ) 
	{
		shared_ptr<Importer> importer = boost::make_shared<Importer>(config);

		shared_ptr<Geodata> geodata;

		try {
			geodata = importer->importXML();

		} catch(excp::InputFormatException& e)
		{
			LOG_SEV(importer_log, error) << "Failed to parse \"" << excp::ErrorOut<excp::InfoFileName>(e) << "\"!";
			LOG_SEV(importer_log, error) << "Error:";
			LOG_SEV(importer_log, error) << excp::ErrorOut<excp::InfoWhat>(e);

			return;
		}

		geodata->save(config->get<string>(opt::importer::path_to_geodata));
	}

};




//! \cond internal
int main(int argc, char** argv)
{
	AlacarteImporterApp app;

	return app.start(argc, argv);
}
//! \endcond
