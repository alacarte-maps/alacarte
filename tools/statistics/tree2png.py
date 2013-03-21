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

def parse_points(data, start, end):
    ps = []
    i = start
    while i < end:
        if data[i] == '(':
            j = i
            while data[j] != ",":
                j += 1
            v1 = int(data[i+1 : j])

            k = j
            while data[k] != ")":
                k += 1
            v2 = int(data[j+1 : k])
            ps.append((v1, v2))
            i = k
        i += 1
    return ps

def parse_lines(data, start, end):
    ls = []
    i = start
    while i < end:
        if data[i] == '(':
            j = i+1
            cnt = 1
            while cnt != 0:
                if data[j] == "(":
                    cnt += 1
                if data[j] == ")":
                    cnt -= 1
                j += 1
            ps = parse_points(data, i+1, j)
            ls.append(ps)
            i = j
        i += 1
    return ls

WIDTH = 512.0
HEIGHT = 512.0

log = "../../tests/data/rendered/nodeTree.log"
pic = "nodeTree.png"
if len(sys.argv) > 2:
    log = sys.argv[1]
    pic = sys.argv[2]

f = open(log)

f.readline() # depth
f.readline() # leafes

surface = cairo.ImageSurface(cairo.FORMAT_RGB24, 512, 512)
cr = cairo.Context(surface)
cr.set_source_rgb(1.0, 1.0, 1.0)
cr.paint()

minX, minY = tileToMercator(8572, 5623, 14)
maxX, maxY = tileToMercator(8577, 5628, 14)
width = maxX - minX
height = maxY - minY

cr.scale(WIDTH / width, HEIGHT / height)
cr.translate(-minX, -minY)

nodes = f.readline()
start = nodes.index("(")
points = parse_points(nodes, start+1, len(nodes))
del nodes
gc.collect()

cr.set_source_rgba(0.0, 0.0, 0.0, 0.2)
r, _ = cr.device_to_user_distance(1.0, 0.0)
for p in points:
    cr.arc(p[0], p[1], r, 0.0, math.pi*2.0)
    cr.fill()
del points
gc.collect()

splitLines = f.readline()
start = splitLines.index("(")
lines = parse_lines(splitLines, start+1, len(splitLines))
del splitLines
gc.collect()

l, _ = cr.device_to_user_distance(2.0, 0.0)
cr.set_line_width(l)
cr.set_source_rgba(1.0, 0.0, 0.0, 1.0)
for p1, p2 in lines:
    cr.move_to(p1[0], p1[1])
    cr.line_to(p2[0], p2[1])
    cr.stroke()
del lines
gc.collect()

surface.flush()
surface.write_to_png(pic)

