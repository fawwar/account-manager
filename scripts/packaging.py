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

PROJECT=""
VERSION=""
SERVICE=''

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

def packaging():
    try:
        if os.name == 'nt':
            print('win-x86_64')
            os.chdir(rootPath)
            if (os.path.isfile('X:\\')):
                print('X:\\ file exist')
                run('net use "X:" /delete /y')
            # compatibility
            #qtAssignPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
            if os.getenv('CI_COMMIT_TAG'):
                print('Release build')
                regExpr(os.environ['CI_COMMIT_TAG'])
                projPath = os.path.join('X:\\','build' , PROJECT, VERSION, 'win-x86_64')
                winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\'+ SERVICE +'" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'

                # compatibility
                qtAssignPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
                WinQtCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\'+ SERVICE +'" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
                run(WinQtCMD)
                if not (os.path.isdir(qtAssignPath)):
                    os.makedirs(qtAssignPath, mode=0o755, exist_ok=True)
                shutil.copy2(rootPath.joinpath(SERVICE+'.zip'), qtAssignPath)
                print('copy file ', qtAssignPath)
                run('net use "X:" /delete /y')

            else:
                print('Test build')
                projPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
                winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\ci\\Packaging\\'+ SERVICE +'" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'

            run(winCMD)
            if not (os.path.isdir(projPath)):
                os.makedirs(projPath, mode=0o755, exist_ok=True)
            shutil.copy2(rootPath.joinpath(SERVICE+'.zip'), projPath)
            print('copy file ', projPath)
            #run('net use "X:" /delete /y')

        else:
            print('linux-x86_64')
            smbtmpPath = os.path.join(rootPath, 'smbtmp')
            os.makedirs(smbtmpPath, mode=0o755, exist_ok=True)
            os.chdir(rootPath)
            if os.getenv('CI_COMMIT_TAG'):
                print ('Release build')
                regExpr(os.environ['CI_COMMIT_TAG'])
                run('mount -t cifs //$SMB_URL/IOT-Release/'+ SERVICE +' smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
                projPath = os.path.join(smbtmpPath,'build' , PROJECT, VERSION, 'linux-x86_64')

                #compatibility
                qtAssignPath = os.path.join(smbtmpPath, PROJECT, 'linux-x86_64')
                os.makedirs(qtAssignPath,  mode=0o755, exist_ok=True)
                print('copy file', qtAssignPath)
                shutil.copy(rootPath.joinpath(SERVICE+'.tar.gz'),qtAssignPath)
                
            else:
                print ('Test build')
                run('mount -t cifs //$SMB_URL/IOT-Release/ci/Packaging/'+ SERVICE +' smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
                projPath = os.path.join(smbtmpPath, PROJECT, 'linux-x86_64')

            os.makedirs(projPath, mode=0o755, exist_ok=True)
            print('copy file ',projPath)
            shutil.copy(rootPath.joinpath(SERVICE+'.tar.gz'),projPath)
            #run('umount smbtmp')
            #print('remove smbtmpPath')
            #shutil.rmtree(smbtmpPath)
               
    except Exception as e:
        print("Error:  %s" % str(e))
        raise SystemExit(-1)
    finally:
        if os.name == 'nt':
            run('net use "X:" /delete /y')
        else:
            run('umount smbtmp')
            print('remove smbtmpPath')
            shutil.rmtree(smbtmpPath)

def getProject(argv):
    global PROJECT
    PROJECT = 'std'
    if len(argv) == 1 :
        if str(argv[0]) == 'bi' or str(argv[0]) == 'telstra':
            PROJECT = str(argv[0])
                   
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
        print('setProject')
        print('     VERSION ', VERSION)
        print('     PROJECT ', PROJECT)
        
    else:
        print('Version Format Error')
        raise SystemExit(-1)

def main(argv):
    global SERVICE
    print (os.environ['CI_PROJECT_NAME'])
    if 'CI_PROJECT_NAME' in os.environ:
        SERVICE = os.environ['CI_PROJECT_NAME']
    else :
        print ('SERVICE_NAME Not Found')
        raise SystemExit(-1)
    getProject(argv) 
    packaging()

if __name__ == "__main__":
    main(sys.argv[1:])
