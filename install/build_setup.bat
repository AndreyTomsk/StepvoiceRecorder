@echo off
rem --------------------------------------------------------
rem Making the recorder installation files.
rem --------------------------------------------------------

del /F /Q "Output\*.exe"

echo "Signing executable..."
"%PROGRAMFILES(X86)%\Windows Kits\10\bin\x86\signtool.exe" sign /f MyCertificate.p12 /p ***REMOVED*** /t http://timestamp.comodoca.com/authenticode ..\bin\SvRec.exe

echo Making setup file...
"%PROGRAMFILES(X86)%\Inno Setup 5\ISCC.exe" SvRec.iss /Q "/sStandard=%PROGRAMFILES(X86)%\Windows Kits\10\bin\x86\signtool.exe sign /f %~dp0\MyCertificate.p12 /p ***REMOVED*** /t http://timestamp.comodoca.com/authenticode $p"

:endmark
rem
rem	finished
rem
