@echo off

rem (Not needed) Preparing build date string for version file (for Russian)
rem for /f "delims=. tokens=1,2,3" %%i in ("%DATE%") do set BUILD_DATE=%%k%%j%%i
rem echo %BUILD_DATE:~2,6%

rem Creating version.h containing version information based on git rev. number
for /f "delims=" %%a in ('git rev-list --all --count') do @set RevNumber=%%a

set VersionDots=1.9.1.%RevNumber%
set VersionCommas=%VersionDots:.=,%

echo #pragma once				> version.h
echo.						>>version.h
echo #define FILEVER    %VersionCommas%		>>version.h
echo #define PRODUCTVER %VersionCommas%		>>version.h
echo.						>>version.h
echo #ifdef UNICODE				>>version.h
echo   #define STRFILEVER L"%VersionDots%\0"	>>version.h
echo #else					>>version.h
echo   #define STRFILEVER  "%VersionDots%\0"	>>version.h
echo #endif					>>version.h
