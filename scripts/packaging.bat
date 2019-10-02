set CUR_PATH=%~dp0
set ROOT_PATH=%CUR_PATH%..
set PROJECT_NAME=account-manager

cd %ROOT_PATH%
CALL "C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat" amd64
MSBuild windows\AccountServer.sln /p:Configuration=std /p:Platform=x64

cd %ROOT_PATH%
xcopy /y /e /c /i %ROOT_PATH%\packaging\qtifw\packages\%PROJECT_NAME% %PROJECT_NAME%
mkdir %PROJECT_NAME%\data
move /y windows\outputs %PROJECT_NAME%\data\%PROJECT_NAME%
7z a %PROJECT_NAME%.zip %PROJECT_NAME%
rmdir /s /q %PROJECT_NAME%
