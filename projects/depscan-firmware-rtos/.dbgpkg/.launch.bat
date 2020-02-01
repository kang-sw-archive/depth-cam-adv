@echo off

echo Build start
make all -j8

echo Check openocd status ...
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "openocd.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg
)

echo Flashing ...
rem %~dp0.launch.halt.bat