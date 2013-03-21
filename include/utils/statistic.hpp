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

#pragma once
#ifndef STATISTIC_HPP
#define STATISTIC_HPP

#include <settings.hpp>

class Configuration;
//For Global „Statistic“ activation
//#define Statistic_Aktivated
//For Memory save usage
//#define Statistic_Less_Memory

class Statistic
{
public:
	typedef boost::posix_time::time_duration duration;
	typedef boost::posix_time::ptime ptime;

	enum Component{
		//Change the order of the following Components will change the output order (file and log), you have to update /alacarte/doc/statistics/stat2html.py
		Cache = 0, ComputeRect, GeoNodes, GeoWays, GeoRelation, StylesheetMatch, Renderer, GeoContainsData, 
		Size //amount of Components
	};
	
	class JobMeasurement {
		friend class Statistic;
	public:
		JobMeasurement(){};
		JobMeasurement(int zoom) : zoom(zoom), nodes(0), ways(0), relations(0) {
			for(int i = 0; i < Component::Size; i++) {
				stopped[i] = false;
			}
		};
		duration getDuration(int i);
	private:
		int zoom;
		unsigned int nodes;
		unsigned int ways;
		unsigned int relations;
		bool stopped[Component::Size];
		ptime jobStartTime;
		ptime startTime[Component::Size];
		ptime stopTime[Component::Size];
	};
	
	shared_ptr<JobMeasurement> startNewMeasurement(int zoom);
	void start(shared_ptr<Statistic::JobMeasurement>& job, Component component) const;
	void stop(shared_ptr<Statistic::JobMeasurement>& job, Component component) const;
	void writeToFile(shared_ptr<Statistic::JobMeasurement>& job, const string& stylesheet, const shared_ptr<Configuration>& config) const;
	void setStats(shared_ptr<Statistic::JobMeasurement>& job, unsigned int nodes, unsigned int ways, unsigned int relations);
	void printStatistic() const;
	
	static Statistic& instance()
	{
		static Statistic _instance;
		return _instance;
	}

private:
	string componentToName(Component component) const;
	Statistic();
	Statistic( const Statistic& ){};

private:
#ifndef Statistic_Less_Memory
	boost::mutex lock;
	std::vector<shared_ptr<JobMeasurement>> measurements;
#endif
};

#endif
