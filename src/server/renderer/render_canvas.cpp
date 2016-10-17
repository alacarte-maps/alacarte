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


/*
 * =====================================================================================
 *
 *       Filename:  render_canvas.cpp
 *
 *    Description:  Canvas objects are wrappers for a set if CairoLayers.
 *
 * =====================================================================================
 */

#include "config.hpp"

#include <cairo-svg.h>
#include <boost/pointer_cast.hpp>

#include "server/tile.hpp"


#include "server/renderer/renderer.hpp"
#include "server/renderer/renderer_private.hpp"

#include "server/renderer/render_canvas.hpp"

class PNGRenderCanvas : public RenderCanvas
{
private:
	CairoLayer layers[LAYER_NUM];
	CairoLayer slice;

public:
	PNGRenderCanvas()
	: PNGRenderCanvas(META_TILE_SIZE*TILE_SIZE,
					  META_TILE_SIZE*TILE_SIZE,
					  TILE_SIZE,
					  TILE_SIZE)
	{
	}

	PNGRenderCanvas(unsigned int layerWidth, unsigned int layerHeight,
					unsigned int sliceWidth, unsigned int sliceHeight);
	virtual ~PNGRenderCanvas() = default;
	virtual void clear();
	virtual CairoLayer* getImageLayers() { return layers; }
	virtual CairoLayer& getSliceLayer() { return slice; }
	virtual Tile::ImageType copySliceImage();
};

class SVGRenderCanvas : public RenderCanvas
{
private:
	CairoLayer layers[LAYER_NUM];
	CairoLayer slice;
	Tile::ImageType buffer;

public:
	SVGRenderCanvas()
	: SVGRenderCanvas(META_TILE_SIZE*TILE_SIZE,
					  META_TILE_SIZE*TILE_SIZE,
					  TILE_SIZE,
					  TILE_SIZE)
	{
	}

	SVGRenderCanvas(unsigned int layerWidth, unsigned int layerHeight,
					unsigned int sliceWidth, unsigned int sliceHeight);

	virtual void clear();
	virtual CairoLayer* getImageLayers() { return layers; }
	virtual CairoLayer& getSliceLayer() { return slice; }
	virtual Tile::ImageType copySliceImage();
};


/**
 * \param numCavas Number of canvas objects to allocate per type.
 */
RenderCanvasFactory::RenderCanvasFactory()
: svgCanvas(boost::dynamic_pointer_cast<RenderCanvas>(boost::make_shared<SVGRenderCanvas>()))
, pngCanvas(boost::dynamic_pointer_cast<RenderCanvas>(boost::make_shared<PNGRenderCanvas>()))
{
}

//! Write function to capture the final PNG image
static cairo_status_t cairoWriter(void* closure, const unsigned char* data,
								  unsigned int length)
{
	Tile::ImageType::element_type* b = (Tile::ImageType::element_type*) closure;
	b->insert(b->end(), data, data+length);
	return CAIRO_STATUS_SUCCESS;
}


/*
 * PNG Canvas.
 */

PNGRenderCanvas::PNGRenderCanvas(unsigned int layerWidth,
								 unsigned int layerHeight,
								 unsigned int sliceWidth,
								 unsigned int sliceHeight)
{
	for (int i = 0; i < LAYER_NUM; i++)
	{
		layers[i].surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, layerWidth, layerHeight);
		layers[i].cr = cairo_create(layers[i].surface);
	}

	slice.surface = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, sliceWidth, sliceHeight);
	slice.cr = cairo_create(slice.surface);

	cairo_font_options_t* fontOpts = cairo_font_options_create();
	cairo_font_options_set_hint_style(fontOpts, CAIRO_HINT_STYLE_NONE);
	cairo_font_options_set_hint_metrics(fontOpts, CAIRO_HINT_METRICS_OFF);
	cairo_set_font_options(layers[LAYER_LABELS].cr, fontOpts);
	cairo_select_font_face(layers[LAYER_LABELS].cr, DEFAULT_FONT,
		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
}

void PNGRenderCanvas::clear()
{
	for (int i = 0; i < LAYER_NUM; i++)
		layers[i].clear();

	slice.clear();
}

Tile::ImageType PNGRenderCanvas::copySliceImage()
{
	cairo_surface_flush(slice.surface);
	Tile::ImageType buffer = boost::make_shared<Tile::ImageType::element_type>();
	buffer->reserve(10*1024);
	cairo_surface_write_to_png_stream(slice.surface, cairoWriter, (void*) buffer.get());
	return buffer;
}

/*
 * SVG Canvas.
 */

SVGRenderCanvas::SVGRenderCanvas(unsigned int layerWidth,
								 unsigned int layerHeight,
								 unsigned int sliceWidth,
								 unsigned int sliceHeight)
	: buffer(boost::make_shared<Tile::ImageType::element_type>())
{
	for (int i = 0; i < LAYER_NUM; i++)
	{
		layers[i].surface = cairo_svg_surface_create_for_stream(NULL, NULL, layerWidth, layerHeight);
		layers[i].cr = cairo_create(layers[i].surface);
	}

	slice.surface = cairo_svg_surface_create_for_stream(cairoWriter,
		(void*) buffer.get(), sliceWidth, sliceHeight);
	slice.cr = cairo_create(slice.surface);

	/* Just select default font face. Font Hinting/Metric does not affect SVG */
	cairo_select_font_face(layers[LAYER_LABELS].cr, DEFAULT_FONT,
		CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
}

void SVGRenderCanvas::clear()
{
	for (int i = 0; i < LAYER_NUM; i++)
		layers[i].clear();

	slice.clear();
	buffer->clear();
}

Tile::ImageType SVGRenderCanvas::copySliceImage()
{
	cairo_surface_flush(layers[0].surface);
	/* do nothing because cairo_surface_flush(surface) will call cairoWriter */

	/* we need to make a copy since the buffer is owned by the canvas and will be cleared */
	Tile::ImageType copiedBuffer = boost::make_shared<Tile::ImageType::element_type>(*buffer);
	return copiedBuffer;
}
