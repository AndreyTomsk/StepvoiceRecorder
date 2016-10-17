@echo off
rem %1 - program version
rem %2 - installer file name

mkdir %~dp0Output
set OutputFile=%~dp0Output\StepvoiceRecorderUpdate.xml
echo Creating update xml at %OutputFile%

echo ^<?xml version="1.0" encoding="UTF-8"?^> > %OutputFile%
echo ^<Application^> >> %OutputFile%
echo   ^<LastVersion^>%1^</LastVersion^> >>%OutputFile%
echo   ^<DownloadLink^>http://stepvoice.com/download/%2^</DownloadLink^> >> %OutputFile%
echo ^</Application^> >> %OutputFile%