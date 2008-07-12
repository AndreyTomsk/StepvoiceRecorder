@echo off
rem --------------------------------------------------------
rem Script for building the recorder with one press. How work:
rem 	1. Checks for a dependend projects exist
rem	   (ANProtect, ANRender, boost, LRCE);
rem	2. Builds dependend projects
rem	3. Builds the server, help file, and install files.
rem 
rem Note: last version number (svn revision) and build date
rem       are changed automatically (see src/version.in).
rem
rem Note: taken from my work project (anyway written by me).
rem --------------------------------------------------------


call "%PROGRAMFILES%\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86


rem 1. Getting files from svn
set PATH=%PATH%;%PROGRAMFILES%\TortoiseSVN\bin\

rem set SVN_ANPROTECT=svn://advansib.tomsk.ru/Projects/Utilites/ANProtect/trunk
rem set SVN_ANRENDER=svn://advansib.tomsk.ru/Projects/ANRender/trunk
rem set SVN_LRCE=svn://advansib.tomsk.ru/Projects/LRCE/trunk
rem set SVN_BOOST=svn://advansib.tomsk.ru/Projects/3rdParty/boost_1_35_0

rem cd ..
rem call :update_dep_dirs ANProtect %SVN_ANPROTECT%
rem call :update_dep_dirs ANRender  %SVN_ANRENDER%
rem call :update_dep_dirs boost     %SVN_BOOST%
rem call :update_dep_dirs LRCE      %SVN_LRCE%


rem 2. Building necessary dependent files
rem devenv ANProtect/ANProtect.sln /Build Release
rem devenv ANRender/ANRender.sln /Build Release
rem devenv LRCE/msvc/lrce.sln /Build Release


rem 3. Building the recorder
cd %~d0%~p0
del /Q bin\*.exe
devenv  mp3_recorder_vc8.sln /Build Release


rem 4. Building help file
cd doc
call help_compile.bat
cd ..


rem 5. Building installation files
cd install
call build_setup.bat
cd ..

echo Done.
exit


rem --------------------------------------------------------
rem Function to get dependent projects from svn
rem
:update_dep_dirs

if exist %1 (
	if not "%1"=="boost" TortoiseProc /command:update /path:"%1" /url:"%2" /notempfile /closeonend:1
)
if NOT exist %1 (
	mkdir %1
	TortoiseProc /command:checkout /path:"%1" /url:"%2" /notempfile /closeonend:1
)

exit /b
