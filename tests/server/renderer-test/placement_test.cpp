
#include "../../tests.hpp"
#include "../../shared/compare.hpp"

#include <boost/algorithm/string.hpp>

#include "utils/point.hpp"
#include "general/geodata.hpp"
#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "server/style.hpp"

#include <cairo.h>

#include <iostream>
#include <fstream>
#include <ctime>

BOOST_AUTO_TEST_SUITE(test_tiles)

class LabelRenderer : public Renderer {
public:
	LabelRenderer() : Renderer(boost::make_shared<Geodata>())
	{
	}
	void renderLabels(cairo_t* cr, std::vector<std::pair<string, FloatPoint> >& toPlace) {
		cairo_save(cr);
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 0.5);
		cairo_font_face_t* font = cairo_toy_font_face_create(DEFAULT_FONT, CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
		cairo_set_font_face(cr, font);
		cairo_set_font_size(cr, 120.0);
		cairo_set_line_width(cr, 2.0);

		cairo_text_extents_t textSize;
		std::list<shared_ptr<Label> > labels;
		int i = 0;
		std::vector<shared_ptr<Style>> styles;
		for (auto& pair : toPlace)
		{
			string& text = pair.first;
			cairo_text_extents(cr, text.c_str(), &textSize);
			shared_ptr<Style> s = boost::make_shared<Style>();
			s->text = text;
			styles.push_back(s);
			FloatPoint center = pair.second + FloatPoint(textSize.width/2.0, textSize.height/2.0);
			FloatRect owner = FloatRect(center.x, center.y, center.x, center.y);
			FloatPoint origin = pair.second - FloatPoint(textSize.x_bearing, textSize.y_bearing);
			shared_ptr<Label> l = boost::make_shared<Label>(FloatRect(pair.second, textSize.width, textSize.height), owner, s->text, s.get(), origin);

			cairo_rectangle(cr, l->box.minX, l->box.minY, l->box.getWidth(), l->box.getHeight());
			cairo_stroke(cr);

			labels.push_back(l);
		}

		std::vector<shared_ptr<Label> > placed;
		placeLabels(labels, placed);

		for (auto& l: placed)
		{
			cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);
			cairo_move_to(cr, l->box.minX, l->box.maxY);
			cairo_show_text(cr, l->style->text.str().c_str());
			cairo_fill(cr);

			cairo_set_source_rgba(cr, 1.0, 0.0, 0.0, 0.5);
			cairo_rectangle(cr, l->box.minX, l->box.minY, l->box.getWidth(), l->box.getHeight());
			cairo_fill(cr);
		}

		cairo_restore(cr);
	}
};

struct placement_test
{
	LabelRenderer* renderer;
	shared_ptr<Geodata> data;

	placement_test()
	{
		renderer = new LabelRenderer();
	}

	void renderLabels(const char* path)
	{
		BOOST_TEST_MESSAGE("Render: " << path);

		cairo_surface_t* surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
			META_TILE_SIZE * TILE_SIZE, META_TILE_SIZE * TILE_SIZE);
		cairo_t* cr = cairo_create(surface);
		cairo_set_source_rgba(cr, 0.0, 0.0, 0.0, 1.0);

		cairo_save(cr);
		cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
		cairo_paint(cr);
		cairo_restore(cr);

		std::vector<std::pair<string, FloatPoint>> toPlace;
		toPlace.push_back(std::pair<string, FloatPoint>("Karlsruhe", FloatPoint(40, 200)));
		toPlace.push_back(std::pair<string, FloatPoint>("Mannheim", FloatPoint(400, 200)));
		toPlace.push_back(std::pair<string, FloatPoint>("Stuttgard", FloatPoint(200, 260)));
		toPlace.push_back(std::pair<string, FloatPoint>("München", FloatPoint(380, 660)));
		toPlace.push_back(std::pair<string, FloatPoint>("Pforzheim", FloatPoint(200, 600)));
		toPlace.push_back(std::pair<string, FloatPoint>("Wien", FloatPoint(240, 680)));
		toPlace.push_back(std::pair<string, FloatPoint>("Paris", FloatPoint(40, 880)));
		toPlace.push_back(std::pair<string, FloatPoint>("Rom", FloatPoint(-40, 880)));
		toPlace.push_back(std::pair<string, FloatPoint>("Nothing", FloatPoint(400, 760)));
		toPlace.push_back(std::pair<string, FloatPoint>("To See", FloatPoint(720, 880)));
		toPlace.push_back(std::pair<string, FloatPoint>("Here", FloatPoint(720, 560)));
		toPlace.push_back(std::pair<string, FloatPoint>("Bielefeld", FloatPoint(420, 840)));
		renderer->renderLabels(cr, toPlace);

		BOOST_TEST_MESSAGE("Writing.");
		cairo_surface_flush(surface);
		cairo_surface_write_to_png(surface, path);
	}

	void checkTile(const char* name)
	{
		string p = (getRenderedDirectory() / (string(name)+".png")).string();
		renderLabels(p.c_str());

		compareTile(name);
	}

	~placement_test()
	{
		delete renderer;
	}
};

ALAC_START_FIXTURE_TEST(placement_test)
	ALAC_FIXTURE_TEST_NAMED(checkTile, label, "label");
ALAC_END_FIXTURE_TEST()

BOOST_AUTO_TEST_SUITE_END()
