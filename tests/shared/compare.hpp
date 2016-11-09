#pragma once
#ifndef COMPARE_HPP
#define COMPARE_HPP

#include <boost/filesystem/path.hpp>

using boost::filesystem::path;


/**
 * Returns a path like "<full-path-to-alacarte>/data"
 * Only read files from this folder.
 */
path getAlaCarteStaticDataDirectory();

/**
 * Returns a path like "<full-path-to-alacarte>/tests/data"
 * Only read files from this folder.
 */
path getTestStaticDataDirectory();

/**
 * Returns a path like "<build-dir>/tests/data"
 * Read/write files from this folder.
 */
path getTestDynamicDataDirectory();

/**
 * Returns a path like "<build-dir>/tests/data/rendered"
 * Read/write files from this folder.
 */
path getRenderedDirectory();

/**
 * Returns a path like "<full-path-to-alacarte>/tests/data/valid"
 * Only read files from this folder.
 */
path getValidDirectory();

void compareTile(const char* name);
void compareFile(const char* name);

#endif
