@echo off
 
echo launching..

start %~dp0.launch.halt.bat

%openocd% -f %~dp0openocd.cfg