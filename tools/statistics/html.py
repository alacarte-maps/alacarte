#!/usr/bin/python
# -*- coding: utf-8 -*-

import math
import os
import matplotlib
import pylab
import numpy

# Settings
PRERENDER = 12

def getHtmlHeader(title):
    return """<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN"\n
        "http://www.w3.org/TR/html4/loose.dtd">\n
        <html>
        <head>
        <meta http-equiv="Content-Type" content="text/html; charset=utf-8">
        <title>""" + title + """</title>
        <style>
         .bar {
            border: 1px solid black;
            height: 2.5em;
            position: absolute;
            text-align: right;
            margin-bottom: 5px;
            float:left;
         }
         div .Cache {
             background-color: #FF6600;
         }
         div .ComputeRect {
             background-color: #666666;
         }
         div .GeoNodes {
             background-color: #800080;
         }
         div .GeoWays {
             background-color: #FF00FF;
         }
         div .GeoRelations {
             background-color: #DD55FF;
         }
         div .StylesheetMatch {
             background-color: #4444FF;
         }
         div .Renderer {
             background-color: #FFFF00;
         }
         div .GeoContainsData {
             background-color: #008000;
         }
        .chart {
          height: 600px;
          margin:0;
          padding:0;
          background:#eee;
          border:1px solid #999;
          width:1200px;
          list-style:none;
          overflow:hidden;
        }
        .chart li{
          list-style:none;
          margin:0;
          padding:0;
          float:left;
          width:6px;
          background:#369;
          border-top:1px solid #eee
        }
        .chart li span{
          position:absolute;
          top:1em;
          left:-9999px;
        }
        .chart li:hover{
          background:#69c;
          border-top-color: #999;
        }
        .chart li:hover span{
          left:1em;
        }
        table {
            border-collapse: collapse;
        }
        td, th {
            border: 1px solid black;
        }
        th {
            background-color: #BBBBBB;
        }
        .scatterplot {
            width: 502px;
            height: 502px;
            border: 1px solid black;
            position: relative;
            margin-left: 10em;
        }
        .point {
            width: 2px;
            height: 2px;
            background-color: #000000;
            position: absolute;
        }
        </style>
        <script src="sorttable.js"></script>
        </head>
        <body>"""

def getHtmlEnd():
    return """ </body></html>"""

def makeSortableTable(objects, attributes):
    html = """<table class="sortable" style="border: 1px solid black;">\n<tr>"""
    for attribute in attributes:
        html += "<th>" + str(attribute) + "</th>"
    html += "</tr>\n"
    for object in objects:
        html += "<tr>"
        for attribute in attributes:
            html += "<td>" + str(object[attribute]) + "</td>"
        html += "</tr>\n"
    html += "</table>\n"
    return html

# Generating HTML-Timechart
# objects: dicts with ints containing the timing
# attributes: ordering of the attributes
def generateTimechart(objects, attributes, scale=1000.0):
    html = '';
    html += '<div style="position: fixed; border-bottom: 1px solid black; background-color: #FFFFFF; z-index: 50; width:100%; margin: 0;">'
    html += '	<h1>Timechart</h1>'
    #Legende:
    html += '	<h2 style="margin-bottom: 0">Legende:</h2>'
    html += '	<div class="bar" style="position:relative; border: 0px solid black;">'
    html += '		<div class="bar Cache " 			style="margin-left: 0px; 	width: 150px;">Cache<br>Duration</div>'
    html += '		<div class="bar ComputeRect " 		style="margin-left: 150px; 	width: 150px;">ComputeRect<br>Duration</div>'
    html += '		<div class="bar GeoContainsData " 	style="margin-left: 300px; 	width: 150px;">GeoContainsData<br>Duration</div>'
    html += '		<div class="bar GeoNodes " 		style="margin-left: 450px; 	width: 150px;">GeoNodes<br>Duration</div>'
    html += '		<div class="bar GeoWays " 			style="margin-left: 600px; 	width: 150px;">GeoWays<br>Duration</div>'
    html += '		<div class="bar GeoRelations " 	style="margin-left: 750px; 	width: 150px;">GeoRelations<br>Duration</div>'
    html += '		<div class="bar StylesheetMatch " 	style="margin-left: 900px; 	width: 150px;">StylesheetMatch<br>Duration</div>'
    html += '		<div class="bar Renderer " 		style="margin-left: 1050px;	width: 150px;">Renderer<br>Duration</div>'
    html += '	</div><br><br>'
    html += '	<p>...... bedeutet, es vergeht einige Zeit zwischen den Jobs</p>\n'
    html += '	<h2>Chart:</h2>'
    html += '</div><br>'
    html += '<div style="clear:both; margin-top: 250px;"></div>'
    currentRow = 0
    amountRows = 1
    rowLength = [0] # Unit: px
    offset = objects[0][attributes[0]] # Unit: µs
    for object in objects:
        # Normalize
        #print(object)
        start = (object[attributes[0]] - offset)/scale # Unit: px
        if min(rowLength) > start:
            amountRows += 1
            rowLength.append(0)
        elif max(rowLength)+1000 < start:
            # A lot of time since last job (more than 1000px space)
            html += """<div class="bar" style="width: 90px; height: """ + str(amountRows*3) + """em; background-color: #CCCCCC; text-align: center; margin-left: """ + str(max(rowLength)+5) + """px; margin-top: 0px; border: 0px solid black;">......</div>\n"""
            maximum = max(rowLength)
            for i in range(len(rowLength)):
                rowLength[i] = maximum
            offset = object[attributes[0]] - (rowLength[0]*scale + (100 * scale))
            start = (object[attributes[0]] - offset)/scale
        currentRow = rowLength.index(min(rowLength))
        html += """ <div class="bar" style="position: relative; margin-top:""" + str(currentRow*3) + """em; border: 0px solid black;">\n"""
        for attribute in attributes[1:]:
            if object[attribute] != 0:
                html += """     <div class="bar """ + str(attribute) + """ " style="margin-left: """ + str(start) + """px; width: """ + str(object[attribute]/scale) + """px;">"""
                if object[attribute]/scale > 100:
                    html += "" + str(attribute) + """<br>""" + str(object[attribute])
                html += """</div>\n"""
                start += object[attribute]/scale
        html += """ </div>\n"""
        rowLength[currentRow] = start
    return html

def calcAverage(list):
    # Calculate average
    sum = 0
    for element in list:
        sum += element
    return sum/len(list)

def calcMedian(list):
    # Median
    n = len(list)
    median = 0
    if n%2:
        median = list[(n+1)/2-1]
    else:
        median = (list[n/2-1]+list[n/2])/2
    return median

def calcAverageAbsoluteDeviation(list):
    # Calculate average absolute Deviation
    sum = 0
    for element in list:
        sum += element
    average = sum/len(list)
    avgAbsDev = 0
    for element in list:
        avgAbsDev += abs(element - average)
    return avgAbsDev*1.0/len(list)

def makeBarDiagram(list):
    # Divide in classes
    minimum = list[0]
    maximum = list[-1]
    width = math.ceil((maximum-minimum)/200.0)
    distribution = [0]*200
    if width == 0:
        distribution[0] = len(list)
    else:
        for element in list:
            index = int((element - minimum) / width)
            if index >= len(distribution):
                index = len(distribution)-1
            distribution[index] += 1
    #print(distribution)
    html = """<div style="position: relative; border-width: 0px"><ul class="chart">"""
    most = max(distribution)
    for i, value in enumerate(distribution):
        html += """<li style="border-top-width:""" + str((most - value)*(600.0/most)) + """px; height: """ + str(value*(600.0/most)) + """px;"><span>""" + str(value) + "mal " + str(int(minimum + i*width)) + " - " + str(int(minimum + (i+1)*width)) + """µs </span></li>\n"""
    html += "</ul></div>"
    return html

def makeScatterPlot(objects, xAttribute, yAttribute, scale = 1.0, opacity=0.2, function="0*n", yName=None, xName=None):
    # xName = complete caption
    # yCaption = yName + "- Dauer in x ms"
    if not yName:
        yName = yAttribute
    if not xName:
        xName = xAttribute
    xValues = []
    yValues = []
    colors = []
    for object in reversed(objects):
        xValues.append(object[xAttribute]/scale)
        yValues.append(object[yAttribute]/scale)
        if yAttribute != 'StylesheetMatch':
            if object['Zoom'] < PRERENDER:
                colors.append('none')
            else:
                colors.append('k')
        else:
            if object['Stylesheet'] == 'default':
                colors.append('w')
            else:
                colors.append('k')
    fig, ax = matplotlib.pyplot.subplots()
    ax.scatter(xValues, yValues, facecolors=colors, alpha=opacity)
    #matplotlib.pyplot.show()
    #ax = image.add_subplot(111)
    ax.set_xlabel(xName)
    if scale < 1000:
        ax.set_ylabel(yName + u" - (Dauer in " + int(scale) + u" µs)")
    else:
        ax.set_ylabel(yName + u" - (Dauer in " + int(scale/1000) + u" ms)")
    ax.set_xscale('symlog')
    ax.set_yscale('symlog')
    if ax.get_xlim()[0] < 1:
        ax.set_xlim(xmin=1)
    if yAttribute == 'GeoContainsData':
        ax.set_xlim(xmin=0)
    if ax.get_ylim()[0] < 1:
        ax.set_ylim(ymin=1)
    ax.grid(True, which='both')
    #matplotlib.pyplot.subplot(211)
    n = numpy.arange(1, ax.get_xlim()[1], 1)
    p0 = matplotlib.pyplot.plot(n, eval(function), 'r-')
    #p1 = matplotlib.pyplot.plot(n, n*numpy.log(n), 'r-')
    #p2 = matplotlib.pyplot.plot(n, n, 'g-')
    #p3 = matplotlib.pyplot.plot(n, n*n, 'b-')
    #matplotlib.pyplot.legend([p1, p2, p3], ["nlogn", "n", "n2"])
    pylab.savefig("images/" + xAttribute + "_" + yAttribute + ".png")
    return "<img src=\"images/" + xAttribute + "_" + yAttribute + ".png\">"

def makeHeatMap(objects, xAttribute, yAttribute, scale = 1.0):
    xValues = []
    yValues = []
    colors = []
    for object in reversed(objects):
        xValues.append(object[xAttribute]/scale)
        yValues.append(object[yAttribute]/scale)
        if yAttribute != 'StylesheetMatch':
            if object['Zoom'] < PRERENDER:
                colors.append('w')
            else:
                colors.append('k')
        else:
            if object['Stylesheet'] == 'default':
                colors.append('w')
            else:
                colors.append('k')
    heatmap, xedges, yedges = numpy.histogram2d(xValues, yValues, bins=50)
    extent = [xedges[0], xedges[-1], yedges[0], yedges[-1]]
    matplotlib.pyplot.clf()
    matplotlib.pyplot.imshow(heatmap, extent=extent)
    matplotlib.pyplot.savefig("images/Heatmap_" + xAttribute + "_" + yAttribute + ".png")
    #matplotlib.pyplot.show()
    #fig, ax = matplotlib.pyplot.subplots()
    #ax.hist2d(xValues, yValues, bins=40)
    #pylab.savefig("Heatmap_" + xAttribute + "_" + yAttribute + ".png")

def makeBoxPlot(objects, component, median=0, scale=1000.0, name=None, start=0):
    if not name:
        name = component
    byZoom = [[], [], [], [], [], [], [], [], [], [], [], [], [], [], [], [], [], [], []]
    for object in objects:
        if object[component]:
            byZoom[object['Zoom']].append(object[component]/scale)
    byZoom = byZoom[start:]
    fig, ax = matplotlib.pyplot.subplots()
    ax.boxplot(byZoom)
    n = numpy.arange(1, ax.get_xlim()[1], 1)
    matplotlib.pyplot.plot(n, [median/scale]*len(n), 'r--')
    if scale < 1000:
        ax.set_ylabel(name + u' - (Dauer in ' + int(scale) + u' µs)')
    else:
        ax.set_ylabel(name + u' - (Dauer in ' + int(scale/1000) + u' ms)')
    ax.set_xlabel('Zoomstufe')
    ax.set_yscale('symlog')
    pylab.xticks(range(1, len(byZoom)+1), range(start, len(byZoom)+start))
    pylab.savefig("images/Barplot_" + component + ".png")
    return "<img src=\"images/Barplot_" + component + ".png\">"

# Generating statistics for each component.
def generateStatistics(objects, attribute):
    html = ""
    html += '<h3>' + str(attribute) + '</h3><ul>'
    list = []
    for object in objects:
         if object[attribute] != 0:
                list.append(object[attribute])
    list = sorted(list)
    average = calcAverage(list)
    html += '<li> Durchschnitt: ' + str(average) + 'µs </li>'
    median = calcMedian(list)
    html += '<li> Median: ' + str(median) + 'µs </li>'
    avgAbsDev = calcAverageAbsoluteDeviation(list)
    html += '<li> mittlere absolute Abweichung: ' + str(avgAbsDev) + 'µs </li>'
    html +=  '</ul>\n'
    html += makeBarDiagram(list)
    html += '<h4>Scatterplot Dauer/Nodes</h4>'
    html += makeScatterPlot(objects, 'Nodes', attribute)
    html += '<h4>Scatterplot Dauer/Ways</h4>'
    html += makeScatterPlot(objects, 'Ways', attribute)
    html += '<h4>Scatterplot Dauer/Relations</h4>'
    html += makeScatterPlot(objects, 'Relations', attribute)
    #makeHeatMap(objects, 'Nodes', attribute)
    # Sort by zoomlevel
    html += '<h4>Time distribution per Zoomlevel</h4>'
    html += makeBoxPlot(objects, attribute, median)
    return html
