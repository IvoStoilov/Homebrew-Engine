@echo off
::Deploy Assimp
cd extern\assimp
call deploy_assimp.bat
cd ..\..
::===================================================
::Deploy .natvis files
cd tools\natvis
call symlink_natvis.bat
cd ..\..
PAUSE