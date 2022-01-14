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
rootPath = scriptPath.parent.parent.parent.joinpath(SERVICE)

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
    
    #run('git clone --branch master https://gitlab-ci-token:${CI_JOB_TOKEN}@git.gorilla-technology.com/vird01/'+ SERVICE +'.git')
    
    status, output = subprocess.getstatusoutput('git clone --branch master https://gitlab-ci-token:${CI_JOB_TOKEN}@git.gorilla-technology.com/vird01/'+ SERVICE +'.git')
    if status == 0:
        print ('status', status)
        print ('output', output)
    else:
        print ('Clone %s %s Error' %(SERVICE ,BRANCH))
        raise SystemExit()
    run ('python3 %s/scripts/build.py'%SERVICE)
    #run ('python3 build.py')
    # packaging function

    #getProject(sys.argv)
    #packaging()

if __name__ == "__main__":
    main(sys.argv[1:])
