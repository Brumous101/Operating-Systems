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

Q = queue.Queue()

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
        
            
def main():
    now = int(time.time())
    
    scons = os.getenv("SCONS",os.path.abspath("scons.bat"))
    P = subprocess.Popen( [scons],
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.STDOUT)
        
    si = P.stdin
    thr = threading.Thread(target=threadFunc, args=(P.stdout,))
    thr.start()

    start = time.time()
    
    while True:
        try:
            line = Q.get(block=True,timeout=TIMEOUT).strip()
        except queue.Empty:
            print("Timeout reached waiting for output!",file=sys.stderr)
            return
    
        now = time.time()
        if now - start > TIMEOUT:
            print("Timeout reached!",file=sys.stderr)
            return
            
        if line == "DONE":
            
            tf = tempfile.NamedTemporaryFile(delete=False)
            try:
                tf.close()
                print("Using temporary file",tf.name)
                si.write(b"~c\n")
                si.write("screendump {}\n".format( tf.name ).encode() )
                si.write(b"quit\n")
                si.flush()
                P.wait(timeout=TIMEOUT)
                with open(tf.name,"rb") as fp:
                    read(fp,None)
                    w = read(fp,None)
                    h = read(fp,None)
                    if w != b"800":
                        print("Width mismatch; expected 800; got '{}'".format(w))
                        return
                    if h != b"600":
                        print("Height mismatch; expected 600; got '{}'".format(h))
                        return
                    read(fp,b'\n')
                    c1 = fp.read(3)
                    c2 = fp.read(3)
                    evenRow = (c1+c2) * 400
                    oddRow = (c2+c1) * 400
                    fp.seek(-6,1)
                    if c1 == c2:
                        print("First two pixels are the same!")
                        return
                    for y in range(600):
                        row = fp.read(800*3)
                        if ( (y%2) == 0 and row != evenRow ) or ( (y%2) != 0 and row != oddRow ):
                            print("Not a proper checkerboard on row",y)
                            return
                print("OK!")
                return
            finally:
                os.unlink(tf.name)
        
main()

    
    
