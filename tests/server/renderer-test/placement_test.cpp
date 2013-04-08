#include "includes.hpp"
#include "../../tests.hpp"
#include "../../shared/compare.hpp"

#include <boost/algorithm/string.hpp>

#include "utils/point.hpp"
#include "general/geodata.hpp"
#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"
#include "server/style.hpp"

#include <cairomm/surface.h>
#include <cairomm/context.h>

#include <iostream>
#include <fstream>
#include <ctime>

BOOST_AUTO_TEST_SUITE(test_tiles)

class LabelRenderer : public Renderer {
public:
	LabelRenderer() : Renderer(boost::make_shared<Geodata>())
	{
	}
	void renderLabels(Cairo::RefPtr<Cairo::Context> cr, std::vector<std::pair<string, FloatPoint> >& toPlace) {
		cr->save();
		cr->set_source_rgba(0.0, 0.0, 0.0, 0.5);
		Cairo::RefPtr<Cairo::ToyFontFace> font = Cairo::ToyFontFace::create(DEFAULT_FONT, Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
		cr->set_font_face(font);
		cr->set_font_size(30.0);
		cr->set_line_width(2.0);

		Cairo::TextExtents textSize;
		std::list<shared_ptr<Label> > labels;
		int i = 0;
		std::vector<shared_ptr<Style>> styles;
		for (auto& pair : toPlace)
		{
			string& text = pair.first;
			cr->get_text_extents(text, textSize);
			shared_ptr<Style> s = boost::make_shared<Style>();
			s->text = text;
			styles.push_back(s);
			FloatPoint center = pair.second + FloatPoint(textSize.width/2.0, textSize.height/2.0);
			FloatRect owner = FloatRect(center.x, center.y, center.x, center.y);
			FloatPoint origin = pair.second - FloatPoint(textSize.x_bearing, textSize.y_bearing);
			shared_ptr<Label> l = boost::make_shared<Label>(FloatRect(pair.second, textSize.width, textSize.height), owner, s->text, s.get(), origin);

			cr->rectangle(l->box.minX, l->box.minY, l->box.getWidth(), l->box.getHeight());
			cr->stroke();

			labels.push_back(l);
		}

		std::vector<shared_ptr<Label> > placed;
		placeLabels(labels, placed);

		for (auto& l: placed)
		{
			cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
			cr->move_to(l->box.minX, l->box.maxY);
			cr->show_text(l->style->text.str());
			cr->fill();

			cr->set_source_rgba(1.0, 0.0, 0.0, 0.5);
			cr->rectangle(l->box.minX, l->box.minY, l->box.getWidth(), l->box.getHeight());
			cr->fill();
		}

		cr->restore();
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

		Cairo::RefPtr<Cairo::Surface> surface = Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32,
			META_TILE_SIZE * TILE_SIZE, META_TILE_SIZE * TILE_SIZE);
		Cairo::RefPtr<Cairo::Context> cr = Cairo::Context::create(surface);
		cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);

		cr->save();
		cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
		cr->paint();
		cr->restore();

		std::vector<std::pair<string, FloatPoint>> toPlace;
		toPlace.push_back(std::pair<string, FloatPoint>("Karlsruhe", FloatPoint(10, 50)));
		toPlace.push_back(std::pair<string, FloatPoint>("Mannheim", FloatPoint(100, 50)));
		toPlace.push_back(std::pair<string, FloatPoint>("Stuttgard", FloatPoint(50, 65)));
		toPlace.push_back(std::pair<string, FloatPoint>("München", FloatPoint(95, 165)));
		toPlace.push_back(std::pair<string, FloatPoint>("Pforzheim", FloatPoint(50, 150)));
		toPlace.push_back(std::pair<string, FloatPoint>("Wien", FloatPoint(60, 170)));
		toPlace.push_back(std::pair<string, FloatPoint>("Paris", FloatPoint(10, 220)));
		toPlace.push_back(std::pair<string, FloatPoint>("Rom", FloatPoint(-10, 220)));
		toPlace.push_back(std::pair<string, FloatPoint>("Nothing", FloatPoint(100, 190)));
		toPlace.push_back(std::pair<string, FloatPoint>("To See", FloatPoint(180, 220)));
		toPlace.push_back(std::pair<string, FloatPoint>("Here", FloatPoint(180, 240)));
		toPlace.push_back(std::pair<string, FloatPoint>("Bielefeld", FloatPoint(105, 210)));
		renderer->renderLabels(cr, toPlace);

		BOOST_TEST_MESSAGE("Writing.");
		surface->flush();
		surface->write_to_png(path);
	}

	void checkTile(const char* name)
	{
		string p = (getRenderedDirectory() / string(name)).native() + string(".png");
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
