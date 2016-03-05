[![Build Status](https://travis-ci.org/alacarte-maps/alacarte.png?branch=master)](https://travis-ci.org/alacarte-maps/alacarte)
[![AGPLv3 licensed](https://img.shields.io/badge/license-AGPLv3-blue.svg)](./LICENSE)

# What is alaCarte? #

![screenshot of Karlsruhe](http://github.com/alacarte-maps/alacarte/raw/master/screenshot.png "Karlsruhe")

alaCarte is a tile renderer for OpenStreetMap data written in C++11, using Cairo for
rendering and Boost-Spirit for [MapCSS](http://wiki.openstreetmap.org/wiki/MapCSS) parsing.

The rendered tiles are served over HTTP using the [Slippy map tilenames](http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames) convention.

To compute which data is needed for rendering a tile, alaCarte uses a variant of
a kd-Tree.

alaCarte was designed with medium dataset size in mind. On a typical machine with
at leat 8GB RAM, alaCarte can handle a unfiltered export from the federal state
of Baden-Wuerttemberg (Germany).

alaCarte was developed as part of a lab course student software project at [KIT](http://algo2.iti.kit.edu).
For the old project files (mostly in German), see the [old project repository](https://bitbucket.org/TheMarex/alacarte).


alaCarte also has a growing website at [http://alacarte-maps.github.io](http://alacarte-maps.github.io/).

## Features ##

* easy to use
* most MapCSS attributes are implemented (see [MapCSS wiki page](http://wiki.openstreetmap.org/wiki/MapCSS/0.2#Vocabulary) for comparison)
* no need to filter OSM exports, you have full access to all attributes at runtime
* stylesheets are updated at runtime (changes are detected automatically)
* tiles can be rendered in groups ("meta tile") to speed up rendering _(not in 0.2)_

# How to build #

	mkdir build
	cd build
	cmake .. -DCMAKE_BUILD_TYPE=Release
	# this starts one job per available core to build alacarte n times faster.
	# Just run “make” without parameters if you don’t want that or run into problems.
	make -j $(nproc)

## Dependencies ##
* Cairo (>=1.12.0)
* Boost (>= 1.55) (Spirit)
* libpng
* cmake (>= 2.8.8)

alacarte's dependency policy is to depend on nothing that's not included or newer than Debian stable.
So currently, we target and test on Debian Jessie, but we're open to support any other OS.

## Usage ##

(from build directory)

	./alacarte-importer osm_export.osm data.carte
	./alacarte-server -g data.carte -s ../data/mapcss

You can use the test Leaftlet-Page located in *tests/html/Leaflet/index.html*
to view the rendered tiles.

You can download fresh OSM exports from [Geofabrik](http://download.geofabrik.de/).

## Install ##

(as root)

	make install

The config file is located in */etc/alacarte.conf*. You should make sure the user
running alacarte has permissions to write to the specified directories for caching
and logging.

# To Do #
* Real database backend (Postgresql?)
* Why can't we render tiles in svg / vector graphics format?
  Should work in theory, but in practice, something in cario seems to rasterize the result in the end…
* Better font rendering (Pango?)
* Unicode support (alaCarte itself supports unicode, the problem is unicode font rendering using Cairo / Pango)
* Implement MapCSS-include
* Better caching of fonts and icons
* Option to "tune" imported data for specific stylesheets
* Use bloom filters for CSS matching, see e.g. https://github.com/servo/rust-selectors
