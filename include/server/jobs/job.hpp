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
 **/

#pragma once
#ifndef JOB_HPP
#define JOB_HPP

#include "settings.hpp"

class TileIdentifier;
class Tile;
class RequestManager;
class Configuration;
class Stylesheet;

/**
 * @brief Abstract Class which computes Tiles by a given TileIdentifier.
 **/
class Job
{
public:
	Job(const shared_ptr<Configuration>& config, const shared_ptr<RequestManager>& manager);
	virtual ~Job();

	/**
	 * @brief Processes the Job.
	 **/
	virtual void process() = 0;
	
protected:
	TESTABLE shared_ptr<Tile> computeTile(const shared_ptr<TileIdentifier>& ti);
	TESTABLE shared_ptr<Tile> computeTileNoneData(const shared_ptr<TileIdentifier>& ti);
	TESTABLE shared_ptr<Stylesheet> getStylesheet(const shared_ptr<TileIdentifier>& orginalTI, shared_ptr<TileIdentifier>& newti);
	TESTABLE FixedRect computeRect(const shared_ptr<TileIdentifier>& ti);

protected:
	//! RequestManager which holds all important components.
	shared_ptr<RequestManager> manager;
	shared_ptr<Configuration> config;
};

#endif
