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
            setPackageXml()
            projPath = os.path.join('X:\\', VERSION, PROJECT, 'win-x86_64')
            winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\account-manager" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
        else:
            print('Test build')
            projPath = os.path.join('X:\\' ,PROJECT, 'win-x86_64')
            winCMD = 'net use /y "X:" "\\\\%SMB_URL%\\IOT-Release\\ci\\account-manager" /u:"GORILLASCIENCE\\%SMB_USERNAME%" %SMB_PASSWORD%'
        
        if (os.path.isfile('X:\\')):
                print('X:\\ file exist')
                run('net use "X:" /delete /y')
        run(winCMD)
        if not (os.path.isdir(projPath)):
            os.makedirs(projPath, mode=0o755, exist_ok=True)
        shutil.copy2(rootPath.joinpath('account-manager.zip'), projPath)
        print('copy file ', projPath)
        run('net use "X:" /delete /y')
        
    else:
        print('linux-x86_64')
        smbtmpPath = os.path.join(rootPath, 'smbtmp')
        os.makedirs(smbtmpPath, mode=0o755, exist_ok=True)
        os.chdir(rootPath)
        if os.getenv('CI_COMMIT_TAG'):
            print ('Release build')
            regExpr(os.environ['CI_COMMIT_TAG'])
            setPackageXml()
            run('mount -t cifs //$SMB_URL/IOT-Release/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            projPath = os.path.join(smbtmpPath, VERSION, PROJECT, 'linux-x86_64')
            
        else:
            print ('Test build')
            run('mount -t cifs //$SMB_URL/IOT-Release/ci/account-manager smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
            projPath = os.path.join(smbtmpPath, PROJECT, 'linux-x86_64')
        
        os.makedirs(projPath, mode=0o755, exist_ok=True)
        print('copy file ',projPath)    
        shutil.copy(rootPath.joinpath('account-manager.tar.gz'),projPath)
        run('umount smbtmp')
        print('remove smbtmpPath')
        shutil.rmtree(smbtmpPath)
        #os.system('rm -rf smbtmp/')

def setPackageXml():
    print('setPackageXml')
    status, output = subprocess.getstatusoutput('git for-each-ref --format="%(refname:short) | %(creatordate:short)" "refs/tags/"')
    result = output.split('\n')
    tagDict = dict(a.split(' | ') for a in result)
    print ('    tagDict ',tagDict)
    xmlPath = os.path.join(rootPath, 'account-manager/meta/package.xml')     
    if os.path.isfile(xmlPath):
        tree = ET.parse(xmlPath)
        root = tree.getroot()
        for releaseDate in root.iter('ReleaseDate'):
            releaseDate.text = tagDict[os.environ['CI_COMMIT_TAG']]
            print ('    ReleaseDate ',releaseDate.text)
        for version in root.iter('Version'):
            version.text = VERSION
            print('     Version ', version.text)
    else:
        print ('package.xml file not existed')
    tree.write(xmlPath, xml_declaration=True, encoding ="UTF-8", method ="xml")

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
        print('setProject')
        print('     VERSION ', VERSION)
        print('     PROJECT ', PROJECT)
        
    else:
        print('Version Format Error')
        raise SystemExit()

def main(argv):
    getProject(sys.argv) 
    mkdir()

if __name__ == "__main__":
    main(sys.argv[1:])