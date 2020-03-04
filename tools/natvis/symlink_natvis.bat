@echo off

call:createLinks "%USERPROFILE%\Documents\Visual Studio 2019\Visualizers\"
goto:eof

:createLinks
SET NATVIS_DST=%~1

IF NOT EXIST "%NATVIS_DST%" (
    mkdir "%NATVIS_DST%"
)

:: echo "%NATVIS_DST%"
:: echo "%~d0%~p0"
mklink "%NATVIS_DST%system.natvis" "%~d0%~p0system.natvis"