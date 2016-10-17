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
#include <boost/thread/mutex.hpp>

class Configuration;

class Statistic
{
public:
	typedef boost::posix_time::time_duration duration;
	typedef boost::posix_time::ptime ptime;

	enum Component{
		//Change the order of the following Components will change the output order (file and log),
		//you have to update /alacarte/doc/statistics/stat2html.py
		Cache = 0,
		ComputeRect,
		GeoNodes,
		GeoWays,
		GeoRelation,
		StylesheetMatch,
		Renderer,
		GeoContainsData,
		Slicing,
		Size //amount of Components
	};


	class JobMeasurement {
		friend class Statistic;
	public:
		JobMeasurement() = default;
		JobMeasurement(const string& stylesheet, int zoom)
			: stylesheet(stylesheet), zoom(zoom), nodes(0), ways(0), relations(0)
		{
			for(int i = 0; i < Component::Size; i++) {
				stopped[i] = false;
			}
		};
		duration getDuration(int i);
	private:
		uint16_t zoom;
		uint32_t nodes;
		uint32_t ways;
		uint32_t relations;
		string stylesheet;
		bool stopped[Component::Size];
		ptime jobStartTime;
		ptime startTime[Component::Size];
		ptime stopTime[Component::Size];
	};

	shared_ptr<JobMeasurement> startNewMeasurement(const string& stylesheet, int zoom);
	void start(shared_ptr<Statistic::JobMeasurement>& job, Component component) const;
	void stop(shared_ptr<Statistic::JobMeasurement>& job, Component component) const;
	void finished(shared_ptr<Statistic::JobMeasurement>& job);
	void setStats(shared_ptr<Statistic::JobMeasurement>& job, unsigned int nodes, unsigned int ways, unsigned int relations);
	void printStatistic() const;

	static const shared_ptr<Statistic>& Get()
	{
		assert(instance);
		return instance;
	}

	static void Init(const shared_ptr<Configuration>& conf)
	{
		instance = shared_ptr<Statistic>(new Statistic(conf));
	}


	~Statistic();
private:
	void writeToFile(const char* filename);
	string componentToName(Component component) const;

	Statistic(const shared_ptr<Configuration>& config);
	Statistic(const Statistic&){};

	static shared_ptr<Statistic> instance;

private:
	shared_ptr<Configuration> config;
	boost::mutex bufferLock;
	std::vector<shared_ptr<JobMeasurement>> measurementsBuffer;

	struct AvgMeasurement
	{
		// for every zoomlevel an average value
		uint32_t count[19];
		float average[19];
	};
	boost::mutex avgLock;
	AvgMeasurement componentAvgs[Component::Size];
};

#endif
