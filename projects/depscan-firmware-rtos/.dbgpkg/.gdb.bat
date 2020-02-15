@echo off

make all -j8

set FILE_RAW=%~dp0../build/depscan-firmware-rtos.elf
set FILE=%FILE_RAW:\=/%
set TP=%TMP%\gdb-ff-%RANDOM%.txt

nul>%TP%
echo target remote :3333	>>%TP%
echo file %FILE%			>>%TP%
echo define setup		>>%TP%
echo make all -j8			>>%TP%
echo file %FILE%			>>%TP%
echo monitor reset halt	>>%TP%
echo end					>>%TP%
echo define rst>>%TP%
echo monitor reset halt>>%TP%
echo end>>%TP%
echo set listsize 25>>%TP%
 
tasklist /FI "IMAGENAME eq openocd.exe" 2>NUL | find /I /N "openocd.exe">NUL
if "%ERRORLEVEL%" neq "0" ( 
	echo starting openocd
	start %openocd% -f .dbgpkg/openocd.cfg
)

start %ARM_NONE_EABI_DIR%\arm-none-eabi-gdb.exe -x %TP%