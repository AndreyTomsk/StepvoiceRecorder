@echo off
rem Building the recorder with one press.
rem Note: last version number (git rev. number) and build
rem date updated automatically (see src/pre_build.bat).


call "%PROGRAMFILES%\Microsoft Visual Studio 8\VC\vcvarsall.bat" x86


rem 1. Recorder
cd %~d0%~p0
del /Q bin\*.exe
devenv mp3_recorder_vc8.sln /Build Release


rem 2. Help file
cd doc
call help_compile.bat
cd ..


rem 3. Installer
cd install
call build_setup.bat
cd ..

echo Done.
