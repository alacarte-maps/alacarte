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

#include "includes.hpp"

#include "utils/statistic.hpp"
#include <general/configuration.hpp>

Statistic::duration Statistic::JobMeasurement::getDuration(int i)
{
	return stopTime[i] - startTime[i];
}

Statistic::Statistic()
{
#ifndef Statistic_Less_Memory
	log4cpp::Category &log = log4cpp::Category::getInstance("Statistic");
	log.infoStream() << "Statistic_Less_Memory is NOT defined, alaCarte requires a lot of Memory!";
#endif
}

shared_ptr<Statistic::JobMeasurement> Statistic::startNewMeasurement(int zoom)
{
#ifdef Statistic_Activated
	shared_ptr<Statistic::JobMeasurement> jm = boost::make_shared<JobMeasurement>(zoom);
	jm->jobStartTime = boost::posix_time::microsec_clock::universal_time();
	#ifndef Statistic_Less_Memory
		lock.lock();
		measurements.push_back(jm);
		lock.unlock();
	#endif
	return jm;
#endif
	return boost::make_shared<JobMeasurement>();
}
void Statistic::setStats(shared_ptr<Statistic::JobMeasurement>& job, unsigned int nodes, unsigned int ways, unsigned int relations)
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
#ifndef Statistic_Less_Memory
	unsigned int summe[Component::Size][19]; //Component , zoom 0 to 18
	unsigned int summanden[Component::Size][19];
	
	for(int a = 0; a < Component::Size; a++) {
		for(int b = 0; b < 19; b++) {
			summe[a][b] = 0;
			summanden[a][b] = 0;
		}
	}
	
	for(const shared_ptr<JobMeasurement> jm : measurements) {
		for(int i = 0; i < Component::Size; i++) {
			if(jm->stopped[i]) {
				summe[i][jm->zoom] += jm->getDuration(i).total_microseconds();
				summanden[i][jm->zoom]++;
			}
		}
	}
	std::stringstream ss;
	for(int c = 0; c < Component::Size; c++) {
		ss << "\n" << componentToName((Component)c) << ": \n";
		for(int z = 0; z < 19; z++) {
			if (summanden[c][z] == 0) continue;
			summe[c][z] = summe[c][z]/(float)summanden[c][z];
			ss << summanden[c][z] << " Measurements on Zoom: " << z << ", average: ";
			if(summe[c][z] >= 1000) {
				ss << summe[c][z]/1000.0 << " Milliseconds\n";
			} else {
				ss <<summe[c][z] << " Microseconds\n";
			}
		}
	}
	
	log4cpp::Category &log = log4cpp::Category::getInstance("Statistic");
	log.infoStream() << ss.str();
#endif //Statistic_Less_Memory
#endif
}

void Statistic::writeToFile(shared_ptr<Statistic::JobMeasurement>& job, const string& stylesheet, const shared_ptr<Configuration>& config) const
{
#ifdef Statistic_Activated
	if (!config->has(opt::server::performance_log)) return;
	std::stringstream ss;
	ss << "JobStart " << job->jobStartTime.time_of_day().total_microseconds() << " ";
	for(int i = 0; i < Component::Size; i++) {
		if(job->stopped[i])
			ss << componentToName((Component)i) << " " << job->getDuration(i).total_microseconds() << " ";
	}
	ss << "Stylesheet " << stylesheet << " Zoom " << job->zoom << " Nodes " << job->nodes << " Ways " << job->ways << " Relations " << job->relations << "\n";
	std::ofstream file;
	//file.open("../doc/statistics/performance.log", std::fstream::app);
	file.open(config->get<string>(opt::server::performance_log).c_str(), std::fstream::app);	
	file << ss.str();
	file.close();
#endif
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
	}
}