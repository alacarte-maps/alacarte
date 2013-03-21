#!/usr/bin/python
# -*- coding: utf-8 -*-

from urlparse import urlparse
from threading import Thread
import httplib, sys
import time
from Queue import Queue

"""Queries all tile and gets measurements."""
CONCURRENT = 20
SERVER = 'http://localhost:8080/default/'
OUTPUT_ALL = True

def doWork():
    while True:
        list = q.get()
        zoom = list[1]
        xCoord = list[0]
        for i in range(2**zoom):
            url = SERVER + str(zoom) + '/' + str(xCoord) + '/' + str(i) + '.png'
            start = time.time()
            status, url = getStatus(url)
            end = time.time()
            if (end-start) <= 0.5:
                if OUTPUT_ALL:
                    print("\033[1;32m" + url + ": " + str(end - start) + "s\033[0;m")
            else:
                print("\033[1;31m" + url + ": " + str(end - start) + "s\033[0;m")
        q.task_done()

def getStatus(ourl):
    try:
        url = urlparse(ourl)
        conn = httplib.HTTPConnection(url.netloc)
        conn.request("HEAD", url.path)
        res = conn.getresponse()
        return res.status, ourl
    except:
        return "error", ourl


global_start = time.time()

q = Queue(CONCURRENT*2)
for i in range(CONCURRENT):
    t=Thread(target=doWork)
    t.daemon=True
    t.start()

try:
    for z in range(18):
        for x in range(2**z):
            q.put([x, z], block=True)
    q.join()
    global_end = time.time()
    print("Totally needed " + str(global_start - global_end) + " Seconds.")
except KeyboardInterrupt:
    sys.exit(1)
