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
        run('"C:\\Program Files\\CMake\\bin\\cmake.exe" -G "Visual Studio 14 2015 Win64" -DPROJECT=' + PROJECT +'  -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD=ON ..', env=env)
      
    else:
        run('cmake -DCMAKE_BUILD_TYPE=Release .. -DPROJECT=' + PROJECT, env=env)
        
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

def setPackageXml(versionStr):
    print('setPackageXml')
    commitType = os.getenv('GITHUB_REF_TYPE')
    if commitType == 'tag':
        run('git tag -d')
        run('git fetch --tags')
        status, output = subprocess.getstatusoutput('git for-each-ref --format="%(refname:short) | %(creatordate:short)" "refs/tags/"')
        result = output.split('\n')
        tagDict = dict(a.split(' | ') for a in result)
        print ('    tagDict ',tagDict)
        tagStr = os.getenv('GITHUB_REF_NAME')
        print ('    tagStr ',tagStr)
        releaseDate = tagDict[tagStr]
    else:
        now = datetime.now()
        Date = now.strftime('%Y-%m-%d')
        releaseDate = str(Date)
    xmlPath = os.path.join(rootPath, 'account-manager/meta/package.xml')
    if os.path.isfile(xmlPath):
        tree = ET.parse(xmlPath)
        root = tree.getroot()
        for version in root.iter('Version'):
            version.text = versionStr
            print('     Version ', version.text)
        for date in root.iter('ReleaseDate'):
            date.text = releaseDate
            print('     ReleaseDate ',date.text)
    else:
        print ('package.xml file not existed')
        raise SystemExit(-1)
    tree.write(xmlPath, xml_declaration=True, encoding ="UTF-8", method ="xml")

def setVersion():
    versionStr = '0.0.0'
    versionPath = rootPath.joinpath('VERSION.txt')
     # if 'CI_COMMIT_TAG' in os.environ:
    commitType = os.getenv('GITHUB_REF_TYPE')
    if commitType == 'tag':
        #versionStr = os.environ['CI_COMMIT_TAG']
        versionStr = os.getenv('GITHUB_REF_NAME')
        # need to modify VERSION.txt 
    elif os.path.exists(versionPath):
        versionStr = Path(versionPath).read_text().replace('\n', '')
   
    versionInfo = versionStr.split('.')
    if len(versionInfo) < 3:
        print ('Version info is wrong ' + versionStr )
        raise SystemExit(-1)
    else:
        # remove non-digit
        for i in range(len(versionInfo)):
            x=filter(str.isdigit, versionInfo[i])
            versionInfo[i] = "".join(x)
    
        if 'GITHUB_RUN_ID' in os.environ:
            versionInfo.append(os.environ['GITHUB_RUN_ID'])
            #setPackageXml(versionStr)
        else:
            versionInfo.append(datetime.now().strftime('%Y%m%d')[3:])
        
    return versionInfo,versionStr

def setProject(argv):
    PROJECT = ''
    if len(argv) == 1 :
        if str(argv[0]) == 'bi' or str(argv[0]) == 'telstra':
            print (str(argv[0]))
            PROJECT = str(argv[0])
                   
    print('Project ',PROJECT)
    return PROJECT

def main(argv):
   
    PROJECT = setProject(argv)
    versionInfo, versionStr  = setVersion()

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
    setPackageXml(versionStr)
    # archive zip
    os.chdir(rootPath)
    createPackage()

if __name__ == "__main__":
    main(sys.argv[1:])

