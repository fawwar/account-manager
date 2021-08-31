#!/usr/bin/env python3
import os
import sys
import subprocess
import shlex
import shutil
import zipfile
from pathlib import Path

encoding = os.device_encoding(1)
# workaround in some env like gitlab runner or docker
if encoding is None:
    encoding = os.device_encoding(0)
if encoding is None:
    encoding = 'utf-8'
elif encoding == 'cp65001':
    encoding = 'utf-8'

#def run(command, cb=lambda x: print(x, end='')):
def run(command, cb=sys.stdout.buffer.write):
    #shlex.split(command)
    global encoding
    process = subprocess.Popen(command, stdout=subprocess.PIPE, shell=True)
    while True:
        output = process.stdout.readline()
        if output == b'' and process.poll() is not None:
            break
        if output:
            cb(output)
    rc = process.poll()
    if rc != 0:
        raise SystemExit(rc)

scriptPath = Path(__file__)
if not scriptPath.is_absolute():
    scriptPath = Path(os.getcwd()).joinpath(scriptPath)
rootPath = scriptPath.parent.parent

def runCMake():
    if os.name == 'nt':
        run('"C:\\Program Files\\CMake\\bin\\cmake.exe" -G "Visual Studio 14 2015 Win64" -DCMAKE_BUILD_TYPE=Release -DCMAKE_VS_INCLUDE_INSTALL_TO_DEFAULT_BUILD=ON ..')
    else:
        run('cmake -DCMAKE_BUILD_TYPE=Release ..')
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

# change to build path
buildPath = rootPath.joinpath('build');
buildPath.mkdir(mode=0o755, exist_ok=True)
# cmake and build
os.chdir(buildPath)
runCMake()
build()
# done, copy outputs
copyOutputs()
# archive zip
os.chdir(rootPath)
createPackage()

