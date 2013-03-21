#!/usr/bin/python
# -*- coding: utf-8 -*-

# TODO: per zoomlevel
import os
import html

# Config
PathToProfileBuild = "../../profile_build/"

class function(dict):
    def __init__(self, line):
        values = line.split(None, 6)
        # print(values)
        self['percentage'] = values[0]
        self['totalTime'] = values[1]
        self['ownTime'] = values[2]
        try:
            int(values[3])
            self['calls'] = values[3]
            self['ownTimePerCall'] = values[4]
            self['totalTimePerCall'] = values[5]
            self['name'] = values[6]
        except:
            # gprof functions
            values = line.split(None, 3)
            self['calls'] = 0
            self['ownTimePerCall'] = 0
            self['totalTimePerCall'] = 0
            self['name'] = values[3]

# Read in data
allFunctions = []
try:
    f = open(PathToProfileBuild + 'server.prof')
except:
    # Generate readable output
    here = os.getcwd()
    os.chdir(PathToProfileBuild)
    os.system('gprof alacarte-server gmon.out > server.prof')
    os.chdir(here)
for i, line in enumerate(f):
    if i < 5:
        # Skip headings
        continue
    if line == "\n":
        # Stop before callstack
        break
    allFunctions.append(function(line))
code = html.getHtmlHeader('gprof')
code += html.makeSortableTable(allFunctions, ['percentage', 'totalTime', 'ownTime', 'calls', 'ownTimePerCall', 'totalTimePerCall', 'name'])
code += html.getHtmlEnd()
file = open('gprof.htm', 'w')
file.write(code)
file.close()
