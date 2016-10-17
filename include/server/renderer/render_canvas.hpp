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

#ifndef RENDER_CANVAS_HPP
#define RENDER_CANVAS_HPP

#include "settings.hpp"

#include <cairo.h>

#include "server/tile.hpp"

//! Stores cairo data for each painting layer of a tile (fill, casing, stroke, ..)
struct CairoLayer
{
	cairo_t* cr;
	cairo_surface_t* surface;

	CairoLayer()
	: cr(NULL)
	, surface(NULL)
	{
	}

	~CairoLayer()
	{
		if (surface)
			cairo_surface_destroy(surface);
		if (cr)
			cairo_destroy(cr);
	}

	void clear()
	{
		cairo_save(cr);

		cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
		cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
		cairo_paint(cr);

		cairo_restore(cr);
	}
};


class RenderCanvas
{
public:
	//! Layers contained in a canvas, used as index for CairoLayer vector
	enum {
		LAYER_FILL = 0,
		LAYER_CASING,
		LAYER_STROKE,
		LAYER_ICONS,
		LAYER_LABELS,
		LAYER_NUM
	};

	virtual ~RenderCanvas() = default;

	//! Clears every layer. Should always be called before use.
	virtual void clear() = 0;
	//! Layers to paint the MetaTile onto
	virtual CairoLayer* getImageLayers() = 0;
	//! Get layer that is the size of a final tile for slicing.
	virtual CairoLayer& getSliceLayer() = 0;
	//! Returns the rendered slice
	virtual Tile::ImageType copySliceImage() = 0;
};

class RenderCanvasFactory
{
public:
	RenderCanvasFactory();

	/**
	 * Returns an empty shared_ptr if format is not supported.
	 */
	shared_ptr<RenderCanvas> getCanvas(TileIdentifier::Format type)
	{
		shared_ptr<RenderCanvas> canvas;
		switch(type)
		{
			case TileIdentifier::Format::PNG:
				canvas = pngCanvas;
				break;
			case TileIdentifier::Format::SVG:
				canvas = svgCanvas;
				break;
			default:
				break;
		}

		return canvas;
	}

private:
	shared_ptr<RenderCanvas> svgCanvas;
	shared_ptr<RenderCanvas> pngCanvas;
};

#endif
