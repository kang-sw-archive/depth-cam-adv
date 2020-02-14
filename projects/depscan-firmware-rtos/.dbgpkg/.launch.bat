@echo off

echo Build start
make all -j8

set FILE_RAW=%~dp0../build/depscan-firmware-rtos.elf
set FILE=%FILE_RAW:\=/%

echo Check openocd status ...
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "openocd.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg -c "program %FILE% verify reset"
)

rem echo Flashing ...
rem %~dp0.launch.halt.bat