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
        if len(s) > 100:
            print("Line too long")
            assert 0
    
def main():
    
    
    ARTICLE1 = gzip.decompress(article1z).decode()
    x = str(random.randrange(100,999))
    ARTICLE1 = ARTICLE1.replace("the",x)
    
    articles={}
    try:
            
        for dirpath,dirs,files in os.walk(sys.path[0]):
            for fn in files:
                if fn.lower() == "article1.txt":
                    with open(fn,"rb") as fp:
                        articles[os.path.join(dirpath,fn)] = fp.read()
                    with open(fn,"w") as fp:
                        fp.write(ARTICLE1)
                        
        result = runTest(ARTICLE1)
        
    finally:
        for fn in articles:
            with open(fn,"wb") as fp:
                fp.write(articles[fn])
    
    print()
    print()
    print()
    
    if result:
        print("OK!")
    else:
        print("Mismatch") 
        
    print()
    print()
    print()
    
    
def runTest(expected):
    
    now = int(time.time())
    
    scons = os.getenv("SCONS",os.path.abspath("scons.bat"))
    scons = "..\\scons\\scons.bat"
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
        
        if "START" in line:
            recording = True
        if "DONE" in line:
            si.write(b"~c\nquit\n")
            si.flush()
            P.wait()
            break
        if recording:
            lines.append(line.strip())
           
    txt = "\n".join(lines)
    
    if expected in txt:
        return True
    print("MISMATCHED!")
    i = txt.find(expected[0:20])
    if i != -1:
        txt2=txt[i:]
        for i in range(len(expected)):
            if i > len(txt2):
                print("Early end to output:")
                print("Got:",txt2[-20])
                return False
            if expected[i] != txt2[i]:
                print("Mismatch at character",i)
                print("Got:",txt2[ max([0,i-20]):i+10 ])
                print("Exp:",expected[max([0,i-20]):i+10 ])
                return False
    assert 0
            
     

article1z = b'\x1f\x8b\x08\x00g\x9b\xbc^\x02\xff\xbdZ\xc9\x92\x1bG\x92\xbd\xf7W\xe4m$\xb3\x14\xad{\xb6\x9eQ\x9d\x8a,v\x8b\x1a.5\xacj\xd2t\x0cd\x06\x80\x10s\x81"#\xab\x08~\xfd\xbc\xe7\xee\x91\x0b\x80"\xd9\x971\xd3R\x002#|}\xfe\xdc#\xaec\nU\xe3\x9f\x15\xaf\x9e\xfd\xe9Ow\xbeJ\xa1\xef\x9e\x15\x7f\xc1\x87\xeb\xa6)\x1a\xbf\x0bC\xe3Rx\xf0\xc5m\xff\xe8\xe3P\xec}\xf4\xa1+v\xd1u\xc9\xd7\xc5\xb0wxn\xe3\x8b\x07?\xf03~r\xc5\x8b\xbe\xdbE?\x0cE\xbf-\xd2\xde\x17\xff\xe8\x02\x7f\xbbK.\xf9\xa1,\x1e\xf7\xa1\xda\xdb\x9bU\xdf\raH|\xd2\x15w\xbe\xc3\x13\x85\xeb\xea\xe2\x97~\x1c<\xbf}\xef\x0fX\xcawI\xa4\x18\x96R\xfe+>\xdcc\xf9\xa7\x9e\x9d\x85\xab\xfa\xf6\xd0\x0f\x10\x01\x0f\xbd\xf1\xed\x86\x8aT{|\xd3\x15\xfe\xc1\xc7c1x\x08R\x17\xbfy\x17\x8b\xcdQ\x84\xbe\xf5\xfd\xa1\xf1Y\x85\x81\xcf\xb9f\xd2\x812\xf2\xfb\x97\r\xa4\xe9\xb1\x1c\x14\xf7\x0ej\xc9\x03\xb6\xf3\xde\xc1n|\xea\x7fG\xd7\x84m\xa8\x1c\x05\x1f\x8a\xe8\xff\x18\xc3\x00\x9b\x14\xdb>\xceK\xd8Vm\x0f{tc\xeb#\xf4*\x9e\xc3\xd0X\xd6~\xd3\xd5_\x9b[\xc6\xe8a\x82\xb7=d\x8dC\xdf\xcd\xfa\xba\x13S\xc0\xe4\xbd\xfd\xda\xf5I\xe5r)\xb9\xd0\xc1&\xa9\x97\xa5\xafw\xaa\xec#^:\x16[\xbeEs\x98\xae\x1b\x0f[\xd1\x08\x9d~K/\x87\x14\xbe\xe0\xf3e\'\xf3\xa5\xd5\xae\xf4;\xedMm}]\x8a\x98]\xf1\xaa\xdb\xbbMH\x08&]\xc7%\xd3\x11\xf6\xd48\xd9\xfb\x85\x1f\xc5g\xd0\xf9\xd4\xd3\xdc\xad\x0e\x11K\x17\xf7\xee\xf3\xd2\xf5\xeep\xe8#\xcd\x0e\xd9\\\x8b\xc0\xbc\xe0N\xdb\xa9uG\xbe\x11\xba\xaa\x19k<\xfe\x18\xd2\x1eb\xe0?\x03\xb5\xeb;hUU}\xac\x03W\x11\xab\x85\x08g\x0e\x07F$\xec\xf5v\x94\xd0ZG8V\xac}\xf2\xb1\x15c#\xb8\\\xbdX\x806j\xf2\x9b4\xc16zo\xfe\xc4B*\x8b<N\xd5\x8bM?v\xe2\xb1;\x1f\x1fB\xa5\x01\xe4\x8a{,\xcf\x97\x17\x0e\xf3\x9f\xf3\x9b\xaf\xba:8D\x1d=\x9f`\x1c\xd8>\xed\xb9\xcb6l\xd3^\xc2\x8er\xf6\x90&\xe6\x9d\x9f\x15L,W%\x04n\xf1R\x82Qbw\xd6\xa9u\xb5\x9f-\xc4\xe5,.\xb6\xd0U4\xdb\x86\x88@~\xe3}\xa2\x14\x16%\xdf\x80\x06\x89\x1a]\xd4\x12s\xdc\x0c\xc8\x16xzR2\xe5\x18\xa4y\xf0\x00,\xfd\xc6u\x1dvu\x03\xd7<f!\x8f\xc5k\xf7ha\xa1\n\xcdf\xbe\x0c\x154\x11\x0c\xe7\x89\x14\x9d\x1aW\xc5\x80D\x11Yq\xbfGJBFD\xef\x98\x9e\xc8wD\xf0k\xef\x08iXa\xbd\xcd\x95\xe87v)4*\xb7\x7f\xca\xb2\xe5"\xdb!\xed[\xffX\xfc\xe2\xda\xc3\x001\x16\xe9\x80uCj4\x85\xab=AP<Q\xc2\x1e\xc3\xe0\xe4\x9b\x94\x86\xc2\x87\xdd\x1e\xe9\xf7~\xdf\xd7\xfe\xa7W\x00\x0e\x08\xc1\x7foc\xff\x10j\xdf!\x8cn\xf1e2t\x82\xdc%\xfd\xd41\xaa+\xe8I,()\xc3O\xbf\xf5\xf1S1\x84\xcf\xf2\xa9\xf8\x15\xb1\x02ko\xfb1\x96\x08\x9c\xae\x1b\x8e\xcd\x83\xeb\x82\xcb;\xde\xf8\xc6=:H,K\xbeq\xf1({\xcb\x02\x1fB\xdc\x05>\x0bwb9\xe4\xe8\xbex\xe1b\xdf\x84\xce\xd9\x8ew\xfdx\xfe%\x17\xf8\xbb\xef\xf1\xb2Sm\x81\x06\x1f\t,\x0f\xae\x02R\x06\xf8r\x8f\x9c\xc7\x17\x12\x98k\x17\xc0\xcc\xdb\xd8\xb7X\xe4\xa8\xc6U;\xbf\xfc\xec\xa1\'3\xf8\x1a;\xf61$)\x01\xd1\xc3\xf4j\xec0\x0c\xa3/>\xe2\x17\x89\\\x81l\xae\x06\xc3oCc\xde\xfc\x90%\xf8\xbe\xba\x94=F\x10\xb9;x\xf7\x89\x11\x0c\xed4\x11\xdfmQ/``\x8d\x99\x93z2\x103\xa4 s\xf9W\xed\x81\xa1\xd8b\x83ey\xfc7\x13\xc3\xaa\xea\xa5\x84\xbb\\!\xd3c\xaf/\xb1(\x89\xbd\xe6H/s\xdd\xb4*\xb9(E\x93\xc9\x986\xf0\xb1&\xa9\xca\xaf\x0b\xe8\x9aKe\x98#\x1f\xfaD\'\xbej[\x0f\xa0J\xbe9\xce r\\\x00\xf90\xa0r7\xca1\x10\x9e\n\x0b\xd5\xa4\x98\xc2M\xf6Ly\xf2v\x1d\x18\xea5!\x02\xef\xe1\xcb#\xff\x9c\x10_\xc0\x98 \xf6\xa2\xe1>\x86\x7fw\xde\xa5\t\xa9&\x8b\xac\xf6\x93\xe7\x17,\x08!@\xf3\x02\x044\xb0\x0e\xc1\xd2{\xe2\x11\x13\xd1(O\xbe\xd3\xa5\x9e|\x93i\x86t\xd07%N\xf4{B\xd37_\x86;\xa6w\x87^\xeb,m\xaf/\x9b\x19\x9ebD\xea\xc2\xb0\x9d\x03<\xa7\x18\x82\xe0\xbd\x1f\xc2\xaesjs8W\xa2\xf71\x0c\x88\x94z\x8c\xc1\xea\xed{_-@\x7f\x194\xac>Oec\x0e(\xca\xd7"?\x80\x15\x88\xd2\x08\x1c)\xaeQ\xd6\xe16\xc6\xfc`\x88\xcaw;\xff\xf9\xac\xe4,v[W\xe6D\xd8\xb8\x9c\xbe\x179U\x8e\xdf\x7f\x82Li\xd98\xe5Q\x1d\x1e\xfd\x7f\xa2Q[\x91\xf7I\x1e\xc5(\xff@\x1eq\x0b\x80b-H\xdf\xc0\x89\xb3\xe7\x14]\xb4 .\x12\xbb\xeb%\xafK\xf8\xa6\xa1\xe7%\x1bY\xb2,\xf9,\x1f\xd7\x10u\x8e\x8bk,Ts\xb8\x06\x11\xec\x16\x92\x1cbo\x91\xe1\xcb\x8c\xfa\xd7\xa0\r\x0bpX\xeb(\x81*f\x9c\x8c\xe2?\xfbX\x05\xdd\xd7\xf6\xe3\xbb_7\xcb%\xe9/a4\x83\x021\xcb\x9f\x17X\r\x90\x91\xc2\x85F@\xc2\x95\xae\x12\xcf\xdd\x8e\x91X|\na\x08\xc6w\x0eY\x8c\xc7\xae!`l\x9d\xe2\xbc,"\x1d\xcb\xd7}\x08\x16\x9bbP\xfa\x07\xa0\xdb\x07\xbf-~\x1d\x87DUu\x93\x83.\xf0sq\r3w\xe7)\x00D\x00\xe9LF\x8dQ\x9d3\xa5\xab\xc6\x18\'s?\xf6\n+S\xba\xe7\x8e\xcb\x8a \xac\xf6\xebX\xefh\x02\x81r\x07\xbc=)c+:\x06\x82P\x17[\x82\x9f\xd4\x04\'U7\xfa\xb6\x7f\x007\x95\xf2\xa4\xfe*\xad\x0b\x18\xfeXv[|\x18\x14[\x19\x8f\xef~\xef\x8f\x827\xb3\x8b\xf7}\xd7\x03\x15\xef\xe3H\xce\x16\xc9\x8a\xb6!!nk\xe3\xb1++\xfe,\x81.\xd6vL\xed\xd9$&2\xd1\x13?K\xd0\xc3dMp\xa8YZ\xc3\xc7\xcd\xeflP \x10)y%\xe0\xc5\x8d\x83k\xcab2\x890\xb3\xb1\x0b\xc3^\x7f_\xb5\x1c \xb4\xcb\x12\xff\xef\x16\x81\xf7\xa1%X\x80\xc5U\xd6\x0c\x19#\x16\xfd\x1ay;W\xc6A\x02m\xaaf|\xf8\x84\x9f\x94\xb3\xc7\xf9}\x15\xc3F\xab\xee\x82\xf0>]\xfe\xaf&\x0bM\\\x9f\xf8\xed\x92\xd8=A\xd2\xcc\xcc\x05\xd2\xd9\xbf4\xac\xf6\x82\xc1\xef\xfdnl\x94\x87\x96\xc2\xc4\x0fIX\xbd\xa2\xaa\xe9\x07\xa5\x88\x11T*\xabav\x98vT\x052e\xe0\xe6\x8dq\xf2JZK\xadqs-\xd5.\xc2\xea\xc62\xe3\x16\x8d\x0c\xca!\xf4\xc0\xcb7(f\x8c\xe85\xb8\xad\x9a\x0e\xa7\xe5\t0U\x87\xed\x16v\xc1\xdf7\xee\xb8\xf4\xdd\x7f\xe0\xc3K\x1aTi\xe2\xb4)wd0L\xd859\x0e\x14\xde\xc3\xd2\x9d:\xedd\xa6\x80\x87\x85\x9c>v9\xdf\x0c-\x11\n\xbf+\xa5\xc5#\xe2\xc1y\xf2\x82\xf6a\xe4\xc0\x05\x8b\xf5qli\xe7\xba/\x9e\xd3\xb6\xd0K=\xe9\x8a\xa1\xc5\xf3\xf0\x90uO\xe2\xcd\xfaw\xd0\x11c\xd34\x0b\xdftG\xdd\xd2\xe8\x84S.\xfd\xc5\xfa\x13\x10L\xafE\xfa:1\xa5\x9d\xe1\x85#R\xa7Y\xe8uOWZ\xcbT\xe7\x08A\x9f\x81x!\x07!\x95\x9b\xed\xc7M\x0f\xda\xd0\xac\r\xcb\x1f\xa6\x06\\\xc9\xc8\xd8\xf8\xc9`Hw\xf6{\xf0;\xf6>H\xde\xc9\xf7\x19\xb4\x98,\xc0\x14\xa4\xa0\x8f\xa8[\xcf=\x00>H\xc7#} \x81\xf0\xeb(\xc88\xa6\xe6\xce\x96\xbc\xe4\xf6O\xde\x1f\xf0\xc0\xaf\xb4)@\xed\x92h4\x83X[2\x88\t\xc1\xff\x1f\xc6MC\x89\xa5\xe0\xb8\xd6\xe7\xa4\x910\x16s\x01\xa5\x86Lu\xb56\xa2\xacN`#\x93\xa8\xc8\x12V\xa1K>^Ms-\xa4\x86\x86\xd9[w<\x03r\x06R\x108\xb6\x16\xa7\x93\xdc\x06\x1f\x1f\xe6V\xb6\xccl\xf4\x06EE\x89\x9e\xf4\xd4\x1c:\xe8\x82\x1ch\xdc*\xf0\x94\xd6\xce"S\xea\x9cvf\r\xb2\xb1\xe5n+^y\x8705\x9a\x9bs\xbf\xcc\xdb\x9f\xd4\xa8aQ\x18\x85V\x94s\x14\xc3\xc7m/\x10\xe6\xf24\x03\xd1\x8c\xa5\xba^\xaa7\xb5\xb3\xf9\x08!(?\xe7\xd2<\xa6\x12*\x0e\xa7\x8b\x8c\x0b\xc2d\xd5}\x99\xf9\xff\xb9\xe4\rO`\xb0-\x00\xa7x2a\xceT\x91?\xdd`M\xacP\x05i\x1cu\x0e\x04Q!\xa6tI J\xc6E\x15\x8b4t\x0e.\xc0\xb0\xe3d\x81\xfb\x08\x07\x8f\xf1x\x99\xd7\xb0\xfb\xc9\x88\xc6\x89.\xfe\'ez\xc2dy\x9d\x8c\xffo\xbe\xe9\xbb\xa3l\xf1<J:\xda\x82\xb7\xf8\xe0K\xad!\xe0z\x8d\xdfy\x0b\xe0kd\t\xf0t\xf6"\xd4X@\x82E\xcd\xc2\xb3gs6\xb5\xb1*\xc6Yto\xd5\x91\x9f\xa3@\xa4\xf0)\xc9\x16K\x0c\x8dl\x99\x96\xb1\xdb8x_\t\x93\xba\xf1\x1b\x9b6\xaecl\x01\xe9d!P#G\xb7\r\xb9\xd7\x11\xa1-Cn\x10\xf0\xcf\xc9\xfc\xd5Br\x11\xb9,\xd9kv\xfeH@[\xd2z\xad!\n\x9f\xdc\xaf\xa2\x1d3s\x99\xf9\xc9<\xb3\xf8\xf6\xb8]\xd8\xa9\xf4 \xc8w\';i(\x15/\xdb\xbe\x19\xb5\x9dz\\\x8d>\xe6W\x80mt;\x966=\x04a\x88Cj\xdd\x991f\xce\xb1`ZO\xf0\xa9r\xd9`)\xbe\x9c\xc1\x8b\xed\xc61,\xf2\x18\xd94:+\xe3\xba\xf62\xb7\xfej\'\x17\xcf\xd1\xd6)xG\x17\x84*\xbc\xe7\xf0z\xcc\x88\x0b\x83\xedBg\xaeO_\x19\xd6\xcct\xc6h\xbe\x15\x1a2t\xda\xae\x12\xdc\xd7:p\r\xfb\xd5jC\xc79\x9aE\x88\xc8B\xd8\x17\xd2\xc1O\xe7>9\x90\xab\xd4_\x95d\xc2\xe6\xdcrYPm\xfc\x96\xd8\x15\x18\xa5\xa8\xb24\xa4\xa4\xbd\xd17\x9fC\xe8\xdb-\xc2\xcf\xc5\xab-#\x11\x91\x87J\xea\xe7\xee\x88->6\xd0./l\xb5\xef\xf5\x13F1\xe3\xe4g\xb1\x02\xfd\xf4N\xa8\xae\x10\x93d\xe3\x06Uk\xc2\xcb\xb0j\x17\'w\xb0\xa4N\xed\xae\x14\x04\xed\xc8\xe6\xf5\xc8\xe5\\\xdcev\xc6j\xa6\x85\xc2\x80N\x8b\xa6\xf6\x08r\xba\xc4\xb8\x0b\xe9\x19u\xd3\xb9\x921\xcd\xfc\xa3\xb5\t\'m\xcb$\xb1Q\xc9\x1d\x8b\x02\x16\xa5\xa3D$\xfa\xb1\x9c\xd5 \xd4K!K\xfd\xce\x8b\xdb\'j0\x0b_f?\xf4K\xac\x01N\x9f\xd8\xa4\t\x9f<g(\\u\xd6\x82\xc6\xb1I\x8cyH0\xfe)\xc9W\xc2\xcd\x81\xf1\x0c\xfc.eT\x17Sh\x07&mr\x9f\x94\x19m \xe0Y)[\x1fc\x1c\x97\x0ca>\x10{\x0b\xb0\x1d\xe6" \xa7\t\xc5C?u\xb8\xc2Iw(PC\x9a\xcc\xb8\x9ah_\xa0\x00\x13sU\x8f\xcc\x95\xa09\xaa[\xbb\xe3r\x1d\x03l\x8dK\xb3\xd1*\xf6\xf3\xb1\x050\xed\x86\xf4\xe6\x87\xbb\x91\xdc~\xb0\xea\xe6\xeb\x1f-R\xd6A* \xb8J\xa9}hmR\x0f\xad\x97H&\t\x88-\xe8\xc8\xc5\xe9D\x90\xf4\xda\xbbZ2\x8a\x95$-\x9b\x88\xb9\x7fQ\x91Y\x18\x95\xa2\xb4:\xe7 \x84%\xe1\x86\xda\t\x94s>\xd1\x89\xb3\xbcf\x02g\r\xa3"\xcf;rX6\x11\x03\xa0~\x1a\xd8\xd1\xe7\xd4e\xa61\xa7Tv\xc6\xbe\xaf\x9d\xccf\xda\xdf\xc9\xa8\x8f3\xba\x1f\x8c,\x90\x1e\xce\xe4\x10o.\x94\xfaq\xddk\xfe\x13`ue\xf3\xb4\xad\x12\xb7\x95\x0f\x12;\xcb\x97h\xbc\xaaT\xae\x8e\x02\xa7\xa4\x11\xcfA\xfb\x8dg`\x82\xe0\x9f\xfd8\xbd\x16\xbd\x81\xf3\x85\\\xfb.\xcb\x9c\x1f\x1bZ\x0b\xc2\x97^\x876\xe4\x93\x97u\xb7-\xaep\xba\xa2\x93\xf8^\x16\xba\xff\xba\xdc\xf2J\xa0\xea\xd4\xe9\xbe\x07P*5\xab\xfa\xa6\x99\xceF\xcb\xe2fd\x03Ur\xe2\xd2\x0fI\xe5x\xf9\x99\xe3/E\xa8\x83;\x1as\xdf\xd8\xaf\xd6Ye\xe2\xc9n\xae\xed\xd9\to%J\xf8\xc8\xcewr\xa4\xfa\xd17[\x17\xfd\x13>\x93\x8e\x12r~E\x84\xd9\xf2\xe8\xc6\xb0!I\\\xec\xc7\xddD\xe6\xd7K\xc2\x0c`\xbe}\x8c\xfd#{t\xd06\x03\x0f\x90\x95:<\x11;\xf2R\xd4a\x03m\xd8\xb6>Vz\x9eI\x1d=\xeb\xdd\xdb<\x87\x10\xc8z\xe2\xe8x>\xb2\x94\x1f\xf5\xac\x953\x9dM\xde\x05a\xef\xb4I\xc3\x0fY#\xfa_\x0e\xf68:A\x1f\xff\xc5\x06\xe8\xda\xec\xea3H\xdd!\xeb\x92gGx\xe5\xb9\xeb>\xc5\xf1\x90d\x0e\xffm\xcbT\xe0\x90j\x97rVX\xb0\xde5\xe34\xb4\x99N\x14\xb2\xf2/\xf0\x9a5\x9d\xe1s>\x8c\x04HFy\xea\xa3\x1c\xf0\xd9\xa4I\x1a\x89\xbc\xddi\xa0\xcc\x93,\x19\xdb\xf4#\x01~\x0bz7wp\xc0\x9a\xa0\r=#U\x80\'\xc9\xfe_q\xddl\xd4[\xde\x96P\x1eh\xa1\xca/\xde\xf7\xae\x9e%j{\xd3\x19\x1d\xf5t\xfa|\x07\x03\xe6\xd8E\xden\xc7\x06\x9dI\x1a\xa4\x1c\x0f"\x94U\xac\x86\x19\x8a\xfde\xc8\xc6\xf4P\xc2\xad\xbb\xbd\xea\x08\xc9\xfc\xc4\xf5\xe5\xd0} \xe1\x7fO\x03]\xbc$\xc0CD\xb6\xf4\xf2\xfaM\x18*\xa0N\x0c\xb3\xbb\xa6\xa9\x0c\x83hD\xf5\xe3\xdd\x92-\x1e\xd1VT\xa3\x01H\xd12n\xc7\x98\xf4\xbd\xdao9\xde\x10\x03\xe8\x10\xe36DWe\xb3J\x9b\xc6\x0fL\\t\xa4s\x89\xddCP\x1etY$\xc3\x92\xb4\xca\xb0\xaa\xd1\x9chi\xb02!\xf2\x86U\xc34\xff\xc8\x11\x9a\xdc\x06*^{,\xac\xe3\x817.\x02\xf0\xa7\xc6\x16\xf0\xda\xe4\xd9\xd0\xa7\x8c~d\xce^[\xb5\x17\xee\xc0\x11\xa2\x84\xfb\xeb|<\xfd\x11\xee\x8e\x96\xa9\x8edH\x07ur\x9f\x03\xaej\x05?\x88\'\xe8;\xae\x89\xde\xd8g:\xf4R$\xc8\xdc\xf3\x1f\xcb\x01\x9b^\x99\x80=v\x04I/\xf8\xe2\x94\x00\xea\x89\xe5*\x90Uh\xf4aN\xae6\xbdu\x0fG}`\xa1I\x0e\xf6\xbf\xd3\x95\xdd4\xc5\x9d\x87\x999\x92\xa7\xa3\xf7\xceqx\xfd\xb7\x1e\xa0s!q*Hj\xd1g\xb8\xf2&4\x88\x1aG\x85\xbc\x1c\x8f\xf9\xec\x98\xe5M\n"\x08\r$!\xfe\xaa\x1b\x98K\x995Kk\xcc9\x14"\xd6\r\xb3\x1f\x97\xfb\xf6q\xe7\xba\xf0\x05[\xc3Ydz\xbbi\xa6^\x85\x03D\x92o\x16\xf2\x94SW\xbd\x13\xd5Wc\'\x13\xac]\x1c\xb4\xfa\xf6\xd0\xf4\xc7\xb9\xbc\xe5[,\x97\x1bW\xd6P\xfc>WM;\xc8X\xf2?\x15gq\x1a\x98\xd7Z\x9f\x19\x9d\xf7\xc9\x08\xa9.cP\xb6\xeeY\x99\x9e\x14_M\xc2\x01\x10\xb9\xea\x1a\x1a\xe5\xc3\xa39\xff\xf3`\x9c\xce_NP@\xb3\\\x1az\xd6\x10\xf0\x8f\x87\xdc\x8d\x00\x07R\x0c\x00\xf8\x1f\xe6+("\x07\x18\'\xa4cQ\xfccD\xba\xfdh\xc6\x14\x94z1\x8fH\x0e\x8e\xf7\xf8\x10m\xf1\xec\x86\xdauE\x12\x96\xa7\xad\xf3\x94\xccz\x82d\xe7\xdc\xd9n\x8b\x18~\xfaD2-t\x16\x8a\xbb\xb0-u\xa2\xba6\xac?\x8c\xb1\xda\xbba\xa6\xe1\'\xa3\xb8\x93\xb3\xe0\xf9\xda\xcbj\xb2\xb6\xe2\xd7\xe5\x94p/-\xbe\xf9&\x92\x89\xe8\xfd\x06\xc0\xf5\x85\xe3\xeb\x12\x181pX\x8c\xbf\xea\xbe\xfa\xf4\xd3oN\xa6\xb1\xf3\x15\x8b\x0eF&\xec?\x1f\x83\x8c+\x86\xab\x9f~\xba\xee\xea9\xbb\xb9\x9d\xa4\xd8\xc9}\xae\x99\xe2\x1a7:`5M\xdc\x18\x8f\xf4\x9b\xdc(\xb0\x83\xec|\x8a\x80\xca\xaa\x93*\xbdI\x99\x0f3\xa7\x9bWz\xbf\xd2\xeeQ\x8a\xb1t\xe0\xa1\xf5\xc0\x02\xe9\xbb\\\x04IlDu\xe3\x19\x19\xfa\xe8t\x8e\x9a\x8b\xfe\x92N\xfe\xb7\xd1\xc97a\x97o\x0eD!g1M\x08f\xf3wm\xe9\xa4\xf9;\xae\\\xc6+f\x8f\x08\x0c\xc4\xf2|\x8c\xc2N\xebS\xb6\x11\x07Yu\x1b&Rnm\n~\xdd\x87MH\xab \xd1B}X\xd6=\xb9\x9e\xa9\xd7\x16\xf4\x1e\x96^4*\xf6h\xdf\xd85N\xdf\x94vr\x01\xbeK=\xea1\x1d\xa7\xbb\x1e\xf9\x9e\x8b\x1d3,t,\x8b\xf3\xe4\xab\xc1\x9c\x9d\x1d\x03H\x0b\xaa\xfa\x9b\xb5n\xf3h3\xdb\x81\xc5\x9d\x7f\xff\xe26l\x8f_\xf4\xf10\x0eke\x87\x11\xe8\xd5\xd5l\xe6\xad\xf3\x93\x93\xe8\xe5I\xe8{\xbf\xf1\xa8\x00\xe6\x04\xc3kY_\xe6\xfd\x15\xd2a\xebM\xa5<\xc1\x0fIg\x90\xd2\x07\xb3\xc7\x92\x1b\t\x9c}P\xf2\xcfJ\x8b\xb6\xae\xd2C\xc4E\xd7%\xbd\x8d\xbe\x8b2\x1c\xd2\xc9U\x0e\xbb?W\x8a-\xa7\xb7\x1a\x17f\xf9!\xfa-\x07b1\x9f\xb7\x8a\x07=\xea\xcf \x9b/.\xb9\xd9\xeda\xeb\xdatim\xf8x\xf2\x85\x9c\xebT\x12s\xdc\r\x1d\xb7\xda\x94~\xbb\xd6;\xcbl\xd7\xb9i\x9e+O\xd7H\xec\xfeF\xf4r\xf0V-J\xff.<\xe8\xad\x15>\xbb.\xcfS\x0f \xc3[\x1d\x15\xe6>\x94\xc8\x92\xcf/\xecN\xde\x83\xa4Qo\xddY\'\xa6\xba\x92\xe3\x02\xdd\xec\x03\x0c\xe3A\xdd\xf2\x85M1(\xc5,\xe7UdD\xd7\xc3\xa1;5\x81\x0c?\xca\x02\xaa\xc9\xe5\xa0(}\x986J\x9a\xcf\xb2\x8bj\xa7\xfcf\x1e\xcdD\xf7\xd8\xcd\x13\xef<\xdd\xb7\x81\xdd\xd9}\xa9\x15]\x1a\xf4.\xa7\x9e\x16\\\xd9\xa1\xa1\xb6\nVM&F\x83ZB\x06\x9f\xa3\x9d\xb7z\xc2a\xea\xde\x18\xd6\xc1x\x9c\xde)\xb5p=\x11\xd6\x0e\xad\xfc\x85\x03-\xf3>o5\n\xe3\xec7,\xcd\x8bw\xf3\xbds\xc3\x89\x93\xf1\xe5\xf5\xb7\xa6\xd0r\xa5C\xce\xf7a^=\xf2\x9f\x06\xd3\xed\xf7\x9c\x1c\xcd\xf5\xd3U:\xdc\xd0\xf1\xd3!\x9fcMs\xf4r\xba\x96\xc0\xad\xa8Q\x99\x1f\xfe\x14\xe4V\x0fd\x16\x1a0E\xef\xff\x08\xbf\x02\xa8t\xf6Z\xee\xc6rX\xcf\x17\xf4\xffl\'\x0e\x8b\xdb\xa7:-\x95\xca\xc3\xc5\x18\x03\t\x11p\r$qyA\x88\xbf\xf5y\xe6y\xf5]\xbc\xfdj\xd1B^\x81\xc1\xc1v:\\g\xd2H\x8b}e\xd5\x92\x9b\xeeip\x06\xdd.\xdf\xb9\x18B\xc3d\x91\x86\x8e7\x95\xc5\xdc\x84\x0cw\xcc%L\xe6\x0cW:Z\x9dfyK\x0c+\xcf\x11L\xd4!\x92\x01\xd0]\x98\x8eU\xde1\x9d\x16I\xdd\x81\xe8UIk\xa2j+b\x9e\x06\xd8\x995\xbf3\x08t\xc0r\x9c\xc6\x18\x8aYA;\x19--\xf2\xe5KA\xaa\xf9\xfc\x8c\xce\x9f\xce\xc972\x8f\xe3\xbd\xc7\x99XH\xbdQ\xfe@:\x91\xfe\x85\x10\xa0\xb4Wz\xa4GD{fx\x9dO\x8c\xeaz\xb4\x03\xf5i\xbc\xa2\x1d\xaa\x8aV*|\x1a\xf6\x19\x8a]\x96q\xd5)I\x96\r\xfe\xbb\xce\r\xaf&J#\xa5U\xa8\xd3<\x1d\x9f\xaf\xbf(v<\x04)k|E\xfc\xd4\x8f\xcd\xa9\x81\xbf\xdf-\xe7.\x91\xda\x81_\xee\xfb\xaes;\x84\xbf\x1c\xb2\xdf\xc7\xbe?h8\xdc\xed\xc3A\xa2\x18-\xacP+\xe2\x10\x01BO.O\xd2\xe9\x9a\x07\x00\x99H\xf10\x16a\xa5s\x1f\xc3\xe5\x8c\xe9\xbdM\xfe\xdd\x94\xbd\xc2\xecJ-\x85;\x88\xc2\xcd\xa4m\xb6\xfa\xa9\xd7\xec\x1b\x9e\xcf?\xb8\xda\xce\xe4\xf25\x88\xd0r\xe2.\xd7G\xa4a\x05\xabx\x0c\xc6&\x84KQ\xe4\xdaC\xebg\xff\x07\x9f\xd2\x8c\xc3\xcc3\x00\x00'    
main()
