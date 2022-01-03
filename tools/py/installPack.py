import subprocess
import sys
import pip
import pkg_resources

scriptPath = Path(__file__)
if not scriptPath.is_absolute():
    scriptPath = Path(os.getcwd()).joinpath(scriptPath)
   
def install(package):
    if hasattr(pip, 'main'):
        pip.main(['install', package])
    else:
        pip._internal.main(['install', package])

def uninstall(package):
    if hasattr(pip, 'main'):
        pip.main(['uninstall', package])
    else:
        pip._internal.main(['uninstall', package])


if __name__ == '__main__':
   
    #install('numpy')
    #install('pandas')
    #uninstall('numpy')

    f = open (scriptPath.joinpath('PackageList.txt'))
    for line in f.readlines():
        print (line)
        install(line)
    f.close()
    
    installed_packages = pkg_resources.working_set
    installed_packages_list = sorted(["%s==%s" % (i.key, i.version)
       for i in installed_packages])
    print(installed_packages_list)
