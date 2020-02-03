@echo off

rem Build project before refresh launch
make all -j8

rem Start openocd if process is not exist
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "openocd.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg
)

exit
rem rem Reset and flesh MPU
rem %arm_none_eabi_dir%\arm-none-eabi-gdb --command .dbgpkg\.gdbcmd