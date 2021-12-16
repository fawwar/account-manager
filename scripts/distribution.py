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
    global rootPath
    versionPath = rootPath.joinpath('VERSION.txt')
    print(versionPath)
    versionStr = Path(versionPath).read_text().replace('\n', '')
    print(versionStr)

    if os.name == 'nt':
        print('win-x86_64')
        path = os.path.join('smbtmp','win-x86_64', versionStr)
        path = rootPath.joinPath(path)
        os.mkdir(path)
        #smbtmpPath = rootPath.joinpath(path)
        #smbtmpPath.mkdir(mode=0o755, exist_ok=True)
    else:
        print('linux-x86_64')
        smptmpPath = rootPath.joinpath('smptmp')
        smptmpPath.mkdir(mode=0o755, exist_ok=True)
        linuxPath = smptmpPath.joinpath('linux-x86_64')
        linuxPath.mkdir(mode=0o755, exist_ok=True)
        verPath = linuxPath.joinpath(str(versionStr))
        verPath.mkdir(mode=0o755, exist_ok=True)

        #os.system('mount -t cifs //$SMB_URL/IOT-Release/ci/comm-server smbtmp -o user=$SMB_USERNAME,iocharset=utf8,password=$SMB_PASSWORD')
        #shutil.rmtree(verPath, ignore_errors=True)
        #shutil.copytree('account-manager.tar.gz', verPath)
        #os.system('umont smbtmp && rm -rf smptmp')

def main(argv):
    mkdir()

if __name__ == "__main__":
    main(sys.argv[1:])
