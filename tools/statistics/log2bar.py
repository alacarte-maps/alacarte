#!/usr/bin/python

import sys, re
import numpy as np
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt

#
# Useage: ./log2bar.py performance.log bar_graph.png
#

exp = re.compile("JobStart \d+ GeoNodes (?P<nodes_query>\d+) GeoWays (?P<ways_query>\d+) GeoRelation (?P<relations_query>\d+) StylesheetMatch (?P<match>\d+) Renderer (?P<rendering>\d+) GeoContainsData (?P<contains_data>\d+) Slicing (?P<slicing>\d+) Stylesheet (?P<stylesheet>\w+) Zoom (?P<zoom>\d+) Nodes (?P<nodes>\d+) Ways (?P<ways>\d+) Relations (?P<relations>\d+)")

f = open(sys.argv[1])
means = {}

components = ['contains_data', 'nodes_query', 'ways_query', 'relations_query', 'match', 'rendering','slicing']
for c in components:
    means[c] = ([0]*19, [0]*19)

for l in f.readlines():
    match = exp.match(l)
    if not match:
        continue
    z = int(match.group('zoom'))
    for c in components:
       comp_avg, comp_cnt = means[c]
       n = comp_cnt[z]
       avg = comp_avg[z]
       avg = avg * (n/(n + 1.0)) + float(match.group(c))/(n+1)
       comp_cnt[z] += 1
       comp_avg[z] = avg

width = 0.35
bars = []
ind = np.arange(19)
offsets = [0]*19
colors = {
'nodes_query': 'white',
'ways_query': 'white',
'relations_query': 'white',
'match': 'blue',
'rendering': 'green',
'contains_data': 'yellow',
'slicing': 'red'
}
for c in components:
    comp_avg, _ = means[c]
    b = plt.bar(ind, comp_avg, width, color=colors[c], bottom=offsets)
    offsets = [o+x for o, x in zip(offsets, comp_avg)]
    bars.append(b)

plt.ylabel('Time in us')
plt.title('Rendering time by Zoomlevel')
plt.xticks(ind+width/2.0, [str(i) for i in range(19)])
ledg = [b[0] for b in bars]
plt.legend(ledg, components)
plt.savefig(sys.argv[2])
