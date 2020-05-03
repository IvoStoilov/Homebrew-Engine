if not exist "assimp-prj" mkdir assimp-prj
if not exist "assimp-lib" mkdir assimp-lib

::Run CMake to generate .sln
cd assimp-prj
call %~dp0..\..\tools\cmake\bin\cmake.exe ..\assimp-repo

::Invoke MSVC to build .sln
SET VC_VARS_ALL_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build"
call %VC_VARS_ALL_PATH%\vcvarsall x64
msbuild Assimp.sln /p:configuration=release /p:platform=x64

::Copy lib to external folder
xcopy lib\Release\assimp-vc142-mt.lib ..\assimp-lib /y

::Clean Assimp.sln
msbuild Assimp.sln /t:Clean /p:configuration=Release /p:platform=x64
cd ..