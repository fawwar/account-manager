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
import xml.etree.ElementTree as ET 

encoding = os.device_encoding(1)
# workaround in some env like gitlab runner or docker
if encoding is None:
    encoding = os.device_encoding(0)
if encoding is None:
    encoding = 'utf-8'
elif encoding == 'cp65001':
    encoding = 'utf-8'

scriptPath = Path(__file__)
if not scriptPath.is_absolute():
    scriptPath = Path(os.getcwd()).joinpath(scriptPath)
rootPath = scriptPath.parent.parent

#def run(command, cb=lambda x: print(x, end='')):
def run(command, cb=sys.stdout.buffer.write, env=dict()):
    #shlex.split(command)
    global encoding
    newEnv = dict(os.environ)
    newEnv.update(env)
    print('run ' + command)
    if len(env) > 0:
        print(env)
    process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True, env=newEnv)
    while True:
        output = process.stdout.readline()
        if output == b'' and process.poll() is not None:
            break
        if output:
            cb(output)
    rc = process.poll()
    if rc != 0:
        raise SystemExit(rc)

def runCMake(PROJECT ,env):
    if os.name == 'nt':
        if PROJECT:
            run('"C:\\Program Files\\CMake\\bin\\cmake.exe" -G "Visual Studio 14 2015 Win64" -DPROJECT=telstra  -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD=ON ..', env=env)
        else:
            run('"C:\\Program Files\\CMake\\bin\\cmake.exe" -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD=ON ..', env=env)
    else:
        if PROJECT:
            run('cmake -DCMAKE_BUILD_TYPE=Release .. -DPROJECT=telstra', env=env)
        else:
            run('cmake -DCMAKE_BUILD_TYPE=Release .. ', env=env)
def build():      
    if os.name == 'nt':
        run('MSBuild account-manager.sln /p:Configuration=Release /p:Platform=x64')
    else:
        run('make install')

def copyOutputs():
    global rootPath
    print('copy outputs')
    dest = rootPath.joinpath('account-manager')
    shutil.rmtree(dest, ignore_errors=True)
    shutil.copytree(rootPath.joinpath('packaging/qtifw/packages/account-manager'), dest)
    dest.joinpath('data').mkdir(mode=0o755, parents=True, exist_ok=True)
    shutil.copytree(rootPath.joinpath('outputs'), dest.joinpath('data/account-manager'))

def createPackage():
    global rootPath
    if os.name == 'nt':
        print('create account-manager.zip')
        p = Path(rootPath.joinpath('account-manager')).glob('**/*')
        files = [x for x in p if x.is_file()]
        zf = zipfile.ZipFile(rootPath.joinpath('account-manager.zip'), mode='w')
        try:
            for x in files:
                zf.write(x.relative_to(rootPath))
        finally:
            zf.close()
    else:
        print('create account-manager.tar.gz')
        run('tar zcf account-manager.tar.gz account-manager')

def setPackageXml():
    global rootPath
    now = datetime.now()
    releaseDate = now.strftime('%Y-%m-%d')
    xmlPath = os.path.join(rootPath, 'account-manager/meta/package.xml')
    
    if os.getenv('CI_COMMIT_TAG'):
        if os.path.isfile(xmlPath):
            tree = ET.parse(xmlPath)
            root = tree.getroot()
            for it in root.iter('ReleaseDate'):
                it.text = str(releaseDate)
            tree.write(xmlPath,xml_declaration=True, encoding ="UTF-8", method ="xml" )
        else:
            print ('package.xml file not exist')

def setVersion():
    versionStr = '0.0.0'
    versionPath = rootPath.joinpath('VERSION.txt')
    if 'CI_COMMIT_TAG' in os.environ:
        versionStr = os.environ['CI_COMMIT_TAG']
        # need to modify VERSION.txt 
    elif os.path.exists(versionPath):
        versionStr = Path(versionPath).read_text().replace('\n', '')
   
    versionInfo = versionStr.split('.')
    if len(versionInfo) < 3:
        print ('Version info is wrong ' + versionStr )
    else:
        # remove non-digit
        for i in range(len(versionInfo)):
            x=filter(str.isdigit, versionInfo[i])
            versionInfo[i] = "".join(x)
    if len(versionInfo) == 3:
        if 'CI_JOB_ID' in os.environ:
            versionInfo.append(os.environ['CI_JOB_ID'])
            #setPackageXml(versionStr)
        else:
            versionInfo.append(datetime.now().strftime('%Y%m%d')[3:])
        
    return versionInfo

def setProject(argv):
    PROJECT = ''
    if len(argv) > 1 :
        if str(argv[1]) == 'bi' or str(argv[1]) == 'telstra':
            print (str(argv[1]))
            PROJECT = str(argv[1])
                   
    print('Project ',PROJECT)
    return PROJECT

def main(argv):
   
    PROJECT = setProject(sys.argv)
    versionInfo = setVersion()

    # change to build path
    buildPath = rootPath.joinpath('build')
    buildPath.mkdir(mode=0o755, exist_ok=True)
    # cmake and build
    os.chdir(buildPath)
    runCMake(PROJECT, env={
        'VERSION_MAJOR': versionInfo[0],
        'VERSION_MINOR': versionInfo[1],
        'VERSION_PATCH': versionInfo[2],
        'VERSION_REVISION': versionInfo[3]
    })
    build()
    # done, copy outputs
    copyOutputs()
    #setPackageXml()
    # archive zip
    os.chdir(rootPath)
    createPackage()

if __name__ == "__main__":
    main(sys.argv[1:])



