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
 *  Maintainer: Florian Jacob
 */


#include "server/stylesheet_manager.hpp"
#include "server/tile_identifier.hpp"
#include "server/meta_identifier.hpp"
#include "server/stylesheet.hpp"
#include "server/cache.hpp"
#include "server/request_manager.hpp"
#include "general/configuration.hpp"
#include "general/geodata.hpp"
#include "server/selectors/selectors.hpp"
#include "server/style_template.hpp"
#include "server/eval/eval.hpp"
#include "server/rule.hpp"
#include "server/job.hpp"
#include <math.h>
#include "server/parser/parser_logger.hpp"

#define DEBUG(...) (log4cpp::Category::getInstance("StylesheetManager").info(__VA_ARGS__));

StylesheetManager::StylesheetManager(const shared_ptr<Configuration>& config)
	: config(config)
	, monitorService(new boost::asio::dir_monitor(ioService))
	, log(log4cpp::Category::getInstance("stylesheet-manager"))
{
	stylesheetFolder = config->get<string>(opt::server::style_source);
}


StylesheetManager::~StylesheetManager()
{
	log.debugStream() << "StylesheetManager destructed";
}

void StylesheetManager::startStylesheetObserving(const shared_ptr<RequestManager>& manager)
{
	this->manager = manager;
	parsedStylesheets[".fallback"] = StylesheetManager::makeFallbackStylesheet(manager->getGeodata());

	fs::directory_iterator end_iter;

	boost::unique_lock<boost::shared_mutex> writeLock(stylesheetsLock);
	for( fs::directory_iterator dir_iter(stylesheetFolder) ; dir_iter != end_iter ; ++dir_iter) {
		if (fs::is_regular_file(dir_iter->status()) && dir_iter->path().extension() == ".mapcss" ) {
			StylesheetManager::onNewStylesheet(dir_iter->path().stem());
		}
	}

	monitorService->add_directory(config->get<string>(opt::server::style_source));
	monitorService->async_monitor(boost::bind(&StylesheetManager::onFileSystemEvent, shared_from_this(), _1, _2));

	monitorThread = boost::thread(boost::bind(&boost::asio::io_service::run, &ioService));
}


void StylesheetManager::stopStylesheetObserving()
{
	monitorService.reset();
	monitorThread.join();
}

bool StylesheetManager::hasStylesheet(const string& path)
{
	boost::shared_lock<boost::shared_mutex> readLock(stylesheetsLock);

	auto entry = parsedStylesheets.find(path);
	bool contained = (entry != parsedStylesheets.end());

	return contained;
}

shared_ptr<Stylesheet> StylesheetManager::getStylesheet(const string& path)
{
	boost::shared_lock<boost::shared_mutex> readLock(stylesheetsLock);

	auto entry = parsedStylesheets.find(path);

	shared_ptr<Stylesheet> result;
	if (entry != parsedStylesheets.end()) {
		result = entry->second;
	} else {
		result = parsedStylesheets[".fallback"];
	}

	return result;
}

// calls must be locked by write-lock
void StylesheetManager::onNewStylesheet(const fs::path& stylesheet_path)
{
	// lock the weak_ptr to manager
	shared_ptr<RequestManager> manager = this->manager.lock();
	assert(manager);

	shared_ptr<Stylesheet> stylesheet;
	int timeout = config->get<int>(opt::server::parse_timeout);

	try {
		std::string new_filename = stylesheet_path.filename().string() + ".mapcss";
		fs::path filename(new_filename);
		stylesheet = Stylesheet::Load(stylesheetFolder / filename, manager->getGeodata(), timeout);

	} catch(excp::ParseException& e)
	{
		shared_ptr<ParserLogger> logger = *boost::get_error_info<excp::InfoParserLogger>(e);

		// Something went wrong!
		logger->errorStream() << "Parsing of file \"" << excp::ErrorOut<excp::InfoFileName>(e, stylesheet_path.string()) << "\" failed:";
		logger->errorStream() << excp::ErrorOut<excp::InfoWhat>(e, "unknown reason!");
		logger->errorStream() << "In line " << excp::ErrorOut<excp::InfoFailureLine>(e) << " column " << excp::ErrorOut<excp::InfoFailureColumn>(e) << ":";

		const string* errLine = boost::get_error_info<excp::InfoFailureLineContent>(e);
		const int* errColumn = boost::get_error_info<excp::InfoFailureColumn>(e);

		if(errLine)
			logger->errorStream() << "'" << *errLine << "'";

		if(errColumn)
			logger->errorStream() << string(*errColumn, ' ') << "^-here";

		return;
	} catch(excp::TimeoutException&)
	{
		shared_ptr<ParserLogger> logger = boost::make_shared<ParserLogger>(stylesheet_path.string());

		logger->errorStream() << "Parsing of stylesheet " << stylesheet_path << " took more then " << timeout << " ms!";
		logger->errorStream() << "Parsing canceled!";
		logger->errorStream() << "You can configure the timeout via '--parse-timeout'.";
		return;
	}

	parsedStylesheets[stylesheet_path] = stylesheet;

	// prerenders the upmost tile as well as all higher zoomlevels that are specified in the configuration
	manager->enqueue(boost::make_shared<MetaIdentifier>(TileIdentifier(0, 0, 0, stylesheet_path.string(), TileIdentifier::PNG)));
}

// calls must be locked by write-lock
void StylesheetManager::onRemovedStylesheet(const fs::path& stylesheet_path)
{
	// lock the weak_ptr to manager
	shared_ptr<RequestManager> manager = this->manager.lock();
	assert(manager);

	manager->getCache()->deleteTiles(stylesheet_path.string());
	parsedStylesheets.erase(stylesheet_path);
	log.infoStream() << "Deleted Stylesheet[" << stylesheet_path << "] from Tile Cache and Stylesheet Cache.";
}

void StylesheetManager::onFileSystemEvent(const boost::system::error_code &ec, const boost::asio::dir_monitor_event &ev)
{
	typedef boost::asio::dir_monitor_event eventtype;

	if(ec != boost::system::error_code())
	{
		// We don't need to inform about the end of watching
		if(ec != boost::asio::error::operation_aborted)
		{
			log.errorStream() << "Error while watching the stylesheet folder[" << stylesheetFolder << "]:";
			log.errorStream() << ec.message();
		}

		return;
	}

	fs::path path = fs::path(ev.filename);

	// only act on .mapcss files that additionally aren't hidden files
	if (path.extension() == ".mapcss" && path.stem().string().find(".") != 0) {
		path = path.stem();

		// lock is outside of functions calls so that remove + add (== changed) can be atomic
		boost::unique_lock<boost::shared_mutex> writeLock(stylesheetsLock);
		switch (ev.type)
		{
		case eventtype::added:
			{
				log.infoStream() << "Stylesheet[" << path << "] added!";
				onNewStylesheet(path);
			}break;

		case eventtype::removed:
			{
				log.infoStream() << "Stylesheet[" << path << "] removed!";
				onRemovedStylesheet(path);
			}break;

		case eventtype::modified:
			{
				log.infoStream() << "Stylesheet[" << path << "] modified!";
				onRemovedStylesheet(path);
				onNewStylesheet(path);
			}break;
		default:
			break;
		}
	}

	// continue to monitor the folder
	monitorService->async_monitor(boost::bind(&StylesheetManager::onFileSystemEvent, shared_from_this(), _1, _2));
}

// hard coded fallback stylesheet, used in case the default stylesheet file doesn't exist
shared_ptr<Stylesheet> StylesheetManager::makeFallbackStylesheet(const shared_ptr<Geodata>& geodata) {
	shared_ptr<StyleTemplate> canvasStyle = boost::make_shared<StyleTemplate>();
	canvasStyle->fill_color = boost::make_shared<eval::Eval<Color>>(Color((uint8)0xEF, (uint8)0xEF, (uint8)0xD0));

	std::vector<shared_ptr<Rule> > rules;

	shared_ptr<Rule> highwayNodeRule = boost::make_shared<Rule>(geodata);
	shared_ptr<ApplySelector> highwayNodeApplier = boost::make_shared<ApplySelector>(highwayNodeRule);
	shared_ptr<Selector> highwayNodeTagSelector = boost::make_shared<HasTagSelector>(highwayNodeRule, highwayNodeApplier, "highway");
	highwayNodeRule->setFirstSelector(highwayNodeTagSelector);
	shared_ptr<StyleTemplate> highwayNodeStyle = boost::make_shared<StyleTemplate>();
	highwayNodeStyle->color = boost::make_shared<eval::Eval<Color>>(Color((uint8)0x00, (uint8)0x00, (uint8)0xFF));
	highwayNodeStyle->width = boost::make_shared<eval::Eval<float>>(5.5);
	highwayNodeRule->setStyleTemplate(highwayNodeStyle);
	highwayNodeRule->setZoomBounds(16, 18);
	highwayNodeRule->setAcceptableType(Rule::Accept_Way);

	rules.push_back(highwayNodeRule);


	shared_ptr<Rule> highwayRule = boost::make_shared<Rule>(geodata);
	shared_ptr<ApplySelector> highwayApplier = boost::make_shared<ApplySelector>(highwayRule);
	shared_ptr<Selector> highwayTagSelector = boost::make_shared<HasTagSelector>(highwayRule, highwayApplier, "highway");
	highwayRule->setFirstSelector(highwayTagSelector);
	shared_ptr<StyleTemplate> highwayStyle = boost::make_shared<StyleTemplate>();
	highwayStyle->color = boost::make_shared<eval::Eval<Color>>(Color((uint8)0x55, (uint8)0x55, (uint8)0x55));
	highwayStyle->width = boost::make_shared<eval::Eval<float>>(2.0);
	highwayRule->setStyleTemplate(highwayStyle);
	highwayRule->setAcceptableType(Rule::Accept_Way);

	rules.push_back(highwayRule);


	shared_ptr<Rule> highwayUpRule = boost::make_shared<Rule>(geodata);
	shared_ptr<ApplySelector> highwayUpApplier = boost::make_shared<ApplySelector>(highwayUpRule);
	shared_ptr<Selector> highwayUpTagSelector = boost::make_shared<HasTagSelector>(highwayUpRule, highwayUpApplier, "highway");
	highwayUpRule->setFirstSelector(highwayUpTagSelector);
	shared_ptr<StyleTemplate> highwayUpStyle = boost::make_shared<StyleTemplate>();
	highwayUpStyle->width = boost::make_shared<eval::Eval<float>>(1.0);
	highwayUpRule->setStyleTemplate(highwayUpStyle);
	highwayUpRule->setZoomBounds(0,15);
	highwayUpRule->setAcceptableType(Rule::Accept_Way);

	rules.push_back(highwayUpRule);


	shared_ptr<Rule> forestRule = boost::make_shared<Rule>(geodata);
	shared_ptr<ApplySelector> forestApplier = boost::make_shared<ApplySelector>(forestRule);
	shared_ptr<Selector> forestTagSelector = boost::make_shared<TagEqualsSelector>(forestRule, forestApplier, "landuse", "forest");
	forestRule->setFirstSelector(forestTagSelector);
	shared_ptr<StyleTemplate> forestStyle = boost::make_shared<StyleTemplate>();
	highwayStyle->fill_color = boost::make_shared<eval::Eval<Color>>(Color((uint8)0x00, (uint8)0xaa, (uint8)0x00));
	forestRule->setStyleTemplate(forestStyle);
	forestRule->setAcceptableType(Rule::Accept_Way);

	rules.push_back(forestRule);

	shared_ptr<Rule> adminRule = boost::make_shared<Rule>(geodata);
	shared_ptr<ApplySelector> adminApplier = boost::make_shared<ApplySelector>(adminRule);
	shared_ptr<Selector> adminWaySelector = boost::make_shared<ChildWaysSelector>(adminRule, adminApplier);
	shared_ptr<Selector> adminTagSelector = boost::make_shared<TagEqualsSelector>(adminRule, adminWaySelector, "boundary", "administrative");
	adminRule->setFirstSelector(adminTagSelector);
	shared_ptr<StyleTemplate> adminStyle = boost::make_shared<StyleTemplate>();
	highwayStyle->fill_color = boost::make_shared<eval::Eval<Color>>(Color((uint8)0xaa, (uint8)0x00, (uint8)0x00));
	highwayUpStyle->width = boost::make_shared<eval::Eval<float>>(2.0);
	adminRule->setStyleTemplate(adminStyle);
	adminRule->setAcceptableType(Rule::Accept_Relation);

	rules.push_back(adminRule);


	return boost::make_shared<Stylesheet>(geodata, rules, canvasStyle);
}

