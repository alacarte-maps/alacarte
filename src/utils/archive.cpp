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
 *  Maintainer: Patrick Niklaus
 */

#include <fstream>

#include "settings.hpp"

#include "utils/archive.hpp"

Archive::Archive(const string& archPath)
	: archPath(archPath)
{
}

void Archive::addFile(const string& filePath)
{
	paths.push_back(filePath);
}

void Archive::write()
{
	std::ofstream out(archPath, std::ofstream::binary);
	if (!out.is_open())
		BOOST_THROW_EXCEPTION(excp::FileNotWritable()  << excp::InfoFileName(archPath));

	// magic number
	out.write(MAGIC, sizeof(MAGIC));

	uint32_t num = paths.size();
	out.write((char*) &num, sizeof(num));

	// write file offsets
	uint64_t* offsets = new uint64_t[paths.size()];
	offsets[0] = (uint64_t) out.tellp() + paths.size() * sizeof(uint64_t);
	for (int i = 0; i < (paths.size() - 1); i++)
	{
		std::ifstream input(paths[i]);
		if (!input.is_open())
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(paths[i]));
		input.seekg(0, input.end);
		offsets[i+1] = offsets[i] + input.tellg();
	}
	out.write((char*) offsets, paths.size() * sizeof(uint64_t));
	delete[] offsets;

	// write files
	for (auto& path : paths)
	{
		std::ifstream input(path, std::ifstream::binary);
		if (!input.is_open())
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(path));
		out << input.rdbuf();
	}
}

void Archive::getOffsets(std::vector<uint64_t>& offsets)
{
	std::ifstream in(archPath, std::ifstream::binary);
	if (!in.is_open())
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(archPath));

	char buf[sizeof(MAGIC)];
	in.read(buf, sizeof(MAGIC));
	// check if CARTE file
	if (strncmp(buf, MAGIC, sizeof(MAGIC)) != 0)
		BOOST_THROW_EXCEPTION(excp::InputFormatException()  << excp::InfoFileName(archPath));

	uint32_t num;
	in.read((char*) &num, sizeof(num));

	for (int i = 0; i < num; i++)
	{
		uint64_t off;
		in.read((char*) &off, sizeof(off));
		offsets.push_back(off);
	}
}
