
#include "../../tests.hpp"
#include "../../shared/compare.hpp"

#include <boost/algorithm/string.hpp>
#include <boost/filesystem/operations.hpp>

#include "utils/transform.hpp"
#include "utils/point.hpp"
#include "general/geodata.hpp"
#include "general/node.hpp"
#include "general/way.hpp"
#include "general/relation.hpp"
#include "server/renderer/renderer.hpp"
#include "server/tile.hpp"
#include "server/style.hpp"
#include "server/tile_identifier.hpp"
#include "server/meta_identifier.hpp"
#include "server/render_attributes.hpp"
#include "server/renderer/render_canvas.hpp"

#include <iostream>
#include <fstream>
#include <ctime>

BOOST_AUTO_TEST_SUITE(feature_test)

/* Tests style attributes that can be set over the Style structure.
 *
 * Tests data from a given file is loaded and specific tiles are rendered with
 * style attributes that are contained in the styles map.
 *
 * The resulting tiles are compared with reference tiles in data/valid/
 */
struct feature_test
{
	std::shared_ptr<Renderer> renderer;
	std::shared_ptr<Geodata> data;

	//! mapping from the value of tag 'name' to the corresponding style if the object
	boost::unordered_map<CachedString, Style> styles;

	/**
	 * \param input path to the geodata file that is used as source.
	 */
	feature_test(const char* input)
	{
		path testData = getInputDirectory() / input;
		BOOST_TEST_MESSAGE("Loading data: " << testData.string());
		BOOST_CHECK(boost::filesystem::exists(testData));

		data = std::make_shared<Geodata>();
		data->load(testData.string());
		renderer = std::make_shared<Renderer>(data);
	}

	//! should be overwriten by all specific tests to generate styles for testing
	virtual void generateStyles() = 0;

	void styleWays(const std::shared_ptr<std::vector<WayId> >& ids, RenderAttributes& attr) const
	{
		for (auto id : *ids)
		{
			Way* w = data->getWay(id);
			auto& tags = w->getTags();

			auto name = tags.find(CachedString("name"));
			if (name != tags.end())
			{
				auto style = styles.find(name->second);
				if (style != styles.end()) {
					Style* s = attr.getNewStyle(id);
					*s = style->second;
				}
			}
		}
	}

	void styleNodes(const std::shared_ptr<std::vector<NodeId> >& ids, RenderAttributes& attr) const
	{
		for (auto id : *ids)
		{
			Node* n = data->getNode(id);
			auto& tags = n->getTags();

			auto name = tags.find(CachedString("name"));
			if (name != tags.end())
			{
				auto style = styles.find(name->second);
				if (style != styles.end()) {
					Style* s = attr.getNewStyle(id);
					*s = style->second;
				}
			}
		}
	}

	void styleRelations(const std::shared_ptr<std::vector<RelId> >& ids, RenderAttributes& attr) const
	{
		for (auto id : *ids)
		{
			Relation* r = data->getRelation(id);
			auto& tags = r->getTags();

			auto name = tags.find(CachedString("name"));
			if (name != tags.end())
			{
				auto style = styles.find(name->second);
				if (style != styles.end()) {
					Style* s = attr.getNewStyle(id);
					*s = style->second;
				}
			}
		}
	}

	/**
	 * \brief Renders a tile to the given path using the test styles.
	 * \param tilePath path to the resulting tile
	 * \param id identifier of the tile that should be rendered
	 */
	void renderTile(const char* tilePath, std::shared_ptr<TileIdentifier> id)
	{
		BOOST_TEST_MESSAGE("Render: " << tilePath);
		RenderAttributes attr;
		Style* canvasStyle = attr.getCanvasStyle();
		canvasStyle->fill_color = Color(1.0f, 1.0f, 1.0f, 1.0f);

		coord_t x0, x1, y0, y1;
		tileToMercator(id->getX(),     id->getY(),     id->getZoom(), x0, y0);
		tileToMercator(id->getX() + 1, id->getY() + 1, id->getZoom(), x1, y1);
		FixedRect r = FixedRect(FixedPoint(x0, y0), FixedPoint(x1, y1));

		auto nodes = data->getNodeIDs(r);
		auto ways = data->getWayIDs(r);
		auto relations = data->getRelationIDs(r);

		generateStyles();

		BOOST_TEST_MESSAGE(" - ways " << ways->size());
		styleWays(ways, attr);

		BOOST_TEST_MESSAGE(" - nodes " << nodes->size());
		styleNodes(nodes, attr);

		BOOST_TEST_MESSAGE(" - relations " << relations->size());
		styleRelations(relations, attr);

		std::shared_ptr<MetaIdentifier> mid = MetaIdentifier::Create(id);
		RenderCanvasFactory factory;
		std::shared_ptr<RenderCanvas> canvas = factory.getCanvas(id->getImageFormat());
		renderer->renderMetaTile(attr, canvas, mid);
		std::shared_ptr<Tile> tile = std::make_shared<Tile>(id);
		renderer->sliceTile(canvas, mid, tile);

		BOOST_TEST_MESSAGE("Writing the tile:");
		std::ofstream out;
		out.open(tilePath);
		BOOST_TEST_MESSAGE("- get data");
        Tile::ImageType png = tile->getImage();
		BOOST_TEST_MESSAGE("- writing (" << png->size() << " Bytes)");
		out.write((const char*) png->data(), png->size());
		out.close();
	}

	/**
	 * \brief renders and compares the givent tile with a reference rendering
	 * \param name basename of the file (e.g. 'test' for 'test.png')
	 * \param id the tile to render and compare
	 */
	void checkTile(const char* name, std::shared_ptr<TileIdentifier> id)
	{
		string p = (getRenderedDirectory() / string(name)).native() + string(".png");
		renderTile(p.c_str(), id);

		compareTile(name);
	}
};

/*
 * Tests the following style attributes:
 *  - fill_color
 *
 * for the following objects:
 *  - ways
 *  - relations
 */
struct fill_test : feature_test {
	fill_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.fill_color = Color(1.0f, 0.0f, 0.0f, 0.5f);
		wayStyle.color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		wayStyle.width = 1.0;

		Style relStyle = Style();
		relStyle.fill_color = Color(1.0f, 0.0f, 0.0f, 0.5f);
		relStyle.color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		relStyle.width = 1.0;

		styles[CachedString("way_1")] = wayStyle;
		styles[CachedString("rel_1")] = relStyle;

		relStyle.fill_color = Color(0.0f, 1.0f, 0.0f, 0.5f);
		styles[CachedString("rel_2")] = relStyle;
	}
};

/*
 * Tests the following style attributes:
 *  - color
 *  - casing_color
 *  - width
 *  - casing_width
 *
 * for the following objects:
 *  - ways
 */
struct line_test : feature_test {
	line_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		wayStyle.casing_color = Color(0.0f, 1.0f, 0.0f, 1.0f);

		// stroke and casing
		wayStyle.width = 5.0;
		wayStyle.casing_width = 3.0;
		styles[CachedString("line_1")] = wayStyle;

		// only casing
		wayStyle.width = 0.0;
		wayStyle.casing_width = 3.0;
		styles[CachedString("line_2")] = wayStyle;

		// only stroke
		wayStyle.width = 1.0;
		wayStyle.casing_width = 0.0;
		styles[CachedString("line_3")] = wayStyle;

		// no stroke, no casing
		wayStyle.width = 0.0;
		wayStyle.casing_width = 0.0;
		styles[CachedString("line_4")] = wayStyle;
	}
};

/*
 * Tests the following style attributes:
 *  - linecap
 *  - casing_linecap
 *
 * for the following objects:
 *  - ways
 */
struct linecaps_test : feature_test {
	linecaps_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.color = Color(0.8f, 0.0f, 0.0f, 1.0f);
		wayStyle.casing_color = Color(0.0f, 0.8f, 0.0f, 1.0f);
		wayStyle.width = 10.0;
		wayStyle.casing_width = 5.0;

		// round casing + none stroke
		wayStyle.linecap = Style::LineCap::CAP_NONE;
		wayStyle.casing_linecap = Style::LineCap::CAP_ROUND;
		styles[CachedString("line_1")] = wayStyle;

		// square casing + round stroke
		wayStyle.linecap = Style::LineCap::CAP_ROUND;
		wayStyle.casing_linecap = Style::LineCap::CAP_SQUARE;
		styles[CachedString("line_2")] = wayStyle;

		// none casing + square stroke
		wayStyle.linecap = Style::LineCap::CAP_SQUARE;
		wayStyle.casing_linecap = Style::LineCap::CAP_NONE;
		styles[CachedString("line_3")] = wayStyle;

		// square casing + square stroke
		wayStyle.linecap = Style::LineCap::CAP_SQUARE;
		wayStyle.casing_linecap = Style::LineCap::CAP_SQUARE;
		styles[CachedString("line_4")] = wayStyle;

		wayStyle.linecap = Style::LineCap::CAP_ROUND;
		wayStyle.casing_linecap = Style::LineCap::CAP_SQUARE;
		styles[CachedString("crossing_1")] = wayStyle;
		wayStyle.color = Color(0.0f, 0.0f, 0.8f, 1.0f);
		styles[CachedString("crossing_2")] = wayStyle;
	}
};

/*
 * Tests the following style attributes:
 *  - linejoin
 *  - casing_linejoin
 *
 * for the following objects:
 *  - ways
 */
struct linejoins_test : feature_test {
	linejoins_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.color = Color(0.8f, 0.0f, 0.0f, 1.0f);
		wayStyle.casing_color = Color(0.0f, 0.8f, 0.0f, 1.0f);
		wayStyle.width = 10.0;
		wayStyle.casing_width = 5.0;

		wayStyle.linejoin = Style::LineJoin::JOIN_MITER;
		wayStyle.casing_linejoin = Style::LineJoin::JOIN_BEVEL;
		styles[CachedString("corner_1")] = wayStyle;

		wayStyle.linejoin = Style::LineJoin::JOIN_BEVEL;
		wayStyle.casing_linejoin = Style::LineJoin::JOIN_ROUND;
		styles[CachedString("corner_2")] = wayStyle;

		wayStyle.linejoin = Style::LineJoin::JOIN_ROUND;
		wayStyle.casing_linejoin = Style::LineJoin::JOIN_MITER;
		styles[CachedString("corner_3")] = wayStyle;

	}
};

/*
 * Tests the following style attributes:
 *  - dashes
 *  - casing_dashes
 *
 * for the following objects:
 *  - ways
 */
struct dashes_test : feature_test {
	dashes_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.color = Color(0.8f, 0.0f, 0.0f, 1.0f);
		wayStyle.casing_color = Color(0.0f, 0.8f, 0.0f, 1.0f);
		wayStyle.width = 8.0;
		wayStyle.casing_width = 4.0;

		// no dashes
		styles[CachedString("line_1")] = wayStyle;

		// stroke dashes
		wayStyle.dashes = {2, 4};
		styles[CachedString("line_2")] = wayStyle;

		// casing dashes and stroke dashes
		wayStyle.casing_dashes = {2, 4};
		styles[CachedString("line_3")] = wayStyle;

		// only casing dashes
		wayStyle.dashes = std::vector<double>();
		wayStyle.casing_dashes = {2, 4};
		styles[CachedString("line_4")] = wayStyle;

		wayStyle.dashes = {1, 2, 3, 4};
		wayStyle.casing_dashes = std::vector<double>();
		styles[CachedString("crossing_1")] = wayStyle;
		wayStyle.dashes = {4};
		styles[CachedString("crossing_2")] = wayStyle;

	}
};

/*
 * Tests the following style attributes:
 *  - text
 *  - text_color
 *  - font_size
 *  - text_halo_radius
 *  - text_halo_color
 *
 * for the following objects:
 *  - ways
 *  - nodes
 */
struct text_test : feature_test {
	text_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style wayStyle = Style();
		wayStyle.color = Color(0.9f, 0.9f, 0.9f, 1.0f);
		wayStyle.casing_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		wayStyle.width = 10.0;
		wayStyle.casing_width = 1.0;

		wayStyle.text = "Keine Panik.";
		wayStyle.font_size = 8.0;
		wayStyle.text_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		wayStyle.text_halo_color = Color(1.0f, 0.0f, 0.0f, 0.3f);
		wayStyle.text_halo_radius = 1.0;
		wayStyle.text_position = Style::TextPosition::POSITION_LINE;
		styles[CachedString("line_se")] = wayStyle;
		styles[CachedString("line_ne")] = wayStyle;
		styles[CachedString("line_sw")] = wayStyle;
		styles[CachedString("line_nw")] = wayStyle;
		styles[CachedString("line_up")] = wayStyle;
		styles[CachedString("line_down")] = wayStyle;
		styles[CachedString("line_left")] = wayStyle;
		styles[CachedString("line_right")] = wayStyle;
		styles[CachedString("arc_1")] = wayStyle;
		styles[CachedString("arc_2")] = wayStyle;
		styles[CachedString("arc_3")] = wayStyle;

		wayStyle.text_position = Style::TextPosition::POSITION_CENTER;
		styles[CachedString("rect_1")] = wayStyle;

		Style nodeStyle = Style();
		Style nodeStyleDefault = Style();
		nodeStyleDefault.color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		nodeStyleDefault.width = 2.0;
		nodeStyleDefault.text_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		nodeStyleDefault.text_position = Style::TextPosition::POSITION_CENTER;
		nodeStyleDefault.text = "Keine Panik.";
		nodeStyleDefault.font_size = 12.0;

		styles[CachedString("point_00")] = nodeStyleDefault;
		// too small
		nodeStyle = nodeStyleDefault;
		nodeStyle.font_size = 0.0;
		styles[CachedString("point_01")] = nodeStyle;
		// no text
		nodeStyle = nodeStyleDefault;
		nodeStyle.text = "";
		styles[CachedString("point_02")] = nodeStyle;
		// transparent color
		nodeStyle = nodeStyleDefault;
		nodeStyle.text_color = Color(0.0f, 0.0f, 0.0f, 0.0f);
		styles[CachedString("point_10")] = nodeStyle;

		nodeStyleDefault.text_halo_color = Color(0.0f, 1.0f, 1.0f, 1.0f);
		nodeStyleDefault.text_halo_radius = 2.0;
		styles[CachedString("point_11")] = nodeStyleDefault;
		// no halo radius
		nodeStyle = nodeStyleDefault;
		nodeStyleDefault.text_halo_radius = 0.0;
		styles[CachedString("point_12")] = nodeStyleDefault;
	}
};

/*
 * Tests the following style attributes:
 *  - icon_image
 *  - icon_width
 *  - icon_height
 *
 * for the following objects:
 *  - nodes
 */
struct icon_test : feature_test {
	icon_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style nodeStyle = Style();
		Style nodeStyleDefault = Style();
		nodeStyleDefault.color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		nodeStyleDefault.width = 2.0;
		nodeStyleDefault.icon_image = (getInputDirectory() / "icons/fast-food-24.png").string();
		nodeStyleDefault.icon_width = 24.0;
		nodeStyleDefault.icon_height = 24.0;

		// height and width set
		nodeStyle = nodeStyleDefault;
		styles[CachedString("point_00")] = nodeStyle;
		// width not set
		nodeStyle.icon_width = -1.0;
		styles[CachedString("point_01")] = nodeStyle;
		// width and height not set
		nodeStyle.icon_height = -1.0;
		styles[CachedString("point_02")] = nodeStyle;
		// height not set
		nodeStyle = nodeStyleDefault;
		nodeStyle.icon_height = -1.0;
		styles[CachedString("point_10")] = nodeStyle;
		// width zero
		nodeStyle = nodeStyleDefault;
		nodeStyle.icon_width = 0.0;
		styles[CachedString("point_11")] = nodeStyle;
		// height zero
		nodeStyle = nodeStyleDefault;
		nodeStyle.icon_height = 0.0;
		styles[CachedString("point_12")] = nodeStyle;
		// different size
		nodeStyle = nodeStyleDefault;
		nodeStyle.icon_width = 32.0;
		nodeStyle.icon_height = 24.0;
		styles[CachedString("point_20")] = nodeStyle;
		// different icons
		nodeStyle = nodeStyleDefault;
		nodeStyleDefault.icon_image = (getInputDirectory() / "icons/beer-24.png").string();
		styles[CachedString("point_21")] = nodeStyle;
		nodeStyle = nodeStyleDefault;
		nodeStyleDefault.icon_image = (getInputDirectory() / "icons/airport-24.png").string();
		styles[CachedString("point_22")] = nodeStyle;
	}
};

/*
 * Tests the following style attributes:
 *  - shield_text
 *  - shield_framer_width
 *  - shield_casing_width
 *  - shield_framer_color
 *  - shield_casing_color
 *
 * for the following objects:
 *  - nodes
 */
struct shield_test : feature_test {
	shield_test(const char* data) : feature_test(data) {}

	virtual void generateStyles()
	{
		Style s = Style();
		s.color = Color(0.8f, 0.8f, 0.8f, 1.0f);
		s.casing_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		s.shield_color = Color(0.8f, 0.8f, 0.8f, 1.0f);
		s.shield_frame_color = Color(1.0f, 0.0f, 0.0f, 1.0f);
		s.shield_casing_color = Color(0.0f, 1.0f, 0.0f, 1.0f);
		s.width = 5.0;
		s.casing_width = 1.0;
		s.shield_text = CachedString("A 1337");
		s.shield_frame_width = 2.0;
		s.shield_casing_width = 1.0;
		s.font_size = 12.0;
		s.text_color = Color(0.0f, 0.0f, 0.0f, 1.0f);
		s.shield_shape = Style::ShieldShape::RECTANGULAR;

		Style ns = s;
		styles[CachedString("point_00")] = ns;
		// round shape
		ns = s;
		ns.shield_shape = Style::ShieldShape::ROUNDED;
		styles[CachedString("point_12")] = ns;
		// no text
		ns = s;
		ns.shield_text = CachedString("");
		styles[CachedString("point_20")] = ns;
		// no casing
		ns = s;
		ns.shield_casing_width = 0.0;
		styles[CachedString("point_02")] = ns;
		// no frame
		ns = s;
		ns.shield_frame_width = 0.0;
		styles[CachedString("point_10")] = ns;
		// no casing, no frame
		ns = s;
		ns.shield_frame_width = 0.0;
		ns.shield_casing_width = 0.0;
		styles[CachedString("point_22")] = ns;

		styles[CachedString("arc_1")] = s;
		styles[CachedString("arc_2")] = s;
		styles[CachedString("arc_3")] = s;
	}
};

ALAC_START_FIXTURE_TEST(fill_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, fill, "fill", std::make_shared<TileIdentifier>(4287, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(line_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, line, "line", std::make_shared<TileIdentifier>(4286, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(linecaps_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, linecaps, "linecaps", std::make_shared<TileIdentifier>(4286, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(linejoins_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, linejoins, "linejoins", std::make_shared<TileIdentifier>(4286, 2811, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(dashes_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, dashes, "dashes", std::make_shared<TileIdentifier>(4286, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(icon_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, icon_size, "icon_size", std::make_shared<TileIdentifier>(4288, 2811, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(shield_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, shield_node_1, "shield_node_1", std::make_shared<TileIdentifier>(4288, 2810, 13, "none", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST_NAMED(checkTile, shield_node_2, "shield_node_2", std::make_shared<TileIdentifier>(4288, 2809, 13, "none", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST_NAMED(checkTile, shield_way, "shield_way", std::make_shared<TileIdentifier>(4288, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

ALAC_START_FIXTURE_TEST(text_test, "renderer_test.carte")
	ALAC_FIXTURE_TEST_NAMED(checkTile, text_node, "text_node", std::make_shared<TileIdentifier>(4288, 2811, 13, "none", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST_NAMED(checkTile, text_way, "text_way", std::make_shared<TileIdentifier>(4287, 2811, 13, "none", TileIdentifier::PNG));
	ALAC_FIXTURE_TEST_NAMED(checkTile, text_find, "text_find", std::make_shared<TileIdentifier>(4288, 2812, 13, "none", TileIdentifier::PNG));
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END()
