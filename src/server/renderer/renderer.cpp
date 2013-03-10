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

#include "includes.hpp"
/*
 * =====================================================================================
 *
 *       Filename:  renderer.cpp
 *
 *    Description:  Renders a tile in the given image format using cairo.
 *
 * =====================================================================================
 */
#include "config.hpp"
#include <boost/unordered_map.hpp>
#include <sigc++/bind.h>

#include "utils/transform.hpp"

#include "general/geodata.hpp"
#include "general/geo_object.hpp"
#include "general/way.hpp"
#include "general/node.hpp"

#include "server/render_attributes.hpp"
#include "server/style.hpp"
#include "server/tile.hpp"
#include "server/tile_identifier.hpp"

#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"

#include "node_renderer.hpp"
#include "way_renderer.hpp"

#include <time.h>

#include "relation_renderer.hpp"

boost::mutex Renderer::renderLock;

class Renderer::PNGWriter : public Renderer::ImageWriter {
private:
	Tile::ImageType buffer;

	Cairo::ErrorStatus cairoWriter(const unsigned char* data,
								   unsigned int length)
	{
		buffer->insert(buffer->end(), data, data+length);
		return CAIRO_STATUS_SUCCESS;
	}

public:
	PNGWriter()
	{
	}

	//! The last given buffer is assumed to be the write buffer
	virtual Cairo::RefPtr<Cairo::Surface> createSurface(const Tile::ImageType& buffer)
	{
		this->buffer = buffer;
		return Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, TILE_SIZE, TILE_SIZE);
	}

	virtual Cairo::RefPtr<Cairo::Surface> createSurface()
	{
		return Cairo::ImageSurface::create(Cairo::FORMAT_ARGB32, TILE_SIZE, TILE_SIZE);
	}

	virtual void write(const Cairo::RefPtr<Cairo::Surface>& surface)
	{
		surface->flush();
		surface->write_to_png_stream(sigc::mem_fun(*this, &Renderer::PNGWriter::cairoWriter));
	}
};

class Renderer::SVGWriter : public Renderer::ImageWriter {
private:
	Tile::ImageType buffer;

	Cairo::ErrorStatus cairoWriter(const unsigned char* data,
								   unsigned int length)
	{
		buffer->insert(buffer->end(), data, data+length);
		return CAIRO_STATUS_SUCCESS;
	}

	Cairo::ErrorStatus doNothing(const unsigned char* data,
								   unsigned int length)
	{
		return CAIRO_STATUS_SUCCESS;
	}

public:
	SVGWriter()
	{
	}

	//! The last given buffer is assumend to be the write buffer
	virtual Cairo::RefPtr<Cairo::Surface> createSurface(const Tile::ImageType& buffer)
	{
		this->buffer = buffer;
		return Cairo::SvgSurface::create_for_stream(sigc::mem_fun(*this,
									 &Renderer::SVGWriter::cairoWriter),
									 TILE_SIZE, TILE_SIZE);
	}

	virtual Cairo::RefPtr<Cairo::Surface> createSurface()
	{
		return Cairo::SvgSurface::create_for_stream(sigc::mem_fun(*this,
									 &Renderer::SVGWriter::doNothing),
									 TILE_SIZE, TILE_SIZE);
	}

	virtual void write(const Cairo::RefPtr<Cairo::Surface>& surface)
	{
		surface->flush();
		/* do nothing because surface->flush() will call cairoWriter */
	}
};

struct Renderer::CairoLayer
{
	Cairo::RefPtr<Cairo::Context> cr;
	Cairo::RefPtr<Cairo::Surface> surface;
	CairoLayer() {}

	/**
	 * Create buffered layer so that the buffer can be converted to image format
	 * Used for the base layer LAYER_FILL.
	 */
	CairoLayer(const shared_ptr<ImageWriter>& writer, const Tile::ImageType& buffer)
	{
		surface = writer->createSurface(buffer);
		cr = Cairo::Context::create(surface);
	}

	//! utility layers don't need a buffer
	CairoLayer(const shared_ptr<ImageWriter>& writer)
	{
		surface = writer->createSurface();
		cr = Cairo::Context::create(surface);
	}

	void clear()
	{
		cr->save();

		cr->set_operator(Cairo::OPERATOR_CLEAR);
		cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
		cr->paint();

		cr->restore();
	}
};

//! Sort objects according to osm-layer and z_index (and id for determinism)
template<typename IdType>
struct CompareObjects
{
	CompareObjects(const boost::unordered_map<IdType, Style*> & styles)
		: styles(styles)
	{
	}

	bool operator ()(IdType first, IdType second) {
		const Style* fs = styles.at(first);
		const Style* ss = styles.at(second);
		return (fs->z_index < ss->z_index)
			|| (fs->z_index == ss->z_index
				&& first.getRaw() < second.getRaw());
	}

	const boost::unordered_map<IdType, Style*> &styles;
};

//! Sort labels according to their area and minX (for determinism)
template <typename LabelType>
bool CompareLabels(const shared_ptr<LabelType>& first, const shared_ptr<LabelType>& second) {
	return (first->style->font_size >  second->style->font_size)
		|| (first->style->font_size == second->style->font_size
			&& first->box.minX < second->box.minX);
}


Renderer::Renderer(const shared_ptr<Geodata>& data)
	: 	data(data), bounds(FloatRect(0.0, 0.0, TILE_SIZE, TILE_SIZE))
{
	double borderX = bounds.getWidth() * TILE_OVERLAP;
	double borderY = bounds.getHeight() * TILE_OVERLAP;
	neighbours[0] = bounds.translate( bounds.getWidth(),  bounds.getHeight());
	neighbours[1] = bounds.translate(-bounds.getWidth(),  bounds.getHeight());
	neighbours[2] = bounds.translate( bounds.getWidth(), -bounds.getHeight());
	neighbours[3] = bounds.translate(-bounds.getWidth(), -bounds.getHeight());
	neighbours[4] = bounds.translate(0.0,  bounds.getHeight());
	neighbours[5] = bounds.translate(0.0, -bounds.getHeight());
	neighbours[6] = bounds.translate( bounds.getWidth(), 0.0);
	neighbours[7] = bounds.translate(-bounds.getWidth(), 0.0);
	neighbourRequests[0] = neighbours[0].grow(borderX, borderY);
	neighbourRequests[1] = neighbours[1].grow(borderX, borderY);
	neighbourRequests[2] = neighbours[2].grow(borderX, borderY);
	neighbourRequests[3] = neighbours[3].grow(borderX, borderY);
	neighbourRequests[4] = neighbours[4].grow(borderX, borderY);
	neighbourRequests[5] = neighbours[5].grow(borderX, borderY);
	neighbourRequests[6] = neighbours[6].grow(borderX, borderY);
	neighbourRequests[7] = neighbours[7].grow(borderX, borderY);
}


Renderer::~Renderer()
{
	log4cpp::Category::getRoot().debugStream() << "Renderer destructed";
}


//! Debug function that prints identifier on the tile
void Renderer::printTileId(const Cairo::RefPtr<Cairo::Context>& cr,
						   const shared_ptr<TileIdentifier>& id)
{
	cr->save();
	cr->set_identity_matrix();
	cr->set_font_size(10);
	cr->move_to(5.0, TILE_SIZE - 10);

	std::ostringstream labelstrm;
	labelstrm << "X: " << id->getX() << " Y: " << id->getY();
	labelstrm << " Zoom: " << id->getZoom();
	labelstrm << " Style: " << id->getStylesheetPath();
	std::string label = labelstrm.str();
	cr->text_path(label.c_str());

	cr->set_source_rgba(1.0, 1.0, 1.0, 1.0);
	cr->set_line_width(2.0);
	cr->stroke_preserve();
	cr->set_source_rgba(0.0, 0.0, 0.0, 1.0);
	cr->fill();
	cr->restore();
}

shared_ptr<Renderer::ImageWriter> Renderer::getWriter(TileIdentifier::Format format) const {
	shared_ptr<ImageWriter> writer;
	switch (format) {
		case TileIdentifier::Format::PNG:
			writer = boost::dynamic_pointer_cast<ImageWriter>(boost::make_shared<PNGWriter>());
			break;
		case TileIdentifier::Format::SVG:
			writer = boost::dynamic_pointer_cast<ImageWriter>(boost::make_shared<SVGWriter>());
			break;
		default:
			// TODO add exception
			break;
	}

	return writer;
}

//! Sort objetcs into painting order
void Renderer::sortObjects(RenderAttributes& map,
						   std::vector<NodeId>& nodes,
						   std::vector<WayId>& ways,
						   std::vector<RelId>& relations) const
{
	const boost::unordered_map<NodeId, Style*> &nodeMap = map.getNodeMap();
	const boost::unordered_map<WayId, Style*> &wayMap  = map.getWayMap();
	const boost::unordered_map<RelId, Style*> &relationMap  = map.getRelationMap();

	ways.reserve(wayMap.size());
	for (auto& pair : wayMap) {
		ways.push_back(pair.first);
	}

	nodes.reserve(nodeMap.size());
	for (auto& pair : nodeMap) {
		nodes.push_back(pair.first);
	}

	relations.reserve(relationMap.size());
	for (auto& pair : relationMap) {
		relations.push_back(pair.first);
	}

	std::sort(ways.begin(),  ways.end(), CompareObjects<WayId>(wayMap));
	std::sort(nodes.begin(), nodes.end(), CompareObjects<NodeId>(nodeMap));
	std::sort(relations.begin(), relations.end(), CompareObjects<RelId>(relationMap));
}

//! Renders a OSM layer onto the given cairo surface
void Renderer::renderObjects(CairoLayer layers[],
							 RenderAttributes& map,
							 std::vector<NodeId>& nodes,
							 std::vector<WayId>& ways,
							 std::vector<RelId>& relations,
							 std::list<shared_ptr<Label>>& labels,
							 std::list<shared_ptr<Shield>>& shields)
{
	const boost::unordered_map<WayId, Style*> &wayStyles = map.getWayMap();
	const boost::unordered_map<NodeId, Style*> &nodeStyles = map.getNodeMap();
	const boost::unordered_map<RelId, Style*> &relationStyles = map.getRelationMap();

	/* find min and max z_index */
	int minZ = 0;
	int maxZ = 0;
	if (ways.size() > 0) {
		minZ = std::min(minZ, wayStyles.at(ways.front())->z_index);
		maxZ = std::max(maxZ, wayStyles.at(ways.back())->z_index);
	}
	if (nodes.size() > 0) {
		minZ = std::min(minZ, nodeStyles.at(nodes.front())->z_index);
		maxZ = std::max(maxZ, nodeStyles.at(nodes.back())->z_index);
	}
	if (relations.size() > 0) {
		minZ = std::min(minZ, relationStyles.at(relations.front())->z_index);
		maxZ = std::max(maxZ, relationStyles.at(relations.back())->z_index);
	}

	IconCache cache;
	auto rid = relations.begin();
	auto wid = ways.begin();
	auto nid = nodes.begin();
	int nextZ; // used to skip empty z-index
	int layer = minZ / 100; // z-index = osm_layer * 100 + mapcss_z_index
	for (int z = minZ; z <= maxZ; z = nextZ)
	{
		if (z/100 != layer) {
			layer = z/100;
			compositeLayers(layers);
		}

		// if nextZ is not overwritten we have reached the end
		nextZ = INT_MAX;

		for (; rid != relations.end(); rid++)
		{
			const Style* s = relationStyles.at(*rid);
			if (s->z_index > z) {
				nextZ = std::min(nextZ, s->z_index);
				break;
			}
			RelationRenderer renderer(data, *rid, s);

			renderer.fill(layers[LAYER_FILL].cr);
		}

		for (; wid != ways.end(); wid++)
		{
			const Style* s = wayStyles.at(*wid);
			if (s->z_index > z) {
				nextZ = std::min(nextZ, s->z_index);
				break;
			}
			WayRenderer renderer(data, *wid, s);

			renderer.fill(layers[LAYER_FILL].cr);
			renderer.casing(layers[LAYER_CASING].cr);
			renderer.stroke(layers[LAYER_STROKE].cr);
			renderer.label(layers[LAYER_LABELS].cr, labels);
			renderer.shield(layers[LAYER_LABELS].cr, shields);
		}

		for (; nid != nodes.end(); nid++)
		{
			const Style* s = nodeStyles.at(*nid);
			if (s->z_index > z) {
				nextZ = std::min(nextZ, s->z_index);
				break;
			}
			NodeRenderer renderer(data, *nid, s);

			renderer.casing(layers[LAYER_CASING].cr);
			renderer.stroke(layers[LAYER_STROKE].cr);
			renderer.label(layers[LAYER_LABELS].cr, labels);
			renderer.shield(layers[LAYER_LABELS].cr, shields);
			renderer.icon(layers[LAYER_ICONS].cr, cache);
		}
	}
}

//! Only renders the shield background, the text is rendered in renderLabels
void Renderer::renderShields(const Cairo::RefPtr<Cairo::Context>& cr,
							 std::vector<shared_ptr<Shield> >& shields)
{
	cr->save();
	cr->set_identity_matrix();
	cr->set_line_join(Cairo::LINE_JOIN_ROUND);

	for (auto& shield : shields)
	{
		const Style* s = shield->style;

		double x0, y0, height, width;
		double border = ceil(s->shield_frame_width/2.0 + s->shield_casing_width);
		x0 = shield->shield.minX + border;
		y0 = shield->shield.minY + border;
		width = shield->shield.getWidth() - 2*border;
		height = shield->shield.getHeight() - 2*border;
		if ((int) s->shield_frame_width % 2 == 1) {
			x0 -= 0.5;
			y0 -= 0.5;
		}
		if (s->shield_shape == Style::ShieldShape::ROUNDED) {
			cr->arc(x0 + height/2.0, y0 + height/2.0,
					height/2.0, M_PI/2.0, 3.0*M_PI/2.0);
			cr->arc(x0 + width - height/2.0, y0 + height/2.0,
					height/2.0, 3.0*M_PI/2.0, M_PI/2.0);
			cr->close_path();
		} else
			cr->rectangle(x0, y0, width, height);

		// shield casing
		if (s->shield_casing_width > 0) {
			cr->set_source_rgba(s->shield_casing_color.r,
					s->shield_casing_color.g,
					s->shield_casing_color.b,
					s->shield_casing_color.a);
			cr->set_line_width(s->shield_frame_width + s->shield_casing_width * 2.0);
			cr->stroke_preserve();
		}

		// shield background
		cr->set_source_rgba(s->shield_color.r,
				s->shield_color.g,
				s->shield_color.b,
				s->shield_color.a);
		cr->fill_preserve();

		// shield frame
		cr->set_source_rgba(s->shield_frame_color.r,
				s->shield_frame_color.g,
				s->shield_frame_color.b,
				s->shield_frame_color.a);
		cr->set_line_width(s->shield_frame_width);
		cr->stroke();
	}

	cr->restore();
}

template <typename LabelType>
void Renderer::renderLabels(const Cairo::RefPtr<Cairo::Context>& cr,
							std::vector<shared_ptr<LabelType> >& labels)
{
	cr->save();
	cr->set_identity_matrix();
	cr->set_line_join(Cairo::LINE_JOIN_ROUND);

	for (auto it = labels.rbegin(); it != labels.rend(); it++)
	{
		const shared_ptr<LabelType>& label = *it;
		const Style* s = label->style;

		cr->set_font_size(s->font_size);

		cr->move_to(label->origin.x, label->origin.y);
		cr->text_path(label->text.str());

		if (s->text_halo_radius > 0.0)
		{
			cr->set_source_rgba(s->text_halo_color.r,
								s->text_halo_color.g,
								s->text_halo_color.b,
								s->text_halo_color.a);
			cr->set_line_width(s->text_halo_radius*2.0);
			cr->stroke_preserve();
		}

		cr->set_source_rgba(s->text_color.r,
							s->text_color.g,
							s->text_color.b,
							s->text_color.a);
		cr->fill();
	}

	cr->restore();
}

//! Checks if all neighbour tile know about the owner of the label
bool Renderer::isCutOff(const FloatRect& box, const FloatRect& owner)
{
	bool tooLarge = false;
	for (int i = 0; i < 8 && !tooLarge; i++)
		tooLarge = box.intersects(neighbours[i]) && !neighbourRequests[i].intersects(owner);
	return tooLarge;
}

//! Place labels with greedy algorithm
void Renderer::placeLabels(const std::list<shared_ptr<Label> >& labels,
						   std::vector<shared_ptr<Label> >& placed)
{
	std::vector<shared_ptr<Label>> contained;
	contained.reserve(labels.size());

	// first sort out all out-of-bounds labels
	for (auto& l : labels) {
		if (bounds.contains(l->box))
			contained.push_back(l);
		else if (bounds.getIntersection(l->box).getArea() > 0.0){
			if (isCutOff(l->box, l->owner))
				continue;

			double intersect_max = 0.0;
			for (auto& other : placed)
				intersect_max = std::max(intersect_max, other->box.getIntersection(l->box).getArea());


			if (intersect_max < RENDERER_LABEL_OVERLAP * l->box.getArea())
				placed.push_back(l);
		}
	}

	for (auto& l : contained) {
		double width = l->box.getWidth();
		double height = l->box.getHeight();
		FloatPoint trans[5] = {
			FloatPoint(0.0, 0.0),         // original
			FloatPoint(0.0, -height/2.0), // above
			FloatPoint(width/2.0, 0.0),   // to the right
			FloatPoint(0.0, height/2.0),  // below
			FloatPoint(width/2.0, 0.0)    // to the left
		};
		FloatRect possible[5];
		for (int i = 0; i < 5; i++)
			possible[i] = l->box.translate(trans[i].x, trans[i].y);

		// stores the size of the intersecting area
		double intersect_max[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
		for (auto& other : placed) {
			for (int i = 0; i < 5; i++) {
				FloatRect intersection = possible[i].getIntersection(other->box);
				intersect_max[i] = std::max(intersect_max[i], intersection.getArea());
			}
		}

		int min = 0;
		for (int i = 1; i < 5; i++)
			if (intersect_max[i] < intersect_max[min]
			 && bounds.contains(possible[i])) // don't push label outside of the bounding-box
				min = i;

		// only place label if intersecting area is 1/10 the label
		if (intersect_max[min] < RENDERER_LABEL_OVERLAP * l->box.getArea()) {
			l->translate(trans[min].x, trans[min].y);
			placed.push_back(l);
		}
	}
}

//! Place labels with greedy algorithm
void Renderer::placeShields(const std::list<shared_ptr<Shield> >& shields,
						   std::vector<shared_ptr<Shield> >& placed)
{
	std::vector<shared_ptr<Shield>> contained;
	contained.reserve(10);

	// first sort out all out-of-bounds labels
	for (auto& shield : shields) {
		if (!bounds.contains(shield->shield))
			continue;

		// stores the size of the intersecting area
		double intersect_max = 0.0;
		for (auto& other : placed) {
				FloatRect intersection = shield->box.getIntersection(other->box);
				intersect_max = std::max(intersect_max, intersection.getArea());
		}

		// only place label if intersecting area is 1/10 the label
		if (intersect_max < RENDERER_SHIELD_OVERLAP * shield->box.getArea())
			placed.push_back(shield);
	}
}

//! Composited all layers into the first layer
void Renderer::compositeLayers(CairoLayer layers[]) const
{
	Cairo::RefPtr<Cairo::Context> cr = layers[0].cr;

	cr->save();
	cr->set_identity_matrix();

	for (int i = 1; i < LAYER_NUM; i++) {
		layers[i].surface->flush();
		cr->set_source(layers[i].surface, 0.0, 0.0);
		cr->paint();
		layers[i].clear();
	}

	cr->restore();
}


void Renderer::setupLayers(CairoLayer layers[], RenderAttributes& map,
						   const Cairo::Matrix& trans,
						   const shared_ptr<ImageWriter>& writer,
						   const Tile::ImageType& buffer) const
{
	const Style* s = map.getCanvasStyle();

	layers[LAYER_FILL] = CairoLayer(writer, buffer);
	layers[LAYER_FILL].cr->set_source_rgba(s->fill_color.r,
										   s->fill_color.g,
										   s->fill_color.b,
										   s->fill_color.a);
	if (s->fill_image.str().size() > 0)
	{
		Cairo::RefPtr<Cairo::ImageSurface> image = Cairo::ImageSurface::create_from_png(s->fill_image.str());
		Cairo::RefPtr<Cairo::SurfacePattern> pattern = Cairo::SurfacePattern::create(image);
		pattern->set_extend(Cairo::Extend::EXTEND_REPEAT);
		layers[LAYER_FILL].cr->set_source(pattern);
	}
	layers[LAYER_FILL].cr->paint();
	layers[LAYER_FILL].cr->transform(trans);

	for (int i = LAYER_FILL + 1; i < LAYER_NUM; i++) {
		layers[i] = CairoLayer(writer);
		layers[i].clear();

		// set coord transformation: mercator -> image
		layers[i].cr->transform(trans);
	}

	// setup default font
	Cairo::FontOptions fontOpts;
	fontOpts.set_hint_style(Cairo::HINT_STYLE_NONE);
	fontOpts.set_hint_metrics(Cairo::HINT_METRICS_OFF);
	layers[LAYER_LABELS].cr->set_font_options(fontOpts);
	Cairo::RefPtr<Cairo::ToyFontFace> font = Cairo::ToyFontFace::create(DEFAULT_FONT,
						Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
	layers[LAYER_LABELS].cr->set_font_face(font);
}

void Renderer::renderTile(RenderAttributes& map, const shared_ptr<Tile>& tile)
{
	// sort objects into acroding to z-index
	std::vector<NodeId> nodes;
	std::vector<WayId>  ways;
	std::vector<RelId>  relations;
	sortObjects(map, nodes, ways, relations);

	shared_ptr<TileIdentifier> id = tile->getIdentifier();

	Tile::ImageType buffer = boost::make_shared<Tile::ImageType::element_type>();
	// optimized for png images in the default stylesheet
	buffer->reserve(100*1024);
	shared_ptr<ImageWriter> writer = getWriter(id->getImageFormat());

	// transform Mercator to tile coordinates
	coord_t x0, y0, x1, y1;
	tileToMercator(id->getX(),   id->getY(),   id->getZoom(), x0, y0);
	tileToMercator(id->getX()+1, id->getY()+1, id->getZoom(), x1, y1);
	Cairo::Matrix trans = Cairo::scaling_matrix(TILE_SIZE / (double) (x1 - x0),
												TILE_SIZE / (double) (y1 - y0));
	trans.translate(-x0, -y0);

#if OLD_CAIRO
	renderLock.lock();
#endif

	CairoLayer layers[LAYER_NUM];
	setupLayers(layers, map, trans, writer, buffer);

	std::list<shared_ptr<Label> > labels;
	std::list<shared_ptr<Shield> > shields;
	// render objects and collect label positions
	renderObjects(layers, map, nodes, ways, relations, labels, shields);

	// sort, place and render shields
	std::vector<shared_ptr<Shield> > placedShields;
	placedShields.reserve(10);
	shields.sort(&CompareLabels<Shield>);
	placeShields(shields, placedShields);
	renderShields(layers[LAYER_LABELS].cr, placedShields);
	renderLabels<Shield>(layers[LAYER_LABELS].cr, placedShields);

	// sort, place and render labels
	std::vector<shared_ptr<Label> > placedLabels;
	placedLabels.reserve(labels.size());
	labels.sort(&CompareLabels<Label>);
	placeLabels(labels, placedLabels);
	renderLabels<Label>(layers[LAYER_LABELS].cr, placedLabels);

#if DEBUG_BUILD
	printTileId(layers[LAYER_LABELS].cr, tile->getIdentifier());
#endif

	compositeLayers(layers);

#if OLD_CAIRO
	renderLock.unlock();
#endif

	writer->write(layers[0].surface);
	tile->setImage(buffer);
}
