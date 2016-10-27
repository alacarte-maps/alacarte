[![Build Status](https://travis-ci.org/alacarte-maps/alacarte.png?branch=master)](https://travis-ci.org/alacarte-maps/alacarte)
[![AGPLv3 licensed](https://img.shields.io/badge/license-AGPLv3-blue.svg)](./LICENSE)

# What is alaCarte? #

![screenshot of Karlsruhe](https://github.com/alacarte-maps/alacarte/raw/master/screenshot.png "Karlsruhe")

alaCarte is a tile renderer for OpenStreetMap data written in C++11, using Cairo for
rendering and Boost-Spirit for [MapCSS][] parsing.

The rendered tiles are served over HTTP using the [Slippy map tilenames][] convention.

To compute which data is needed for rendering a tile, alaCarte uses a variant of
a STR-Tree.

alaCarte was designed with medium dataset size in mind. On a typical machine with
at leat 8GB RAM, alaCarte can handle a unfiltered export from the federal state
of Baden-Wuerttemberg (Germany).

alaCarte was developed as part of a lab course student software project at [KIT][].
For the old project files (mostly in German), see the [old project repository][].

[MapCSS]: https://wiki.openstreetmap.org/wiki/MapCSS
[Slippy map tilenames]: https://wiki.openstreetmap.org/wiki/Slippy_map_tilenames
[KIT]: https://algo2.iti.kit.edu
[old project repository]: https://bitbucket.org/TheMarex/alacarte


## alaCarte Links ##
* [Website](https://alacarte-maps.github.io)
* [Developer Documentation](https://alacarte-maps.github.io/alacarte/documentation/)
* [github Project](https://github.com/alacarte-maps/alacarte)


## Features ##

* easy to use
* most MapCSS attributes are implemented (see [MapCSS wiki page][] for comparison)
* no need to filter OSM exports, you have full access to all attributes at runtime
* stylesheets are updated at runtime (changes are detected automatically)
* tiles can be rendered in groups ("meta tile") to speed up rendering

[MapCSS wiki page]: https://wiki.openstreetmap.org/wiki/MapCSS/0.2#Vocabulary


# How to build #

```bash
git clone git@github.com:alacarte-maps/alacarte.git
mkdir alacarte/build
cd alacarte/build
cmake .. -DCMAKE_BUILD_TYPE=Release
# this starts one job per available core to build alacarte n times faster.
# Just run “make” without parameters if you don’t want that or run into problems.
make -j $(nproc)
```

## Build the Documentation #
Build the developer documentation with doxygen:

```bash
make doc
```

You'll then find the documentation at `doc/doxygen/html/index.html`.

The manpages are built with asciidoc:

```bash
make man
```

The results get stored in `build/manpages/`.


## Dependencies ##
* Cairo (>=1.12.0)
* Boost (>= 1.55) (Spirit)
* libpng
* cmake (>= 2.8.8)
* asciidoc (>= 8.6.9) (optional, needed for manpage generation)

alacarte's dependency policy is to depend on nothing that's not included or newer than Debian stable.
So currently, we target and test on Debian Jessie, but we're open to support any other OS.

## Usage ##

See the manpages of
[alacarte-maps-importer](https://alacarte-maps.github.io/alacarte/manpages/alacarte-maps-importer.1.html)
and
[alacarte-maps-server](https://alacarte-maps.github.io/alacarte/manpages/alacarte-maps-server.1.html).

You can use the test Leaftlet-Page located in `tests/html/Leaflet/index.html`
to view the rendered tiles.

You can download fresh OSM exports from [Geofabrik](https://download.geofabrik.de/).

## Install ##

(as root)

	make install

The config file is located in `/etc/alacarte-maps.conf`. You should make sure the user
running alacarte has permissions to write to the specified directories for caching
and logging.


*alacarte is an awesome lolipop!!!*
