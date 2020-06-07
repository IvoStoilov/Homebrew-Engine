@echo off
::cd extern\assimp
::call deploy_assimp.bat
::cd ..\..

call tools\premake5\premake5_alpha15.exe --file=project\_premake_scripts\w_premake_Homebrew.lua vs2019
call tools\premake5\premake5_alpha15.exe --file=project\_premake_scripts\w_premake_SystemSandbox.lua vs2019
call tools\premake5\premake5_alpha15.exe --file=project\_premake_scripts\w_premake_GraphicsSandbox.lua vs2019
PAUSE