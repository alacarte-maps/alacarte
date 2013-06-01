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

#define ROUND_PAGE(_X) (((_X) % PAGE_SIZE == 0) ? (_X) : (((_X) / PAGE_SIZE + 1) * PAGE_SIZE))
#define PAGE_SIZE (4 * 1024)

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

	// number of files
	uint32_t num = paths.size();
	out.write((char*) &num, sizeof(num));

	uint64_t header_size = (uint64_t) out.tellp();
	// length of offsets
	header_size += paths.size() * sizeof(entry_t);

	// write file offsets
	entry_t* entries = new entry_t[paths.size()];
	entries[0].offset = ROUND_PAGE(header_size);
	for (int i = 0; i < paths.size(); i++)
	{
		std::ifstream input(paths[i]);
		if (!input.is_open())
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(paths[i]));
		input.seekg(0, input.end);
		uint64_t length = input.tellg();
		entries[i].length = length;

		if (i < paths.size() - 1)
			entries[i+1].offset = ROUND_PAGE(entries[i].offset + length);
	}
	out.write((char*) entries, paths.size() * sizeof(entry_t));
	delete[] entries;

	// padding
	while (out.tellp() % PAGE_SIZE != 0)
		out.write("\0", 1);

	// write files
	for (int i = 0; i < paths.size(); i++)
	{
		std::ifstream input(paths[i], std::ifstream::binary);
		if (!input.is_open())
			BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(paths[i]));
		out << input.rdbuf();

		// padding
		while (out.tellp() % PAGE_SIZE != 0)
			out.write("\0", 1);
	}
}

void Archive::getEntries(std::vector<Archive::entry_t>& entries)
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
		entry_t e;
		in.read((char*) &e, sizeof(e));
		entries.push_back(e);
	}
}
