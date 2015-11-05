@echo off
rem --------------------------------------------------------
rem Making the recorder installation files.
rem --------------------------------------------------------

	set PATH=%PATH%;%PROGRAMFILES%\Inno Setup 5\

	del /F /Q "Output\*.exe"

	echo "Signing executable..."
	"C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\Bin\SignTool.exe" sign /f MyCertificate.p12 /p ***REMOVED*** /t http://timestamp.comodoca.com/authenticode ..\bin\SvRec.exe

	echo Making setup file...
	ISCC.exe SvRec.iss /Q "/sStandard=C:\Program Files\Microsoft Visual Studio 8\Common7\Tools\Bin\SignTool.exe sign /f %~dp0\MyCertificate.p12 /p ***REMOVED*** /t http://timestamp.comodoca.com/authenticode $p"

:endmark
rem
rem	finished
rem
