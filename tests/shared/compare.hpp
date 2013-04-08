#pragma once
#ifndef COMPARE_HPP
#define COMPARE_HPP

#include <boost/algorithm/string.hpp>

using boost::filesystem::path;

path getTestDirectory();
path getDataDirectory();
path getInputDirectory();
path getOutputDirectory();
path getRenderedDirectory();
path getDiffDirectory();
path getValidDirectory();

void compareTile(const char* name);
void compareFile(const char* name);

#endif
