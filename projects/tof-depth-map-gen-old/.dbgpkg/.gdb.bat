@echo off

make all -j8

set FILE_RAW=%~dp0../build/tof-depth-map-gen-revision.elf
set FILE=%FILE_RAW:\=/%
set TP=%TMP%\gdb-ff-%RANDOM%.txt

nul>%TP%
echo target remote :3333	>>%TP%
echo define setup		>>%TP%
echo make all -j8			>>%TP%
echo file %FILE%			>>%TP%
echo monitor reset halt	>>%TP%
echo load >>%TP% 
echo end					>>%TP%
echo define rst>>%TP%
echo monitor reset halt>>%TP%
echo end>>%TP%
echo set listsize 25>>%TP%
 
start %ARM_NONE_EABI_DIR%\arm-none-eabi-gdb.exe -x %TP%