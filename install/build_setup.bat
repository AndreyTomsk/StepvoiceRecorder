@echo off
rem --------------------------------------------------------
rem Making the recorder installation files.
rem --------------------------------------------------------

del /F /Q "Output\*.exe"

echo "Loading certificate password..."
set /p MyCertificatePassword=<MyCertificate.password
if exist MyCertificate.p12 (
  set SignString="'%PROGRAMFILES(X86)%\Windows Kits\10\bin\x86\signtool.exe' sign /f %~dp0\MyCertificate.p12 /p %MyCertificatePassword% /t http://timestamp.comodoca.com/authenticode"
) else (
  set SignString=""
)

set SignToolCommand=%SignString%
set SignToolCommand=%SignToolCommand:"=%
set SignToolCommand=%SignToolCommand:'="%

set SignToolInno=%SignString%
set SignToolInno=%SignToolInno:"=%
set SignToolInno=%SignToolInno:'=%

if NOT %SignString% == "" (

  echo "Signing executable..."
  %SignToolCommand% ..\bin\SvRec.exe

  echo "Making setup file..."
  "%PROGRAMFILES(X86)%\Inno Setup 5\ISCC.exe" SvRec.iss /Q "/sStandard=%SignToolInno% $p"

) else (

  echo Making setup file without signing...
  "%PROGRAMFILES(X86)%\Inno Setup 5\ISCC.exe" SvRec.iss /Q "/DSkipSigning"

)
