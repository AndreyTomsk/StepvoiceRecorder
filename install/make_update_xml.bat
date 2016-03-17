@echo off
rem %1 - program version
rem %2 - installer file name

set OutputFile=./Output/StepvoiceRecorderUpdate.xml

echo ^<?xml version="1.0" encoding="UTF-8"?^> > %OutputFile%
echo ^<Application^> >> %OutputFile%
echo   ^<LastVersion^>%1^</LastVersion^> >>%OutputFile%
echo   ^<DownloadLink^>http://stepvoice.com/download/%2^</DownloadLink^> >> %OutputFile%
echo ^</Application^> >> %OutputFile%