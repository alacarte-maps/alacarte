#!/usr/bin/python
import cairo
import sys
import math
import gc

RADIUS = 6378137.0
FACTOR = 100.0
NORM = (FACTOR * RADIUS/2.0)
def tileToMercator (tx, ty, zoom):
    n = (1 << zoom)
    x =  (tx * 2.0 / n - 1) * NORM
    y = -(ty * 2.0 / n - 1) * NORM
    return (int(x), int(y))

def parse_rect(data, start, end):
    closing = data.find(")", start, end)
    values = [ int(n.strip()) for n in data[start+2:closing].split(",")]
    return values

def parse_leaf(data, start, end):
    rects = []
    pos = data.find("R(", start, end)
    while pos > 0 and pos < end:
        nextRect = data.find("R(", pos+1, end)
        rect = parse_rect(data, pos, nextRect)
        rects.append(rect)
        pos = nextRect
    return rects

WIDTH = 512.0
HEIGHT = 512.0

log = "nodes.bin.log"
pic = "nodesLeaves.png"
if len(sys.argv) > 2:
    log = sys.argv[1]
    pic = sys.argv[2]

f = open(log)

surface = cairo.ImageSurface(cairo.FORMAT_RGB24, 512, 512)
cr = cairo.Context(surface)
cr.set_source_rgb(1.0, 1.0, 1.0)
cr.paint()

minX, minY = tileToMercator(267, 174, 9)
maxX, maxY = tileToMercator(271, 178, 9)
width = maxX - minX
height = maxY - minY

cr.scale(WIDTH / width, HEIGHT / height)
cr.translate(-minX, -minY)

leaves = f.read()

l, _ = cr.device_to_user_distance(2.0, 0.0)
cr.set_line_width(l)
cr.set_source_rgba(1.0, 0.0, 0.0, 0.1)
start = 0
pos = leaves.find("L(", start)
while pos >= 0 and pos < len(leaves):
    end = leaves.find("L(", pos + 1)
    end = end > 0 and end or len(leaves)
    rects = parse_leaf(leaves, pos, end);

    for (x0, y0, x1, y1) in rects:
        cr.rectangle(x0, y0, x1-x0, y1-y0)
        cr.fill()
    pos = end

surface.flush()
surface.write_to_png(pic)

