set CUR_PATH=%~dp0
set ROOT_PATH=%CUR_PATH%..
set PROJECT_NAME=account-manager
set PROJECT=%1

cd %ROOT_PATH%
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64

IF "%PROJECT%" == "telstra" (
  MSBuild windows\account-manager.sln /p:Configuration=telstra /p:Platform=x64
) ELSE (
  MSBuild windows\account-manager.sln /p:Configuration=std /p:Platform=x64
)
