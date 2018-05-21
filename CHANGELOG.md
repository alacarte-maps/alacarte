# Change Log #
All notable changes to this project will be documented in this file.
This project adheres to [Semantic Versioning](http://semver.org/).

## [0.4.0] - 2016-12-28 ##
### Added ###
- man pages for alacarte-importer and alacarte-server.
- The importer can now limit the area to import via `min-lat`, `max-lat`, `min-lon` and `max-lon` arguments
  to reduce memory consumption if you don't need the whole area of your source data.
- support for Debian Jessie (as it's the new stable).

### Changed ###
- alaCarte now uses boost::log instead of log4cpp.
- alaCarte now uses Cairo directly instead of Cairomm.
- renamed everything necessary from alacarte to alacarte-maps
  to avoid clashes with the [GNOME menu editor alacarte](https://en.wikipedia.org/wiki/Alacarte).

### Removed ###
- support for Debian Wheezy (as it's now oldstable and its boost version is now too old).
- dependency on log4cpp.
- dependency on Cairomm.
- dependency on SigC++.

### Fixed ###
- repaired & improved builds on Windows (32bit & 64bit) & FreeBSD.
- cleaned up C++ and MapCSS code for better readability and understandability.
- cleaned and translated developer documentation.
- building now works with clang.
- speed up build times a little.
- possible segfault when some data is not present (#43).
- fixed some problems with automatic file change detection by updating dir_monitor to a newer version.


## [0.3.0] - 2013-05-04 ##
### Added ###
- near complete support for MapCSS styling attributes
- rendering of 4x4 tiles ("meta tiles") at the same time to gain speed (and visual quality)

### Changed ###
- reduced memory consumption thanks to using a STR-Tree for rectangle queries


## [0.2.1] - 2013-04-10 ##
### Fixed ###
- solved packaging problems to ease installation.


## 0.2.0 - 2013-03-21 ##
(initial import to github)
### Main Features ###
- most MapCSS attributes are now implemented.
- no need to filter OSM exports, you have full access to all attributes at runtime.
- stylesheets are updated at runtime (changes are detected automatically).

## 0.1.0 - 2013-? ##
– forever forgotten in history…


This uses [Keep a CHANGELOG](http://keepachangelog.com/) as a template.


[0.4.0]: https://github.com/alacarte-maps/alacarte/compare/v0.3.0...HEAD
[0.3.0]: https://github.com/alacarte-maps/alacarte/compare/v0.2.1...v0.3.0
[0.2.1]: https://github.com/alacarte-maps/alacarte/compare/v0.2.0...v0.2.1
