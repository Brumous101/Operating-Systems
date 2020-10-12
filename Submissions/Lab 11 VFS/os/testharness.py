#!/usr/bin/env python3

TIMEOUT=10

import time
import sys
import subprocess
import queue
import threading
import os
import os.path
import tempfile
import gzip
import io
import hashlib
import random
import re

Q = queue.Queue()



class Expectation:
    def __init__(self,a,b):
        self.shortname=a
        self.longname=b
        self.shortok=False
        self.longok=False


keeplooping=True
def threadFunc(so):
    while keeplooping:
        ln = ""
        while True:
            b = so.read(1)
            if len(b) == 0:
                return
            elif b == b"\n":
                break
            else:
                ln += b.decode(errors="replace")
        print(ln)
        Q.put(ln)
   
def read(fp,delim=None):
    s=b""
    while True:
        c = fp.read(1)
        if c == None:
            return s
        if (delim == None and c.isspace()) or c == delim:
            return s
        s += c
        if len(s) > 100:
            print("Line too long")
            assert 0
     

     

def main():
        
    points=0

    summary=io.StringIO()
    
    suites={}
    for dirpath, dirs,files in os.walk("."):
        for fn in files:
            if fn.lower() == "testsuite.c":
                p = os.path.join(dirpath,fn)
                with open(p,"rb") as fp:
                    data = fp.read()
                    H = hashlib.sha1()
                    H.update(data.replace(b"\r",b""))
                    if "13850e13d9a3448ed81910b72c3babc496dddbc6" == H.hexdigest():
                        suites[p] = data
    
    if len(suites) == 0:
        print("Could not find unmodified testsuite.c")
        return
        
    try:
        expected= "All OK %d" % random.randrange(0,1000)
        print("Expecting:",expected)
        
        for p in suites:
            with open(p,"wb") as fp:
                fp.write( suites[p].replace(b"All OK",expected.encode()))
        if doTheTest(expected):
            print("\n\n\nOK\n\n\n")
        else:
            print("\n\n\nNot OK\n\n\n")
    finally:
        for p in suites:
            with open(p,"wb") as fp:
                fp.write(suites[p])
    
def doTheTest(expected):
    
    now = int(time.time())
    
    scons = os.getenv("SCONS",os.path.abspath("scons.bat"))
    scons = "..\\scons\\scons.bat"
    subprocess.check_call( [scons,"-c"] )
    P = subprocess.Popen( [scons],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)
        
    si = P.stdin
    thr = threading.Thread(target=threadFunc, args=(P.stdout,))
    thr.start()

    start = time.time()
    lines=[]
    recording=False
    
    while True:
        try:
            line = Q.get(block=True,timeout=TIMEOUT).strip()
        except queue.Empty:
            print("Timeout reached waiting for output!",file=sys.stderr)
            P.terminate()
            return
    
        now = time.time()
        if now - start > TIMEOUT:
            print("Timeout reached!",file=sys.stderr)
            return
            
        line = line.strip()
        if expected in line:
            si.write(b"~c\nquit\n")
            si.flush()
            P.wait()
            return True
    
main()
