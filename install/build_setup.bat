@echo off
rem --------------------------------------------------------
rem Making the recorder installation files.
rem --------------------------------------------------------

	set PATH=%PATH%;%PROGRAMFILES%\Inno Setup 5\

	del /F /Q "Output\*.exe"

	echo Making setup file...
	ISCC.exe SvRec.iss /Q

:endmark
rem
rem	finished
rem
