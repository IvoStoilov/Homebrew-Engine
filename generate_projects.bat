@echo off
call extern\Premake5\premake5.exe --file=project\_premake_scripts\premake_Homebrew.lua vs2019
call extern\Premake5\premake5.exe --file=project\_premake_scripts\premake_SystemSandbox.lua vs2019
PAUSE