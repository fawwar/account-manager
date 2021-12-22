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

PROJECT=""
VERSION=""

scriptPath = Path(__file__)
if not scriptPath.is_absolute():
    scriptPath = Path(os.getcwd()).joinpath(scriptPath)
rootPath = scriptPath.parent.parent

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



def mkdir():
    if os.name == 'nt':
        print('win-x86_64')
        os.chdir(rootPath)
        if os.getenv('CI_COMMIT_TAG'):
            print('Release build')
            regExpr(os.environ['CI_COMMIT_TAG'])
            projPath = os.path.join('X:\\', VERSION, PROJECT, 'win-x86_64')
            winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\account-manager" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
        else:
            print('Test build')
            projPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
            winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\ci\\account-manager" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
        
        if (os.path.isfile('X:\\')):
                print('X:\\ file exist')
                os.system('net use "X:" /delete /y')
        os.system(winCMD)
        if not (os.path.exists(projPath)):
            os.makedirs(projPath, mode=0o755, exist_ok=True)
        shutil.copy2(rootPath.joinpath('account-manager.zip'), projPath)
        print('copy file ', projPath)
        os.system('net use "X:" /delete /y')
        print('net use "X:" /delete /y')
        
    else:
        print('linux-x86_64')
        smbtmpPath = os.path.join(rootPath, 'smbtmp')
        os.makedirs(smbtmpPath, mode=0o755, exist_ok=True)
        #os.system('umount smbtmp')
        os.chdir(rootPath)
        if os.getenv('CI_COMMIT_TAG'):
            print ('Release build')
            regExpr(os.environ['CI_COMMIT_TAG'])
            os.system('mount -t cifs //$SMB_URL/IOT-Release/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            projPath = os.path.join(smbtmpPath, VERSION, PROJECT, 'linux-x86_64')
            #os.makedirs(projPath, mode=0o755, exist_ok=True)
        else:
            print ('Test build')
            os.system('mount -t cifs //$SMB_URL/IOT-Release/ci/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            projPath = os.path.join(smbtmpPath, PROJECT, 'linux-x86_64')
            #os.makedirs(projPath, mode=0o755, exist_ok=True)
        
        os.makedirs(projPath, mode=0o755, exist_ok=True)
        print('copy file ',projPath)    
        shutil.copy(rootPath.joinpath('account-manager.tar.gz'),projPath)
        print('umount smbtmp')
        os.system('umount smbtmp')
        print('remove smbtmpPath')
        shutil.rmtree(smbtmpPath)
        #os.system('rm -rf smbtmp/')
               
def getProject(argv):
    global PROJECT
    PROJECT = 'std'
    if len(argv) > 1 :
        if str(argv[1]) == 'bi' or str(argv[1]) == 'telstra':
            PROJECT = str(argv[1])
                   
    print('Project ',PROJECT)

def regExpr(s):
    global PROJECT
    global VERSION
    pattern = '(\d+\.\d+\.\d+)([-\x00-\x7F]{0,})' 
    match = re.search(pattern,s)
    if match != None:
        if not match.group(2)[1:]:
            PROJECT = 'std'
        else:
            PROJECT = match.group(2)[1:]
        VERSION = match.group(1)
        print('VERSION', VERSION)
        print('PROJECT', PROJECT)
        
    else:
        print('Version Format Error')
        raise SystemExit()

def main(argv):
    getProject(sys.argv) 
    mkdir()

if __name__ == "__main__":
    main(sys.argv[1:])
