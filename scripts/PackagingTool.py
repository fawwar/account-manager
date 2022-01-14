#!/usr/bin/env python3
import os
import sys
import argparse
import subprocess
import shlex
import shutil
import zipfile
from pathlib import Path
from datetime import datetime
import re
import xml.etree.ElementTree as ET
import numpy as np

PROJECT=''
VERSION=''
SERVICE=''

scriptPath = Path(__file__)
if not scriptPath.is_absolute():
    scriptPath = Path(os.getcwd()).joinpath(scriptPath)
rootPath = scriptPath.parent.parent.joinpath(SERVICE)

def run(command, cb= sys.stdout.buffer.write):
    print ('run '+ command)
    process = subprocess.Popen(command, stdout= subprocess.PIPE, shell=True)
    while True:
        output = process.stdout.readline()
        if output == b'' and process.poll() is not None:
            break
        if output:
            cb(output)
    rc = process.poll()
    if rc != 0:
        raise SystemExit(rc)

def packaging():
    
    if os.name == 'nt':
        print('win-x86_64')
        run('tools\\py\\python.exe .\\%s\\scripts\\build.py'%SERVICE)
        os.chdir(rootPath)
        print('Test build')
        projPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
        winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\ci\\Packaging\\'+ SERVICE +'" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
           
        if (os.path.isfile('X:\\')):
                print('X:\\ file exist')
                run('net use "X:" /delete /y')
        run(winCMD)
        if not (os.path.isdir(projPath)):
            os.makedirs(projPath, mode=0o755, exist_ok=True)
        shutil.copy2(rootPath.joinpath(SERVICE, SERVICE+'.zip'), projPath)
        print('copy file ', projPath)
        run('net use "X:" /delete /y')

    else:
        print('linux-x86_64')
        run ('python3 %s/scripts/build.py'%SERVICE)
        smbtmpPath = os.path.join(rootPath, 'smbtmp')
        os.makedirs(smbtmpPath, mode=0o755, exist_ok=True)
        os.chdir(rootPath)
        print ('Test build')
        run('mount -t cifs //$SMB_URL/IOT-Release/ci/Packaging/'+ SERVICE +' smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
        projPath = os.path.join(smbtmpPath, 'linux-x86_64')

        os.makedirs(projPath, mode=0o755, exist_ok=True)
        print('rootPath ',rootPath)
        print('copy file ',projPath)
        shutil.copy(rootPath.joinpath(SERVICE, SERVICE+'.tar.gz'),projPath)
        run('umount smbtmp')
        print('remove smbtmpPath')
        shutil.rmtree(smbtmpPath)

def main(argv):
    global SERVICE
    BRANCH = ''

    if 'BRANCH' in os.environ:
        BRANCH = os.environ['BRANCH']
    else:
        BRANCH = 'master'
    print ('    BRANCH: ', BRANCH)

    if 'SERVICE' in os.environ:    
        SERVICE = os.environ['SERVICE']
        print ('    SERVICE: ', SERVICE)
    else :
        print ('SERVICE_NAME Not Found')
        raise SystemExit()
        
    status, output = subprocess.getstatusoutput('git clone -b '+ BRANCH +' https://gitlab-ci-token:${CI_JOB_TOKEN}@git.gorilla-technology.com/vird01/'+ SERVICE +'.git')
    if status == 0:
        print ('status', status)
        print ('output', output)
    else:
        print ('Clone %s %s Error' %(SERVICE ,BRANCH))
        raise SystemExit(status)

    # check build.py exist 
    #run ('python3 %s/scripts/build.py'%SERVICE)
    #run ('python3 build.py')

    packaging()

if __name__ == "__main__":
    main(sys.argv[1:])
