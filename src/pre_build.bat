@echo off

rem (Not needed) Preparing build date string for version file (for Russian)
rem for /f "delims=. tokens=1,2,3" %%i in ("%DATE%") do set BUILD_DATE=%%k%%j%%i
rem echo %BUILD_DATE:~2,6%

rem Creating version.h containing version information based on svn rev. number
set PATH=%PATH%;%PROGRAMFILES%\TortoiseSVN\bin\
SubWCRev.exe .. version.in version.h
