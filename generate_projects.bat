@echo off
call extern\Premake5\premake5.exe --file=project\_premake_scripts\w_premake_Homebrew.lua vs2019
call extern\Premake5\premake5.exe --file=project\_premake_scripts\w_premake_SystemSandbox.lua vs2019
call extern\Premake5\premake5.exe --file=project\_premake_scripts\w_premake_GraphicsSandbox.lua vs2019
PAUSE