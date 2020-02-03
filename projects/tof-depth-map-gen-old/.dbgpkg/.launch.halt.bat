@echo off

timeout 1

set FILE_RAW=%~dp0../build/tof-depth-map-gen-revision.elf
set FILE=%FILE_RAW:\=/%
set TP=%TMP%\ff%RANDOM%.txt

nul>%TP%
echo reset halt>>%TP%
echo program %FILE%>>%TP% 

rem Send rq
nc localhost 4444 -w 1 < %TP%
exit