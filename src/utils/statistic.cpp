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
 *  Maintainer: Bernhard Scheirle
 */


#include <fstream>

#include "utils/statistic.hpp"
#include "general/configuration.hpp"

#define DEBUG(...) (log4cpp::Category::getInstance("Statistic").info(__VA_ARGS__));

shared_ptr<Statistic> Statistic::instance;

Statistic::duration Statistic::JobMeasurement::getDuration(int i)
{
	return stopTime[i] - startTime[i];
}

Statistic::Statistic(const shared_ptr<Configuration>& config)
	: config(config)
{

	for (int i = 0; i < Component::Size; i++)
	{
		std::fill_n(componentAvgs[i].count, 19, 0);
		std::fill_n(componentAvgs[i].average, 19, 0);
	}
}

Statistic::~Statistic()
{
	if (config->has(opt::server::performance_log))
		writeToFile(config->get<string>(opt::server::performance_log).c_str());
}

shared_ptr<Statistic::JobMeasurement> Statistic::startNewMeasurement(const string& stylesheet, int zoom)
{
#ifdef Statistic_Activated
	shared_ptr<Statistic::JobMeasurement> jm = boost::make_shared<JobMeasurement>(stylesheet, zoom);
	jm->jobStartTime = boost::posix_time::microsec_clock::universal_time();
	return jm;
#endif
	return boost::make_shared<JobMeasurement>();
}
void Statistic::setStats(shared_ptr<Statistic::JobMeasurement>& job,
						 unsigned int nodes,
						 unsigned int ways,
						 unsigned int relations)
{
#ifdef Statistic_Activated
	job->nodes = nodes;
	job->ways = ways;
	job->relations = relations;
#endif
}
void Statistic::start(shared_ptr<Statistic::JobMeasurement>& job, Component component) const
{
#ifdef Statistic_Activated
	job->startTime[component] = boost::posix_time::microsec_clock::universal_time();
#endif
}

void Statistic::stop(shared_ptr<Statistic::JobMeasurement>& job, Component component) const
{
#ifdef Statistic_Activated
	job->stopTime[component] = boost::posix_time::microsec_clock::universal_time();
	job->stopped[component] = true;
#endif
}
void Statistic::printStatistic() const
{
#ifdef Statistic_Activated
	std::stringstream ss;
	for(int c = 0; c < Component::Size; c++) {
		ss << "\n" << componentToName((Component)c) << ": \n";
		for(int z = 0; z < 19; z++) {
			uint32_t n = componentAvgs[c].count[z];
			if (n == 0)
				continue;
			float avg = componentAvgs[c].average[z];
			ss << n << " Measurements on Zoom: " << z << ", average: ";
			if(avg >= 1000) {
				ss << avg/1000.0 << " ms\n";
			} else {
				ss << avg << " µs\n";
			}
		}
	}

	log4cpp::Category &log = log4cpp::Category::getInstance("Statistic");
	log.infoStream() << ss.str();
#endif
}

void Statistic::finished(shared_ptr<Statistic::JobMeasurement>& job)
{
#ifdef Statistic_Activated
	avgLock.lock();
	uint32_t n;
	for(int c = 0; c < Component::Size; c++) {
		if (!job->stopped[c])
			continue;

		AvgMeasurement& m = componentAvgs[c];

		n = m.count[job->zoom];
		m.average[job->zoom] *= n/(float)(n + 1.0f);
		m.average[job->zoom] += job->getDuration(c).total_microseconds() / (float)(n + 1.0f);
		m.count[job->zoom]++;
	}
	avgLock.unlock();

	if (!config->has(opt::server::performance_log))
		return;

	boost::mutex::scoped_lock scoped(bufferLock);

	measurementsBuffer.push_back(job);
	if(measurementsBuffer.size() > 100)
	{
		writeToFile(config->get<string>(opt::server::performance_log).c_str());
		measurementsBuffer.clear();
	}
#endif
}

void Statistic::writeToFile(const char* filename)
{
	std::ofstream file;
	file.open(filename, std::fstream::app);

	for (auto& job : measurementsBuffer) {
		std::stringstream ss;
		ss << "JobStart " << job->jobStartTime.time_of_day().total_microseconds() << " ";
		for(int i = 0; i < Component::Size; i++) {
			if(job->stopped[i])
				ss << componentToName((Component)i) << " " << job->getDuration(i).total_microseconds() << " ";
		}
		ss << "Stylesheet " << job->stylesheet << " Zoom " << job->zoom << " Nodes " << job->nodes << " Ways " << job->ways << " Relations " << job->relations << "\n";
		file << ss.str();
	}

	file.close();
}

string Statistic::componentToName(Component component) const
{
	switch(component) 
	{
		case Cache: 			return "Cache";
		case ComputeRect: 		return "ComputeRect";
		case GeoNodes: 			return "GeoNodes";
		case GeoWays: 			return "GeoWays";
		case GeoRelation: 		return "GeoRelation";
		case StylesheetMatch: 	return "StylesheetMatch";
		case Renderer: 			return "Renderer";
		case GeoContainsData: 	return "GeoContainsData";
		case Slicing: 			return "Slicing";
		default:
			assert(false);
	}
}
