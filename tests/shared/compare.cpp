
#include "../tests.hpp"
#include "../config.hpp"

#include "compare.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include <cairo.h>

#include <fstream>
#include <iostream>

path getTestDirectory()
{
	path dir = path(TEST_DIRECTORY);
	if(!boost::filesystem::exists(dir))
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(dir.string()));
	return dir;
}

path getDataDirectory()
{
	path dir = getTestDirectory() / "data";
	if(!boost::filesystem::exists(dir))
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(dir.string()));
	return dir;
}

path getOutputDirectory()
{
	path dir = getDataDirectory() / "output";
	if(!boost::filesystem::exists(dir))
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(dir.string()));
	return dir;
}

path getInputDirectory()
{
	path dir = getDataDirectory() / "input";
	if(!boost::filesystem::exists(dir))
		BOOST_THROW_EXCEPTION(excp::FileNotFoundException()  << excp::InfoFileName(dir.string()));
	return dir;
}

path getRenderedDirectory()
{
	path dir = getOutputDirectory() / "rendered";
	if(!boost::filesystem::exists(dir))
		boost::filesystem::create_directory(dir);
	return dir;
}

path getDiffDirectory()
{
	path dir = getOutputDirectory() / "diff";
	if(!boost::filesystem::exists(dir))
		boost::filesystem::create_directory(dir);
	return dir;
}

path getValidDirectory()
{
	path dir = getInputDirectory() / "valid";
	BOOST_CHECK(boost::filesystem::exists(dir));
	return dir;
}

/**
 * @brief Compare a reference png to rendered png.
 * @param name the name of the test tile. e.g. for the file to be named "home.png", give name "home"
 */
void compareTile(const char* name)
{
	std::string file = std::string(name) + ".png";
	path rendered = getRenderedDirectory() / file;
	path valid = getValidDirectory() / file;
	path diff = getDiffDirectory() / file;

	BOOST_TEST_MESSAGE("Loading images:");
	BOOST_TEST_MESSAGE(" - valid: " << valid);
	cairo_surface_t* valid_surface = cairo_image_surface_create_from_png(valid.string().c_str());
	BOOST_TEST_MESSAGE(" - rendered: " << rendered);
	cairo_surface_t* rendered_surface = cairo_image_surface_create_from_png(rendered.string().c_str());
	cairo_t* cr = cairo_create(valid_surface);

	int v_h = cairo_image_surface_get_height(valid_surface);
	int v_w = cairo_image_surface_get_width(valid_surface);
	int r_h = cairo_image_surface_get_height(rendered_surface);
	int r_w = cairo_image_surface_get_width(rendered_surface);
	BOOST_CHECK(v_h == r_h && v_w == r_w);

	BOOST_TEST_MESSAGE("Compute:");
	BOOST_TEST_MESSAGE(" - difference: " << diff);
	// compute inverse difference between images
	unsigned char* valid_data    = cairo_image_surface_get_data(valid_surface);
	unsigned char* rendered_data = cairo_image_surface_get_data(rendered_surface);
	bool changed = false;
	for (int i = 0; i < v_h * v_w; i++) {
		if (!changed)
			changed = *((uint32_t*) valid_data) != *((uint32_t*) rendered_data);
		valid_data[0] = 0xFF - abs(valid_data[0]-rendered_data[0]);
		valid_data[1] = 0xFF - abs(valid_data[1]-rendered_data[1]);
		valid_data[2] = 0xFF - abs(valid_data[2]-rendered_data[2]);
		valid_data[3] = 0xFF - abs(valid_data[3]-rendered_data[3]);
		valid_data += 4;
		rendered_data += 4;
	}

	cairo_surface_flush(valid_surface);
	cairo_surface_write_to_png(valid_surface, diff.string().c_str());

	BOOST_CHECK(!changed);
}

/**
 * @brief Compare a reference file to rendered file.
 * @param name the name of the test tile. e.g. for the file to be named "home.svg", give name "home.svg"
 */
void compareFile(const char* name)
{
	std::string file = std::string(name);
	path rendered = getRenderedDirectory() / file;
	path valid = getValidDirectory() / file;
	using namespace std;

	BOOST_TEST_MESSAGE("Loading images:");
	BOOST_TEST_MESSAGE(" - valid: " << valid);
	ifstream valid_file (valid.native(), ios::in | ios::binary | ios::ate);
	BOOST_TEST_MESSAGE(" - rendered: " << rendered);
	ifstream rendered_file (rendered.native(), ios::in | ios::binary | ios::ate);

	BOOST_CHECK(valid_file.is_open());
	BOOST_CHECK(rendered_file.is_open());

	ifstream::pos_type valid_size = valid_file.tellg();
	ifstream::pos_type rendered_size = rendered_file.tellg();
	BOOST_CHECK(valid_size == rendered_size);

	BOOST_TEST_MESSAGE("Compute:");
	unsigned char* valid_data = new unsigned char[valid_size];
	valid_file.seekg(0, ios::beg);
	valid_file.read((char*) valid_data, valid_size);
	valid_file.close();

	unsigned char* rendered_data = new unsigned char[rendered_size];
	rendered_file.seekg(0, ios::beg);
	rendered_file.read((char*) rendered_data, rendered_size);
	rendered_file.close();

	bool changed = false;
	for (unsigned i = 0; i < valid_size; i++)
		if (!changed)
			changed = (valid_data[i] != rendered_data[i]);

	BOOST_CHECK(!changed);
}
