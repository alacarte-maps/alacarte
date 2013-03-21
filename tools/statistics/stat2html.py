#!/usr/bin/python
# -*- coding: utf-8 -*-

# TODO: per zoomlevel

import math
import html
from threading import Thread
import multiprocessing
import os

# README:
# sudo apt-get install python-matplotlib

# Settings
INPUT = "performance.log"
THREADS = 1

# Read in data
allStats = []
f = open(INPUT)
for line in f:
    list = line.split(" ");
    stats = {'JobStart': int(list[1]),
            'GeoContainsData': int(list[-11]),
            'Stylesheet': list[-9],
            'Zoom': int(list[-7]),
            'Nodes': int(list[-5]),
            'Ways': int(list[-3]),
            'Relations': int(list[-1])}
    if len(list) == 16: # empty Tile
        stats['ComputeRect'] = int(list[3])
        stats['Cache'] = 0
        stats['GeoNodes'] = 0
        stats['GeoWays'] = 0
        stats['GeoRelations'] = 0
        stats['StylesheetMatch'] = 0
        stats['Renderer'] = 0
    if len(list) == 18: # cached Tile
        stats['Cache'] = int(list[3])
        stats['ComputeRect'] = int(list[5])
        stats['GeoNodes'] = 0
        stats['GeoWays'] = 0
        stats['GeoRelations'] = 0
        stats['StylesheetMatch'] = 0
        stats['Renderer'] = 0
    elif len(list) == 28: # not cached Tile
        stats['Cache'] = int(list[3])
        stats['ComputeRect'] = int(list[5])
        stats['GeoNodes'] = int(list[7])
        stats['GeoWays'] = int(list[9])
        stats['GeoRelations'] = int(list[11])
        stats['StylesheetMatch'] = int(list[13])
        stats['Renderer'] = int(list[15])
    allStats.append(stats)
allStats = sorted(allStats, key=lambda foo: foo['JobStart'])

#print(allStats)

def renderComponent(component, allStats):
    print("Rendering component " + component)
    code = html.getHtmlHeader(component)
    code += html.generateStatistics(allStats, component)
    code += html.getHtmlEnd()
    file = open(component + ".htm", "w")
    file.write(code)
    file.close()

try:
    os.mkdir('images')
except:
    pass

# Generating statistics for each component.
#components = ['Cache', 'ComputeRect', 'GeoContainsData', 'GeoNodes', 'GeoWays', 'GeoRelations', 'StylesheetMatch', 'Renderer']
components = ['Cache', 'GeoContainsData', 'GeoNodes', 'Renderer']
for component in components:
    #renderComponent(component, allStats)
    list = []
    for object in allStats:
        if object[component] != 0:
            list.append(object[component])
    list = sorted(list)
    if component == 'GeoContainsData':
        list = list[0:-4]
    median = html.calcMedian(list)
    if component == 'Cache':
        html.makeBoxPlot(allStats, component, median=median, scale=1.0, name='Cache::getTile()')
    elif component == 'GeoContainsData':
        html.makeBoxPlot(allStats, component, median=median, scale=1.0, name='GeoData::containsData()')
    elif component == 'GeoNodes':
        html.makeBoxPlot(allStats, component, median=median, scale=1.0, start=6, name='GeoData::getNodeIDs()')
    else:
        html.makeBoxPlot(allStats, component, median=median, start=6, name='Renderer::renderTile()')


# Render only some special graphs
html.makeScatterPlot(allStats, 'Nodes', 'GeoContainsData', yName='GeoData::containsData()', function='0*n', opacity=0.2)
html.makeScatterPlot(allStats, 'Nodes', 'GeoNodes', yName='GeoData::getNodeIds()')
html.makeScatterPlot(allStats, 'Ways', 'Renderer', yName='Renderer::renderTile()')
html.makeScatterPlot(allStats, 'Ways', 'StylesheetMatch', yName='Stylesheet::match()')


# Generating HTML-Timechart
print("Generating HTML-Timechart")
code = html.getHtmlHeader('Timechart')
code += html.generateTimechart(allStats, ['JobStart', 'Cache', 'ComputeRect', 'GeoContainsData', 'GeoNodes', 'GeoWays', 'GeoRelations', 'StylesheetMatch', 'Renderer'])
code += html.getHtmlEnd()
file = open("timechart.htm", "w")
file.write(code)
file.close()

# Generating index file.
code = html.getHtmlHeader('Index')
for component in ['Cache', 'ComputeRect', 'GeoContainsData', 'GeoNodes', 'GeoWays', 'GeoRelations', 'StylesheetMatch', 'Renderer']:
    code += "<a href=\"" + component + ".htm\">" + component + "</a><br/>\n"
code += "<a href=\"timechart.htm\">Timechart</a><br/>\n"
code += "<a href=\"gprof.htm\">Gprof output</a><br/>\n"
file = open("index.htm", "w")
file.write(code)
file.close()
