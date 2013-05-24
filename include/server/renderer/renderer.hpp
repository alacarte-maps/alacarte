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

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include "settings.hpp"

#include <boost/thread/mutex.hpp>

#include <cairomm/surface.h>
#include <cairomm/context.h>

#include "server/tile_identifier.hpp"
#include "server/tile.hpp"

#define TILE_SIZE 256

class MetaTile;
class MetaIdentifier;
class RenderAttributes;
class Geodata;
class GeoObject;
class Tile;
class Node;
class Way;
class Style;
class Label;
class Shield;
class AssetCache;

class Renderer
{
public:
	Renderer(const shared_ptr<Geodata>& data);
	~Renderer();

	TESTABLE void renderEmptyTile(RenderAttributes& map, const shared_ptr<Tile>& tile);
	TESTABLE void renderMetaTile(RenderAttributes& map, const shared_ptr<MetaTile>& tile);
	TESTABLE void sliceTile(const shared_ptr<MetaTile>& meta, const shared_ptr<Tile>& tile) const;

protected:
	void placeLabels(const std::list<shared_ptr<Label> >& labels,
					 std::vector<shared_ptr<Label> >& placed);
	void placeShields(const std::list<shared_ptr<Shield> >& shields,
					 std::vector<shared_ptr<Shield> >& placed);

private:
	class PNGWriter;
	class ImageWriter;
	class SVGWriter;

	//! stores the actual data
	const shared_ptr<Geodata> data;
	FloatRect bounds;
	FloatRect neighbours[8];
	FloatRect neighbourRequests[8];

	//! Stores cairo data for each painting layer of a tile (fill, casing, stroke, ..)
	class CairoLayer;

	//! Abstract base class for PNG and SVG writers
	class ImageWriter {
	public:
		virtual ~ImageWriter() {};
		virtual Cairo::RefPtr<Cairo::Surface> createSurface() = 0;
		virtual Cairo::RefPtr<Cairo::Surface> createSurface(const Tile::ImageType& buffer) = 0;
		virtual void write(const Cairo::RefPtr<Cairo::Surface>& surface) = 0;
	};

	//! Layers to paint onto
	enum {
		LAYER_FILL = 0,
		LAYER_CASING,
		LAYER_STROKE,
		LAYER_ICONS,
		LAYER_LABELS,
		LAYER_NUM
	};

	shared_ptr<ImageWriter> getWriter(TileIdentifier::Format format, int width, int height) const;
	void printTileId(const Cairo::RefPtr<Cairo::Context>& cr, const shared_ptr<TileIdentifier>& id) const;
	void sortObjects(RenderAttributes& map, std::vector<NodeId>& nodes, std::vector<WayId>& ways, std::vector<RelId>& relations) const;
	bool isCutOff(const FloatRect& box, const FloatRect& owner);
	void compositeLayers(CairoLayer layers[]) const;
	void setupLayers(CairoLayer layers[], const shared_ptr<ImageWriter>& writer, AssetCache& cache) const;
	void paintBackground(const CairoLayer& layer, const Style* canvasStyle) const;
	void renderObjects(CairoLayer layers[], RenderAttributes& map,
					   const Cairo::Matrix& transform,
					   std::vector<NodeId>& nodes, std::vector<WayId>& ways, std::vector<RelId>& relations,
					   std::list<shared_ptr<Label>>& labels,
					   std::list<shared_ptr<Shield>>& shields,
					   AssetCache& cache) const;
	template <typename LabelType>
	void renderLabels(const Cairo::RefPtr<Cairo::Context>& cr,
					  std::vector<shared_ptr<LabelType> >& labels, AssetCache& cache) const;
	void renderShields(const Cairo::RefPtr<Cairo::Context>& cr,
					  std::vector<shared_ptr<Shield> >& shields) const;
	void renderArea(const FixedRect& area,
					CairoLayer layers[],
					double width, double height,
					RenderAttributes& map,
					AssetCache& cache);

	//! lock calls to cairo for old versions that are not thread-safe.
	static boost::mutex renderLock;
};

#endif
