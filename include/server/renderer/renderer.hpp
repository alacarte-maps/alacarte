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

#include <cairo.h>

#include "server/tile_identifier.hpp"
#include "server/tile.hpp"

#define TILE_SIZE 256

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
class RenderCanvas;
struct CairoLayer;

class Renderer
{
public:
	Renderer(const std::shared_ptr<Geodata>& data);
	~Renderer();

	TESTABLE void renderEmptyTile(RenderAttributes& map, const std::shared_ptr<RenderCanvas>& canvas, const std::shared_ptr<Tile>& tile);
	TESTABLE void renderMetaTile(RenderAttributes& map,  const std::shared_ptr<RenderCanvas>& canvas, const std::shared_ptr<MetaIdentifier>& id);
	TESTABLE void sliceTile(const std::shared_ptr<RenderCanvas>& canvas, const std::shared_ptr<MetaIdentifier>& id, const std::shared_ptr<Tile>& tile) const;

protected:
	void placeLabels(const std::list<std::shared_ptr<Label> >& labels,
					 std::vector<std::shared_ptr<Label> >& placed);
	void placeShields(const std::list<std::shared_ptr<Shield> >& shields,
					 std::vector<std::shared_ptr<Shield> >& placed);

private:

	//! stores the actual data
	const std::shared_ptr<Geodata> data;
	FloatRect bounds;
	FloatRect neighbours[8];
	FloatRect neighbourRequests[8];

	void printTileId(cairo_t* cr, const std::shared_ptr<TileIdentifier>& id) const;
	void sortObjects(RenderAttributes& map, std::vector<NodeId>& nodes, std::vector<WayId>& ways, std::vector<RelId>& relations) const;
	bool isCutOff(const FloatRect& box, const FloatRect& owner);
	void compositeLayers(CairoLayer* layers) const;
	void paintBackground(CairoLayer& layer, const Style* canvasStyle) const;
	void renderObjects(CairoLayer* layers, RenderAttributes& map, const cairo_matrix_t* transform,
					   std::vector<NodeId>& nodes, std::vector<WayId>& ways, std::vector<RelId>& relations,
					   std::list<std::shared_ptr<Label>>& labels, std::list<shared_ptr<Shield>>& shields,
					   AssetCache& cache) const;
	template <typename LabelType>
	void renderLabels(cairo_t* cr, std::vector<std::shared_ptr<LabelType> >& labels, AssetCache& cache) const;
	void renderShields(cairo_t* cr, std::vector<std::shared_ptr<Shield> >& shields) const;
	void renderArea(const FixedRect& area,
					const std::shared_ptr<RenderCanvas>& canvas,
					double width, double height,
					RenderAttributes& map,
					AssetCache& cache);

	//! lock calls to cairo for old versions that are not thread-safe.
	static boost::mutex renderLock;
};

#endif
