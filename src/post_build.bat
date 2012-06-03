@echo off

rem The script updates current directory to ..\doc\Keys
rem and protects target application, set in .aspr config.

pushd ..\doc\Keys
"C:\Program Files\ASProtect\ASProtect.exe" -process ".\StepVoice Recorder.aspr"
popd
