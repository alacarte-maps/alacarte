#!/usr/bin/python
# -*- coding: utf-8 -*-

from urlparse import urlparse
from threading import Thread
import httplib, sys
import time

concurrent = 20

def doWork(xCoord):
    for i in range(20):
        url='http://localhost:8080/ceyx/16/' + str(34288+xCoord) + '/' + str(22493+i) + '.png'
        status,url=getStatus(url)

def getStatus(ourl):
    try:
        url = urlparse(ourl)
        conn = httplib.HTTPConnection(url.netloc)
        conn.request("HEAD", url.path)
        res = conn.getresponse()
        return res.status, ourl
    except:
        return "error", ourl


threads = []
start = time.time()
print(start)

for i in range(concurrent):
    t=Thread(target=doWork, args=(i,))
    t.daemon=True
    t.start()
    threads.append(t)

# Wait for all
for thread in threads:
    thread.join()

end = time.time()
print(end)
print("Das Rendern dauerte %1.2f Sekunden" % (end - start))
