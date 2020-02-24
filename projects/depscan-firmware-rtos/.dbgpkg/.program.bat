@echo off

echo Build start
make all -j8

set FILE_RAW=%~dp0../build/depscan-firmware-rtos-g431kb.elf
set FILE=%FILE_RAW:\=/%
%openocd% -f .dbgpkg/openocd.cfg -c "program %FILE% verify reset exit"