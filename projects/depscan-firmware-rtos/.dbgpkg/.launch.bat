@echo off

make all -j8

rem Start openocd if process is not exist
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "openocd.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg
)