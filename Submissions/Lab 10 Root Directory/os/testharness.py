#!/usr/bin/env python3

TIMEOUT=100

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
    
    names=[]
    
    with open("testsuite.py","w") as fp:
        print("def copy(fool):",file=fp)
        print('    fool.go(["hd.img",',file=fp)
        
        toDelete = random.randrange(1,7)

        for i,n in enumerate([7,1,2,3,4,5,6]):
            fname = "article{}.txt".format(n)
            xfname = fname[:i] + fname[i].upper()+fname[i+1:]
            print('"cp","{}","{}",'.format(fname,xfname), file=fp)
            if n != toDelete:
                names.append( ("ARTICLE{}.TXT".format(n),xfname) )
        print('"cp","const.txt","const.txt",', file=fp)
        names.append( ("CONST.TXT","const.txt") )
        print('"cp","article2.txt","my fancy filename.txt",',file=fp)
        names.append( ("MYFANC~1.TXT","my fancy filename.txt" ) )
        print('"cp","article3.txt","this is a really long filename.c",',file=fp)
        names.append( ("THISIS~1.C","this is a really long filename.c") )
        print('"cp","billofrights.txt","BILL.TXT",',file=fp)
        names.append( ("BILL.TXT","BILL.TXT") )
        
        now=time.time()
        t=now-int(now)
        t *= 1000
        t = int(t)
        t = "%03d" % t
        n = "A"+t+".TXT"
        print('"cp","billofrights.txt","{}",'.format(n),file=fp )
        names.append( (n,n) )
        
        
        name2L = "a" + t + "c def ghij klmn opq.txt"
        name2S = "A" + t + "CD~1.TXT"
        print('"cp","billofrights.txt","{}",'.format(name2L),file=fp )
        names.append( ( name2S, name2L ) )
        
        print('"rm","article{}.txt"'.format(toDelete),file=fp )
         
        print("])",file=fp)
        
         
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
        if line == "START":
            recording=True
        elif line == "DONE":
            si.write(b"~c\nquit\n")
            si.flush()
            P.wait()
            break
        elif recording:
            lines.append(line)
            
    expected=[]
    for s,l in names:
        expected.append( Expectation( s,l ) )
        
    
    gotTimes = True;
    timerex = re.compile(r"(\d{4})-(\d{2})-(\d{2}) (\d{1,2}):(\d{2}):(\d{2})");

    expectedDate = os.getenv("FOOL_NOW",None)
    if expectedDate == None:
        expectedDate = now
    else:
        expectedDate = time.mktime(time.strptime(expectedDate,"%Y-%m-%d %H:%M:%S"))
    for line in lines:
        ok=False
        
        for exp in expected:
            shortname = exp.shortname
            longname = exp.longname
            i = line.find(shortname)
            if i != -1:
                if exp.shortok:
                    print("Duplicate short name: " + shortname)
                    return
                j = i+len(shortname)
                if j < len(line) and line[j] not in " \t\n\r":
                    pass
                else:
                    exp.shortok = True
                    ok = True
            
            i = line.find(longname)
            if i != -1: 
                if exp.longok:
                    print("Duplicate long name: " + longname)
                    return
                j = i+len(longname)
                if j < len(line) and line[j] not in " \t\n\r":
                    pass
                else:
                    exp.longok = True
                    ok = True
        if ok:
            if gotTimes:
                M = timerex.search(line)
                if M:
                    year = int(M.group(1))
                    month = int(M.group(2))
                    day = int(M.group(3))
                    hour = int(M.group(4))
                    minute = int(M.group(5))
                    second = int(M.group(6))
                    T = time.mktime((
                        year, 
                        month,
                        day,
                        hour,
                        minute,
                        second,
                        0,
                        0,
                        -1
                    ))
                    timeDiff = abs( expectedDate - T )
                    if(timeDiff > 4):
                        print("Bad time on line " + line)
                        gotTimes = False
                else:
                    print("No creation time on line " + line)
                    gotTimes = False
        else:
            print("Extraneous line:", line)
            return



    longNamesOK=True
    for exp in expected:
        if exp.longok == False:
            longNamesOK = False;
            print("No bonus: Missing long name "+exp.longname)
            #break

    #if the long names are OK, we don't care about the short ones.
    if not longNamesOK:
        for exp in expected:
            if exp.shortok == False:
                print("Missing short name " + exp.shortname)
                break

    print()
    print()
    print()
    
    score = 100
    print(    "OK                            +100%")
    
    if(longNamesOK):
        print("Long name bonus:              + 75%")
        score += 75
    else:
        print("No long name bonus:           +  0%")

    if(gotTimes) :
        print("Creation time bonus:          + 25%")
        score += 25
    else:
        print("No creation time bonus:       +  0%")

    print("Total score:                  ",str(score)+"%")

    if expectedDate == now:
        print("Note: Used current date/time for checks")
    else:
        print("Note: Using forged date/time")    
      
    
main()
