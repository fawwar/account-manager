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

def mount(path_to_smb, path_to_local_dir):
    retcode = subprocess.call(["mount", "-t", "cifs", path_to_smb, path_to_local_dir])

def unmount(path_to_local_dir):
    """Unmounts the local SMB directory"""
    retcode = subprocess.call(["umount", path_to_local_dir])

def mkdir():
    if os.name == 'nt':
        print('win-x86_64')
        path = os.path.join('smbtmp','win-x86_64', versionStr)
        path = rootPath.joinPath(path)
        os.mkdir(path)
        
    else:
        print('linux-x86_64')
        print('smptmpPath start')
        smptmpPath = rootPath.joinpath('smptmp')
        #smptmpPath.makedirs(mode=0o755, exist_ok=True)
        os.makedirs(smptmpPath)
        print('smptmpPath end')
        if os.getenv('CI_COMMIT_TAG'):
            print ('Release build')
            regExpr(os.environ['CI_COMMIT_TAG'])
            os.system('mount -t cifs //$SMB_URL/IOT-Release/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            verPath = smptmpPath.joinpath(VERSION)
            #verPath.makedirs(mode=0o755, exist_ok=True)
            os.makedirs(verPath)
            projPath = verPath.joinpath(PROJECT)
            #projPath.makedirs(mode=0o755, exist_ok=True)
            os.makedirs(projPath)
            linuxPath = projPath.joinpath('linux-x86_64')
            #linuxPath.makedirs(mode=0o755, exist_ok=True)
            os.makedirs(linuxPath)
            
        else:
            print ('Test build')
            os.system('mount -t cifs //$SMB_URL/IOT-Release/ci/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            projPath = smptmpPath.joinpath(PROJECT)
            #projPath.makedirs(mode=0o755, exist_ok=True)
            os.makedirs(projPath)
            linuxPath = projPath.joinpath('linux-x86_64')
            #linuxPath.makedirs(mode=0o755, exist_ok=True)
            os.makedirs(linuxPath)
        print('copy file')    
        shutil.copy(rootPath.joinpath('account-manager.tar.gz'),linuxPath)
        print('remove smptmpPath')
        shutil.rmtree(smptmpPath)
        print('umount smptmp')
        os.system('umount smbtmp')
        #os.system('rm -rf smbtmp/')
        
        #os.system('mount -t cifs //$SMB_URL/IOT-Release/ci/comm-server smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
        #shutil.rmtree(verPath, ignore_errors=True)
        #shutil.copytree('account-manager.tar.gz', verPath)
        #os.system('umont smbtmp && rm -rf smptmp')

def setProject(argv):
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
    setProject(sys.argv) 
    mkdir()

if __name__ == "__main__":
    main(sys.argv[1:])
