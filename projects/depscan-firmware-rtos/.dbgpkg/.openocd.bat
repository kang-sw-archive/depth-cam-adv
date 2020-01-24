@echo off

rem Build project before refresh launch
make all -j8

rem Start openocd if process is not exist
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "myapp.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg
)

rem Reset and flesh MPU
%arm_none_eabi_dir%\arm-none-eabi-gdb --command .dbgpkg\.gdbcmd