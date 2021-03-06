if not exist "assimp-prj" mkdir assimp-prj
if not exist "assimp-lib" mkdir assimp-lib

::Run CMake to generate .sln
cd assimp-prj
call %~dp0..\..\tools\cmake\bin\cmake.exe ..\assimp-repo

::======================================================================================================
::Invoke MSVC to build .sln Build in Release
SET VC_VARS_ALL_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build"
call %VC_VARS_ALL_PATH%\vcvarsall x64
msbuild Assimp.sln /p:configuration=release /p:platform=x64

::======================================================================================================
::Invoke MSVC to build .sln Build in Debug
SET VC_VARS_ALL_PATH="C:\Program Files (x86)\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build"
call %VC_VARS_ALL_PATH%\vcvarsall x64
msbuild Assimp.sln /p:configuration=debug /p:platform=x64

::======================================================================================================
::Copy Release lib to external folder
xcopy lib\Release\assimp-vc142-mt.lib ..\assimp-lib /y
xcopy lib\Release\zlibstatic.lib ..\assimp-lib /y
xcopy lib\Release\IrrXML.lib ..\assimp-lib /y
::Copy Debig lib to external folder
xcopy lib\Debug\assimp-vc142-mtd.lib ..\assimp-lib /y
xcopy lib\Debug\zlibstaticd.lib ..\assimp-lib /y
xcopy lib\Debug\IrrXMLd.lib ..\assimp-lib /y
::Copy pdb to external folder
::xcopy code\Debug\assimp-vc142-mtd.pdb ..\assimp-lib /y
::xcopy contrib\zlib\zlibstatic.dir\Debug\zlibstatic.pdb ..\assimp-lib /y
::xcopy contrib\irrXML\irrXML.dir\Debug\IrrXML.pdb ..\assimp-lib /y
::Copy auto-generated config.h
xcopy include\assimp\config.h ..\assimp-repo\include\assimp /y

::======================================================================================================
::Clean Assimp.sln
msbuild Assimp.sln /t:Clean /p:configuration=Debug /p:platform=x64
::Clean Assimp.sln
msbuild Assimp.sln /t:Clean /p:configuration=Release /p:platform=x64

::======================================================================================================
cd ..